#include "gauge_render.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "gauge_style.h"
#include "par_lcd_s035.h"
#include "pump_bg.h"
#include "text5x7.h"

static bool gLcdReady = false;
static bool gStaticReady = false;
static bool gDynamicReady = false;
static uint16_t gPrevCurrent = 0u;
static uint16_t gPrevPower = 0u;
static uint16_t gPrevVoltage = 0u;
static uint8_t gPrevSoc = 0u;
static uint8_t gPrevTemp = 0u;
static uint16_t gPrevCpuPct = 0u;
static uint16_t gPrevAnomaly = 0u;
static uint8_t gPrevWear = 0u;
static uint8_t gTraceAx[100];
static uint8_t gTraceAy[100];
static uint8_t gTraceAz[100];
static uint8_t gTraceGx[100];
static uint8_t gTraceGy[100];
static uint8_t gTraceGz[100];
static uint8_t gTraceTemp[100];
static uint8_t gTraceBaro[100];
static uint8_t gTraceRh[100];
static uint16_t gTraceCount = 0u;
static bool gTraceReady = false;
static uint32_t gFrameCounter = 0u;
static uint8_t gPrevBarLevel = 255u;
static uint8_t gPrevFillPct = 255u;
static bool gPrevAiEnabled = false;
static uint8_t gPrevAiStatus = 255u;
static uint8_t gPrevAiFaultFlags = 255u;
static uint16_t gPrevThermalRisk = 65535u;
static uint8_t gPrevDrift = 255u;
static bool gAlertVisualValid = false;
static uint8_t gAlertVisualStatus = 255u;
static bool gAlertVisualSevere = false;
static bool gAlertVisualAiEnabled = false;
static bool gAlertVisualRecording = false;
static char gAlertVisualDetail[30];
static uint32_t gScopeSampleAccumUs = 0u;
static bool gTimelineTouchLatch = false;
static bool gScopePaused = true;
static bool gRecordConfirmActive = false;
static uint8_t gRecordConfirmAction = 0u; /* 0:none, 1:start, 2:stop, 3:clear-flash */
static bool gRecordStartRequest = false;
static bool gRecordStopRequest = false;
static bool gClearFlashRequest = false;
static bool gModalWasActive = false;
static uint8_t gPlayheadPos = 99u;
static bool gPlayheadValid = false;
static int16_t gAccelXmg = 0;
static int16_t gAccelYmg = 0;
static int16_t gAccelZmg = 1000;
static bool gAccelValid = false;
static int16_t gLinAccelXmg = 0;
static int16_t gLinAccelYmg = 0;
static int16_t gLinAccelZmg = 1000;
static bool gLinAccelValid = false;
static int16_t gGyroXdps = 0;
static int16_t gGyroYdps = 0;
static int16_t gGyroZdps = 0;
static bool gGyroValid = false;
static int16_t gMagXmgauss = 0;
static int16_t gMagYmgauss = 0;
static int16_t gMagZmgauss = 0;
static bool gMagValid = false;
static bool gMagEverValid = false;
static int32_t gCompassVxFilt = 0;
static int32_t gCompassVyFilt = 0;
static bool gCompassFiltPrimed = false;
static bool gMagCalPrimed = false;
static int16_t gMagCalMinX = 0;
static int16_t gMagCalMaxX = 0;
static int16_t gMagCalMinY = 0;
static int16_t gMagCalMaxY = 0;
static int16_t gMagCalMinZ = 0;
static int16_t gMagCalMaxZ = 0;
static int16_t gBaroDhpa = 10132;
static bool gBaroValid = false;
static int16_t gShtTempC10 = 250;
static int16_t gShtRhDpct = 500;
static bool gShtValid = false;
static int16_t gSttsTempC10 = 250;
static bool gSttsValid = false;
static uint8_t gBoardTempC = 25u;
static int16_t gBoardTempC10 = 250;
static bool gBoardTempValid = false;
static uint8_t gAnomMode = 0u;
static uint8_t gAnomTune = 1u;
static bool gAnomTraining = false;
static bool gAnomTrainedReady = false;
static uint8_t gAnomLevelAx = 0u;
static uint8_t gAnomLevelAy = 0u;
static uint8_t gAnomLevelAz = 0u;
static uint8_t gAnomLevelTemp = 0u;
static uint8_t gAnomOverall = 0u;
static uint16_t gRtcHh = 0u;
static uint8_t gRtcMm = 0u;
static uint8_t gRtcSs = 0u;
static uint8_t gRtcDs = 0u;
static bool gRtcValid = false;
static bool gHelpVisible = false;
static uint8_t gHelpPage = 0u;
static bool gSettingsVisible = false;
static bool gLimitsVisible = false;
static bool gLiveBannerMode = false;
static uint16_t gLimitGWarnMg = 12000u;
static uint16_t gLimitGFailMg = 15000u;
static int16_t gLimitTempLowC10 = 0;
static int16_t gLimitTempHighC10 = 700;
static uint16_t gLimitGyroDps = 500u;
static uint8_t gLogRateHz = 10u;
static char gModelName[48] = "UNKNOWN";
static char gModelVersion[16] = "0.0.0";
static char gExtensionVersion[16] = "0.1.0";
static float gUiBallXmgFilt = 0.0f;
static float gUiBallYmgFilt = 0.0f;
static bool gUiBallFiltPrimed = false;
static uint32_t gUiRpmRand = 0x73A51C9Du;
static uint16_t gUiRpmTenths = 0u;
static bool gUiRpmSchedulePrimed = false;
static bool gUiMotorRunning = false;
/* Dose rate is in milli-units per hour (mU/h). 1000 mU = 1 U. */
static uint32_t gUiDoseRateUhMilli = 4000u; /* 4.000 U/h startup target for 3-day 3mL profile. */
static uint32_t gUiDoseRateNextUpdateDs = 0u;
static uint32_t gUiMotorPulseNextDs = 0u;
static uint32_t gUiMotorPulseEndDs = 0u;
/* 3.0 mL reservoir starts at 92% and drains from dose flow model. */
static float gUiReservoirMl = 2.76f;
static uint8_t gUiReservoirPct = 92u;
static bool gUiReservoirPrimed = false;
static uint8_t gUiGlucoseMgdl = 98u;
static int8_t gUiGlucoseDir = 1;
static uint32_t gUiGlucoseNextStepDs = 0u;
static bool gUiGlucoseSchedPrimed = false;
static uint8_t gPrevGlucoseMgdl = 255u;
static bool gActivityModelPrimed = false;
static uint32_t gActivityPrevDs = 0u;
static int16_t gActivityPrevBaroDhpa = 10132;
static float gActivityScoreFilt = 0.0f;
static uint8_t gActivityPct = 0u;
static uint8_t gActivityStage = 0u;
static float gActivityBaroRateHpaS = 0.0f;
static uint8_t gTransportMode = 0u;
static uint8_t gTransportConfidencePct = 0u;

enum
{
    ACTIVITY_REST = 0,
    ACTIVITY_LIGHT = 1,
    ACTIVITY_MODERATE = 2,
    ACTIVITY_ACTIVE = 3,
    ACTIVITY_HEAVY = 4,
};

enum
{
    TRANSPORT_ON_FOOT = 0,
    TRANSPORT_SKATE = 1,
    TRANSPORT_SCOOTER = 2,
    TRANSPORT_BIKE = 3,
    TRANSPORT_CAR = 4,
    TRANSPORT_AIR = 5,
};

static void CopyUiTextUpper(char *dst, size_t dst_size, const char *src)
{
    if ((dst == NULL) || (dst_size == 0u))
    {
        return;
    }
    if ((src == NULL) || (src[0] == '\0'))
    {
        snprintf(dst, dst_size, "%s", "UNKNOWN");
        return;
    }

    size_t i = 0u;
    for (; (src[i] != '\0') && (i < (dst_size - 1u)); i++)
    {
        char c = src[i];
        if ((c >= 'a') && (c <= 'z'))
        {
            c = (char)(c - ('a' - 'A'));
        }
        else if (c == '_')
        {
            c = '-';
        }
        dst[i] = c;
    }
    dst[i] = '\0';
}

static uint32_t NextUiRand(void)
{
    uint32_t x = gUiRpmRand;
    x ^= (x << 13);
    x ^= (x >> 17);
    x ^= (x << 5);
    gUiRpmRand = x;
    return x;
}

static uint32_t UiNowDs(void)
{
    if (gRtcValid)
    {
        return (uint32_t)gRtcHh * 36000u +
               (uint32_t)gRtcMm * 600u +
               (uint32_t)gRtcSs * 10u +
               (uint32_t)gRtcDs;
    }

    /* Fallback when RTC is unavailable: approximate deciseconds from frame count. */
    return gFrameCounter / 3u;
}

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

static uint8_t ActivityStageFromPct(uint8_t pct)
{
    if (pct >= 78u)
    {
        return ACTIVITY_HEAVY;
    }
    if (pct >= 58u)
    {
        return ACTIVITY_ACTIVE;
    }
    if (pct >= 38u)
    {
        return ACTIVITY_MODERATE;
    }
    if (pct >= 16u)
    {
        return ACTIVITY_LIGHT;
    }
    return ACTIVITY_REST;
}

static const char *ActivityStageText(uint8_t stage)
{
    switch (stage)
    {
        case ACTIVITY_LIGHT:
            return "LIGHT";
        case ACTIVITY_MODERATE:
            return "MODERATE";
        case ACTIVITY_ACTIVE:
            return "ACTIVE";
        case ACTIVITY_HEAVY:
            return "HEAVY";
        default:
            return "REST";
    }
}

static const char *ActivityCodeText(uint8_t stage)
{
    switch (stage)
    {
        case ACTIVITY_LIGHT:
            return "ACT-L1";
        case ACTIVITY_MODERATE:
            return "ACT-L2";
        case ACTIVITY_ACTIVE:
            return "ACT-L3";
        case ACTIVITY_HEAVY:
            return "ACT-L4";
        default:
            return "ACT-L0";
    }
}

static uint16_t ActivityColor(const gauge_style_preset_t *style, uint8_t stage)
{
    if (stage >= ACTIVITY_HEAVY)
    {
        return 0xF800u;
    }
    if (stage >= ACTIVITY_ACTIVE)
    {
        return 0xFFE0u;
    }
    return style->palette.accent_green;
}

static const char *TransportModeText(uint8_t mode)
{
    switch (mode)
    {
        case TRANSPORT_SKATE:
            return "SKATE";
        case TRANSPORT_SCOOTER:
            return "SCOOT";
        case TRANSPORT_BIKE:
            return "BIKE";
        case TRANSPORT_CAR:
            return "CAR";
        case TRANSPORT_AIR:
            return "AIR";
        default:
            return "FOOT";
    }
}

static void UpdateActivityModel(void)
{
    uint32_t now_ds = UiNowDs();
    float dt_s = 0.1f;
    float accel_mag_mg = 1000.0f;
    float accel_dev_mg = 0.0f;
    float lin_mag_mg = 0.0f;
    float accel_dyn_mg;
    float accel_norm;
    float gyro_mag_dps;
    float gyro_norm;
    float baro_rate_hpa_s = 0.0f;
    float baro_norm;
    float score;
    float effort_scale = 1.0f;
    uint8_t transport_detected = TRANSPORT_ON_FOOT;
    uint8_t transport_conf = 65u;

    if (!gActivityModelPrimed)
    {
        gActivityModelPrimed = true;
        gActivityPrevDs = now_ds;
        gActivityPrevBaroDhpa = gBaroDhpa;
    }
    else
    {
        uint32_t delta_ds = now_ds - gActivityPrevDs;
        if ((delta_ds > 0u) && (delta_ds < 300u))
        {
            dt_s = (float)delta_ds * 0.1f;
        }
    }

    if (gAccelValid)
    {
        accel_mag_mg = sqrtf((float)(gAccelXmg * gAccelXmg) +
                             (float)(gAccelYmg * gAccelYmg) +
                             (float)(gAccelZmg * gAccelZmg));
        accel_dev_mg = fabsf(accel_mag_mg - 1000.0f);
    }

    if (gLinAccelValid)
    {
        lin_mag_mg = sqrtf((float)(gLinAccelXmg * gLinAccelXmg) +
                           (float)(gLinAccelYmg * gLinAccelYmg) +
                           (float)(gLinAccelZmg * gLinAccelZmg));

        /* Guard against feeds where "linear" stream still carries gravity magnitude. */
        if (lin_mag_mg < 550.0f)
        {
            accel_dyn_mg = lin_mag_mg;
        }
        else
        {
            accel_dyn_mg = accel_dev_mg;
        }
    }
    else if (gAccelValid)
    {
        accel_dyn_mg = accel_dev_mg;
    }
    else
    {
        accel_dyn_mg = 0.0f;
    }
    accel_norm = ClampF32(accel_dyn_mg / 320.0f, 0.0f, 1.0f);

    if (gGyroValid)
    {
        gyro_mag_dps = sqrtf((float)(gGyroXdps * gGyroXdps) +
                             (float)(gGyroYdps * gGyroYdps) +
                             (float)(gGyroZdps * gGyroZdps)) * 0.1f;
    }
    else
    {
        gyro_mag_dps = 0.0f;
    }
    gyro_norm = ClampF32(gyro_mag_dps / 180.0f, 0.0f, 1.0f);

    if (gBaroValid && (dt_s > 0.01f))
    {
        float delta_hpa = ((float)gBaroDhpa - (float)gActivityPrevBaroDhpa) * 0.1f;
        baro_rate_hpa_s = delta_hpa / dt_s;
    }
    gActivityBaroRateHpaS = baro_rate_hpa_s;
    baro_norm = ClampF32(fabsf(baro_rate_hpa_s) / 0.10f, 0.0f, 1.0f);

    /* Automatic transport-mode estimate (no user setting required). */
    if ((accel_dyn_mg < 28.0f) && (gyro_mag_dps < 8.0f) && (baro_norm < 0.18f))
    {
        transport_detected = TRANSPORT_ON_FOOT;
        transport_conf = 92u;
    }
    else if ((fabsf(baro_rate_hpa_s) > 0.18f) && (gyro_mag_dps < 35.0f) && (accel_dyn_mg < 180.0f))
    {
        transport_detected = TRANSPORT_AIR;
        transport_conf = 84u;
    }
    else if ((accel_dyn_mg > 40.0f) && (accel_dyn_mg < 180.0f) &&
             (gyro_mag_dps > 10.0f) && (gyro_mag_dps < 65.0f) &&
             (baro_norm < 0.35f))
    {
        transport_detected = TRANSPORT_CAR;
        transport_conf = 75u;
    }
    else if ((accel_dyn_mg > 220.0f) && (gyro_mag_dps > 90.0f))
    {
        transport_detected = TRANSPORT_BIKE;
        transport_conf = 76u;
    }
    else if ((accel_dyn_mg > 170.0f) && (gyro_mag_dps > 60.0f))
    {
        transport_detected = TRANSPORT_SCOOTER;
        transport_conf = 68u;
    }
    else if ((accel_dyn_mg > 125.0f) && (gyro_mag_dps > 45.0f))
    {
        transport_detected = TRANSPORT_SKATE;
        transport_conf = 62u;
    }
    else
    {
        transport_detected = TRANSPORT_ON_FOOT;
        transport_conf = 72u;
    }

    switch (transport_detected)
    {
        case TRANSPORT_AIR:
            effort_scale = 0.42f;
            break;
        case TRANSPORT_CAR:
            effort_scale = 0.55f;
            break;
        case TRANSPORT_BIKE:
            effort_scale = 0.84f;
            break;
        case TRANSPORT_SCOOTER:
            effort_scale = 0.78f;
            break;
        case TRANSPORT_SKATE:
            effort_scale = 0.74f;
            break;
        default:
            effort_scale = 1.00f;
            break;
    }

    score = (0.52f * accel_norm) + (0.33f * gyro_norm) + (0.15f * baro_norm);
    if ((accel_dyn_mg < 40.0f) && (gyro_mag_dps < 10.0f) && (baro_norm < 0.22f))
    {
        score *= 0.25f;
    }
    score = ClampF32(score * effort_scale, 0.0f, 1.0f);
    gActivityScoreFilt = (gActivityScoreFilt * 0.78f) + (score * 0.22f);
    gActivityPct = (uint8_t)(gActivityScoreFilt * 100.0f + 0.5f);
    gActivityStage = ActivityStageFromPct(gActivityPct);
    gTransportMode = transport_detected;
    gTransportConfidencePct = transport_conf;

    gActivityPrevDs = now_ds;
    gActivityPrevBaroDhpa = gBaroDhpa;
}

#define SCOPE_TRACE_POINTS 100u
#define SCOPE_FAST_STEP_US 100000u
#define SCOPE_SAMPLE_PERIOD_US 100000u
#define MAG_CAL_MIN_SPAN 80

#define RGB565(r, g, b) (uint16_t)((((uint16_t)(r) & 0xF8u) << 8) | (((uint16_t)(g) & 0xFCu) << 3) | (((uint16_t)(b) & 0xF8u) >> 3))
#define WARN_YELLOW RGB565(255, 255, 0)
#define ALERT_RED RGB565(255, 0, 0)
#define TRACE_AX_COLOR RGB565(80, 240, 255)
#define TRACE_AY_COLOR RGB565(255, 180, 80)
#define TRACE_AZ_COLOR RGB565(180, 120, 255)
#define TRACE_GX_COLOR RGB565(255, 96, 96)
#define TRACE_GY_COLOR RGB565(255, 220, 96)
#define TRACE_GZ_COLOR RGB565(180, 255, 120)
#define TRACE_TEMP_GREEN RGB565(64, 224, 96)
#define TRACE_TEMP_YELLOW RGB565(255, 220, 72)
#define TRACE_TEMP_RED RGB565(255, 72, 72)
#define TRACE_BARO_COLOR RGB565(110, 190, 255)
#define TRACE_RH_COLOR RGB565(120, 220, 255)

enum
{
    PANEL_X0 = 18,
    PANEL_Y0 = 14,
    PANEL_X1 = 462,
    PANEL_Y1 = 306,
    MAIN_CX = 236,
    MAIN_CY = 182,
    MAIN_R = 84,
    MID_TOP_CX = 76,
    MID_TOP_CY = 124,
    MID_BOT_CX = 76,
    MID_BOT_CY = 234,
    MID_R = 42,
    BATT_X = 391,
    BATT_Y = 291,
    BATT_W = 66,
    BATT_H = 21,
    SCOPE_X = 328,
    SCOPE_Y = 24,
    SCOPE_W = 151,
    SCOPE_H = 92,
    TERM_X = 328,
    TERM_Y = 124,
    TERM_W = 151,
    TERM_H = 176,
    RTC_TEXT_Y = 284,
    BAR_X0 = 0,
    BAR_Y0 = 161,
    BAR_X1 = 24,
    BAR_Y1 = 294,
    BAR_SEGMENTS = 10,
    SECTION2_CX = 240,
    AI_PILL_X0 = GAUGE_RENDER_AI_PILL_X0,
    AI_PILL_Y0 = GAUGE_RENDER_AI_PILL_Y0,
    AI_PILL_X1 = GAUGE_RENDER_AI_PILL_X1,
    AI_PILL_Y1 = GAUGE_RENDER_AI_PILL_Y1,
    AI_SET_X0 = GAUGE_RENDER_AI_SET_X0,
    AI_SET_Y0 = GAUGE_RENDER_AI_SET_Y0,
    AI_SET_X1 = GAUGE_RENDER_AI_SET_X1,
    AI_SET_Y1 = GAUGE_RENDER_AI_SET_Y1,
    AI_HELP_X0 = GAUGE_RENDER_AI_HELP_X0,
    AI_HELP_Y0 = GAUGE_RENDER_AI_HELP_Y0,
    AI_HELP_X1 = GAUGE_RENDER_AI_HELP_X1,
    AI_HELP_Y1 = GAUGE_RENDER_AI_HELP_Y1,
    ALERT_X0 = 147,
    ALERT_Y0 = 33,
    ALERT_X1 = 325,
    ALERT_Y1 = 71,
    TIMELINE_X0 = SCOPE_X,
    TIMELINE_Y0 = 4,
    TIMELINE_X1 = SCOPE_X + SCOPE_W,
    TIMELINE_Y1 = SCOPE_Y - 2,
    REC_CONFIRM_X0 = 108,
    REC_CONFIRM_Y0 = 110,
    REC_CONFIRM_X1 = 372,
    REC_CONFIRM_Y1 = 214,
    REC_CONFIRM_YES_X0 = 146,
    REC_CONFIRM_YES_Y0 = 182,
    REC_CONFIRM_YES_X1 = 230,
    REC_CONFIRM_YES_Y1 = 204,
    REC_CONFIRM_NO_X0 = 250,
    REC_CONFIRM_NO_Y0 = 182,
    REC_CONFIRM_NO_X1 = 334,
    REC_CONFIRM_NO_Y1 = 204,
    GYRO_WIDGET_CX = 88,
    GYRO_WIDGET_R = 62,
    GYRO_WIDGET_CY = PUMP_BG_HEIGHT / 2,
    COMPASS_WIDGET_R = 30,
    COMPASS_WIDGET_CX = GYRO_WIDGET_CX,
    COMPASS_WIDGET_CY = GYRO_WIDGET_CY - GYRO_WIDGET_R - 68,
};

static int32_t ClampI32(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

static __attribute__((unused)) uint16_t HeadingDegFromMag(int32_t mx, int32_t my)
{
    if ((mx == 0) && (my == 0))
    {
        return 0u;
    }

    /* 0 deg = up/North, 90 = East, 180 = South, 270 = West. */
    {
        float heading = atan2f((float)mx, (float)(-my)) * (180.0f / 3.14159265f);
        uint16_t deg;
        if (heading < 0.0f)
        {
            heading += 360.0f;
        }
        if (heading >= 360.0f)
        {
            heading -= 360.0f;
        }
        deg = (uint16_t)(heading + 0.5f);
        if (deg >= 360u)
        {
            deg = 0u;
        }
        return deg;
    }
}

static uint8_t ScaleTo8(uint32_t value, uint32_t max_value)
{
    if (max_value == 0u)
    {
        return 0u;
    }
    if (value >= max_value)
    {
        return 255u;
    }
    return (uint8_t)((value * 255u) / max_value);
}

static uint8_t ScaleSignedTo8(int32_t value, int32_t abs_max)
{
    int32_t v = ClampI32(value, -abs_max, abs_max);
    return (uint8_t)(((v + abs_max) * 255) / (2 * abs_max));
}

static int16_t TempC10ToF10(int16_t temp_c10)
{
    return (int16_t)((temp_c10 * 9) / 5 + 320);
}

static void FormatTempCF(char *out, size_t out_len, int16_t temp_c10)
{
    int16_t temp_f10 = TempC10ToF10(temp_c10);
    int16_t c_abs = (temp_c10 < 0) ? (int16_t)-temp_c10 : temp_c10;
    int16_t f_abs = (temp_f10 < 0) ? (int16_t)-temp_f10 : temp_f10;
    snprintf(out,
             out_len,
             "TEMP %s%2d.%1dC %s%3d.%1dF",
             (temp_c10 < 0) ? "-" : "",
             (int)(c_abs / 10),
             (int)(c_abs % 10),
             (temp_f10 < 0) ? "-" : "",
             (int)(f_abs / 10),
             (int)(f_abs % 10));
}

static void FormatAccelGCompact(char *out, size_t out_len, int16_t ax_mg, int16_t ay_mg, int16_t az_mg)
{
    int16_t ax_abs = (ax_mg < 0) ? (int16_t)-ax_mg : ax_mg;
    int16_t ay_abs = (ay_mg < 0) ? (int16_t)-ay_mg : ay_mg;
    int16_t az_abs = (az_mg < 0) ? (int16_t)-az_mg : az_mg;
    snprintf(out,
             out_len,
             "ACC %c%d.%03d %c%d.%03d %c%d.%03dg",
             (ax_mg < 0) ? '-' : '+', (int)(ax_abs / 1000), (int)(ax_abs % 1000),
             (ay_mg < 0) ? '-' : '+', (int)(ay_abs / 1000), (int)(ay_abs % 1000),
             (az_mg < 0) ? '-' : '+', (int)(az_abs / 1000), (int)(az_abs % 1000));
}

static void FormatShieldEnvCompact(char *out, size_t out_len)
{
    int16_t p_abs = (gBaroDhpa < 0) ? (int16_t)-gBaroDhpa : gBaroDhpa;
    int16_t rh_abs = (gShtRhDpct < 0) ? (int16_t)-gShtRhDpct : gShtRhDpct;
    int16_t sht_abs = (gShtTempC10 < 0) ? (int16_t)-gShtTempC10 : gShtTempC10;

    snprintf(out,
             out_len,
             "B%s%4d.%1dh H %s%2d.%1d S%s%2d.%1d",
             gBaroValid ? "" : "-",
             (int)(p_abs / 10),
             (int)(p_abs % 10),
             gShtValid ? "" : "-",
             (int)(rh_abs / 10),
             (int)(rh_abs % 10),
             gShtValid ? ((gShtTempC10 < 0) ? "-" : "+") : "-",
             (int)(sht_abs / 10),
             (int)(sht_abs % 10));
}

static void FormatDewAltCompact(char *out, size_t out_len)
{
    int16_t dew_c10 = gShtTempC10;
    int32_t alt_m = 0;
    int16_t dew_abs;
    int32_t alt_abs;
    const char *dew_tag = gShtValid ? "" : "-";
    const char *alt_tag = gBaroValid ? "" : "-";
    char dew_sign;
    char alt_sign;

    if (gShtValid)
    {
        /* Dew point approximation: Td = T - ((100 - RH) / 5). */
        dew_c10 = (int16_t)(gShtTempC10 - ((1000 - gShtRhDpct) / 5));
    }
    if (gBaroValid)
    {
        /* Near-sea-level altitude estimate: ~8.3 m per hPa from 1013.2 hPa baseline. */
        alt_m = ((int32_t)(10132 - gBaroDhpa) * 83) / 100;
    }

    dew_sign = (dew_c10 < 0) ? '-' : '+';
    dew_abs = (dew_c10 < 0) ? (int16_t)-dew_c10 : dew_c10;
    alt_sign = (alt_m < 0) ? '-' : '+';
    alt_abs = (alt_m < 0) ? -alt_m : alt_m;

    snprintf(out,
             out_len,
             "D%s%c%2d.%1dC A%s%c%4dm",
             dew_tag,
             dew_sign,
             (int)(dew_abs / 10),
             (int)(dew_abs % 10),
             alt_tag,
             alt_sign,
             (int)alt_abs);
}

static int16_t DisplayTempC10(const power_sample_t *sample)
{
    if (gSttsValid)
    {
        return gSttsTempC10;
    }
    if (gShtValid)
    {
        return gShtTempC10;
    }
    if (gBoardTempValid)
    {
        return gBoardTempC10;
    }
    if (sample != NULL)
    {
        return (int16_t)sample->temp_c * 10;
    }
    return 250;
}

static uint8_t DisplayTempC(const power_sample_t *sample)
{
    int16_t c10 = DisplayTempC10(sample);
    int32_t c = c10 / 10;
    if (c < 0)
    {
        c = 0;
    }
    if (c > 100)
    {
        c = 100;
    }
    return (uint8_t)c;
}

static uint16_t DisplayPowerW(const power_sample_t *sample)
{
    uint32_t p_w = ((uint32_t)sample->voltage_mV * (uint32_t)sample->current_mA) / 100000u;
    if (p_w > 14000u)
    {
        p_w = 14000u;
    }
    return (uint16_t)p_w;
}

static uint16_t __attribute__((unused)) TempTraceColorFromScaled(uint8_t temp_scaled)
{
    uint8_t temp_c = (uint8_t)(((uint16_t)temp_scaled * 100u) / 255u);
    if (temp_c >= 85u)
    {
        return TRACE_TEMP_RED;
    }
    if (temp_c >= 75u)
    {
        return TRACE_TEMP_YELLOW;
    }
    return TRACE_TEMP_GREEN;
}

static uint16_t __attribute__((unused)) TempTraceColorFromC10(int16_t temp_c10)
{
    if (temp_c10 >= 850)
    {
        return TRACE_TEMP_RED;
    }
    if (temp_c10 >= 750)
    {
        return TRACE_TEMP_YELLOW;
    }
    return TRACE_TEMP_GREEN;
}

static const char *AnomModeText(uint8_t mode, bool training_active)
{
    if (mode == 2u)
    {
        return "M3 LIMIT";
    }
    if (mode == 1u)
    {
        return training_active ? "M2 TRAIN" : "M2 MON";
    }
    return "M1 ADAPT";
}

static __attribute__((unused)) const char *AnomTuneText(uint8_t tune)
{
    if (tune == 2u)
    {
        return "STRICT";
    }
    if (tune == 0u)
    {
        return "LOOSE";
    }
    return "NORMAL";
}

static uint16_t AnomLevelColor(uint8_t level)
{
    if (level >= 3u)
    {
        return ALERT_RED;
    }
    if (level >= 2u)
    {
        return RGB565(255, 170, 40);
    }
    if (level >= 1u)
    {
        return WARN_YELLOW;
    }
    return RGB565(140, 170, 190);
}

static char __attribute__((unused)) AnomLevelTag(uint8_t level)
{
    if (level >= 3u)
    {
        return 'M';
    }
    if (level >= 2u)
    {
        return 'm';
    }
    if (level >= 1u)
    {
        return 'W';
    }
    return '-';
}

static void PushScopeSample(void)
{
    uint16_t cap = (uint16_t)sizeof(gTraceAx);
    uint8_t t = DisplayTempC(NULL);
    int16_t acc_x = gLinAccelValid ? gLinAccelXmg : gAccelXmg;
    int16_t acc_y = gLinAccelValid ? gLinAccelYmg : gAccelYmg;
    int16_t acc_z = gLinAccelValid ? gLinAccelZmg : gAccelZmg;
    uint8_t ax = ScaleSignedTo8(acc_x, 2000);
    uint8_t ay = ScaleSignedTo8(acc_y, 2000);
    uint8_t az = ScaleSignedTo8(acc_z, 2000);
    uint8_t gx = gGyroValid ? ScaleSignedTo8(gGyroXdps / 10, 2000) : ScaleSignedTo8(0, 2000);
    uint8_t gy = gGyroValid ? ScaleSignedTo8(gGyroYdps / 10, 2000) : ScaleSignedTo8(0, 2000);
    uint8_t gz = gGyroValid ? ScaleSignedTo8(gGyroZdps / 10, 2000) : ScaleSignedTo8(0, 2000);
    uint8_t tp = ScaleTo8(t, 100u);
    uint8_t bp = ScaleTo8((uint32_t)ClampI32((int32_t)gBaroDhpa - 9600, 0, 1200), 1200u);
    uint8_t rh = ScaleTo8((uint32_t)ClampI32(gShtRhDpct, 0, 1000), 1000u);

    gScopeSampleAccumUs += SCOPE_FAST_STEP_US;
    if (gScopeSampleAccumUs < SCOPE_SAMPLE_PERIOD_US)
    {
        return;
    }
    gScopeSampleAccumUs -= SCOPE_SAMPLE_PERIOD_US;

    if (gTraceCount < cap)
    {
        gTraceAx[gTraceCount] = ax;
        gTraceAy[gTraceCount] = ay;
        gTraceAz[gTraceCount] = az;
        gTraceGx[gTraceCount] = gx;
        gTraceGy[gTraceCount] = gy;
        gTraceGz[gTraceCount] = gz;
        gTraceTemp[gTraceCount] = tp;
        gTraceBaro[gTraceCount] = bp;
        gTraceRh[gTraceCount] = rh;
        gTraceCount++;
    }
    else
    {
        uint16_t i;
        for (i = 1u; i < cap; i++)
        {
            gTraceAx[i - 1u] = gTraceAx[i];
            gTraceAy[i - 1u] = gTraceAy[i];
            gTraceAz[i - 1u] = gTraceAz[i];
            gTraceGx[i - 1u] = gTraceGx[i];
            gTraceGy[i - 1u] = gTraceGy[i];
            gTraceGz[i - 1u] = gTraceGz[i];
            gTraceTemp[i - 1u] = gTraceTemp[i];
            gTraceBaro[i - 1u] = gTraceBaro[i];
            gTraceRh[i - 1u] = gTraceRh[i];
        }
        gTraceAx[cap - 1u] = ax;
        gTraceAy[cap - 1u] = ay;
        gTraceAz[cap - 1u] = az;
        gTraceGx[cap - 1u] = gx;
        gTraceGy[cap - 1u] = gy;
        gTraceGz[cap - 1u] = gz;
        gTraceTemp[cap - 1u] = tp;
        gTraceBaro[cap - 1u] = bp;
        gTraceRh[cap - 1u] = rh;
        gTraceReady = true;
    }
}

static void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t width, uint16_t color)
{
    int32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t dy = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = dx + dy;
    int32_t half = (width > 1) ? (width / 2) : 0;

    for (;;)
    {
        par_lcd_s035_fill_rect(x0 - half, y0 - half, x0 + half, y0 + half, color);
        if ((x0 == x1) && (y0 == y1))
        {
            break;
        }

        {
            int32_t e2 = 2 * err;
            if (e2 >= dy)
            {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }
}

static __attribute__((unused)) void DrawLineClippedCircle(int32_t x0,
                                  int32_t y0,
                                  int32_t x1,
                                  int32_t y1,
                                  int32_t width,
                                  uint16_t color,
                                  int32_t cx,
                                  int32_t cy,
                                  int32_t r)
{
    int32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t dy = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = dx + dy;
    int32_t half = (width > 1) ? (width / 2) : 0;
    int32_t r2 = r * r;

    for (;;)
    {
        int32_t ddx = x0 - cx;
        int32_t ddy = y0 - cy;
        if ((ddx * ddx + ddy * ddy) <= r2)
        {
            par_lcd_s035_fill_rect(x0 - half, y0 - half, x0 + half, y0 + half, color);
        }
        if ((x0 == x1) && (y0 == y1))
        {
            break;
        }

        {
            int32_t e2 = 2 * err;
            if (e2 >= dy)
            {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }
}

static void DrawTextUi(int32_t x, int32_t y, int32_t scale, const char *text, uint16_t fg)
{
    edgeai_text5x7_draw_scaled(x + 1, y + 1, scale, text, RGB565(0, 0, 0));
    edgeai_text5x7_draw_scaled(x, y, scale, text, fg);
}

static void DrawTextUiCrisp(int32_t x, int32_t y, int32_t scale, const char *text, uint16_t fg)
{
    edgeai_text5x7_draw_scaled(x, y, scale, text, fg);
}

static void DrawTextUi120(int32_t x, int32_t y, const char *text, uint16_t fg)
{
    /* Approximate +20% perceived size for scale-1 text using a light expansion pass. */
    edgeai_text5x7_draw_scaled(x + 1, y + 1, 1, text, RGB565(0, 0, 0));
    edgeai_text5x7_draw_scaled(x, y, 1, text, fg);
    edgeai_text5x7_draw_scaled(x + 1, y, 1, text, fg);
}

static __attribute__((unused)) void DrawTextUi125(int32_t x, int32_t y, const char *text, uint16_t fg)
{
    /* Approximate ~1.56x by expanding two pixels in X and Y. */
    edgeai_text5x7_draw_scaled(x + 1, y + 1, 1, text, RGB565(0, 0, 0));
    edgeai_text5x7_draw_scaled(x + 2, y + 1, 1, text, RGB565(0, 0, 0));
    edgeai_text5x7_draw_scaled(x + 1, y + 2, 1, text, RGB565(0, 0, 0));
    edgeai_text5x7_draw_scaled(x, y, 1, text, fg);
    edgeai_text5x7_draw_scaled(x + 1, y, 1, text, fg);
    edgeai_text5x7_draw_scaled(x + 2, y, 1, text, fg);
    edgeai_text5x7_draw_scaled(x, y + 1, 1, text, fg);
    edgeai_text5x7_draw_scaled(x + 1, y + 1, 1, text, fg);
    edgeai_text5x7_draw_scaled(x, y + 2, 1, text, fg);
}

static void BlitPumpBgRegion(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    int32_t y;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= PUMP_BG_WIDTH) x1 = PUMP_BG_WIDTH - 1;
    if (y1 >= PUMP_BG_HEIGHT) y1 = PUMP_BG_HEIGHT - 1;
    if ((x0 > x1) || (y0 > y1))
    {
        return;
    }

    for (y = y0; y <= y1; y++)
    {
        par_lcd_s035_blit_rect(x0, y, x1, y, (uint16_t *)&g_pump_bg_rgb565[(y * PUMP_BG_WIDTH) + x0]);
    }
}

static void DrawTerminalLine(int32_t y, const char *text, uint16_t color)
{
    int32_t x0 = TERM_X + 4;
    int32_t x1 = TERM_X + TERM_W - 4;
    int32_t yy;
    int32_t avail_px;
    char clipped[40];
    size_t n;

    if (x0 < 0)
    {
        x0 = 0;
    }
    if (x1 >= PUMP_BG_WIDTH)
    {
        x1 = PUMP_BG_WIDTH - 1;
    }

    for (yy = y - 1; yy <= (y + 9); yy++)
    {
        if ((yy >= 0) && (yy < PUMP_BG_HEIGHT))
        {
            par_lcd_s035_blit_rect(x0, yy, x1, yy, (uint16_t *)&g_pump_bg_rgb565[(yy * PUMP_BG_WIDTH) + x0]);
        }
    }

    avail_px = x1 - x0 + 1;
    if (avail_px <= 0)
    {
        return;
    }

    n = 0u;
    while ((text[n] != '\0') && (n < (sizeof(clipped) - 1u)))
    {
        clipped[n] = text[n];
        n++;
    }
    clipped[n] = '\0';

    while ((n > 0u) && (edgeai_text5x7_width(1, clipped) > avail_px))
    {
        n--;
        clipped[n] = '\0';
    }

    DrawTextUi(x0, y, 1, clipped, color);
}

static __attribute__((unused)) void DrawRing(int32_t cx, int32_t cy, int32_t r_outer, int32_t thickness, uint16_t ring, uint16_t inner)
{
    par_lcd_s035_draw_filled_circle(cx, cy, r_outer, ring);
    par_lcd_s035_draw_filled_circle(cx, cy, r_outer - thickness, inner);
}

static __attribute__((unused)) int16_t ClampI16(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo)
    {
        return (int16_t)lo;
    }
    if (v > hi)
    {
        return (int16_t)hi;
    }
    return (int16_t)v;
}

static __attribute__((unused)) void CompassDisplayVector(int32_t *vx, int32_t *vy, bool *valid)
{
    int32_t mx = gMagXmgauss;
    int32_t my = gMagYmgauss;
    int32_t mz = gMagZmgauss;
    int32_t span_x = (int32_t)gMagCalMaxX - (int32_t)gMagCalMinX;
    int32_t span_y = (int32_t)gMagCalMaxY - (int32_t)gMagCalMinY;
    int32_t span_z = (int32_t)gMagCalMaxZ - (int32_t)gMagCalMinZ;
    int32_t off_x = ((span_x >= MAG_CAL_MIN_SPAN) ? ((gMagCalMinX + gMagCalMaxX) / 2) : 0);
    int32_t off_y = ((span_y >= MAG_CAL_MIN_SPAN) ? ((gMagCalMinY + gMagCalMaxY) / 2) : 0);
    int32_t off_z = ((span_z >= MAG_CAL_MIN_SPAN) ? ((gMagCalMinZ + gMagCalMaxZ) / 2) : 0);
    float mfx;
    float mfy;
    float mfz;
    float ax;
    float ay;
    float az;
    float anorm;
    float ux;
    float uy;
    float uz;
    float mdotu;
    float hx;
    float hy;
    float hz;
    float hxy_norm;

    if (!gMagEverValid)
    {
        *vx = 0;
        *vy = 0;
        *valid = false;
        return;
    }

    mfx = (float)(mx - off_x);
    mfy = (float)(my - off_y);
    mfz = (float)(mz - off_z);
    hx = mfx;
    hy = mfy;
    hz = mfz;

    if (gAccelValid)
    {
        ax = (float)gAccelXmg;
        ay = (float)gAccelYmg;
        az = (float)gAccelZmg;
        anorm = sqrtf((ax * ax) + (ay * ay) + (az * az));
        if (anorm > 100.0f)
        {
            /* Tilt compensation by projecting magnetic vector onto horizontal plane. */
            ux = ax / anorm;
            uy = ay / anorm;
            uz = az / anorm;
            mdotu = (mfx * ux) + (mfy * uy) + (mfz * uz);
            hx = mfx - (mdotu * ux);
            hy = mfy - (mdotu * uy);
            hz = mfz - (mdotu * uz);
        }
    }

    (void)hz;
    /* Board frame: X=North-ish, Y=East-ish. Screen vector uses +X right, +Y down. */
    *vx = (int32_t)hy;
    *vy = -(int32_t)hx;
    hxy_norm = sqrtf((hx * hx) + (hy * hy));
    *valid = (hxy_norm > 1.0f);
}

static __attribute__((unused)) void DrawCompassWidgetFrame(const gauge_style_preset_t *style)
{
    (void)style;
    /* Compass intentionally disabled until heading tracking is validated. */
}

static __attribute__((unused)) void DrawCompassWidgetDynamic(void)
{
    /* Compass intentionally disabled until heading tracking is validated. */
}

static __attribute__((unused)) void DrawGyroWidgetFrame(const gauge_style_preset_t *style)
{
    (void)style;
}

static __attribute__((unused)) void DrawGyroWidgetDynamic(const gauge_style_preset_t *style)
{
    (void)style;
}

static void DrawSpaceboxBackground(void)
{
    int32_t y;
    for (y = 0; y < PUMP_BG_HEIGHT; y++)
    {
        par_lcd_s035_blit_rect(0, y, PUMP_BG_WIDTH - 1, y,
                               (uint16_t *)&g_pump_bg_rgb565[y * PUMP_BG_WIDTH]);
    }
}

static void DrawScopeFrame(const gauge_style_preset_t *style)
{
    int32_t ty;
    int32_t lx0 = TIMELINE_X0;
    int32_t mid = (TIMELINE_X0 + TIMELINE_X1) / 2;
    int32_t lx1 = mid - 1;
    int32_t rx0 = mid;
    int32_t rx1 = TIMELINE_X1;

    (void)style;
    par_lcd_s035_fill_rect(SCOPE_X, SCOPE_Y, SCOPE_X + SCOPE_W, SCOPE_Y + SCOPE_H, RGB565(18, 3, 7));
    par_lcd_s035_fill_rect(SCOPE_X + 2, SCOPE_Y + 2, SCOPE_X + SCOPE_W - 2, SCOPE_Y + SCOPE_H - 2, RGB565(7, 10, 12));
    par_lcd_s035_fill_rect(TIMELINE_X0 + 1, TIMELINE_Y0 + 1, TIMELINE_X1 - 1, TIMELINE_Y1 - 1, RGB565(20, 28, 34));
    if (gLiveBannerMode)
    {
        par_lcd_s035_fill_rect(TIMELINE_X0 + 1, TIMELINE_Y0 + 1, TIMELINE_X1 - 1, TIMELINE_Y1 - 1, RGB565(22, 78, 112));
        DrawLine(TIMELINE_X0 + 1, TIMELINE_Y0 + 2, TIMELINE_X1 - 1, TIMELINE_Y0 + 2, 1, RGB565(120, 220, 255));
        DrawLine(TIMELINE_X0 + 1, TIMELINE_Y1 - 2, TIMELINE_X1 - 1, TIMELINE_Y1 - 2, 1, RGB565(70, 170, 220));
        ty = TIMELINE_Y0 + ((TIMELINE_Y1 - TIMELINE_Y0 - 7) / 2);
        DrawTextUi(TIMELINE_X0 + ((TIMELINE_X1 - TIMELINE_X0 + 1 - edgeai_text5x7_width(1, "LIVE")) / 2),
                   ty,
                   1,
                   "LIVE",
                   RGB565(192, 242, 255));
    }
    else
    {
        par_lcd_s035_fill_rect(lx0 + 1,
                               TIMELINE_Y0 + 1,
                               lx1 - 1,
                               TIMELINE_Y1 - 1,
                               gScopePaused ? RGB565(20, 180, 36) : RGB565(24, 82, 210));
        par_lcd_s035_fill_rect(rx0 + 1, TIMELINE_Y0 + 1, rx1 - 1, TIMELINE_Y1 - 1, RGB565(220, 24, 24));
        ty = TIMELINE_Y0 + ((TIMELINE_Y1 - TIMELINE_Y0 - 7) / 2);
        DrawTextUi(lx0 + ((lx1 - lx0 + 1 - edgeai_text5x7_width(1, gScopePaused ? "PLAY" : "STOP")) / 2),
                   ty,
                   1,
                   gScopePaused ? "PLAY" : "STOP",
                   gScopePaused ? RGB565(232, 255, 232) : RGB565(210, 236, 255));
        DrawTextUi(rx0 + ((rx1 - rx0 + 1 - edgeai_text5x7_width(1, "REC")) / 2), ty, 1, "REC", RGB565(255, 232, 232));
    }
}

static __attribute__((unused)) void DrawCenterWireBox(void)
{
    /* Removed per project direction: no accel wire-box visual. */
}

static __attribute__((unused)) void DrawCenterAccelBall(void)
{
    /* Removed per project direction: no accel box/ball visual. */
}

static void DrawGlucoseIndicator(void)
{
    /* Slowly changing glucose model: 96..106 mg/dL, step every 90..180 s. */
    uint32_t now_ds = UiNowDs();
    char bg_text[20];
    int32_t bg_x;
    int32_t bg_y = 224; /* moved up by two scale-2 row heights (2*14 px). */

    if ((!gUiGlucoseSchedPrimed) || ((int32_t)(now_ds - gUiGlucoseNextStepDs) >= 0))
    {
        uint32_t r = NextUiRand();
        if (gUiGlucoseMgdl <= 96u)
        {
            gUiGlucoseDir = 1;
        }
        else if (gUiGlucoseMgdl >= 106u)
        {
            gUiGlucoseDir = -1;
        }
        else if ((r % 8u) == 0u)
        {
            /* Occasional direction changes keep the trend natural-looking. */
            gUiGlucoseDir = (int8_t)(-gUiGlucoseDir);
        }

        if (gUiGlucoseDir > 0)
        {
            gUiGlucoseMgdl++;
        }
        else
        {
            gUiGlucoseMgdl--;
        }

        gUiGlucoseNextStepDs = now_ds + 900u + (r % 901u); /* 90..180 seconds per 1 mg/dL step. */
        gUiGlucoseSchedPrimed = true;
    }

    snprintf(bg_text, sizeof(bg_text), "%3u mg/dL", (unsigned int)gUiGlucoseMgdl);
    bg_x = SECTION2_CX - (edgeai_text5x7_width(2, bg_text) / 2);
    DrawTextUi(bg_x, bg_y, 2, bg_text, RGB565(124, 255, 124));
    gPrevGlucoseMgdl = gUiGlucoseMgdl;
}

static void DrawMedicalOverlayData(const gauge_style_preset_t *style, const power_sample_t *sample, bool ai_enabled)
{
    char line[40];
    uint16_t rpm_ma;
    uint32_t now_ds;
    uint32_t rate_step;
    uint32_t pulse_interval_ds;
    uint32_t pulse_width_ds;
    uint32_t dose_rate_mlh_x100;
    uint32_t fill_pct_u32;
    float target_ml_h;
    float running_ml_h;
    float pulse_h;
    float pulse_ml;
    uint16_t okay = style->palette.text_primary;
    uint16_t warn = WARN_YELLOW;
    uint16_t sev = (sample->anomaly_score_pct >= 65u) ? ALERT_RED :
                   ((sample->anomaly_score_pct >= 35u) ? warn : okay);
    bool pumping = false;
    uint16_t pump_color = WARN_YELLOW;
    (void)ai_enabled;

    /* Motor area (top-left icon). */
    BlitPumpBgRegion(20, 18, 142, 102);
    now_ds = UiNowDs();
    if (!gUiRpmSchedulePrimed)
    {
        gUiMotorRunning = false;
        gUiRpmTenths = 0u;
        gUiDoseRateUhMilli = 4000u; /* 4.000 U/h baseline (~0.040 mL/h). */
        gUiDoseRateNextUpdateDs = now_ds + 12000u + (NextUiRand() % 12001u); /* retune every 20..40 min */
        gUiMotorPulseNextDs = now_ds + 120u; /* first basal pulse in ~12s */
        gUiMotorPulseEndDs = now_ds;
        gUiRpmSchedulePrimed = true;
    }

    /* Basal profile update (still in 0.025 U/h increments). Keep centered near
     * 4 U/h so a 3 mL cartridge trends toward ~3-day runtime. */
    if ((int32_t)(now_ds - gUiDoseRateNextUpdateDs) >= 0)
    {
        rate_step = NextUiRand() % 81u;          /* 0..80 */
        gUiDoseRateUhMilli = 3000u + (rate_step * 25u); /* 3.000 .. 5.000 U/h */
        gUiDoseRateNextUpdateDs = now_ds + 12000u + (NextUiRand() % 12001u); /* 20..40 min */
    }

    /* Motor behavior: brief micro-dosing pulses instead of long run windows.
     * Each pulse delivers a fixed micro-bolus volume, and pulse interval is
     * computed from current U/h so total delivery matches configured dose. */
    if (gUiReservoirMl > 0.0f)
    {
        if (gUiMotorRunning && ((int32_t)(now_ds - gUiMotorPulseEndDs) >= 0))
        {
            gUiMotorRunning = false;
            gUiRpmTenths = 0u;
        }

        if ((!gUiMotorRunning) && ((int32_t)(now_ds - gUiMotorPulseNextDs) >= 0))
        {
            float interval_ds_f;

            if (gUiDoseRateUhMilli < 25u)
            {
                gUiDoseRateUhMilli = 25u;
            }

            pulse_width_ds = 8u + (NextUiRand() % 5u); /* 0.8 .. 1.2 s motor movement */
            gUiRpmTenths = (uint16_t)(120u + (NextUiRand() % 141u)); /* 12.0 .. 26.0 RPM pulse */
            gUiMotorRunning = true;
            gUiMotorPulseEndDs = now_ds + pulse_width_ds;
            /* Dosing tied to motor behavior:
             * - running mL/h scales with RPM (20 RPM => 0.04 mL/h on U100 baseline profile)
             * - pulse interval computed from target U/h (0.025 U/h increments) */
            running_ml_h = ((float)gUiRpmTenths / 10.0f) * 0.002f;
            pulse_h = ((float)pulse_width_ds) / 36000.0f;
            pulse_ml = running_ml_h * pulse_h;
            if (pulse_ml < 0.00001f)
            {
                pulse_ml = 0.00001f;
            }
            target_ml_h = ((float)gUiDoseRateUhMilli) / 100000.0f; /* U100: 100 U/mL */
            if (target_ml_h < 0.00025f)
            {
                target_ml_h = 0.00025f;
            }
            interval_ds_f = (pulse_ml / target_ml_h) * 36000.0f;
            pulse_interval_ds = (uint32_t)(interval_ds_f + 0.5f);
            if (pulse_interval_ds < 30u)
            {
                pulse_interval_ds = 30u;
            }
            if (pulse_interval_ds > 36000u)
            {
                pulse_interval_ds = 36000u;
            }
            gUiMotorPulseNextDs = now_ds + pulse_interval_ds;

            gUiReservoirMl -= pulse_ml;
            if (gUiReservoirMl < 0.0f)
            {
                gUiReservoirMl = 0.0f;
            }
        }
    }
    else
    {
        gUiMotorRunning = false;
        gUiRpmTenths = 0u;
    }

    /* Reservoir drain model:
     * - starts at 92% of a 3.0 mL cartridge
     * - drains directly from delivered micro-bolus pulses. */
    if (!gUiReservoirPrimed)
    {
        gUiReservoirMl = 3.0f * 0.92f;
        gUiReservoirPct = 92u;
        gUiReservoirPrimed = true;
    }
    fill_pct_u32 = (uint32_t)((gUiReservoirMl * 100.0f / 3.0f) + 0.5f);
    if (fill_pct_u32 > 100u)
    {
        fill_pct_u32 = 100u;
    }
    gUiReservoirPct = (uint8_t)fill_pct_u32;

    pumping = gUiMotorRunning;
    running_ml_h = gUiMotorRunning ? (((float)gUiRpmTenths / 10.0f) * 0.002f) : 0.0f;
    snprintf(line, sizeof(line), "RPM:%2u.%1u",
             (unsigned int)(gUiRpmTenths / 10u),
             (unsigned int)(gUiRpmTenths % 10u));
    DrawTextUi(22, 24, 2, line, okay);
    rpm_ma = (uint16_t)((((uint32_t)gUiRpmTenths * 95u) + 245u) / 490u); /* map 0.0..49.0 RPM to 0..95mA */
    snprintf(line, sizeof(line), "%2umA", (unsigned int)rpm_ma);
    DrawTextUi(34, 67, 2, line, sev);
    /* Clear band beneath ANOM so no stale/stray text remains. */
    BlitPumpBgRegion(8, 118, 136, 156);
    snprintf(line, sizeof(line), "ANOM:%3u%%", (unsigned int)sample->anomaly_score_pct);
    DrawTextUi(10, 123, 2, line, sev);
    snprintf(line, sizeof(line), "WEAR:%3u%%", (unsigned int)sample->connector_wear_pct);
    DrawTextUi(10, 143, 2, line, sev);

    /* Pump area (bottom-left icon). */
    /* Keep pump text clean but avoid the elapsed-time band in the center segment. */
    BlitPumpBgRegion(30, 240, 198, 281);
    BlitPumpBgRegion(30, 282, 169, 300);
    snprintf(line, sizeof(line), "PUMP %s", pumping ? "ACTIVE" : "IDLE");
    DrawTextUi(34, 246, 2, line, pump_color);
    dose_rate_mlh_x100 = (uint32_t)((running_ml_h * 100.0f) + 0.5f); /* mL/h * 100 */
    snprintf(line, sizeof(line), "RATE:%u.%02u ML/H",
             (unsigned int)(dose_rate_mlh_x100 / 100u),
             (unsigned int)(dose_rate_mlh_x100 % 100u));
    DrawTextUi(34, 262, 2, line, pump_color);
    snprintf(line, sizeof(line), "FILL:%3u%%", (unsigned int)gUiReservoirPct);
    DrawTextUi(34, 278, 2, line, pump_color);

    /* Human status text is no longer drawn here; keep center area untouched. */
}

static void DrawHumanOrientationPointer(const gauge_style_preset_t *style)
{
    int32_t cx = MAIN_CX + 2;
    int32_t cy = MAIN_CY - 22;
    int32_t r = MAIN_R - 6;
    int32_t tach_r = (r * 97) / 100; /* +10% from previous 88% sizing => ~97% radius. */
    int32_t ring_x0 = cx - r - 8;
    int32_t ring_y0 = cy - r - 8;
    int32_t ring_x1 = cx + r + 8;
    int32_t ring_y1 = cy + r + 8;
    int32_t ball_x = cx;
    int32_t ball_y = cy;
    int32_t i;
    int32_t lit_segments;
    int32_t total_segments = 54; /* 270 degrees / 5-degree bars. */
    uint16_t ball_color = RGB565(46, 102, 190); /* medium dark blue when right-side up */
    (void)style;

    if (gAccelValid)
    {
        float fx;
        float fy;
        float maxr = (float)(r - 8);
        float mag;

        if (!gUiBallFiltPrimed)
        {
            gUiBallXmgFilt = (float)gAccelXmg;
            gUiBallYmgFilt = (float)gAccelYmg;
            gUiBallFiltPrimed = true;
        }
        else
        {
            /* UI-only accel projection filter for stable but responsive motion. */
            gUiBallXmgFilt = (gUiBallXmgFilt * 0.70f) + ((float)gAccelXmg * 0.30f);
            gUiBallYmgFilt = (gUiBallYmgFilt * 0.70f) + ((float)gAccelYmg * 0.30f);
        }

        fx = (-gUiBallXmgFilt / 1000.0f) * maxr;
        fy = (-gUiBallYmgFilt / 1000.0f) * maxr;
        mag = sqrtf((fx * fx) + (fy * fy));
        if ((mag > maxr) && (mag > 0.001f))
        {
            fx = fx * (maxr / mag);
            fy = fy * (maxr / mag);
        }
        ball_x = cx + (int32_t)fx;
        ball_y = cy + (int32_t)fy;
    }
    if (gAccelValid && (gAccelZmg < -200))
    {
        ball_color = RGB565(255, 186, 104); /* light orange when upside down */
    }

    /* Restore the full human-circle area before redrawing gauge to prevent pointer trails. */
    BlitPumpBgRegion(ring_x0, ring_y0, ring_x1, ring_y1);

    /* 270-degree tach arc (vertically flipped) with top 90 degrees open.
     * Sweep: 135 -> 45 degrees (CCW), 5-degree segments:
     * - first 120 deg (24 seg) green
     * - next 90 deg (18 seg) yellow
     * - final 60 deg (12 seg) red */
    lit_segments = (int32_t)(((uint32_t)gActivityPct * (uint32_t)total_segments + 50u) / 100u);
    if (lit_segments < 0)
    {
        lit_segments = 0;
    }
    if (lit_segments > total_segments)
    {
        lit_segments = total_segments;
    }
    for (i = 0; i < total_segments; i++)
    {
        float deg = 135.0f + (float)(i * 5);
        float rad = deg * 0.01745329252f;
        int32_t x0 = cx + (int32_t)lroundf(cosf(rad) * (float)(tach_r - 8));
        int32_t y0 = cy + (int32_t)lroundf(sinf(rad) * (float)(tach_r - 8));
        int32_t x1 = cx + (int32_t)lroundf(cosf(rad) * (float)tach_r);
        int32_t y1 = cy + (int32_t)lroundf(sinf(rad) * (float)tach_r);
        uint16_t on_color;
        uint16_t off_color;
        uint16_t draw_color;

        if (i < 24)
        {
            on_color = RGB565(70, 220, 110);  /* green zone */
            off_color = RGB565(20, 56, 30);
        }
        else if (i < 42)
        {
            on_color = RGB565(255, 214, 72);  /* yellow zone */
            off_color = RGB565(72, 56, 18);
        }
        else
        {
            on_color = RGB565(255, 84, 84);   /* red zone */
            off_color = RGB565(72, 20, 20);
        }
        draw_color = (i < lit_segments) ? on_color : off_color;
        DrawLine(x0, y0, x1, y1, 3, draw_color);
    }

    /* ~14x14 filled circle (line-filled) for the rolling marker (2x larger). */
    DrawLine(ball_x - 2, ball_y - 6, ball_x + 2, ball_y - 6, 1, ball_color);
    DrawLine(ball_x - 4, ball_y - 5, ball_x + 4, ball_y - 5, 1, ball_color);
    DrawLine(ball_x - 5, ball_y - 4, ball_x + 5, ball_y - 4, 1, ball_color);
    DrawLine(ball_x - 6, ball_y - 3, ball_x + 6, ball_y - 3, 1, ball_color);
    DrawLine(ball_x - 6, ball_y - 2, ball_x + 6, ball_y - 2, 1, ball_color);
    DrawLine(ball_x - 6, ball_y - 1, ball_x + 6, ball_y - 1, 1, ball_color);
    DrawLine(ball_x - 6, ball_y, ball_x + 6, ball_y, 1, ball_color);
    DrawLine(ball_x - 6, ball_y + 1, ball_x + 6, ball_y + 1, 1, ball_color);
    DrawLine(ball_x - 6, ball_y + 2, ball_x + 6, ball_y + 2, 1, ball_color);
    DrawLine(ball_x - 6, ball_y + 3, ball_x + 6, ball_y + 3, 1, ball_color);
    DrawLine(ball_x - 5, ball_y + 4, ball_x + 5, ball_y + 4, 1, ball_color);
    DrawLine(ball_x - 4, ball_y + 5, ball_x + 4, ball_y + 5, 1, ball_color);
    DrawLine(ball_x - 2, ball_y + 6, ball_x + 2, ball_y + 6, 1, ball_color);
    DrawLine(ball_x - 5, ball_y - 2, ball_x - 5, ball_y + 2, 1, RGB565(220, 255, 230));

}

static void DrawRecordConfirmOverlay(void)
{
    par_lcd_s035_fill_rect(REC_CONFIRM_X0 - 3, REC_CONFIRM_Y0 - 3, REC_CONFIRM_X1 + 3, REC_CONFIRM_Y1 + 3, RGB565(0, 0, 0));
    par_lcd_s035_fill_rect(REC_CONFIRM_X0, REC_CONFIRM_Y0, REC_CONFIRM_X1, REC_CONFIRM_Y1, RGB565(6, 8, 12));
    DrawLine(REC_CONFIRM_X0, REC_CONFIRM_Y0, REC_CONFIRM_X1, REC_CONFIRM_Y0, 2, RGB565(255, 120, 120));
    DrawLine(REC_CONFIRM_X0, REC_CONFIRM_Y1, REC_CONFIRM_X1, REC_CONFIRM_Y1, 2, RGB565(255, 120, 120));
    DrawLine(REC_CONFIRM_X0, REC_CONFIRM_Y0, REC_CONFIRM_X0, REC_CONFIRM_Y1, 2, RGB565(255, 120, 120));
    DrawLine(REC_CONFIRM_X1, REC_CONFIRM_Y0, REC_CONFIRM_X1, REC_CONFIRM_Y1, 2, RGB565(255, 120, 120));
    if (gRecordConfirmAction == 3u)
    {
        DrawTextUi(REC_CONFIRM_X0 + 50, REC_CONFIRM_Y0 + 16, 1, "CLEAR FLASH MEMORY?", RGB565(255, 232, 232));
        DrawTextUi(REC_CONFIRM_X0 + 52, REC_CONFIRM_Y0 + 34, 1, "THIS ERASES ALL LOG DATA", RGB565(255, 190, 190));
    }
    else if (gRecordConfirmAction == 2u)
    {
        DrawTextUi(REC_CONFIRM_X0 + 56, REC_CONFIRM_Y0 + 16, 1, "STOP RECORDING?", RGB565(255, 232, 232));
        DrawTextUi(REC_CONFIRM_X0 + 44, REC_CONFIRM_Y0 + 34, 1, "PLAYBACK WILL BE AVAILABLE", RGB565(255, 190, 190));
    }
    else
    {
        DrawTextUi(REC_CONFIRM_X0 + 18, REC_CONFIRM_Y0 + 16, 1, "START NEW RECORDING?", RGB565(255, 232, 232));
        DrawTextUi(REC_CONFIRM_X0 + 30, REC_CONFIRM_Y0 + 34, 1, "THIS WILL ERASE STORED DATA", RGB565(255, 190, 190));
    }

    par_lcd_s035_fill_rect(REC_CONFIRM_YES_X0, REC_CONFIRM_YES_Y0, REC_CONFIRM_YES_X1, REC_CONFIRM_YES_Y1, RGB565(30, 170, 36));
    par_lcd_s035_fill_rect(REC_CONFIRM_NO_X0, REC_CONFIRM_NO_Y0, REC_CONFIRM_NO_X1, REC_CONFIRM_NO_Y1, RGB565(80, 80, 90));
    DrawTextUi(REC_CONFIRM_YES_X0 + ((REC_CONFIRM_YES_X1 - REC_CONFIRM_YES_X0 + 1 - edgeai_text5x7_width(1, "YES")) / 2),
               REC_CONFIRM_YES_Y0 + 7,
               1,
               "YES",
               RGB565(230, 255, 230));
    DrawTextUi(REC_CONFIRM_NO_X0 + ((REC_CONFIRM_NO_X1 - REC_CONFIRM_NO_X0 + 1 - edgeai_text5x7_width(1, "NO")) / 2),
               REC_CONFIRM_NO_Y0 + 7,
               1,
               "NO",
               RGB565(235, 235, 245));
}

static void DrawAiPill(const gauge_style_preset_t *style, bool ai_enabled)
{
    uint16_t fill = ai_enabled ? RGB565(22, 80, 28) : RGB565(45, 20, 18);
    uint16_t txt = ai_enabled ? RGB565(180, 255, 170) : RGB565(255, 210, 180);
    int32_t x0 = AI_SET_X1 + 1;
    int32_t x1 = AI_HELP_X0 - 1;
    const char *label = ai_enabled ? "AI ON" : "AI OFF";
    int32_t scale = 2;
    int32_t tw = edgeai_text5x7_width(scale, label);
    int32_t th = 7 * scale;
    int32_t lx;
    int32_t ly;

    (void)style;
    if (tw > (x1 - x0 - 4))
    {
        scale = 1;
        tw = edgeai_text5x7_width(scale, label);
        th = 7 * scale;
    }
    lx = x0 + ((x1 - x0 + 1) - tw) / 2;
    ly = AI_PILL_Y0 + (((AI_PILL_Y1 - AI_PILL_Y0 + 1) - th) / 2);
    par_lcd_s035_fill_rect(x0, AI_PILL_Y0, x1, AI_PILL_Y1, fill);
    DrawTextUiCrisp(lx, ly, scale, label, txt);
}

static void DrawPillRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t fill, uint16_t edge)
{
    int32_t mid = (y0 + y1) / 2;
    int32_t r = (y1 - y0 + 1) / 2;
    int32_t xl = x0 + r;
    int32_t xr = x1 - r;
    if (xl > xr)
    {
        xl = xr = (x0 + x1) / 2;
    }

    par_lcd_s035_fill_rect(xl, y0, xr, y1, fill);
    par_lcd_s035_draw_filled_circle(xl, mid, r, fill);
    par_lcd_s035_draw_filled_circle(xr, mid, r, fill);
    DrawLine(x0, y0, x1, y0, 1, edge);
    DrawLine(x0, y1, x1, y1, 1, edge);
}

static void DrawAiSideButtons(void)
{
    int32_t set_cx = (AI_SET_X0 + AI_SET_X1) / 2;
    int32_t set_cy = (AI_SET_Y0 + AI_SET_Y1) / 2;
    int32_t help_cx = (AI_HELP_X0 + AI_HELP_X1) / 2;
    int32_t help_cy = (AI_HELP_Y0 + AI_HELP_Y1) / 2;
    uint16_t set_fill = gSettingsVisible ? RGB565(72, 70, 14) : RGB565(52, 50, 10);
    uint16_t set_txt = RGB565(255, 245, 150);
    uint16_t help_fill = gHelpVisible ? RGB565(12, 64, 76) : RGB565(10, 44, 52);
    uint16_t help_txt = RGB565(176, 244, 255);
    uint16_t edge = RGB565(210, 214, 220);

    DrawPillRect(AI_SET_X0, AI_SET_Y0, AI_SET_X1, AI_SET_Y1, set_fill, edge);
    DrawTextUiCrisp(set_cx - (edgeai_text5x7_width(2, "*") / 2), set_cy - 7, 2, "*", set_txt);

    DrawPillRect(AI_HELP_X0, AI_HELP_Y0, AI_HELP_X1, AI_HELP_Y1, help_fill, edge);
    DrawTextUiCrisp(help_cx - (edgeai_text5x7_width(2, "?") / 2), help_cy - 7, 2, "?", help_txt);
}

static void DrawPopupCloseButton(int32_t panel_x1, int32_t panel_y0)
{
    int32_t bx1 = panel_x1 - 8;
    int32_t bx0 = bx1 - 22;
    int32_t by0 = panel_y0 + 8;
    int32_t by1 = by0 + 22;
    uint16_t fill = RGB565(150, 24, 24);
    uint16_t edge = RGB565(255, 170, 170);
    uint16_t xcol = RGB565(255, 244, 244);

    DrawPillRect(bx0, by0, bx1, by1, fill, edge);
    DrawLine(bx0 + 4, by0 + 4, bx1 - 4, by1 - 4, 1, xcol);
    DrawLine(bx0 + 4, by1 - 4, bx1 - 4, by0 + 4, 1, xcol);
}

static void DrawAdjustArrowIcon(int32_t x0, int32_t y0, int32_t w, int32_t h, bool up, uint16_t color)
{
    int32_t cx = x0 + (w / 2);
    int32_t cy = y0 + (h / 2);
    int32_t span = (w < h ? w : h) / 3;
    if (span < 4)
    {
        span = 4;
    }
    if (up)
    {
        DrawLine(cx - span, cy + 2, cx, cy - span, 1, color);
        DrawLine(cx + span, cy + 2, cx, cy - span, 1, color);
    }
    else
    {
        DrawLine(cx - span, cy - 2, cx, cy + span, 1, color);
        DrawLine(cx + span, cy - 2, cx, cy + span, 1, color);
    }
}

static void DrawPopupModalBase(void)
{
    /* Dirty-region modal redraw: only repaint active popup region to reduce touch latency/flicker. */
    if (gSettingsVisible)
    {
        par_lcd_s035_fill_rect(GAUGE_RENDER_SET_PANEL_X0 - 4,
                               GAUGE_RENDER_SET_PANEL_Y0 - 4,
                               GAUGE_RENDER_SET_PANEL_X1 + 4,
                               GAUGE_RENDER_SET_PANEL_Y1 + 4,
                               RGB565(6, 8, 12));
    }
    if (gHelpVisible)
    {
        par_lcd_s035_fill_rect(GAUGE_RENDER_HELP_PANEL_X0 - 4,
                               GAUGE_RENDER_HELP_PANEL_Y0 - 4,
                               GAUGE_RENDER_HELP_PANEL_X1 + 4,
                               GAUGE_RENDER_HELP_PANEL_Y1 + 4,
                               RGB565(6, 8, 12));
    }
    if (gLimitsVisible)
    {
        par_lcd_s035_fill_rect(GAUGE_RENDER_LIMIT_PANEL_X0 - 4,
                               GAUGE_RENDER_LIMIT_PANEL_Y0 - 4,
                               GAUGE_RENDER_LIMIT_PANEL_X1 + 4,
                               GAUGE_RENDER_LIMIT_PANEL_Y1 + 4,
                               RGB565(6, 8, 12));
    }
}

static void DrawSettingsPopup(void)
{
    int32_t x0 = GAUGE_RENDER_SET_PANEL_X0;
    int32_t y0 = GAUGE_RENDER_SET_PANEL_Y0;
    int32_t x1 = GAUGE_RENDER_SET_PANEL_X1;
    int32_t y1 = GAUGE_RENDER_SET_PANEL_Y1;
    uint16_t panel = RGB565(18, 19, 22);
    uint16_t edge = RGB565(52, 54, 58);
    uint16_t body = RGB565(214, 215, 217);
    uint16_t dim = RGB565(150, 152, 156);
    uint16_t button_idle = RGB565(26, 27, 31);
    uint16_t button_selected = RGB565(210, 214, 222);
    uint16_t text_selected = RGB565(10, 10, 12);
    int32_t label_col_right = GAUGE_RENDER_SET_MODE_X0 - 12;
    int32_t mode_label_y = GAUGE_RENDER_SET_MODE_Y0 + ((GAUGE_RENDER_SET_MODE_H - 7) / 2);
    int32_t run_label_y = GAUGE_RENDER_SET_RUN_Y0 + ((GAUGE_RENDER_SET_RUN_H - 7) / 2);
    int32_t tune_label_y = GAUGE_RENDER_SET_TUNE_Y0 + ((GAUGE_RENDER_SET_TUNE_H - 7) / 2);
    int32_t ai_label_y = GAUGE_RENDER_SET_AI_Y0 + ((GAUGE_RENDER_SET_AI_H - 7) / 2);
    int32_t lim_label_y = GAUGE_RENDER_SET_LIMIT_BTN_Y0 + ((GAUGE_RENDER_SET_LIMIT_BTN_H - 7) / 2);
    int32_t clear_label_y = GAUGE_RENDER_SET_CLEAR_BTN_Y0 + ((GAUGE_RENDER_SET_CLEAR_BTN_H - 7) / 2);
    int32_t log_label_y = GAUGE_RENDER_SET_LOG_Y0 + ((GAUGE_RENDER_SET_LOG_H - 7) / 2);
    char log_rate_line[16];

    par_lcd_s035_fill_rect(x0 - 3, y0 - 3, x1 + 3, y1 + 3, RGB565(0, 0, 0));
    par_lcd_s035_fill_rect(x0, y0, x1, y1, panel);
    DrawLine(x0, y0, x1, y0, 2, edge);
    DrawLine(x0, y1, x1, y1, 2, edge);
    DrawLine(x0, y0, x0, y1, 2, edge);
    DrawLine(x1, y0, x1, y1, 2, edge);
    DrawTextUi(x0 + 10, y0 + 10, 1, "SETTINGS", body);
    DrawPopupCloseButton(x1, y0);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "MODE"), mode_label_y, 1, "MODE", body);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "RUN"), run_label_y, 1, "RUN", body);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "SENS"), tune_label_y, 1, "SENS", body);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "AI"), ai_label_y, 1, "AI", body);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "LIMITS"), lim_label_y, 1, "LIMITS", body);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "FLASH"), clear_label_y, 1, "FLASH", body);
    DrawTextUi(label_col_right - edgeai_text5x7_width(1, "LOG HZ"), log_label_y, 1, "LOG HZ", body);
    DrawTextUi(x0 + 108, y0 + 10, 1, "TAP X OR OUTSIDE TO CLOSE", dim);

    for (int32_t i = 0; i < 2; i++)
    {
        int32_t bx0 = GAUGE_RENDER_SET_MODE_X0 + i * (GAUGE_RENDER_SET_MODE_W + GAUGE_RENDER_SET_MODE_GAP);
        int32_t by0 = GAUGE_RENDER_SET_MODE_Y0;
        int32_t bx1 = bx0 + GAUGE_RENDER_SET_MODE_W - 1;
        int32_t by1 = by0 + GAUGE_RENDER_SET_MODE_H - 1;
        bool sel = ((uint8_t)i == gAnomMode);
        const char *t = (i == 0) ? "ADAPT" : "TRAINED";
        uint16_t f = sel ? button_selected : button_idle;
        uint16_t tc = sel ? text_selected : body;
        DrawPillRect(bx0, by0, bx1, by1, f, edge);
        DrawTextUi(bx0 + ((GAUGE_RENDER_SET_MODE_W - edgeai_text5x7_width(1, t)) / 2),
                   by0 + ((GAUGE_RENDER_SET_MODE_H - 7) / 2),
                   1,
                   t,
                   tc);
    }

    for (int32_t i = 0; i < 2; i++)
    {
        int32_t bx0 = GAUGE_RENDER_SET_RUN_X0 + i * (GAUGE_RENDER_SET_RUN_W + GAUGE_RENDER_SET_RUN_GAP);
        int32_t by0 = GAUGE_RENDER_SET_RUN_Y0;
        int32_t bx1 = bx0 + GAUGE_RENDER_SET_RUN_W - 1;
        int32_t by1 = by0 + GAUGE_RENDER_SET_RUN_H - 1;
        bool sel = (i == 0) ? !gLiveBannerMode : gLiveBannerMode;
        const char *t = (i == 0) ? "TRAIN" : "LIVE";
        uint16_t f = sel ? button_selected : button_idle;
        uint16_t tc = sel ? text_selected : body;
        DrawPillRect(bx0, by0, bx1, by1, f, edge);
        DrawTextUi(bx0 + ((GAUGE_RENDER_SET_RUN_W - edgeai_text5x7_width(1, t)) / 2),
                   by0 + ((GAUGE_RENDER_SET_RUN_H - 7) / 2),
                   1,
                   t,
                   tc);
    }

    for (int32_t i = 0; i < 3; i++)
    {
        int32_t bx0 = GAUGE_RENDER_SET_TUNE_X0 + i * (GAUGE_RENDER_SET_TUNE_W + GAUGE_RENDER_SET_TUNE_GAP);
        int32_t by0 = GAUGE_RENDER_SET_TUNE_Y0;
        int32_t bx1 = bx0 + GAUGE_RENDER_SET_TUNE_W - 1;
        int32_t by1 = by0 + GAUGE_RENDER_SET_TUNE_H - 1;
        bool sel = ((uint8_t)i == gAnomTune);
        const char *t = (i == 0) ? "LOOSE" : (i == 1) ? "NORM" : "STRICT";
        uint16_t f = sel ? button_selected : button_idle;
        uint16_t tc = sel ? text_selected : body;
        DrawPillRect(bx0, by0, bx1, by1, f, edge);
        DrawTextUi(bx0 + ((GAUGE_RENDER_SET_TUNE_W - edgeai_text5x7_width(1, t)) / 2),
                   by0 + ((GAUGE_RENDER_SET_TUNE_H - 7) / 2),
                   1,
                   t,
                   tc);
    }

    for (int32_t i = 0; i < 2; i++)
    {
        int32_t bx0 = GAUGE_RENDER_SET_AI_X0 + i * (GAUGE_RENDER_SET_AI_W + GAUGE_RENDER_SET_AI_GAP);
        int32_t by0 = GAUGE_RENDER_SET_AI_Y0;
        int32_t bx1 = bx0 + GAUGE_RENDER_SET_AI_W - 1;
        int32_t by1 = by0 + GAUGE_RENDER_SET_AI_H - 1;
        bool sel = (i == 0) ? !gPrevAiEnabled : gPrevAiEnabled;
        const char *t = (i == 0) ? "AI OFF" : "AI ON";
        uint16_t f = sel ? button_selected : button_idle;
        uint16_t tc = sel ? text_selected : body;
        DrawPillRect(bx0, by0, bx1, by1, f, edge);
        DrawTextUi(bx0 + ((GAUGE_RENDER_SET_AI_W - edgeai_text5x7_width(1, t)) / 2),
                   by0 + ((GAUGE_RENDER_SET_AI_H - 7) / 2),
                   1,
                   t,
                   tc);
    }

    {
        int32_t bx0 = GAUGE_RENDER_SET_LIMIT_BTN_X0;
        int32_t by0 = GAUGE_RENDER_SET_LIMIT_BTN_Y0;
        int32_t bx1 = bx0 + GAUGE_RENDER_SET_LIMIT_BTN_W - 1;
        int32_t by1 = by0 + GAUGE_RENDER_SET_LIMIT_BTN_H - 1;
        const char *t = "OPEN LIMITS";
        DrawPillRect(bx0, by0, bx1, by1, button_idle, edge);
        DrawTextUi(bx0 + ((GAUGE_RENDER_SET_LIMIT_BTN_W - edgeai_text5x7_width(1, t)) / 2),
                   by0 + ((GAUGE_RENDER_SET_LIMIT_BTN_H - 7) / 2),
                   1,
                   t,
                   body);
    }

    {
        int32_t bx0 = GAUGE_RENDER_SET_CLEAR_BTN_X0;
        int32_t by0 = GAUGE_RENDER_SET_CLEAR_BTN_Y0;
        int32_t bx1 = bx0 + GAUGE_RENDER_SET_CLEAR_BTN_W - 1;
        int32_t by1 = by0 + GAUGE_RENDER_SET_CLEAR_BTN_H - 1;
        const char *t = "CLEAR FLASH";
        DrawPillRect(bx0, by0, bx1, by1, RGB565(58, 18, 18), edge);
        DrawTextUi(bx0 + ((GAUGE_RENDER_SET_CLEAR_BTN_W - edgeai_text5x7_width(1, t)) / 2),
                   by0 + ((GAUGE_RENDER_SET_CLEAR_BTN_H - 7) / 2),
                   1,
                   t,
                   RGB565(255, 215, 215));
    }

    {
        int32_t by0 = GAUGE_RENDER_SET_LOG_Y0;
        int32_t by1 = by0 + GAUGE_RENDER_SET_LOG_H - 1;
        int32_t dec_x0 = GAUGE_RENDER_SET_LOG_DEC_X0;
        int32_t dec_x1 = dec_x0 + GAUGE_RENDER_SET_LOG_DEC_W - 1;
        int32_t val_x0 = GAUGE_RENDER_SET_LOG_VAL_X0;
        int32_t val_x1 = val_x0 + GAUGE_RENDER_SET_LOG_VAL_W - 1;
        int32_t inc_x0 = GAUGE_RENDER_SET_LOG_INC_X0;
        int32_t inc_x1 = inc_x0 + GAUGE_RENDER_SET_LOG_INC_W - 1;

        snprintf(log_rate_line, sizeof(log_rate_line), "%uHZ", (unsigned int)gLogRateHz);

        DrawPillRect(dec_x0, by0, dec_x1, by1, button_idle, edge);
        DrawAdjustArrowIcon(dec_x0, by0, GAUGE_RENDER_SET_LOG_DEC_W, GAUGE_RENDER_SET_LOG_H, false, body);
        DrawPillRect(val_x0, by0, val_x1, by1, button_selected, edge);
        DrawTextUi(val_x0 + ((GAUGE_RENDER_SET_LOG_VAL_W - edgeai_text5x7_width(1, log_rate_line)) / 2),
                   by0 + ((GAUGE_RENDER_SET_LOG_H - 7) / 2),
                   1,
                   log_rate_line,
                   text_selected);
        DrawPillRect(inc_x0, by0, inc_x1, by1, button_idle, edge);
        DrawAdjustArrowIcon(inc_x0, by0, GAUGE_RENDER_SET_LOG_INC_W, GAUGE_RENDER_SET_LOG_H, true, body);
    }

    DrawTextUi(x0 + 10, y0 + 252, 1, "MODEL:", dim);
    DrawTextUi(x0 + 58, y0 + 252, 1, gModelName, body);
    DrawTextUi(x0 + 10, y0 + 264, 1, "EIL EXT:", dim);
    DrawTextUi(x0 + 58, y0 + 264, 1, gExtensionVersion, dim);
    DrawTextUi(x0 + 110, y0 + 264, 1, "MODEL V:", dim);
    DrawTextUi(x0 + 166, y0 + 264, 1, gModelVersion, dim);
}

void GaugeRender_SetProfileInfo(const char *model_name, const char *model_version, const char *extension_version)
{
    CopyUiTextUpper(gModelName, sizeof(gModelName), model_name);
    CopyUiTextUpper(gModelVersion, sizeof(gModelVersion), model_version);
    CopyUiTextUpper(gExtensionVersion, sizeof(gExtensionVersion), extension_version);
}

void GaugeRender_SetLogRateHz(uint8_t hz)
{
    gLogRateHz = hz;
}

static void DrawLimitsPopup(void)
{
    int32_t x0 = GAUGE_RENDER_LIMIT_PANEL_X0;
    int32_t y0 = GAUGE_RENDER_LIMIT_PANEL_Y0;
    int32_t x1 = GAUGE_RENDER_LIMIT_PANEL_X1;
    int32_t y1 = GAUGE_RENDER_LIMIT_PANEL_Y1;
    uint16_t panel = RGB565(18, 19, 22);
    uint16_t edge = RGB565(52, 54, 58);
    uint16_t body = RGB565(214, 215, 217);
    uint16_t dim = RGB565(150, 152, 156);
    uint16_t button_idle = RGB565(26, 27, 31);
    char value[20];
    const char *labels[5] = {"G WARN", "G FAIL", "TEMP LOW", "TEMP HIGH", "GYRO LIMIT"};

    par_lcd_s035_fill_rect(x0 - 3, y0 - 3, x1 + 3, y1 + 3, RGB565(0, 0, 0));
    par_lcd_s035_fill_rect(x0, y0, x1, y1, panel);
    DrawLine(x0, y0, x1, y0, 2, edge);
    DrawLine(x0, y1, x1, y1, 2, edge);
    DrawLine(x0, y0, x0, y1, 2, edge);
    DrawLine(x1, y0, x1, y1, 2, edge);
    DrawTextUi(x0 + 10, y0 + 8, 2, "LIMITS", body);
    DrawPopupCloseButton(x1, y0);
    DrawTextUi(x0 + 14, y0 + 272, 1, "TAP X OR OUTSIDE TO CLOSE", dim);

    for (int32_t i = 0; i < 5; i++)
    {
        int32_t row_y0 = GAUGE_RENDER_LIMIT_ROW_Y0 + i * (GAUGE_RENDER_LIMIT_ROW_H + GAUGE_RENDER_LIMIT_ROW_GAP);
        int32_t row_y1 = row_y0 + GAUGE_RENDER_LIMIT_ROW_H - 1;
        int32_t minus_x0 = GAUGE_RENDER_LIMIT_MINUS_X0;
        int32_t minus_x1 = minus_x0 + GAUGE_RENDER_LIMIT_MINUS_W - 1;
        int32_t plus_x0 = GAUGE_RENDER_LIMIT_PLUS_X0;
        int32_t plus_x1 = plus_x0 + GAUGE_RENDER_LIMIT_PLUS_W - 1;

        if (i == 0)
        {
            uint16_t whole = (uint16_t)(gLimitGWarnMg / 1000u);
            uint16_t tenths = (uint16_t)((gLimitGWarnMg % 1000u) / 100u);
            snprintf(value, sizeof(value), "%u.%ug", (unsigned int)whole, (unsigned int)tenths);
        }
        else if (i == 1)
        {
            uint16_t whole = (uint16_t)(gLimitGFailMg / 1000u);
            uint16_t tenths = (uint16_t)((gLimitGFailMg % 1000u) / 100u);
            snprintf(value, sizeof(value), "%u.%ug", (unsigned int)whole, (unsigned int)tenths);
        }
        else if (i == 2)
        {
            snprintf(value, sizeof(value), "%dC", (int)(gLimitTempLowC10 / 10));
        }
        else if (i == 3)
        {
            snprintf(value, sizeof(value), "%dC", (int)(gLimitTempHighC10 / 10));
        }
        else
        {
            snprintf(value, sizeof(value), "%udps", (unsigned int)gLimitGyroDps);
        }

        DrawPillRect(GAUGE_RENDER_LIMIT_ROW_X0,
                     row_y0,
                     GAUGE_RENDER_LIMIT_ROW_X0 + GAUGE_RENDER_LIMIT_ROW_W - 1,
                     row_y1,
                     RGB565(20, 21, 24),
                     edge);
        DrawTextUi(GAUGE_RENDER_LIMIT_ROW_X0 + 10,
                   row_y0 + ((GAUGE_RENDER_LIMIT_ROW_H - 7) / 2),
                   1,
                   labels[i],
                   body);

        DrawPillRect(minus_x0, row_y0, minus_x1, row_y1, button_idle, edge);
        DrawPillRect(plus_x0, row_y0, plus_x1, row_y1, button_idle, edge);
        DrawTextUi(minus_x0 + ((GAUGE_RENDER_LIMIT_MINUS_W - edgeai_text5x7_width(1, "DOWN")) / 2),
                   row_y0 + ((GAUGE_RENDER_LIMIT_ROW_H - 7) / 2),
                   1,
                   "DOWN",
                   body);
        DrawTextUi(plus_x0 + ((GAUGE_RENDER_LIMIT_PLUS_W - edgeai_text5x7_width(1, "UP")) / 2),
                   row_y0 + ((GAUGE_RENDER_LIMIT_ROW_H - 7) / 2),
                   1,
                   "UP",
                   body);
        DrawTextUi(226, row_y0 + ((GAUGE_RENDER_LIMIT_ROW_H - 7) / 2), 1, value, RGB565(186, 228, 248));
    }
}

static void DrawHelpPopup(void)
{
    int32_t x0 = GAUGE_RENDER_HELP_PANEL_X0;
    int32_t y0 = GAUGE_RENDER_HELP_PANEL_Y0;
    int32_t x1 = GAUGE_RENDER_HELP_PANEL_X1;
    int32_t y1 = GAUGE_RENDER_HELP_PANEL_Y1;
    uint16_t panel = RGB565(18, 19, 22);
    uint16_t edge = RGB565(52, 54, 58);
    uint16_t body = RGB565(214, 215, 217);
    uint16_t dim = RGB565(150, 152, 156);
    uint16_t btn_idle = RGB565(26, 27, 31);

    par_lcd_s035_fill_rect(x0 - 3, y0 - 3, x1 + 3, y1 + 3, RGB565(0, 0, 0));
    par_lcd_s035_fill_rect(x0, y0, x1, y1, panel);
    DrawLine(x0, y0, x1, y0, 2, edge);
    DrawLine(x0, y1, x1, y1, 2, edge);
    DrawLine(x0, y0, x0, y1, 2, edge);
    DrawLine(x1, y0, x1, y1, 2, edge);
    DrawTextUi(x0 + 10, y0 + 8, 2, "HELP", body);
    DrawTextUi(x0 + 318, y0 + 12, 1, (gHelpPage == 0u) ? "PAGE 1 OF 2" : "PAGE 2 OF 2", dim);
    DrawPopupCloseButton(x1, y0);
    DrawPillRect(GAUGE_RENDER_HELP_NEXT_X0,
                 GAUGE_RENDER_HELP_NEXT_Y0,
                 GAUGE_RENDER_HELP_NEXT_X1,
                 GAUGE_RENDER_HELP_NEXT_Y1,
                 btn_idle,
                 edge);
    DrawTextUi(GAUGE_RENDER_HELP_NEXT_X0 + 18, GAUGE_RENDER_HELP_NEXT_Y0 + 9, 1, "NEXT PAGE", body);
    if (gHelpPage == 0u)
    {
        DrawTextUi(x0 + 12, y0 + 42, 1, "QUICK START", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 56, 1, "STAR OPENS SETTINGS  HELP KEY OPENS NEXT PAGE", body);
        DrawTextUi(x0 + 12, y0 + 70, 1, "MODE ADAPT LEARNS  MODE TRAINED USES STORED BASELINE", body);
        DrawTextUi(x0 + 12, y0 + 84, 1, "RUN MODE TRAIN OR LIVE", body);
        DrawTextUi(x0 + 12, y0 + 98, 1, "OPEN LIMITS TO SET G TEMP AND GYRO THRESHOLDS", body);

        DrawTextUi(x0 + 12, y0 + 118, 1, "MAIN SCREEN CONTROL", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 132, 1, "TOP LEFT PLAY STOP", body);
        DrawTextUi(x0 + 12, y0 + 146, 1, "TOP RIGHT RECORD TRAINING DATA", body);
        DrawTextUi(x0 + 12, y0 + 160, 1, "RECORD AND STOP REQUIRE CONFIRM DIALOG", body);

        DrawTextUi(x0 + 12, y0 + 180, 1, "ALERT MEANING", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 194, 1, "GREEN NORMAL  YELLOW WARNING  RED FAIL", body);
        DrawTextUi(x0 + 12, y0 + 208, 1, "TRAINING OR RECORDING STATES OVERRIDE ALERT TEXT", body);

        DrawTextUi(x0 + 12, y0 + 228, 1, "PERSISTENCE", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 242, 1, "MODE, RUN, AI, SENS, LIMITS SAVE ON CHANGE", body);
        DrawTextUi(x0 + 12, y0 + 256, 1, "SETTINGS RESTORE AUTOMATICALLY AFTER REBOOT", body);
        DrawTextUi(x0 + 12, y0 + 264, 1, "TAP X OR OUTSIDE TO CLOSE", dim);
    }
    else
    {
        DrawTextUi(x0 + 12, y0 + 42, 1, "DEEP DIVE", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 56, 1, "ADAPT MODE BASELINE UPDATES FROM LIVE SIGNALS", body);
        DrawTextUi(x0 + 12, y0 + 70, 1, "TRAINED MODE FREEZES LEARNING USES STORED MODEL", body);
        DrawTextUi(x0 + 12, y0 + 84, 1, "RUN TRAIN ENABLES RECORD AND PLAY WORKFLOW", body);
        DrawTextUi(x0 + 12, y0 + 98, 1, "RUN LIVE USES REAL TIME SENSOR STREAM ONLY", body);

        DrawTextUi(x0 + 12, y0 + 118, 1, "THRESHOLDS", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 132, 1, "G WARN G FAIL SET PACKAGE SHOCK LEVELS", body);
        DrawTextUi(x0 + 12, y0 + 146, 1, "TEMP LOW TEMP HIGH SET OPERATING WINDOW", body);
        DrawTextUi(x0 + 12, y0 + 160, 1, "GYRO LIMIT SETS ROTATION RATE LIMIT", body);

        DrawTextUi(x0 + 12, y0 + 180, 1, "INTEGRATION NOTES", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 194, 1, "HOST FIRMWARE KEEPS PRIMARY CONTROL LOGIC", body);
        DrawTextUi(x0 + 12, y0 + 208, 1, "AI LAYER ADDS WATCHDOG AND PREDICTIVE SIGNALS", body);
        DrawTextUi(x0 + 12, y0 + 222, 1, "USE ALERT AND REASON OUTPUTS TO GATE ACTIONS", body);

        DrawTextUi(x0 + 12, y0 + 242, 1, "UI TIP", RGB565(210, 234, 255));
        DrawTextUi(x0 + 12, y0 + 256, 1, "TAP HELP KEY AGAIN TO RETURN TO PAGE 1", body);
        DrawTextUi(x0 + 12, y0 + 264, 1, "TAP X OR OUTSIDE TO CLOSE", dim);
    }
}

static void DrawTerminalFrame(const gauge_style_preset_t *style)
{
    int32_t y;
    for (y = TERM_Y; y <= (TERM_Y + TERM_H); y++)
    {
        par_lcd_s035_blit_rect(TERM_X, y, TERM_X + TERM_W, y,
                               (uint16_t *)&g_pump_bg_rgb565[(y * PUMP_BG_WIDTH) + TERM_X]);
    }
    DrawTextUi(TERM_X + 6, TERM_Y + 6, 1, "STATUS", style->palette.text_primary);
    DrawLine(TERM_X + 6, TERM_Y + 16, TERM_X + TERM_W - 6, TERM_Y + 16, 1, RGB565(110, 20, 30));
}

static uint16_t AiStatusColor(const gauge_style_preset_t *style, uint8_t ai_status)
{
    (void)ai_status;
    return ActivityColor(style, gActivityStage);
}

static const char *AiStatusText(uint8_t ai_status)
{
    (void)ai_status;
    return ActivityStageText(gActivityStage);
}

static bool IsSevereAlertCondition(const power_sample_t *sample)
{
    (void)sample;
    return gActivityStage >= ACTIVITY_HEAVY;
}

static void BuildAnomalyReason(const power_sample_t *sample, char *out, size_t out_len)
{
    const char *baro_state = "FLAT";
    const char *motion_state = ActivityStageText(gActivityStage);
    const char *transport_state = TransportModeText(gTransportMode);
    (void)sample;

    if (gBaroValid)
    {
        if (gActivityBaroRateHpaS >= 0.03f)
        {
            baro_state = "DOWN";
        }
        else if (gActivityBaroRateHpaS <= -0.03f)
        {
            baro_state = "UP";
        }
    }
    snprintf(out, out_len, "%s %s %s %s", ActivityCodeText(gActivityStage), motion_state, transport_state, baro_state);
}


static void DrawAiAlertOverlay(const gauge_style_preset_t *style, const power_sample_t *sample, bool ai_enabled)
{
    uint16_t color;
    int32_t tx;
    bool severe;
    bool recording = !gScopePaused;
    bool training_mode = (!gLiveBannerMode && (gAnomMode == 1u) && gScopePaused);
    const char *normal_label = "SYSTEM REST";
    uint8_t status = AI_STATUS_NORMAL;
    char detail[30];

    if (gSettingsVisible || gHelpVisible || gLimitsVisible || gRecordConfirmActive)
    {
        gAlertVisualValid = false;
        par_lcd_s035_fill_rect(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y1, RGB565(2, 3, 5));
        return;
    }

    severe = IsSevereAlertCondition(sample);
    if (gActivityStage >= ACTIVITY_HEAVY)
    {
        status = AI_STATUS_FAULT;
    }
    else if (gActivityStage >= ACTIVITY_ACTIVE)
    {
        status = AI_STATUS_WARNING;
    }
    else
    {
        status = AI_STATUS_NORMAL;
    }
    BuildAnomalyReason(sample, detail, sizeof(detail));

    if (recording)
    {
        if (gAlertVisualValid &&
            gAlertVisualRecording &&
            (strcmp(gAlertVisualDetail, "RECORDING") == 0))
        {
            return;
        }

        par_lcd_s035_fill_rect(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y1, RGB565(2, 3, 5));
        tx = ALERT_X0 + ((ALERT_X1 - ALERT_X0 + 1) - edgeai_text5x7_width(3, "RECORDING")) / 2;
        DrawTextUi(tx, ALERT_Y0 + 10, 3, "RECORDING", ALERT_RED);
        gAlertVisualValid = true;
        gAlertVisualStatus = status;
        gAlertVisualSevere = false;
        gAlertVisualAiEnabled = ai_enabled;
        gAlertVisualRecording = true;
        snprintf(gAlertVisualDetail, sizeof(gAlertVisualDetail), "%s", "RECORDING");
        return;
    }

    if (training_mode)
    {
        if (gAlertVisualValid &&
            !gAlertVisualRecording &&
            (strcmp(gAlertVisualDetail, "TRAINING") == 0))
        {
            return;
        }

        par_lcd_s035_fill_rect(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y1, RGB565(2, 3, 5));
        tx = ALERT_X0 + ((ALERT_X1 - ALERT_X0 + 1) - edgeai_text5x7_width(3, "TRAINING")) / 2;
        DrawTextUi(tx, ALERT_Y0 + 10, 3, "TRAINING", WARN_YELLOW);
        gAlertVisualValid = true;
        gAlertVisualStatus = status;
        gAlertVisualSevere = false;
        gAlertVisualAiEnabled = ai_enabled;
        gAlertVisualRecording = false;
        snprintf(gAlertVisualDetail, sizeof(gAlertVisualDetail), "%s", "TRAINING");
        return;
    }

    if (gAlertVisualValid &&
        (gAlertVisualStatus == status) &&
        (gAlertVisualSevere == severe) &&
        (gAlertVisualAiEnabled == ai_enabled) &&
        (gAlertVisualRecording == recording) &&
        (strcmp(gAlertVisualDetail, detail) == 0))
    {
        return;
    }

    par_lcd_s035_fill_rect(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y1, RGB565(2, 3, 5));
    if (status == AI_STATUS_NORMAL)
    {
        color = ActivityColor(style, gActivityStage);
        par_lcd_s035_fill_rect(ALERT_X0 + 1, ALERT_Y0 + 1, ALERT_X1 - 1, ALERT_Y1 - 1, RGB565(6, 16, 10));
        DrawLine(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y0, 2, color);
        DrawLine(ALERT_X0, ALERT_Y1, ALERT_X1, ALERT_Y1, 2, color);
        DrawLine(ALERT_X0, ALERT_Y0, ALERT_X0, ALERT_Y1, 2, color);
        DrawLine(ALERT_X1, ALERT_Y0, ALERT_X1, ALERT_Y1, 2, color);
        tx = ALERT_X0 + ((ALERT_X1 - ALERT_X0 + 1) - edgeai_text5x7_width(2, normal_label)) / 2;
        DrawTextUi(tx, ALERT_Y0 + 12, 2, normal_label, RGB565(220, 255, 220));
    }
    else
    {
        color = severe ? ALERT_RED : AiStatusColor(style, status);
        par_lcd_s035_fill_rect(ALERT_X0 + 1, ALERT_Y0 + 1, ALERT_X1 - 1, ALERT_Y1 - 1, RGB565(18, 3, 7));
        DrawLine(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y0, 2, color);
        DrawLine(ALERT_X0, ALERT_Y1, ALERT_X1, ALERT_Y1, 2, color);
        DrawLine(ALERT_X0, ALERT_Y0, ALERT_X0, ALERT_Y1, 2, color);
        DrawLine(ALERT_X1, ALERT_Y0, ALERT_X1, ALERT_Y1, 2, color);
        tx = ALERT_X0 + ((ALERT_X1 - ALERT_X0 + 1) - edgeai_text5x7_width(2, AiStatusText(status))) / 2;
        DrawTextUi(tx, ALERT_Y0 + 8, 2, AiStatusText(status), color);
        tx = ALERT_X0 + ((ALERT_X1 - ALERT_X0 + 1) - edgeai_text5x7_width(1, detail)) / 2;
        DrawTextUi(tx, ALERT_Y0 + 28, 1, detail, style->palette.text_primary);
    }

    gAlertVisualValid = true;
    gAlertVisualStatus = status;
    gAlertVisualSevere = severe;
    gAlertVisualAiEnabled = ai_enabled;
    gAlertVisualRecording = recording;
    snprintf(gAlertVisualDetail, sizeof(gAlertVisualDetail), "%s", detail);
}

static void DrawTerminalDynamic(const gauge_style_preset_t *style, const power_sample_t *sample, uint16_t cpu_pct, bool ai_enabled)
{
    char line[48];
    int16_t gx10 = gGyroValid ? gGyroXdps : 0;
    int16_t gy10 = gGyroValid ? gGyroYdps : 0;
    int16_t gz10 = gGyroValid ? gGyroZdps : 0;
    int16_t gx_abs = (gx10 < 0) ? (int16_t)-gx10 : gx10;
    int16_t gy_abs = (gy10 < 0) ? (int16_t)-gy10 : gy10;
    int16_t gz_abs = (gz10 < 0) ? (int16_t)-gz10 : gz10;
    const char *mode_text = gLiveBannerMode ? "LIVE" : ((gAnomMode == 1u && gScopePaused) ? "TRAIN" : (gScopePaused ? "PLAY" : "REC"));
    uint8_t status = AI_STATUS_NORMAL;
    uint16_t ai_color = AiStatusColor(style, status);
    const char *status_text = ai_enabled ? AiStatusText(status) : "OFF";
    const char *sys_text = ai_enabled ? ActivityCodeText(gActivityStage) : "OFF";
    const char *anom_text = AnomModeText(gAnomMode, gAnomTraining);
    uint16_t anom_color = AnomLevelColor(gAnomOverall);
    const char *transport_text = TransportModeText(gTransportMode);
    (void)sample;

    if (gActivityStage >= ACTIVITY_HEAVY)
    {
        status = AI_STATUS_FAULT;
    }
    else if (gActivityStage >= ACTIVITY_ACTIVE)
    {
        status = AI_STATUS_WARNING;
    }
    ai_color = AiStatusColor(style, status);
    status_text = ai_enabled ? AiStatusText(status) : "OFF";

    /* Refresh header band from background image to keep terminal visually transparent. */
    {
        int32_t y;
        for (y = TERM_Y + 3; y <= (TERM_Y + 16); y++)
        {
            par_lcd_s035_blit_rect(TERM_X + 3, y, TERM_X + TERM_W - 3, y,
                                   (uint16_t *)&g_pump_bg_rgb565[(y * PUMP_BG_WIDTH) + TERM_X + 3]);
        }
    }
    DrawTextUi(TERM_X + 6, TERM_Y + 6, 1, "STATUS", style->palette.text_primary);
    DrawLine(TERM_X + 6, TERM_Y + 16, TERM_X + TERM_W - 6, TERM_Y + 16, 1, RGB565(110, 20, 30));

    (void)cpu_pct;

    snprintf(line, sizeof(line), "AI %s", status_text);
    DrawTerminalLine(TERM_Y + 26, line, ai_color);

    snprintf(line, sizeof(line), "MODE %s SYS %s", mode_text, sys_text);
    DrawTerminalLine(TERM_Y + 42, line, style->palette.text_secondary);

    FormatTempCF(line, sizeof(line), DisplayTempC10(sample));
    DrawTerminalLine(TERM_Y + 58, line, style->palette.text_primary);

    snprintf(line, sizeof(line),
             "GYR X%c%4d.%1d Y%c%4d.%1d Z%c%4d.%1d",
             (gx10 < 0) ? '-' : '+',
             (int)(gx_abs / 10),
             (int)(gx_abs % 10),
             (gy10 < 0) ? '-' : '+',
             (int)(gy_abs / 10),
             (int)(gy_abs % 10),
             (gz10 < 0) ? '-' : '+',
             (int)(gz_abs / 10),
             (int)(gz_abs % 10));
    DrawTerminalLine(TERM_Y + 74, line, RGB565(170, 240, 255));

    if (gLinAccelValid)
    {
        FormatAccelGCompact(line, sizeof(line), gLinAccelXmg, gLinAccelYmg, gLinAccelZmg);
    }
    else
    {
        snprintf(line, sizeof(line), "ACC +0.000 +0.000 +1.000g");
    }
    DrawTerminalLine(TERM_Y + 90, line, RGB565(170, 240, 255));
    FormatShieldEnvCompact(line, sizeof(line));
    DrawTerminalLine(TERM_Y + 106, line, RGB565(176, 218, 238));

    FormatDewAltCompact(line, sizeof(line));
    DrawTerminalLine(TERM_Y + 122, line, RGB565(176, 218, 238));

    snprintf(line, sizeof(line), "TRN %s %2u ACT %3u %s", transport_text,
             (unsigned int)gTransportConfidencePct,
             (unsigned int)gActivityPct,
             anom_text);
    DrawTerminalLine(TERM_Y + 138, line, (gActivityStage >= ACTIVITY_ACTIVE) ? ActivityColor(style, gActivityStage) : anom_color);

    if (gHelpVisible)
    {
        DrawTerminalLine(TERM_Y + 154, "*:SET ?:HELP", RGB565(180, 220, 248));
    }
    else
    {
        DrawTerminalLine(TERM_Y + 154, "              ", style->palette.text_secondary);
    }
}

static void DrawBatteryIndicatorFrame(const gauge_style_preset_t *style)
{
    int32_t nub_w = 11;
    int32_t nub_h = 10;
    int32_t text_x = BATT_X - edgeai_text5x7_width(1, "BATT") - 2;
    int32_t text_y = BATT_Y + ((BATT_H - 7) / 2);
    int32_t nub_x0 = BATT_X + BATT_W - 1;
    int32_t nub_y0 = BATT_Y + ((BATT_H - nub_h) / 2);
    int32_t nub_x1 = nub_x0 + nub_w;
    par_lcd_s035_fill_rect(BATT_X, BATT_Y, BATT_X + BATT_W, BATT_Y + BATT_H, RGB565(245, 245, 245));
    par_lcd_s035_fill_rect(BATT_X + 1, BATT_Y + 1, BATT_X + BATT_W - 1, BATT_Y + BATT_H - 1, RGB565(72, 76, 84));
    /* Keep terminal attached and thick enough to read as a battery nub on LCD. */
    par_lcd_s035_fill_rect(nub_x0, nub_y0, nub_x1, nub_y0 + nub_h, RGB565(245, 245, 245));
    DrawTextUi(text_x, text_y, 1, "BATT", style->palette.text_secondary);
}

static void DrawBatteryIndicatorDynamic(const gauge_style_preset_t *style, uint8_t soc)
{
    int32_t inner_x0 = BATT_X + 3;
    int32_t inner_y0 = BATT_Y + 3;
    int32_t inner_w = BATT_W - 6;
    int32_t inner_h = BATT_H - 6;
    int32_t fill = (soc * inner_w) / 100;
    int32_t text_x_lbl = BATT_X - edgeai_text5x7_width(1, "BATT") - 2;
    int32_t text_y_lbl = BATT_Y + ((BATT_H - 7) / 2);
    char line[8];
    int32_t text_x;
    uint16_t fill_color = style->palette.accent_green;

    if (soc < 25u)
    {
        fill_color = style->palette.accent_red;
    }
    else if (soc < 50u)
    {
        fill_color = RGB565(255, 180, 24);
    }

    DrawBatteryIndicatorFrame(style);
    par_lcd_s035_fill_rect(inner_x0, inner_y0, inner_x0 + inner_w, inner_y0 + inner_h, RGB565(82, 86, 92));
    if (fill > 0)
    {
        par_lcd_s035_fill_rect(inner_x0, inner_y0, inner_x0 + fill, inner_y0 + inner_h, fill_color);
    }

    snprintf(line, sizeof(line), "%3u%%", soc);
    text_x = BATT_X + ((BATT_W - edgeai_text5x7_width(1, line)) / 2);
    DrawTextUi(text_x, inner_y0 + ((inner_h - 7) / 2), 1, line, style->palette.text_primary);
    DrawTextUi(text_x_lbl, text_y_lbl, 1, "BATT", style->palette.text_secondary);
}

static void DrawScopeDynamic(const gauge_style_preset_t *style, bool ai_enabled)
{
    int32_t px0 = SCOPE_X + 6;
    int32_t py0 = SCOPE_Y + 18;
    int32_t pw = SCOPE_W - 12;
    int32_t ph = SCOPE_H - 24;
    int32_t y_bottom = py0 + ph - 1;
    uint16_t axis_color = RGB565(120, 120, 128);
    uint16_t n = (gTraceCount < SCOPE_TRACE_POINTS) ? gTraceCount : SCOPE_TRACE_POINTS;
    uint16_t max_start = (gTraceCount > SCOPE_TRACE_POINTS) ? (gTraceCount - SCOPE_TRACE_POINTS) : 0u;
    uint16_t start = max_start;
    int32_t prev_x = 0;
    int32_t prev_ax = 0;
    int32_t prev_ay = 0;
    int32_t prev_az = 0;
    int32_t prev_gx = 0;
    int32_t prev_gy = 0;
    int32_t prev_gz = 0;
    int32_t prev_tp = 0;
    int32_t prev_bp = 0;
    uint16_t ax_color = TRACE_AX_COLOR;
    uint16_t ay_color = TRACE_AY_COLOR;
    uint16_t az_color = TRACE_AZ_COLOR;
    uint16_t gx_color = TRACE_GX_COLOR;
    uint16_t gy_color = TRACE_GY_COLOR;
    uint16_t gz_color = TRACE_GZ_COLOR;
    uint16_t i;
    int32_t y_min = py0 + 1;
    int32_t y_max = y_bottom - 1;
    (void)ai_enabled;

    par_lcd_s035_fill_rect(px0, py0, px0 + pw, py0 + ph, RGB565(4, 6, 8));
    DrawLine(px0, py0, px0, py0 + ph - 1, 1, axis_color);
    DrawLine(px0, py0 + ph - 1, px0 + pw - 1, py0 + ph - 1, 1, axis_color);

    if (n == 0u)
    {
        return;
    }

    for (i = 0u; i < n; i++)
    {
        int32_t x = px0 + ((int32_t)i * (pw - 1)) / (int32_t)((n > 1u) ? (n - 1u) : 1u);
        uint16_t idx = (uint16_t)(start + i);
        int32_t y_ax = y_bottom - (int32_t)((gTraceAx[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_ay = y_bottom - (int32_t)((gTraceAy[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_az = y_bottom - (int32_t)((gTraceAz[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_gx = y_bottom - (int32_t)((gTraceGx[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_gy = y_bottom - (int32_t)((gTraceGy[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_gz = y_bottom - (int32_t)((gTraceGz[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_tp = y_bottom - (int32_t)((gTraceTemp[idx] * (uint32_t)(ph - 4)) / 255u);
        int32_t y_bp = y_bottom - (int32_t)((gTraceBaro[idx] * (uint32_t)(ph - 4)) / 255u);
        y_ax = ClampI32(y_ax, y_min, y_max);
        y_ay = ClampI32(y_ay, y_min, y_max);
        y_az = ClampI32(y_az, y_min, y_max);
        y_gx = ClampI32(y_gx, y_min, y_max);
        y_gy = ClampI32(y_gy, y_min, y_max);
        y_gz = ClampI32(y_gz, y_min, y_max);
        y_tp = ClampI32(y_tp, y_min, y_max);
        y_bp = ClampI32(y_bp, y_min, y_max);

        if (i > 0u)
        {
            uint16_t tp_color = TempTraceColorFromScaled(gTraceTemp[idx]);
            DrawLine(prev_x, prev_ax, x, y_ax, 1, ax_color);
            DrawLine(prev_x, prev_ay, x, y_ay, 1, ay_color);
            DrawLine(prev_x, prev_az, x, y_az, 1, az_color);
            DrawLine(prev_x, prev_gx, x, y_gx, 1, gx_color);
            DrawLine(prev_x, prev_gy, x, y_gy, 1, gy_color);
            DrawLine(prev_x, prev_gz, x, y_gz, 1, gz_color);
            DrawLine(prev_x, prev_tp, x, y_tp, 1, tp_color);
            DrawLine(prev_x, prev_bp, x, y_bp, 1, TRACE_BARO_COLOR);
        }

        prev_x = x;
        prev_ax = y_ax;
        prev_ay = y_ay;
        prev_az = y_az;
        prev_gx = y_gx;
        prev_gy = y_gy;
        prev_gz = y_gz;
        prev_tp = y_tp;
        prev_bp = y_bp;
    }

    if (gPlayheadValid)
    {
        int32_t head_x = px0 + ((int32_t)gPlayheadPos * (pw - 1)) / 99;
        uint16_t head_color = gScopePaused ? RGB565(80, 255, 120) : RGB565(255, 88, 88);
        DrawLine(head_x, py0 + 1, head_x, py0 + ph - 2, 1, head_color);
    }

    {
        int32_t rec_x = px0 + pw - 8;
        int32_t rec_y = py0 + 8;
        uint16_t ring;
        uint16_t fill;
        if (gLiveBannerMode)
        {
            ring = RGB565(0, 40, 62);
            fill = RGB565(96, 220, 255);
        }
        else
        {
            ring = gScopePaused ? RGB565(0, 48, 0) : RGB565(70, 0, 0);
            fill = gScopePaused ? RGB565(48, 255, 96) : RGB565(255, 28, 28);
        }
        par_lcd_s035_draw_filled_circle(rec_x, rec_y, 4, ring);
        par_lcd_s035_draw_filled_circle(rec_x, rec_y, 3, fill);
    }

    {
        int32_t ty;
        int32_t lx0 = TIMELINE_X0;
        int32_t mid = (TIMELINE_X0 + TIMELINE_X1) / 2;
        int32_t lx1 = mid - 1;
        int32_t rx0 = mid;
        int32_t rx1 = TIMELINE_X1;
        if (gLiveBannerMode)
        {
            par_lcd_s035_fill_rect(TIMELINE_X0 + 1, TIMELINE_Y0 + 1, TIMELINE_X1 - 1, TIMELINE_Y1 - 1, RGB565(22, 78, 112));
            DrawLine(TIMELINE_X0 + 1, TIMELINE_Y0 + 2, TIMELINE_X1 - 1, TIMELINE_Y0 + 2, 1, RGB565(120, 220, 255));
            DrawLine(TIMELINE_X0 + 1, TIMELINE_Y1 - 2, TIMELINE_X1 - 1, TIMELINE_Y1 - 2, 1, RGB565(70, 170, 220));
            ty = TIMELINE_Y0 + ((TIMELINE_Y1 - TIMELINE_Y0 - 7) / 2);
            DrawTextUi(TIMELINE_X0 + ((TIMELINE_X1 - TIMELINE_X0 + 1 - edgeai_text5x7_width(1, "LIVE")) / 2),
                       ty,
                       1,
                       "LIVE",
                       RGB565(192, 242, 255));
        }
        else
        {
            par_lcd_s035_fill_rect(TIMELINE_X0 + 1, TIMELINE_Y0 + 1, TIMELINE_X1 - 1, TIMELINE_Y1 - 1, RGB565(20, 28, 34));
            par_lcd_s035_fill_rect(lx0 + 1,
                                   TIMELINE_Y0 + 1,
                                   lx1 - 1,
                                   TIMELINE_Y1 - 1,
                                   gScopePaused ? RGB565(20, 180, 36) : RGB565(24, 82, 210));
            par_lcd_s035_fill_rect(rx0 + 1, TIMELINE_Y0 + 1, rx1 - 1, TIMELINE_Y1 - 1, RGB565(220, 24, 24));
            ty = TIMELINE_Y0 + ((TIMELINE_Y1 - TIMELINE_Y0 - 7) / 2);
            DrawTextUi(lx0 + ((lx1 - lx0 + 1 - edgeai_text5x7_width(1, gScopePaused ? "PLAY" : "STOP")) / 2),
                       ty,
                       1,
                       gScopePaused ? "PLAY" : "STOP",
                       gScopePaused ? RGB565(232, 255, 232) : RGB565(210, 236, 255));
            DrawTextUi(rx0 + ((rx1 - rx0 + 1 - edgeai_text5x7_width(1, "REC")) / 2), ty, 1, "REC", RGB565(255, 232, 232));
        }
    }

}

static void DrawLeftBargraphFrame(const gauge_style_preset_t *style)
{
    int32_t i;
    int32_t inner_x0 = BAR_X0 + 2;
    int32_t inner_x1 = BAR_X1 - 2;
    int32_t inner_y0 = BAR_Y0 + 2;
    int32_t inner_y1 = BAR_Y1 - 2;
    int32_t seg_step = (inner_y1 - inner_y0 + 1) / BAR_SEGMENTS;
    int32_t y;
    int32_t label_y = PUMP_BG_HEIGHT - 16;
    int32_t label_scale = 1;
    int32_t label_x1;

    for (y = label_y - 2; y <= BAR_Y1 + 1; y++)
    {
        if ((y >= 0) && (y < PUMP_BG_HEIGHT))
        {
            par_lcd_s035_blit_rect(BAR_X0, y, BAR_X0 + 170, y,
                                   (uint16_t *)&g_pump_bg_rgb565[(y * PUMP_BG_WIDTH) + BAR_X0]);
        }
    }

    DrawLine(BAR_X0, BAR_Y0, BAR_X1, BAR_Y0, 1, style->palette.text_primary);
    DrawLine(BAR_X0, BAR_Y1, BAR_X1, BAR_Y1, 1, style->palette.text_primary);
    DrawLine(BAR_X0, BAR_Y0, BAR_X0, BAR_Y1, 1, style->palette.text_primary);
    DrawLine(BAR_X1, BAR_Y0, BAR_X1, BAR_Y1, 1, style->palette.text_primary);

    for (i = 1; i < BAR_SEGMENTS; i++)
    {
        int32_t y = inner_y1 - (i * seg_step);
        DrawLine(inner_x0, y, inner_x1, y, 1, RGB565(70, 120, 86));
    }

    label_x1 = (BAR_X0 + 2) + edgeai_text5x7_width(label_scale, "T --.-C --.-F") + edgeai_text5x7_width(label_scale, "  ");
    par_lcd_s035_fill_rect(BAR_X0, label_y - 2, label_x1, label_y + 11, TRACE_AX_COLOR);
    DrawTextUi120(BAR_X0 + 2, label_y, "T --.-C --.-F", RGB565(4, 18, 26));
}

static void DrawLeftBargraphDynamic(const gauge_style_preset_t *style, uint8_t fill_pct, int16_t temp_c10)
{
    int32_t i;
    int32_t level = ClampI32(((int32_t)fill_pct * BAR_SEGMENTS) / 100, 0, BAR_SEGMENTS);
    int32_t inner_x0 = BAR_X0 + 3;
    int32_t inner_x1 = BAR_X1 - 3;
    int32_t inner_y0 = BAR_Y0 + 3;
    int32_t inner_y1 = BAR_Y1 - 3;
    int32_t inner_h = (inner_y1 - inner_y0 + 1);
    int32_t seg_step = inner_h / BAR_SEGMENTS;
    bool low_fill = (fill_pct < 20u);
    bool warn_fill = (fill_pct < 40u);
    char line[20];
    int32_t y;
    int32_t label_y = PUMP_BG_HEIGHT - 16;
    int32_t label_scale = 1;
    int32_t label_x1;

    if (((uint8_t)level == gPrevBarLevel) && (fill_pct == gPrevFillPct))
    {
        return;
    }

    for (y = label_y - 2; y <= BAR_Y1 + 1; y++)
    {
        if ((y >= 0) && (y < PUMP_BG_HEIGHT))
        {
            par_lcd_s035_blit_rect(BAR_X0, y, BAR_X0 + 170, y,
                                   (uint16_t *)&g_pump_bg_rgb565[(y * PUMP_BG_WIDTH) + BAR_X0]);
        }
    }

    DrawLine(BAR_X0, BAR_Y0, BAR_X1, BAR_Y0, 1, style->palette.text_primary);
    DrawLine(BAR_X0, BAR_Y1, BAR_X1, BAR_Y1, 1, style->palette.text_primary);
    DrawLine(BAR_X0, BAR_Y0, BAR_X0, BAR_Y1, 1, style->palette.text_primary);
    DrawLine(BAR_X1, BAR_Y0, BAR_X1, BAR_Y1, 1, style->palette.text_primary);

    for (i = 0; i < BAR_SEGMENTS; i++)
    {
        int32_t seg_top = inner_y1 - ((i + 1) * seg_step) + 2;
        int32_t seg_bot = inner_y1 - (i * seg_step) - 1;
        uint16_t color = RGB565(70, 120, 86);

        if (seg_top < inner_y0)
        {
            seg_top = inner_y0;
        }
        if (seg_bot > inner_y1)
        {
            seg_bot = inner_y1;
        }
        if (seg_top > seg_bot)
        {
            continue;
        }

        if (i < level)
        {
            if (low_fill)
            {
                color = style->palette.accent_red;
            }
            else if (warn_fill)
            {
                color = RGB565(255, 180, 24);
            }
            else if (i < ((BAR_SEGMENTS * 4) / 5))
            {
                color = style->palette.accent_green;
            }
            else
            {
                color = RGB565(48, 180, 80);
            }
        }

        par_lcd_s035_fill_rect(inner_x0, seg_top, inner_x1, seg_bot, color);
    }

    {
        int16_t temp_f10 = TempC10ToF10(temp_c10);
        int16_t c_abs = (temp_c10 < 0) ? (int16_t)-temp_c10 : temp_c10;
        int16_t f_abs = (temp_f10 < 0) ? (int16_t)-temp_f10 : temp_f10;
        snprintf(line,
                 sizeof(line),
                 "T %2d.%1dC %3d.%1dF",
                 (int)(c_abs / 10),
                 (int)(c_abs % 10),
                 (int)(f_abs / 10),
                 (int)(f_abs % 10));
    }
    label_x1 = (BAR_X0 + 2) + edgeai_text5x7_width(label_scale, line) + edgeai_text5x7_width(label_scale, "  ");
    par_lcd_s035_fill_rect(BAR_X0, label_y - 2, label_x1, label_y + 11, TRACE_AX_COLOR);
    DrawTextUi120(BAR_X0 + 2, label_y, line, low_fill ? style->palette.accent_red : RGB565(4, 18, 26));

    gPrevBarLevel = (uint8_t)level;
    gPrevFillPct = fill_pct;
}

static void DrawStaticDashboard(const gauge_style_preset_t *style, power_replay_profile_t profile)
{
    int32_t brand_x;
    int32_t rtc_x;
    (void)profile;

    DrawSpaceboxBackground();
    DrawTextUi(2, 0, 1, "(c)RICHARD HABERKERN", style->palette.text_secondary);

    DrawLeftBargraphFrame(style);
    DrawLine(0, BAR_Y0, 26, BAR_Y0, 1, style->palette.text_primary);
    DrawLine(26, BAR_Y0, 26, BAR_Y1, 1, style->palette.text_primary);
    DrawLine(26, MID_TOP_CY + 54, 50, MID_TOP_CY + 54, 1, style->palette.text_primary);
    DrawLine(26, BAR_Y1, 26, BAR_Y1 + 6, 1, style->palette.text_primary);
    DrawLine(26, BAR_Y1 + 6, 50, BAR_Y1 + 6, 1, style->palette.text_primary);
    DrawLine(TERM_X + TERM_W - 4, 78, TERM_X + TERM_W - 38, 78, 1, style->palette.text_primary);
    DrawLine(TERM_X + TERM_W - 38, 78, TERM_X + TERM_W - 38, 258, 1, style->palette.text_primary);
    DrawLine(TERM_X + TERM_W - 38, 258, TERM_X + 14, 258, 1, style->palette.text_primary);
    par_lcd_s035_fill_rect(170, RTC_TEXT_Y - 2, 308, RTC_TEXT_Y + 15, RGB565(2, 3, 5));
    rtc_x = ((PANEL_X0 + PANEL_X1) / 2) - (edgeai_text5x7_width(2, "--:--:--") / 2);
    DrawTextUi(rtc_x, RTC_TEXT_Y, 2, "--:--:--", RGB565(120, 164, 188));
    brand_x = ((PANEL_X0 + PANEL_X1) / 2) - (edgeai_text5x7_width(2, "NXP EDGEAI") / 2);
    DrawTextUi(brand_x, 303, 2, "NXP EDGEAI", RGB565(255, 208, 52));

    DrawBatteryIndicatorFrame(style);
    DrawScopeFrame(style);
    DrawTerminalFrame(style);
    DrawAiPill(style, false);
    DrawAiSideButtons();
}

bool GaugeRender_Init(void)
{
    const gauge_style_preset_t *style;

    gLcdReady = par_lcd_s035_init();
    if (gLcdReady)
    {
        style = GaugeStyle_GetCockpitPreset();
        DrawStaticDashboard(style, POWER_REPLAY_PROFILE_WIRED);
        gStaticReady = true;
        gDynamicReady = false;
        gTraceCount = 0u;
        gTraceReady = false;
        gFrameCounter = 0u;
        gPrevBarLevel = 255u;
        gPrevFillPct = 255u;
        gPrevGlucoseMgdl = 255u;
        gPrevAnomaly = 0u;
        gPrevWear = 0u;
        gPrevAiEnabled = false;
        gPrevAiStatus = 255u;
        gPrevAiFaultFlags = 255u;
        gPrevThermalRisk = 65535u;
        gPrevDrift = 255u;
        gAlertVisualValid = false;
        gScopeSampleAccumUs = 0u;
        gTimelineTouchLatch = false;
        gScopePaused = true;
        gRecordConfirmActive = false;
        gRecordConfirmAction = 0u;
        gRecordStartRequest = false;
        gRecordStopRequest = false;
        gModalWasActive = false;
        gCompassVxFilt = 0;
        gCompassVyFilt = 0;
        gCompassFiltPrimed = false;
        gMagCalPrimed = false;
        gMagCalMinX = 0;
        gMagCalMaxX = 0;
        gMagCalMinY = 0;
        gMagCalMaxY = 0;
        gMagCalMinZ = 0;
        gMagCalMaxZ = 0;
    }
    return gLcdReady;
}

void GaugeRender_SetAccel(int16_t ax_mg, int16_t ay_mg, int16_t az_mg, bool valid)
{
    gAccelXmg = ax_mg;
    gAccelYmg = ay_mg;
    gAccelZmg = az_mg;
    gAccelValid = valid;
}

void GaugeRender_SetLinearAccel(int16_t ax_mg, int16_t ay_mg, int16_t az_mg, bool valid)
{
    gLinAccelXmg = ax_mg;
    gLinAccelYmg = ay_mg;
    gLinAccelZmg = az_mg;
    gLinAccelValid = valid;
}

void GaugeRender_SetGyro(int16_t gx_dps, int16_t gy_dps, int16_t gz_dps, bool valid)
{
    gGyroXdps = gx_dps;
    gGyroYdps = gy_dps;
    gGyroZdps = gz_dps;
    gGyroValid = valid;
}

void GaugeRender_SetMag(int16_t mx_mgauss, int16_t my_mgauss, int16_t mz_mgauss, bool valid)
{
    (void)mx_mgauss;
    (void)my_mgauss;
    (void)mz_mgauss;
    (void)valid;
    gMagXmgauss = 0;
    gMagYmgauss = 0;
    gMagZmgauss = 0;
    gMagValid = false;
    gMagEverValid = false;
}

void GaugeRender_SetBaro(int16_t pressure_dhpa, bool valid)
{
    gBaroDhpa = pressure_dhpa;
    gBaroValid = valid;
}

void GaugeRender_SetSht(int16_t temp_c10, int16_t rh_dpct, bool valid)
{
    gShtTempC10 = temp_c10;
    gShtRhDpct = rh_dpct;
    gShtValid = valid;
}

void GaugeRender_SetStts(int16_t temp_c10, bool valid)
{
    gSttsTempC10 = temp_c10;
    gSttsValid = valid;
}

void GaugeRender_SetBoardTempC(uint8_t temp_c, bool valid)
{
    gBoardTempC = temp_c;
    gBoardTempC10 = (int16_t)temp_c * 10;
    gBoardTempValid = valid;
}

void GaugeRender_SetBoardTempC10(int16_t temp_c10, bool valid)
{
    gBoardTempC10 = temp_c10;
    if (temp_c10 < 0)
    {
        gBoardTempC = 0u;
    }
    else
    {
        gBoardTempC = (uint8_t)(temp_c10 / 10);
    }
    gBoardTempValid = valid;
}

void GaugeRender_SetRuntimeClock(uint16_t hh, uint8_t mm, uint8_t ss, uint8_t ds, bool valid)
{
    gRtcHh = (hh > 9999u) ? 9999u : hh;
    gRtcMm = mm;
    gRtcSs = ss;
    gRtcDs = (ds > 9u) ? 9u : ds;
    gRtcValid = valid;
}

void GaugeRender_SetLimitInfo(uint16_t g_warn_mg,
                              uint16_t g_fail_mg,
                              int16_t temp_low_c10,
                              int16_t temp_high_c10,
                              uint16_t gyro_limit_dps)
{
    gLimitGWarnMg = g_warn_mg;
    gLimitGFailMg = g_fail_mg;
    gLimitTempLowC10 = temp_low_c10;
    gLimitTempHighC10 = temp_high_c10;
    gLimitGyroDps = gyro_limit_dps;
}

void GaugeRender_SetAnomalyInfo(uint8_t mode,
                                uint8_t tune,
                                bool training_active,
                                bool trained_ready,
                                uint8_t level_ax,
                                uint8_t level_ay,
                                uint8_t level_az,
                                uint8_t level_temp,
                                uint8_t overall_level)
{
    gAnomMode = mode;
    gAnomTune = tune;
    gAnomTraining = training_active;
    gAnomTrainedReady = trained_ready;
    gAnomLevelAx = level_ax;
    gAnomLevelAy = level_ay;
    gAnomLevelAz = level_az;
    gAnomLevelTemp = level_temp;
    gAnomOverall = overall_level;
}

void GaugeRender_SetHelpVisible(bool visible)
{
    gHelpVisible = visible;
    if (visible)
    {
        gSettingsVisible = false;
        gLimitsVisible = false;
    }
}

void GaugeRender_SetHelpPage(uint8_t page)
{
    gHelpPage = (page > 1u) ? 1u : page;
}

void GaugeRender_NextHelpPage(void)
{
    gHelpPage = (uint8_t)((gHelpPage == 0u) ? 1u : 0u);
}

void GaugeRender_SetSettingsVisible(bool visible)
{
    gSettingsVisible = visible;
    if (visible)
    {
        gHelpVisible = false;
        gLimitsVisible = false;
    }
}

void GaugeRender_SetLimitsVisible(bool visible)
{
    gLimitsVisible = visible;
    if (visible)
    {
        gHelpVisible = false;
        gSettingsVisible = false;
    }
}

bool GaugeRender_IsLimitsVisible(void)
{
    return gLimitsVisible;
}

void GaugeRender_SetLiveBannerMode(bool enabled)
{
    gLiveBannerMode = enabled;
}

bool GaugeRender_IsLiveBannerMode(void)
{
    return gLiveBannerMode;
}

void GaugeRender_DrawGyroFast(void)
{
    if (!gLcdReady || !gStaticReady)
    {
        return;
    }
    if (gHelpVisible || gSettingsVisible || gLimitsVisible || gRecordConfirmActive)
    {
        return;
    }
    PushScopeSample();
}

void GaugeRender_DrawFrame(const power_sample_t *sample, bool ai_enabled, power_replay_profile_t profile)
{
    const gauge_style_preset_t *style;
    uint16_t cpu_pct;
    uint16_t power_w;
    bool modal_active;
    (void)profile;

    if (!gLcdReady || sample == 0)
    {
        return;
    }

    style = GaugeStyle_GetCockpitPreset();
    power_w = DisplayPowerW(sample);
    modal_active = (gSettingsVisible || gHelpVisible || gLimitsVisible || gRecordConfirmActive);

    if (gModalWasActive && !modal_active)
    {
        /* Modal just closed: redraw static background to clear any stale overlay pixels. */
        DrawStaticDashboard(style, profile);
        gStaticReady = true;
        gDynamicReady = false;
        gPrevSoc = 0u;
        gPrevBarLevel = 255u;
        gPrevFillPct = 255u;
        gPrevGlucoseMgdl = 255u;
        gPrevAiEnabled = !ai_enabled;
        gAlertVisualValid = false;
    }

    if (!gStaticReady)
    {
        DrawStaticDashboard(style, profile);
        gStaticReady = true;
        gDynamicReady = false;
        gPrevBarLevel = 255u;
        gPrevFillPct = 255u;
        gPrevGlucoseMgdl = 255u;
        gPrevAnomaly = 0u;
        gPrevWear = 0u;
        gPrevAiEnabled = false;
        gPrevAiStatus = 255u;
        gPrevAiFaultFlags = 255u;
        gPrevThermalRisk = 65535u;
        gPrevDrift = 255u;
        gAlertVisualValid = false;
        gUiBallFiltPrimed = false;
    }

    cpu_pct = (uint16_t)(20u + (sample->current_mA / 1400u) + (sample->temp_c / 2u));
    if (cpu_pct > 98u)
    {
        cpu_pct = 98u;
    }
    gFrameCounter++;
    UpdateActivityModel();

    if (gSettingsVisible || gHelpVisible || gLimitsVisible)
    {
        DrawPopupModalBase();
        if (gSettingsVisible)
        {
            DrawSettingsPopup();
        }
        if (gHelpVisible)
        {
            DrawHelpPopup();
        }
        if (gLimitsVisible)
        {
            DrawLimitsPopup();
        }
        gModalWasActive = modal_active;
        return;
    }

    {
        char rtc_line[20];
        int32_t rtc_x;
        if (gRtcValid)
        {
            snprintf(rtc_line, sizeof(rtc_line), "%04u:%02u:%02u",
                     (unsigned int)gRtcHh,
                     (unsigned int)gRtcMm,
                     (unsigned int)gRtcSs);
        }
        else
        {
            snprintf(rtc_line, sizeof(rtc_line), "----:--:--");
        }
        par_lcd_s035_fill_rect(170, RTC_TEXT_Y - 2, 308, RTC_TEXT_Y + 15, RGB565(2, 3, 5));
        rtc_x = ((PANEL_X0 + PANEL_X1) / 2) - (edgeai_text5x7_width(2, rtc_line) / 2);
        DrawTextUi(rtc_x, RTC_TEXT_Y, 2, rtc_line, RGB565(160, 220, 255));
    }

    PushScopeSample();
    DrawScopeFrame(style);
    DrawScopeDynamic(style, ai_enabled);
    gDynamicReady = true;

    par_lcd_s035_fill_rect(TERM_X, SCOPE_Y + SCOPE_H + 1, TERM_X + TERM_W, TERM_Y - 1, RGB565(2, 3, 5));
    {
        int32_t lx = SCOPE_X + 8;
        int32_t ly = SCOPE_Y + SCOPE_H + 1;
        uint16_t t_color = TempTraceColorFromC10(DisplayTempC10(sample));
        DrawTextUi(lx, ly, 1, "AX", TRACE_AX_COLOR);
        lx += edgeai_text5x7_width(1, "AX ");
        DrawTextUi(lx, ly, 1, "AY", TRACE_AY_COLOR);
        lx += edgeai_text5x7_width(1, "AY ");
        DrawTextUi(lx, ly, 1, "AZ", TRACE_AZ_COLOR);
        lx += edgeai_text5x7_width(1, "AZ ");
        DrawTextUi(lx, ly, 1, "T", t_color);
        lx += edgeai_text5x7_width(1, "T ");
        DrawTextUi(lx, ly, 1, "GX", TRACE_GX_COLOR);
        lx += edgeai_text5x7_width(1, "GX ");
        DrawTextUi(lx, ly, 1, "GY", TRACE_GY_COLOR);
        lx += edgeai_text5x7_width(1, "GY ");
        DrawTextUi(lx, ly, 1, "GZ", TRACE_GZ_COLOR);
        lx += edgeai_text5x7_width(1, "GZ ");
        DrawTextUi(lx, ly, 1, "BP", TRACE_BARO_COLOR);
    }
    DrawHumanOrientationPointer(style);
    if (!(gSettingsVisible || gHelpVisible || gLimitsVisible))
    {
        DrawAiAlertOverlay(style, sample, ai_enabled);
    }
    else
    {
        par_lcd_s035_fill_rect(ALERT_X0, ALERT_Y0, ALERT_X1, ALERT_Y1, RGB565(2, 3, 5));
    }

    DrawBatteryIndicatorDynamic(style, 82u);

    DrawTerminalDynamic(style, sample, cpu_pct, ai_enabled);
    DrawMedicalOverlayData(style, sample, ai_enabled);
    DrawLeftBargraphDynamic(style, gUiReservoirPct, DisplayTempC10(sample));
    DrawGlucoseIndicator();
    DrawAiSideButtons();
    if (gSettingsVisible)
    {
        DrawSettingsPopup();
    }
    if (gHelpVisible)
    {
        DrawHelpPopup();
    }
    if (gLimitsVisible)
    {
        DrawLimitsPopup();
    }
    if (gRecordConfirmActive)
    {
        DrawRecordConfirmOverlay();
    }
    gModalWasActive = modal_active;

    if (ai_enabled != gPrevAiEnabled)
    {
        DrawAiPill(style, ai_enabled);
        DrawAiSideButtons();
    }

    gPrevCurrent = sample->current_mA;
    gPrevPower = power_w;
    gPrevVoltage = sample->voltage_mV;
    gPrevSoc = 82u;
    gPrevTemp = sample->temp_c;
    gPrevCpuPct = cpu_pct;
    gPrevAnomaly = sample->anomaly_score_pct;
    gPrevWear = sample->connector_wear_pct;
    gPrevAiStatus = sample->ai_status;
    gPrevAiFaultFlags = sample->ai_fault_flags;
    gPrevThermalRisk = sample->thermal_risk_s;
    gPrevDrift = sample->degradation_drift_pct;
    gPrevAiEnabled = ai_enabled;
    (void)gTraceReady;
}

bool GaugeRender_HandleTouch(int32_t x, int32_t y, bool pressed)
{
    bool in_yes = (x >= REC_CONFIRM_YES_X0) && (x <= REC_CONFIRM_YES_X1) &&
                  (y >= REC_CONFIRM_YES_Y0) && (y <= REC_CONFIRM_YES_Y1);
    bool in_no = (x >= REC_CONFIRM_NO_X0) && (x <= REC_CONFIRM_NO_X1) &&
                 (y >= REC_CONFIRM_NO_Y0) && (y <= REC_CONFIRM_NO_Y1);
    bool in_timeline = (x >= TIMELINE_X0) && (x <= TIMELINE_X1) && (y >= TIMELINE_Y0) && (y <= TIMELINE_Y1);
    int32_t mid = (TIMELINE_X0 + TIMELINE_X1) / 2;
    bool in_left = in_timeline && (x < mid);
    bool in_right = in_timeline && (x >= mid);
    bool changed = false;
    bool live_banner_mode = gLiveBannerMode;

    if (gRecordConfirmActive)
    {
        if (!pressed)
        {
            gTimelineTouchLatch = false;
            return false;
        }
        if (!gTimelineTouchLatch)
        {
            gTimelineTouchLatch = true;
            if (in_yes)
            {
                gRecordConfirmActive = false;
                if (gRecordConfirmAction == 2u)
                {
                    gRecordStopRequest = true;
                }
                else if (gRecordConfirmAction == 3u)
                {
                    gClearFlashRequest = true;
                }
                else
                {
                    gRecordStartRequest = true;
                }
                gRecordConfirmAction = 0u;
                changed = true;
            }
            else if (in_no)
            {
                gRecordConfirmActive = false;
                gRecordConfirmAction = 0u;
                changed = true;
            }
        }
        return changed;
    }

    /* Help/settings popups own touch interaction; block timeline state changes behind them. */
    if (gHelpVisible || gSettingsVisible || gLimitsVisible)
    {
        if (!pressed)
        {
            gTimelineTouchLatch = false;
        }
        return false;
    }

    if (live_banner_mode)
    {
        if (!pressed)
        {
            gTimelineTouchLatch = false;
        }
        return false;
    }

    if (!pressed || (!in_left && !in_right))
    {
        gTimelineTouchLatch = false;
        if (!pressed)
        {
            return false;
        }
    }

    if (!pressed || (!in_left && !in_right))
    {
        return false;
    }

    if (!gTimelineTouchLatch)
    {
        gTimelineTouchLatch = true;
        if (in_left && !gScopePaused)
        {
            gRecordConfirmActive = true;
            gRecordConfirmAction = 2u;
            changed = true;
        }
        else if (in_left && gScopePaused)
        {
            /* PLAY pressed again while already in PLAY: treat as restart request. */
            changed = true;
        }
        else if (in_right && gScopePaused)
        {
            gRecordConfirmActive = true;
            gRecordConfirmAction = 1u;
            changed = true;
        }
    }

    return changed;
}

uint8_t GaugeRender_GetTimelineHour(void)
{
    return 0u;
}

bool GaugeRender_IsRecordMode(void)
{
    return !gScopePaused;
}

void GaugeRender_SetPlayhead(uint8_t position_0_to_99, bool valid)
{
    gPlayheadPos = (position_0_to_99 > 99u) ? 99u : position_0_to_99;
    gPlayheadValid = valid;
}

void GaugeRender_SetRecordMode(bool record_mode)
{
    gScopePaused = !record_mode;
}

bool GaugeRender_IsRecordConfirmActive(void)
{
    return gRecordConfirmActive;
}

bool GaugeRender_ConsumeRecordStartRequest(void)
{
    bool requested = gRecordStartRequest;
    gRecordStartRequest = false;
    return requested;
}

bool GaugeRender_ConsumeRecordStopRequest(void)
{
    bool requested = gRecordStopRequest;
    gRecordStopRequest = false;
    return requested;
}

void GaugeRender_RequestClearFlashConfirm(void)
{
    gHelpVisible = false;
    gSettingsVisible = false;
    gLimitsVisible = false;
    gRecordConfirmActive = true;
    gRecordConfirmAction = 3u;
}

bool GaugeRender_ConsumeClearFlashRequest(void)
{
    bool requested = gClearFlashRequest;
    gClearFlashRequest = false;
    return requested;
}
