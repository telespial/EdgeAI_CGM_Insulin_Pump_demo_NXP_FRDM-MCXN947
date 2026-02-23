#include "cgm_preprocess.h"

#include <math.h>
#include <string.h>

static float ClampF32(float v, float lo, float hi)
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

static float Median3(float a, float b, float c)
{
    if (a > b)
    {
        float t = a;
        a = b;
        b = t;
    }
    if (b > c)
    {
        float t = b;
        b = c;
        c = t;
    }
    if (a > b)
    {
        float t = a;
        a = b;
        b = t;
    }
    return b;
}

static void ComputeNotchCoeffs(cgm_preprocess_t *st)
{
    float w0;
    float alpha;
    float c0;
    float b0;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;

    if ((!st->cfg.notch_enable) || (st->cfg.sample_rate_hz <= 0.1f) ||
        (st->cfg.notch_f0_hz <= 0.01f) || (st->cfg.notch_q <= 0.1f))
    {
        st->cfg.notch_enable = false;
        st->notch_b0 = 1.0f;
        st->notch_b1 = 0.0f;
        st->notch_b2 = 0.0f;
        st->notch_a1 = 0.0f;
        st->notch_a2 = 0.0f;
        return;
    }

    w0 = 2.0f * 3.14159265358979323846f * st->cfg.notch_f0_hz / st->cfg.sample_rate_hz;
    c0 = cosf(w0);
    alpha = sinf(w0) / (2.0f * st->cfg.notch_q);

    b0 = 1.0f;
    b1 = -2.0f * c0;
    b2 = 1.0f;
    a0 = 1.0f + alpha;
    a1 = -2.0f * c0;
    a2 = 1.0f - alpha;

    if (fabsf(a0) < 1.0e-6f)
    {
        st->cfg.notch_enable = false;
        st->notch_b0 = 1.0f;
        st->notch_b1 = 0.0f;
        st->notch_b2 = 0.0f;
        st->notch_a1 = 0.0f;
        st->notch_a2 = 0.0f;
        return;
    }

    st->notch_b0 = b0 / a0;
    st->notch_b1 = b1 / a0;
    st->notch_b2 = b2 / a0;
    st->notch_a1 = a1 / a0;
    st->notch_a2 = a2 / a0;
}

void CgmPreprocess_Init(cgm_preprocess_t *st, const cgm_preprocess_config_t *cfg)
{
    if ((st == NULL) || (cfg == NULL))
    {
        return;
    }

    memset(st, 0, sizeof(*st));
    st->cfg = *cfg;
    if (st->cfg.decimation_factor == 0u)
    {
        st->cfg.decimation_factor = 1u;
    }
    st->cfg.anti_alias_alpha = ClampF32(st->cfg.anti_alias_alpha, 0.01f, 1.0f);
    ComputeNotchCoeffs(st);
}

void CgmPreprocess_InitDefault(cgm_preprocess_t *st)
{
    cgm_preprocess_config_t cfg;

    memset(&cfg, 0, sizeof(cfg));
    cfg.adc_counts_to_na = 0.50f;
    cfg.adc_offset_counts = 8.0f;
    cfg.linear_k2 = 0.00012f;
    cfg.ref_nominal_mv = 1200.0f;
    cfg.anti_alias_alpha = 0.25f;
    cfg.decimation_factor = 2u;
    cfg.notch_enable = true;
    cfg.notch_f0_hz = 0.20f;
    cfg.notch_q = 2.0f;
    cfg.sample_rate_hz = 1.0f;
    cfg.cal_gain_mgdl_per_na = 1.25f;
    cfg.cal_offset_mgdl = 40.0f;
    cfg.temp_coeff_mgdl_per_c = 0.35f;
    cfg.temp_ref_c = 32.0f;
    cfg.filter_alpha_min = 0.10f;
    cfg.filter_alpha_max = 0.35f;
    cfg.noise_ref_na = 5.0f;
    cfg.trend_alpha_min = 0.15f;
    cfg.trend_alpha_max = 0.55f;

    CgmPreprocess_Init(st, &cfg);
}

void CgmPreprocess_Push(cgm_preprocess_t *st,
                        const cgm_raw_sample_t *sample,
                        cgm_preprocess_output_t *out)
{
    float x_counts;
    float x_na;
    float x_lin_na;
    float x_ref_na;
    float x_aa;
    float x_decim;
    float x_impulse;
    float x_notch;
    float glucose_mgdl;
    float glucose_filt_mgdl;
    float trend_mgdl_min;
    float trend_raw_mgdl_min;
    float noise_inst_na;
    float sqi_raw;
    float sqi_norm;
    float filter_alpha;
    float trend_alpha;
    uint8_t sqi_pct;

    if ((st == NULL) || (sample == NULL) || (out == NULL))
    {
        return;
    }

    memset(out, 0, sizeof(*out));

    x_counts = (float)sample->adc_counts + st->cfg.adc_offset_counts;
    x_na = x_counts * st->cfg.adc_counts_to_na;
    x_lin_na = x_na + st->cfg.linear_k2 * x_na * x_na;

    if (sample->ref_mv > 50.0f)
    {
        x_ref_na = x_lin_na * (st->cfg.ref_nominal_mv / sample->ref_mv);
    }
    else
    {
        x_ref_na = x_lin_na;
    }

    if (!st->aa_primed)
    {
        st->aa_state = x_ref_na;
        st->aa_primed = true;
    }
    st->aa_state += st->cfg.anti_alias_alpha * (x_ref_na - st->aa_state);
    x_aa = st->aa_state;

    st->decim_sum += x_aa;
    st->decim_count++;
    if (st->decim_count < st->cfg.decimation_factor)
    {
        return;
    }
    x_decim = st->decim_sum / (float)st->decim_count;
    st->decim_sum = 0.0f;
    st->decim_count = 0u;

    st->median_hist[0] = st->median_hist[1];
    st->median_hist[1] = st->median_hist[2];
    st->median_hist[2] = x_decim;
    if (st->median_fill < 3u)
    {
        st->median_fill++;
    }
    if (st->median_fill >= 3u)
    {
        x_impulse = Median3(st->median_hist[0], st->median_hist[1], st->median_hist[2]);
    }
    else
    {
        x_impulse = x_decim;
    }

    if (st->cfg.notch_enable)
    {
        x_notch = st->notch_b0 * x_impulse +
                  st->notch_b1 * st->notch_x1 +
                  st->notch_b2 * st->notch_x2 -
                  st->notch_a1 * st->notch_y1 -
                  st->notch_a2 * st->notch_y2;
        st->notch_x2 = st->notch_x1;
        st->notch_x1 = x_impulse;
        st->notch_y2 = st->notch_y1;
        st->notch_y1 = x_notch;
    }
    else
    {
        x_notch = x_impulse;
    }

    noise_inst_na = fabsf(x_impulse - x_notch);
    st->noise_ema_na = 0.85f * st->noise_ema_na + 0.15f * noise_inst_na;
    if (st->cfg.noise_ref_na <= 0.1f)
    {
        st->cfg.noise_ref_na = 5.0f;
    }
    sqi_raw = 100.0f - (st->noise_ema_na / st->cfg.noise_ref_na) * 100.0f;
    sqi_raw = ClampF32(sqi_raw, 0.0f, 100.0f);
    sqi_pct = (uint8_t)(sqi_raw + 0.5f);
    sqi_norm = sqi_raw / 100.0f;

    filter_alpha = st->cfg.filter_alpha_min +
                   sqi_norm * (st->cfg.filter_alpha_max - st->cfg.filter_alpha_min);
    filter_alpha = ClampF32(filter_alpha, st->cfg.filter_alpha_min, st->cfg.filter_alpha_max);
    trend_alpha = st->cfg.trend_alpha_min +
                  sqi_norm * (st->cfg.trend_alpha_max - st->cfg.trend_alpha_min);
    trend_alpha = ClampF32(trend_alpha, st->cfg.trend_alpha_min, st->cfg.trend_alpha_max);

    glucose_mgdl = st->cfg.cal_gain_mgdl_per_na * x_notch + st->cfg.cal_offset_mgdl;
    glucose_mgdl += st->cfg.temp_coeff_mgdl_per_c * (sample->temp_c - st->cfg.temp_ref_c);
    glucose_mgdl = ClampF32(glucose_mgdl, 40.0f, 400.0f);

    if (!st->trend_primed)
    {
        st->prev_glucose_mgdl = glucose_mgdl;
        st->prev_glucose_filt_mgdl = glucose_mgdl;
        st->prev_trend_mgdl_min = 0.0f;
        st->trend_primed = true;
    }

    glucose_filt_mgdl = (1.0f - filter_alpha) * st->prev_glucose_filt_mgdl + filter_alpha * glucose_mgdl;
    trend_raw_mgdl_min = (glucose_filt_mgdl - st->prev_glucose_filt_mgdl) * st->cfg.sample_rate_hz * 60.0f;
    trend_mgdl_min = (1.0f - trend_alpha) * st->prev_trend_mgdl_min + trend_alpha * trend_raw_mgdl_min;

    st->prev_glucose_mgdl = glucose_mgdl;
    st->prev_glucose_filt_mgdl = glucose_filt_mgdl;
    st->prev_trend_mgdl_min = trend_mgdl_min;

    out->output_ready = true;
    out->glucose_mgdl = glucose_mgdl;
    out->glucose_filtered_mgdl = glucose_filt_mgdl;
    out->trend_mgdl_min = trend_mgdl_min;
    out->sensor_current_na = x_notch;
    out->sqi_pct = sqi_pct;
    out->adaptive_alpha = filter_alpha;
}
