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
    st->cfg.calibration_stale_days = ClampF32(st->cfg.calibration_stale_days, 1.0f, 30.0f);
    st->cfg.drift_alpha = ClampF32(st->cfg.drift_alpha, 0.0001f, 0.10f);
    st->cfg.drift_limit_mgdl = ClampF32(st->cfg.drift_limit_mgdl, 0.1f, 40.0f);
    st->cfg.drift_warn_mgdl = ClampF32(st->cfg.drift_warn_mgdl, 0.1f, st->cfg.drift_limit_mgdl);
    st->cfg.sensitivity_short_alpha = ClampF32(st->cfg.sensitivity_short_alpha, 0.01f, 1.0f);
    st->cfg.sensitivity_long_alpha = ClampF32(st->cfg.sensitivity_long_alpha, 0.001f, 0.50f);
    st->cfg.sensitivity_ratio_low = ClampF32(st->cfg.sensitivity_ratio_low, 0.10f, 0.95f);
    st->cfg.sensitivity_ratio_high = ClampF32(st->cfg.sensitivity_ratio_high, 1.05f, 5.0f);
    st->cfg.lag_tau_s = ClampF32(st->cfg.lag_tau_s, 30.0f, 1200.0f);
    st->cfg.lag_gain_min = ClampF32(st->cfg.lag_gain_min, 0.0f, 0.20f);
    st->cfg.lag_gain_max = ClampF32(st->cfg.lag_gain_max, st->cfg.lag_gain_min, 0.30f);
    st->cfg.lag_comp_limit_mgdl = ClampF32(st->cfg.lag_comp_limit_mgdl, 0.5f, 20.0f);
    if (st->cfg.sensitivity_hold_samples == 0u)
    {
        st->cfg.sensitivity_hold_samples = 3u;
    }
    st->cal_gain_runtime = st->cfg.cal_gain_mgdl_per_na;
    st->cal_offset_runtime = st->cfg.cal_offset_mgdl;
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
    cfg.calibration_stale_days = 7.0f;
    cfg.drift_alpha = 0.010f;
    cfg.drift_limit_mgdl = 18.0f;
    cfg.drift_warn_mgdl = 10.0f;
    cfg.sensitivity_short_alpha = 0.25f;
    cfg.sensitivity_long_alpha = 0.03f;
    cfg.sensitivity_ratio_low = 0.72f;
    cfg.sensitivity_ratio_high = 1.30f;
    cfg.sensitivity_hold_samples = 3u;
    cfg.lag_enable = true;
    cfg.lag_tau_s = 180.0f;
    cfg.lag_gain_max = 0.08f;
    cfg.lag_gain_min = 0.02f;
    cfg.lag_comp_limit_mgdl = 4.0f;

    CgmPreprocess_Init(st, &cfg);
}

void CgmPreprocess_SetCalibration(cgm_preprocess_t *st, float gain_mgdl_per_na, float offset_mgdl)
{
    if (st == NULL)
    {
        return;
    }
    st->cal_gain_runtime = ClampF32(gain_mgdl_per_na, 0.10f, 10.0f);
    st->cal_offset_runtime = ClampF32(offset_mgdl, -100.0f, 200.0f);
    st->calibration_age_days = 0.0f;
}

void CgmPreprocess_ResetCalibrationAge(cgm_preprocess_t *st)
{
    if (st == NULL)
    {
        return;
    }
    st->calibration_age_days = 0.0f;
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
    float temp_comp_mgdl;
    float glucose_pre_drift_mgdl;
    float dt_s;
    float sensitivity_mag;
    float sensitivity_ratio;
    bool sensitivity_change;
    bool calibration_stale;
    bool drift_warn;
    float lag_alpha;
    float lag_gain;
    float lag_comp_mgdl;
    float glucose_kinetics_mgdl;

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

    sensitivity_mag = fabsf(x_notch - st->prev_sensor_current_na);
    st->prev_sensor_current_na = x_notch;
    st->sensitivity_short_ema = (1.0f - st->cfg.sensitivity_short_alpha) * st->sensitivity_short_ema +
                                st->cfg.sensitivity_short_alpha * sensitivity_mag;
    st->sensitivity_long_ema = (1.0f - st->cfg.sensitivity_long_alpha) * st->sensitivity_long_ema +
                               st->cfg.sensitivity_long_alpha * sensitivity_mag;
    if (st->sensitivity_long_ema < 0.001f)
    {
        st->sensitivity_long_ema = 0.001f;
    }
    sensitivity_ratio = st->sensitivity_short_ema / st->sensitivity_long_ema;
    if ((sensitivity_ratio < st->cfg.sensitivity_ratio_low) ||
        (sensitivity_ratio > st->cfg.sensitivity_ratio_high))
    {
        if (st->sensitivity_counter < 255u)
        {
            st->sensitivity_counter++;
        }
    }
    else if (st->sensitivity_counter > 0u)
    {
        st->sensitivity_counter--;
    }
    st->sensitivity_change_active = (st->sensitivity_counter >= st->cfg.sensitivity_hold_samples);
    sensitivity_change = st->sensitivity_change_active;

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

    glucose_mgdl = st->cal_gain_runtime * x_notch + st->cal_offset_runtime;
    temp_comp_mgdl = st->cfg.temp_coeff_mgdl_per_c * (sample->temp_c - st->cfg.temp_ref_c);
    glucose_pre_drift_mgdl = glucose_mgdl + temp_comp_mgdl;
    st->drift_state_mgdl += st->cfg.drift_alpha *
                            (glucose_pre_drift_mgdl - (st->trend_primed ? st->prev_glucose_filt_mgdl : glucose_pre_drift_mgdl));
    st->drift_state_mgdl = ClampF32(st->drift_state_mgdl, -st->cfg.drift_limit_mgdl, st->cfg.drift_limit_mgdl);
    glucose_mgdl = glucose_pre_drift_mgdl - st->drift_state_mgdl;
    glucose_mgdl = ClampF32(glucose_mgdl, 40.0f, 400.0f);

    dt_s = (float)st->cfg.decimation_factor / st->cfg.sample_rate_hz;
    if (dt_s < 0.01f)
    {
        dt_s = 1.0f;
    }
    st->calibration_age_days += dt_s / 86400.0f;
    calibration_stale = (st->calibration_age_days >= st->cfg.calibration_stale_days);
    drift_warn = (fabsf(st->drift_state_mgdl) >= st->cfg.drift_warn_mgdl);
    if (calibration_stale)
    {
        sqi_raw -= 15.0f;
    }
    if (drift_warn)
    {
        sqi_raw -= 12.0f;
    }
    if (sensitivity_change)
    {
        sqi_raw -= 18.0f;
    }
    sqi_raw = ClampF32(sqi_raw, 0.0f, 100.0f);
    sqi_pct = (uint8_t)(sqi_raw + 0.5f);
    sqi_norm = sqi_raw / 100.0f;
    filter_alpha = st->cfg.filter_alpha_min +
                   sqi_norm * (st->cfg.filter_alpha_max - st->cfg.filter_alpha_min);
    filter_alpha = ClampF32(filter_alpha, st->cfg.filter_alpha_min, st->cfg.filter_alpha_max);
    trend_alpha = st->cfg.trend_alpha_min +
                  sqi_norm * (st->cfg.trend_alpha_max - st->cfg.trend_alpha_min);
    trend_alpha = ClampF32(trend_alpha, st->cfg.trend_alpha_min, st->cfg.trend_alpha_max);

    if (!st->trend_primed)
    {
        st->prev_glucose_mgdl = glucose_mgdl;
        st->prev_glucose_filt_mgdl = glucose_mgdl;
        st->prev_kinetic_mgdl = glucose_mgdl;
        st->prev_trend_mgdl_min = 0.0f;
        st->trend_primed = true;
    }

    glucose_filt_mgdl = (1.0f - filter_alpha) * st->prev_glucose_filt_mgdl + filter_alpha * glucose_mgdl;
    glucose_kinetics_mgdl = glucose_filt_mgdl;
    lag_comp_mgdl = 0.0f;
    lag_gain = 0.0f;
    if (st->cfg.lag_enable)
    {
        float dt_kin_s = 1.0f / st->cfg.sample_rate_hz;
        if (dt_kin_s < 0.01f)
        {
            dt_kin_s = 1.0f;
        }
        lag_alpha = expf(-dt_kin_s / st->cfg.lag_tau_s);
        st->lag_state_mgdl = lag_alpha * st->lag_state_mgdl + (glucose_filt_mgdl - st->prev_glucose_filt_mgdl);
        lag_gain = st->cfg.lag_gain_min + sqi_norm * (st->cfg.lag_gain_max - st->cfg.lag_gain_min);
        lag_gain = ClampF32(lag_gain, st->cfg.lag_gain_min, st->cfg.lag_gain_max);
        lag_comp_mgdl = ClampF32(lag_gain * st->lag_state_mgdl,
                                 -st->cfg.lag_comp_limit_mgdl,
                                 st->cfg.lag_comp_limit_mgdl);
        glucose_kinetics_mgdl = ClampF32(glucose_filt_mgdl + lag_comp_mgdl, 40.0f, 400.0f);
    }

    trend_raw_mgdl_min = (glucose_kinetics_mgdl - st->prev_kinetic_mgdl) * st->cfg.sample_rate_hz * 60.0f;
    trend_mgdl_min = (1.0f - trend_alpha) * st->prev_trend_mgdl_min + trend_alpha * trend_raw_mgdl_min;

    st->prev_glucose_mgdl = glucose_kinetics_mgdl;
    st->prev_glucose_filt_mgdl = glucose_filt_mgdl;
    st->prev_kinetic_mgdl = glucose_kinetics_mgdl;
    st->prev_trend_mgdl_min = trend_mgdl_min;

    out->output_ready = true;
    out->glucose_mgdl = glucose_kinetics_mgdl;
    out->glucose_filtered_mgdl = glucose_filt_mgdl;
    out->trend_mgdl_min = trend_mgdl_min;
    out->sensor_current_na = x_notch;
    out->sqi_pct = sqi_pct;
    out->adaptive_alpha = filter_alpha;
    out->calibration_age_days = st->calibration_age_days;
    out->drift_comp_mgdl = st->drift_state_mgdl;
    out->calibration_stale = calibration_stale;
    out->drift_warn = drift_warn;
    out->sensitivity_change = sensitivity_change;
    out->lag_comp_mgdl = lag_comp_mgdl;
    out->lag_gain_applied = lag_gain;
}
