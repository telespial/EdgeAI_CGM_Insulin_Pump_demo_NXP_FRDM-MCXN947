#include "gauge_render.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "par_lcd_s035.h"
#include "pump_bg.h"
#include "text5x7.h"

#define RGB565(r, g, b) (uint16_t)((((uint16_t)(r) & 0xF8u) << 8) | (((uint16_t)(g) & 0xFCu) << 3) | (((uint16_t)(b) & 0xF8u) >> 3))

enum
{
    SCREEN_W = 480,
    SCREEN_H = 320,
    GRAPH_X = 328,
    GRAPH_Y = 24,
    GRAPH_W = 148,
    GRAPH_H = 94,
    TERM_X = 328,
    TERM_Y = 124,
    TERM_W = 148,
    TERM_H = 172,
    TRACE_POINTS = 96
};

static const uint16_t kPanel = RGB565(4, 16, 28);
static const uint16_t kPanelEdge = RGB565(70, 180, 255);
static const uint16_t kText = RGB565(220, 245, 255);
static const uint16_t kAccent = RGB565(96, 255, 170);
static const uint16_t kWarn = RGB565(255, 200, 70);
static const uint16_t kFault = RGB565(255, 96, 96);
static const uint16_t kShade = RGB565(2, 12, 22);

static bool g_ready = false;
static int16_t g_gyro_x_dps = 0;
static int16_t g_gyro_y_dps = 0;
static int16_t g_gyro_z_dps = 0;
static bool g_gyro_valid = false;
static uint16_t g_graph[TRACE_POINTS];
static uint8_t g_graph_count = 0u;
static bool g_help_visible = false;
static bool g_settings_visible = false;
static bool g_limits_visible = false;
static bool g_record_mode = false;
static uint8_t g_playhead = 0u;
static bool g_playhead_valid = false;

static int32_t ClampI32(int32_t v, int32_t lo, int32_t hi)
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

static void FillRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t c)
{
    par_lcd_s035_fill_rect(x0, y0, x1, y1, c);
}

static void DrawRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t c)
{
    FillRect(x0, y0, x1, y0, c);
    FillRect(x0, y1, x1, y1, c);
    FillRect(x0, y0, x0, y1, c);
    FillRect(x1, y0, x1, y1, c);
}

static void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t c)
{
    int32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t dy = -((y1 > y0) ? (y1 - y0) : (y0 - y1));
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = dx + dy;

    while (true)
    {
        FillRect(x0, y0, x0, y0, c);
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

static void DrawText(int32_t x, int32_t y, int32_t scale, const char *s, uint16_t c)
{
    edgeai_text5x7_draw_scaled(x, y, scale, s, c);
}

static void BlitBackground(void)
{
    par_lcd_s035_blit_rect(0, 0, PUMP_BG_WIDTH - 1, PUMP_BG_HEIGHT - 1, (uint16_t *)g_pump_bg_rgb565);
}

static void DrawFixedPanels(void)
{
    DrawRect(8, 8, SCREEN_W - 9, SCREEN_H - 9, kPanelEdge);
    DrawRect(324, 12, SCREEN_W - 10, SCREEN_H - 12, kPanelEdge);
    FillRect(GRAPH_X, GRAPH_Y, GRAPH_X + GRAPH_W, GRAPH_Y + GRAPH_H, kPanel);
    FillRect(TERM_X, TERM_Y, TERM_X + TERM_W, TERM_Y + TERM_H, kPanel);
    DrawRect(GRAPH_X, GRAPH_Y, GRAPH_X + GRAPH_W, GRAPH_Y + GRAPH_H, kPanelEdge);
    DrawRect(TERM_X, TERM_Y, TERM_X + TERM_W, TERM_Y + TERM_H, kPanelEdge);
}

static void DrawGyroPointerOverlay(void)
{
    int32_t cx = 240;
    int32_t cy = 156;
    int32_t r = 84;
    int32_t px;
    int32_t py;
    char line[32];
    int32_t gx = ClampI32(g_gyro_x_dps, -1000, 1000);
    int32_t gy = ClampI32(g_gyro_y_dps, -1000, 1000);

    px = cx + (gx * (r - 6)) / 1000;
    py = cy - (gy * (r - 6)) / 1000;

    /* Ring + pointer only; no gyro sphere / no accel ball visuals. */
    DrawRect(188, 244, 318, 308, kPanelEdge);
    FillRect(189, 245, 317, 307, kShade);
    DrawLine(cx - r, cy, cx + r, cy, RGB565(16, 90, 120));
    DrawLine(cx, cy - r, cx, cy + r, RGB565(16, 90, 120));
    DrawLine(cx, cy, px, py, kAccent);
    FillRect(px - 2, py - 2, px + 2, py + 2, kAccent);

    snprintf(line, sizeof(line), "GYR X%4d", (int)g_gyro_x_dps);
    DrawText(196, 252, 1, line, kText);
    snprintf(line, sizeof(line), "GYR Y%4d", (int)g_gyro_y_dps);
    DrawText(196, 268, 1, line, kText);
    DrawText(196, 284, 1, g_gyro_valid ? "TRACK OK" : "TRACK NA", g_gyro_valid ? kAccent : kWarn);
}

static void DrawMotorOverlay(const power_sample_t *s)
{
    char line[32];
    uint8_t anomaly = (uint8_t)ClampI32((int32_t)s->anomaly_score_pct, 0, 100);
    int32_t fill = 0;
    uint16_t sev = (anomaly >= 65u) ? kFault : (anomaly >= 35u ? kWarn : kAccent);

    FillRect(10, 30, 178, 122, kShade);
    DrawRect(10, 30, 178, 122, kPanelEdge);
    DrawText(18, 38, 1, "MOTOR", kText);

    snprintf(line, sizeof(line), "RPM %4u", (unsigned)s->voltage_mV);
    DrawText(18, 54, 1, line, kText);
    snprintf(line, sizeof(line), "CUR %4umA", (unsigned)s->current_mA);
    DrawText(18, 68, 1, line, kText);
    snprintf(line, sizeof(line), "LOAD %3u%%", (unsigned)s->connector_wear_pct);
    DrawText(18, 82, 1, line, kText);
    DrawText(18, 96, 1, "ANOM", sev);

    fill = (anomaly * 148) / 100;
    DrawRect(16, 102, 168, 116, kPanelEdge);
    FillRect(18, 104, 18 + fill, 114, sev);
}

static void DrawPumpOverlay(const power_sample_t *s)
{
    char line[32];
    uint8_t fill_pct = (uint8_t)ClampI32((int32_t)s->soc_pct, 0, 100);
    bool pumping = (s->power_mW > 180u);
    int32_t fill_h = (fill_pct * 86) / 100;

    FillRect(16, 178, 178, 300, kShade);
    DrawRect(16, 178, 178, 300, kPanelEdge);
    DrawText(24, 186, 1, "PUMP", kText);
    snprintf(line, sizeof(line), "RATE %3uMLH", (unsigned)(s->power_mW / 10u));
    DrawText(24, 202, 1, line, kText);
    snprintf(line, sizeof(line), "FILL %3u%%", (unsigned)fill_pct);
    DrawText(24, 218, 1, line, kText);
    DrawText(24, 234, 1, pumping ? "STATE PUMPING" : "STATE IDLE", pumping ? kAccent : kWarn);

    DrawRect(124, 196, 166, 288, kPanelEdge);
    FillRect(125, 197, 165, 287, RGB565(4, 20, 32));
    FillRect(126, 286 - fill_h, 164, 286, kAccent);
    if (pumping)
    {
        DrawLine(108, 236, 122, 236, kAccent);
        DrawLine(108, 228, 122, 228, kAccent);
        DrawLine(108, 244, 122, 244, kAccent);
    }
}

static void DrawHeaderAndClock(const power_sample_t *s)
{
    char elapsed[24];
    uint32_t t = s->elapsed_charge_s;
    uint32_t hh = t / 3600u;
    uint32_t mm = (t % 3600u) / 60u;
    uint32_t ss = t % 60u;

    snprintf(elapsed, sizeof(elapsed), "%04u:%02u:%02u", (unsigned)hh, (unsigned)mm, (unsigned)ss);
    DrawText(166, 8, 2, "NXP EDGEAI", kText);
    DrawText(186, 286, 2, elapsed, kAccent);
}

static void DrawGraph(const power_sample_t *s)
{
    uint8_t i;
    int32_t y;

    if (g_graph_count < TRACE_POINTS)
    {
        g_graph[g_graph_count++] = (uint16_t)ClampI32((int32_t)s->anomaly_score_pct * 10, 0, 1000);
    }
    else
    {
        memmove(&g_graph[0], &g_graph[1], (TRACE_POINTS - 1u) * sizeof(g_graph[0]));
        g_graph[TRACE_POINTS - 1u] = (uint16_t)ClampI32((int32_t)s->anomaly_score_pct * 10, 0, 1000);
    }

    FillRect(GRAPH_X + 2, GRAPH_Y + 2, GRAPH_X + GRAPH_W - 2, GRAPH_Y + GRAPH_H - 2, kPanel);
    DrawText(GRAPH_X + 8, GRAPH_Y + 6, 1, "GRAPH", kText);
    DrawLine(GRAPH_X + 8, GRAPH_Y + GRAPH_H - 14, GRAPH_X + GRAPH_W - 8, GRAPH_Y + GRAPH_H - 14, kPanelEdge);
    for (i = 1u; i < g_graph_count; i++)
    {
        int32_t x0 = GRAPH_X + 10 + ((i - 1u) * (GRAPH_W - 20)) / (TRACE_POINTS - 1u);
        int32_t x1 = GRAPH_X + 10 + (i * (GRAPH_W - 20)) / (TRACE_POINTS - 1u);
        int32_t y0 = GRAPH_Y + GRAPH_H - 16 - ((int32_t)g_graph[i - 1u] * (GRAPH_H - 24)) / 1000;
        int32_t y1 = GRAPH_Y + GRAPH_H - 16 - ((int32_t)g_graph[i] * (GRAPH_H - 24)) / 1000;
        DrawLine(x0, y0, x1, y1, kAccent);
    }

    y = GRAPH_Y + 12;
    DrawText(GRAPH_X + 82, y, 1, g_playhead_valid ? "PLY ON" : "PLY OFF", g_playhead_valid ? kAccent : kWarn);
}

static void DrawTerminal(const power_sample_t *s, bool ai_enabled)
{
    char line[40];
    uint16_t ai_color = ai_enabled ? kAccent : kWarn;

    FillRect(TERM_X + 2, TERM_Y + 2, TERM_X + TERM_W - 2, TERM_Y + TERM_H - 2, kPanel);
    DrawText(TERM_X + 8, TERM_Y + 6, 1, "TERMINAL", kText);

    snprintf(line, sizeof(line), "M RPM  %4u", (unsigned)s->voltage_mV);
    DrawText(TERM_X + 8, TERM_Y + 26, 1, line, kText);
    snprintf(line, sizeof(line), "M CUR  %4u mA", (unsigned)s->current_mA);
    DrawText(TERM_X + 8, TERM_Y + 42, 1, line, kText);
    snprintf(line, sizeof(line), "M LOAD %3u %%", (unsigned)s->connector_wear_pct);
    DrawText(TERM_X + 8, TERM_Y + 58, 1, line, kText);
    snprintf(line, sizeof(line), "PUMP   %3u MLH", (unsigned)(s->power_mW / 10u));
    DrawText(TERM_X + 8, TERM_Y + 74, 1, line, kText);
    snprintf(line, sizeof(line), "TEMP   %3u C", (unsigned)s->temp_c);
    DrawText(TERM_X + 8, TERM_Y + 90, 1, line, kText);
    snprintf(line, sizeof(line), "SCORE  %3u %%", (unsigned)s->anomaly_score_pct);
    DrawText(TERM_X + 8, TERM_Y + 106, 1, line, kText);
    DrawText(TERM_X + 8, TERM_Y + 122, 1, (s->ai_status == AI_STATUS_FAULT) ? "ALERT  FAULT" : (s->ai_status == AI_STATUS_WARNING) ? "ALERT  WARN" : "ALERT  NORMAL", (s->ai_status == AI_STATUS_FAULT) ? kFault : (s->ai_status == AI_STATUS_WARNING) ? kWarn : kAccent);
    DrawText(TERM_X + 8, TERM_Y + 138, 1, ai_enabled ? "AI     ON" : "AI     OFF", ai_color);
}

bool GaugeRender_Init(void)
{
    g_ready = par_lcd_s035_init();
    if (!g_ready)
    {
        return false;
    }
    BlitBackground();
    DrawFixedPanels();
    return true;
}

void GaugeRender_DrawFrame(const power_sample_t *sample, bool ai_enabled, power_replay_profile_t profile)
{
    (void)profile;
    if ((!g_ready) || (sample == NULL))
    {
        return;
    }

    BlitBackground();
    DrawFixedPanels();
    DrawHeaderAndClock(sample);
    DrawMotorOverlay(sample);
    DrawPumpOverlay(sample);
    DrawGyroPointerOverlay();
    DrawGraph(sample);
    DrawTerminal(sample, ai_enabled);
}

void GaugeRender_SetAccel(int16_t ax_mg, int16_t ay_mg, int16_t az_mg, bool valid)
{
    (void)ax_mg;
    (void)ay_mg;
    (void)az_mg;
    (void)valid;
}

void GaugeRender_SetLinearAccel(int16_t ax_mg, int16_t ay_mg, int16_t az_mg, bool valid)
{
    (void)ax_mg;
    (void)ay_mg;
    (void)az_mg;
    (void)valid;
}

void GaugeRender_SetGyro(int16_t gx_dps, int16_t gy_dps, int16_t gz_dps, bool valid)
{
    g_gyro_x_dps = gx_dps;
    g_gyro_y_dps = gy_dps;
    g_gyro_z_dps = gz_dps;
    g_gyro_valid = valid;
}

void GaugeRender_SetMag(int16_t mx_mgauss, int16_t my_mgauss, int16_t mz_mgauss, bool valid)
{
    (void)mx_mgauss;
    (void)my_mgauss;
    (void)mz_mgauss;
    (void)valid;
}

void GaugeRender_SetBaro(int16_t pressure_dhpa, bool valid)
{
    (void)pressure_dhpa;
    (void)valid;
}

void GaugeRender_SetSht(int16_t temp_c10, int16_t rh_dpct, bool valid)
{
    (void)temp_c10;
    (void)rh_dpct;
    (void)valid;
}

void GaugeRender_SetStts(int16_t temp_c10, bool valid)
{
    (void)temp_c10;
    (void)valid;
}

void GaugeRender_SetBoardTempC(uint8_t temp_c, bool valid)
{
    (void)temp_c;
    (void)valid;
}

void GaugeRender_SetBoardTempC10(int16_t temp_c10, bool valid)
{
    (void)temp_c10;
    (void)valid;
}

void GaugeRender_SetProfileInfo(const char *model_name, const char *model_version, const char *extension_version)
{
    (void)model_name;
    (void)model_version;
    (void)extension_version;
}

void GaugeRender_SetLogRateHz(uint8_t hz)
{
    (void)hz;
}

void GaugeRender_SetRuntimeClock(uint16_t hh, uint8_t mm, uint8_t ss, uint8_t ds, bool valid)
{
    (void)hh;
    (void)mm;
    (void)ss;
    (void)ds;
    (void)valid;
}

void GaugeRender_SetLimitInfo(uint16_t g_warn_mg,
                              uint16_t g_fail_mg,
                              int16_t temp_low_c10,
                              int16_t temp_high_c10,
                              uint16_t gyro_limit_dps)
{
    (void)g_warn_mg;
    (void)g_fail_mg;
    (void)temp_low_c10;
    (void)temp_high_c10;
    (void)gyro_limit_dps;
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
    (void)mode;
    (void)tune;
    (void)training_active;
    (void)trained_ready;
    (void)level_ax;
    (void)level_ay;
    (void)level_az;
    (void)level_temp;
    (void)overall_level;
}

void GaugeRender_SetHelpVisible(bool visible)
{
    g_help_visible = visible;
}

void GaugeRender_SetHelpPage(uint8_t page)
{
    (void)page;
}

void GaugeRender_NextHelpPage(void)
{
}

void GaugeRender_SetSettingsVisible(bool visible)
{
    g_settings_visible = visible;
}

void GaugeRender_SetLimitsVisible(bool visible)
{
    g_limits_visible = visible;
}

bool GaugeRender_IsLimitsVisible(void)
{
    return g_limits_visible;
}

void GaugeRender_SetLiveBannerMode(bool enabled)
{
    (void)enabled;
}

bool GaugeRender_IsLiveBannerMode(void)
{
    return false;
}

void GaugeRender_DrawGyroFast(void)
{
}

bool GaugeRender_HandleTouch(int32_t x, int32_t y, bool pressed)
{
    (void)x;
    (void)y;
    (void)pressed;
    return g_help_visible || g_settings_visible || g_limits_visible;
}

uint8_t GaugeRender_GetTimelineHour(void)
{
    return 0u;
}

bool GaugeRender_IsRecordMode(void)
{
    return g_record_mode;
}

void GaugeRender_SetPlayhead(uint8_t position_0_to_99, bool valid)
{
    g_playhead = position_0_to_99;
    g_playhead_valid = valid;
}

void GaugeRender_SetRecordMode(bool record_mode)
{
    g_record_mode = record_mode;
}

bool GaugeRender_IsRecordConfirmActive(void)
{
    return false;
}

bool GaugeRender_ConsumeRecordStartRequest(void)
{
    return false;
}

bool GaugeRender_ConsumeRecordStopRequest(void)
{
    return false;
}

void GaugeRender_RequestClearFlashConfirm(void)
{
}

bool GaugeRender_ConsumeClearFlashRequest(void)
{
    return false;
}
