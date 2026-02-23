# Project State

Last updated: 2026-02-23

## Current Status
- Project framework scaffold created.
- Build/flash workflow scripts added.
- Git repository initialized locally.

## Hardware Setup
- Board: FRDM-MCXN947
- Probe: MCU-Link CMSIS-DAP (use `#1`)
- Debug USB: J17

## Build/Flash Config
- Build Command: ./tools/build_frdmmcxn947.sh debug
- Flash Command: /usr/local/LinkServer/LinkServer flash --probe '#1' MCXN947:FRDM-MCXN947 load --addr 0x0 {BIN_PATH}
- Binary: mcuxsdk_ws/build/edgeai_package_transport_anomaly_demo_cm33_core0.bin
- Status File: STATUS.md

## Update 2026-02-22
- Change: Initial standalone framework scaffold with docs/scripts/source layout.
- Result: ok

## Update 2026-02-22
- Change: Added project-local AGENTS instructions (gitignored), initialized git, committed framework scaffold, and pushed `main` to GitHub.
- Result: ok

## Update 2026-02-22
- Change: Implemented first-pass medical LCD graphics layout slice using same 480x320 background image:
  - Added `src/medical_lcd_layout.c` and `src/medical_lcd_layout.h`
  - Added platform drawing HAL contract `src/medical_display_hal.h`
  - Added temporary no-op host compile stub `src/medical_display_stub.c`
  - Imported background asset `src/spacebox_bg.h`
  - Updated `src/main.c` to drive static+dynamic layout demo path
- UI layout coverage in this slice:
  - `NXP EDGEAI` retained in top-center region
  - elapsed time retained in bottom-center region
  - right-side graph + terminal panels retained
  - left/center medical visuals added: motor, pump, wearer-context figure
- Verification:
  - `cc -std=c11 -Wall -Wextra -I./src src/main.c src/medical_lcd_layout.c src/medical_display_stub.c -o /tmp/edgeai_medical_lcd_layout_demo`
  - execution output: `EdgeAI medical device LCD layout staged.`
- Result: ok

## Update 2026-02-22
- Change: Switched LCD background source to project image `docs/pump.png` and generated `src/pump_bg.h` (480x320 RGB565).
- Change: Updated renderer (`src/medical_lcd_layout.c`) to blit `g_pump_bg_rgb565` and keep overlay layout:
  - top-center `NXP EDGEAI`
  - bottom-center elapsed time
  - right-third graph + terminal
  - left/center motor + pump + wearer graphics
- Verification:
  - Host compile smoke test PASS:
    - `cc -std=c11 -Wall -Wextra -I./src src/main.c src/medical_lcd_layout.c src/medical_display_stub.c -o /tmp/edgeai_medical_lcd_layout_demo`
  - Scripted preflight/build PASS with `BUILD_CMD` override.
  - Flash attempt blocked by configuration (no MCU `BIN_PATH` set in project state): `Binary is empty. Set BIN_PATH or docs/PROJECT_STATE.md`.
- Result: ok (graphics milestone), flash pending MCU build artifact integration.

## Update 2026-02-22
- Change: Refreshed LCD background from the current `docs/pump.png` and regenerated `src/pump_bg.h`.
- Change: Reworked overlay strategy to align active data labels with icon regions in the new background art:
  - motor region now shows live `RPM`, `CUR`, `LOAD`, and anomaly bar indicator
  - pump region now shows `RATE`, `FILL`, and `STATE` with tank fill-level visualization and pumping activity marker
  - human center region now shows gyro ring pointer around the large central circle plus `INV`/`DROP` status labels
- Change: Extended display state contract (`src/medical_lcd_layout.h`) with dynamic fields:
  - `motor_anomaly_pct`, `pump_fill_pct`, `pump_active`, `gyro_angle_deg`, `inverted`, `drop_detected`
- Verification:
  - Host compile/run PASS:
    - `cc -std=c11 -Wall -Wextra -I./src src/main.c src/medical_lcd_layout.c src/medical_display_stub.c -lm -o /tmp/edgeai_medical_lcd_layout_demo`
  - Scripted preflight/build PASS with `BUILD_CMD` override.
  - Flash attempt remains blocked pending MCU binary config (`BIN_PATH` empty).
- Result: ok (overlay behavior implemented), flash pending firmware build artifact path.

## Update 2026-02-22
- Change: Removed legacy motion widgets from active LCD renderer path:
  - removed gyro sphere visual
  - removed accel box/ball visual
  - retained only gyro ring pointer plus motor/pump/human-aligned overlays
- Change: Wired medical project into MCU SDK build/flash flow using local `tools/build_frdmmcxn947.sh` and `tools/flash_frdmmcxn947.sh`, with overlay wrapper sourcing this repo's `src/` code.
- Verification:
  - Build PASS:
    - `./tools/build_frdmmcxn947.sh debug`
    - output binary: `mcuxsdk_ws/build/edgeai_package_transport_anomaly_demo_cm33_core0.bin`
  - Flash PASS:
    - `./tools/flash_frdmmcxn947.sh`
    - runner: `linkserver`, probe `#1`, target `MCXN947:FRDM-MCXN947`
- Notes:
  - `west update` in setup reported non-critical failures for unrelated repos; required demo build/flash path still completed successfully.
- Result: ok

## Update 2026-02-22
- Change: Restored package-demo visual baseline for LCD rendering behavior in medical firmware:
  - same `NXP EDGEAI` placement/color/size as package demo
  - same elapsed-time placement/color/size as package demo
  - right-third timeline buttons (`PLAY/STOP`, `REC`, `LIVE`), graph, and terminal kept in package-demo style/flow
- Change: Removed center motion widgets per direction:
  - removed gyro sphere/ring widget rendering path
  - removed accel wire-box and accel ball rendering path
- Change: Added transparent medical data labels (no black boxes/borders) positioned over background icons:
  - motor labels (`RPM`, current, anomaly, wear)
  - pump labels (state/rate/fill)
  - human labels (gyro values, pose inversion, drop risk)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Applied requested medical UI refinements on top of package-style baseline:
  - magnetometer stream disabled in render path and `MAG` removed from terminal output
  - left temperature bargraph reduced significantly while staying on far-left edge
  - motor overlay labels shifted down by two text rows
  - battery indicator moved between motor and pump regions and reduced by 20%
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Moved left temperature bargraph to bottom-edge placement and removed black bargraph background fills so the bargraph renders transparently over the background image.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Adjusted temp bargraph layout/detail styling:
  - moved bargraph upward from the previous bottom-most position so temp text is fully visible
  - increased bargraph height by 10% while keeping width unchanged
  - set temp-text background box color to match `AX` trace color
  - moved pump `IDLE/RATE/FILL` text right by ~2 characters and down by 2 text rows
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Moved temp C/F label to the very bottom edge of the LCD render area.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Increased temp C/F label text size to large (2x font scale) and expanded the bottom temp label box accordingly.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Reduced bottom temp C/F text size from large to medium (single-scale) and resized the label box to match.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Extended the blue temp label background by one character width past the trailing `F`.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Resized temp bargraph to requested dimensions:
  - height increased to approximately 2x current size
  - width increased by 25%
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Moved `(c)RICHARD HABERKERN` text to top edge of LCD render area.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Moved battery label and graphic to the lower-right corner under terminal.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Shifted battery label/graphic by requested relative offset:
  - left by 25% of battery graphic width
  - up by 50% of battery graphic height
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Refined battery widget alignment:
  - moved positive terminal to vertical center of battery body
  - moved `BATT` label to left-center of battery graphic
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Corrected battery icon readability:
  - positive terminal centered vertically on right side of battery body
  - `BATT` text rendered persistently at left-center of battery graphic
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-22
- Change: Styled battery positive terminal as a white boxed `+` and aligned it with the `BATT` text baseline; retained persistent `BATT` label redraw each frame.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Reworked battery indicator rendering for better readability on-device:
  - battery body now uses a stronger white outline around the dark interior
  - right-side positive terminal is a thicker, centered white rectangular nub attached to the battery body
  - battery frame is redrawn in the dynamic path each frame to prevent partial visual overwrite
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved battery widget down by ~10% of battery height to avoid upper-edge clipping against terminal area (`BATT_Y: 287 -> 289`).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Refined battery placement and SOC text alignment:
  - moved battery down 2 additional pixels (`BATT_Y: 289 -> 291`)
  - centered `82%` SOC text vertically inside the battery body/fill region
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted branding/time vertical layout:
  - moved `NXP EDGEAI` down by ~50% text-height (`y: 286 -> 293`)
  - moved elapsed-time row to maintain 5 px vertical padding from `NXP EDGEAI` (`RTC_TEXT_Y: 259 -> 274`)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Shifted both brand and elapsed-time text down by an additional 10 px:
  - `NXP EDGEAI` y: `293 -> 303`
  - elapsed-time y (`RTC_TEXT_Y`): `274 -> 284`
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated human-center text overlays per request:
  - removed `AI: ACTIVE/OFF` label from center region
  - moved `GYR`, `POSE`, and `DROP` lines below the human figure into the band above elapsed time
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated human-status text formatting:
  - corrected `GYR` label to `GYRO`
  - increased human-status text visual size by approximately 25% using a controlled overdraw helper for scale-1 LCD font rendering
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed center human-status text ghosting/overlap artifacts by restoring the underlying background image region before redrawing `GYRO`, `POSE`, and `DROP` lines each frame.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Expanded center GYRO line to include Z-axis values (`GYRO X/Y/Z`) instead of only X/Y.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added a human-orientation pointer gauge around the center figure:
  - speedometer-style circular tick ring
  - rotating 360-degree arrow pointer driven by accelerometer orientation
  - pointer color changes to red when upside-down is detected
  - background restore added for the gauge region to prevent pointer trails
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Refined orientation gauge visuals to match target style:
  - enlarged gauge diameter to align with the center dial graphic (`r = MAIN_R - 6`)
  - replaced needle-style pointer with a head-only arrow marker that points to tick marks
  - kept 360-degree rotation and upside-down red-state behavior
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Applied visibility/layout fixes:
  - moved warning/alert box up by 15 pixels (`ALERT_Y0: 48 -> 33`, `ALERT_Y1: 86 -> 71`)
  - clamped all seven scope traces (`AX/AY/AZ/GX/GY/GZ/T`) to the graph interior so no trace disappears on border lines
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted gyro orientation gauge geometry:
  - increased circle diameter by 25%
  - moved gauge center up by 20 pixels
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved gyro orientation circle center right by 2 pixels.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved gyro orientation circle up by 2 pixels.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Corrected layer ordering so gyro pointer/tick graphics render beneath warning-box and terminal UI layers (no clipping/overdraw of foreground panels).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Removed center `GYRO`, `POSE`, and `DROP` text overlays; the center status region is now background-only.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS (after removing now-unused helper)
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Applied combined gyro-display and pointer-style refinements:
  - terminal gyro line now shows one decimal place (`X/Y/Z` as `dps` with tenths)
  - live gyro feed to renderer upgraded to 0.1 dps resolution (with playback scaling compatibility)
  - orientation pointer circle diameter reduced by 20%
  - removed all tick marks from the pointer circle
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Re-referenced the human orientation pointer so it points down when the LCD is held facing the user (screen plane parallel to face), instead of using the prior vertical sign convention.
  - implementation: pointer angle now uses `atan2(-Y, Z)` in `DrawHumanOrientationPointer()`.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Applied additional 180-degree phase correction to orientation pointer because hardware posture still rendered inverted.
  - implementation: after `atan2(-Y, Z)`, add `+180deg` and wrap to `[0, 360)`.
  - behavior target: screen-facing posture points down toward ground.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added a second gravity orientation access widget and stabilized the main human pointer for better visual behavior during movement.
  - main center pointer: UI-only smoothing added (low-pass on accel-derived orientation plus per-frame angular delta clamp) to reduce rolling/jitter.
  - new gravity indicator: compact `GRAV` mini-widget with circular marker and `TABLE/TILT` status, placed in left-middle region outside the human graphic.
  - sensor integrity: raw accelerometer/gyro streams and model-facing data paths are unchanged; damping is display-only.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Revised orientation UI per request:
  - removed the added left-segment gravity widget so the pump-text area remains in its prior layout.
  - replaced center arrow marker with a rolling ball inside the existing orientation circle.
  - rolling ball is accelerometer-driven (360-degree orientation mapping), light green in normal orientation, and light orange when upside down.
  - UI-only damping remains active for stable motion while leaving raw sensor/model data unchanged.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Tuned rolling-ball orientation behavior for requested kinematics:
  - reduced damping and increased per-frame angle step limit so motion responds faster.
  - changed ball radius from fixed-edge to tilt-proportional (`sqrt(X^2+Y^2)/|g|`), so:
    - table-flat posture drives ball toward center,
    - larger tilt drives ball outward toward the circle edge.
  - preserved screen-facing posture direction mapping (ball moves toward bottom when held parallel to eyes).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Increased center rolling-ball marker size by ~2x (from ~7x7 to ~14x14 visual footprint) for better readability.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated pump overlay typography:
  - `PUMP`, `RATE`, and `FILL` lines changed to yellow.
  - pump lines rendered ~50% larger using a new `DrawTextUi150()` helper (thickened 1x glyph pass).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved the full pump text block down toward the bottom-left F/C area while preserving 50%-larger yellow styling.
  - `PUMP`/`RATE`/`FILL` Y positions shifted by +20 px (`218/236/254` -> `238/256/274`).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Increased pump text size by ~20% from the previous custom enlarged style.
  - adjusted `DrawTextUi150()` glyph thickening passes to render approximately ~1.8x 1x size.
  - applies to `PUMP`, `RATE`, and `FILL` lines.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Increased `PUMP` / `RATE` / `FILL` text by an additional ~25%.
  - updated `DrawTextUi150()` to render approximately ~2.25x 1x size (from ~1.8x).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed pump text readability issue by switching from thickened pseudo-scaling to true scaled font rendering.
  - removed `DrawTextUi150()` helper.
  - `PUMP` / `RATE` / `FILL` now render with `DrawTextUi(..., scale=2, ...)`, preserving yellow color and shadow.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed bottom-circle accel-ball clipping/flicker in center area.
  - removed legacy center-region background blit (`BlitPumpBgRegion(172, 226, 322, 267)`) that overlapped the lower arc of the orientation circle.
  - kept pump-text region-only background restore active for text artifact cleanup.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Finalized pump-line artifact cleanup and unit labeling:
  - rate unit text normalized to explicit `ML/H` on the pump line.
  - pump text redraw background region narrowed (`x1: 240 -> 198`) to avoid any overlap with the center orientation-ball area.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Corrected remaining unit rendering and improved orientation-ball behavior.
  - added `/` glyph to `text5x7` font so `ML/H` renders correctly on-screen.
  - replaced center ball motion model with filtered accel X/Y projection and circular clamp:
    - table-flat -> ball near center,
    - tilt/motion -> ball moves in corresponding 2D direction within circle bounds.
  - removed now-unused angle-based ball state variables from renderer.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Corrected left/right orientation direction for the center ball.
  - inverted accel-X projection sign in ball mapping.
  - up/down mapping intentionally unchanged.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved `PUMP` / `RATE` / `FILL` text down by one font row (scale-2 row, +14 px) toward the bottom strip.
  - Y positions: `232/248/264` -> `246/262/278`.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed elapsed-timer clipping caused by pump text background restore overlap.
  - replaced single restore rectangle with two segmented regions that avoid the timer band in center area.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted motor headline styling/position.
  - moved `MTR RPM:...` line up by ~2.5 rows (`y: 62 -> 42`).
  - increased `MTR` line text size by ~25% using `DrawTextUi125()` helper.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated motor headline content and size.
  - label changed from `MTR RPM` to `RPM`.
  - headline increased by an additional ~25% by strengthening `DrawTextUi125()` expansion pass.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved motor `RPM` headline up by one additional text row.
  - draw position changed from `y=34` to `y=26` in `src/gauge_render.c`.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Refined motor `RPM` headline readability and placement.
  - moved one more row up (`y: 26 -> 18`).
  - switched render from thickened pseudo-scale helper to crisp native scale-2 draw for sharper edges and larger text (~30% increase vs prior 1.56x style).
  - marked legacy `DrawTextUi125` helper as `__attribute__((unused))` to satisfy `-Werror` after migration.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` first attempt FAIL (`DrawTextUi125` unused warning treated as error).
  - `./tools/build_frdmmcxn947.sh debug` PASS after helper annotation fix.
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`).
- Result: ok

## Update 2026-02-23
- Change: Improved `RPM` headline readability and horizontal placement.
  - moved right by two character cells at scale-2 (`x: 22 -> 46`, +24 px).
  - changed renderer from crisp single-pass to UI shadow style at scale-2 (`DrawTextUi`) for clearer contrast on the motor background.
  - added motor-region background restore (`BlitPumpBgRegion(20,12,220,102)`) before dynamic motor text draw to avoid stale glyph remnants after repositioning.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Tuned motor `RPM` headline position/size.
  - moved downward by about one-third row (`y: 18 -> 23`, +5 px).
  - reduced text size by about 20% by switching from scale-2 shadow render to `DrawTextUi125` pass.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed `RPM` readability regression and top-center UI clipping.
  - replaced `DrawTextUi125` with `DrawTextUi(..., scale=2)` for clearer `RPM` line rendering.
  - narrowed motor background restore from `BlitPumpBgRegion(20,12,220,102)` to `BlitPumpBgRegion(20,18,142,102)` so redraw no longer intrudes into the `* AI ON ?` row and warning panel area.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted motor `RPM` row horizontal position.
  - moved left by two character cells (`x: 46 -> 22`, -24 px).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added synthetic RPM demo behavior for motor headline.
  - display now shows `RPM:x.y` where value is either `0.0` or random in `0.1..25.0`.
  - update interval is now random `10.0..30.0` seconds (RTC decisecond scheduler; frame-count fallback if RTC is unavailable).
  - implemented with lightweight xorshift PRNG state and decisecond next-update scheduling.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added timed zero behavior to synthetic RPM demo.
  - when random RPM enters `0.0`, it now holds at zero for a random `5.0..20.0` seconds.
  - after zero-hold expires, it resumes non-zero random RPM updates with the existing `10.0..30.0` second cadence.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated motor RPM/current demo lines.
  - random RPM non-zero range expanded from `0.1..25.0` to `0.1..49.0`, so values like `21.x` and `49.0` can occur.
  - rewired the `I` row to display only current value text (`NNmA`) with no label.
  - current is now derived from RPM via linear mapping `0.0..49.0 RPM` -> `0..95 mA`.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted current (`mA`) row layout.
  - moved right by four character cells (`x: 22 -> 46`, +24 px).
  - increased text size by ~50% using `DrawTextUi125` helper.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Standardized motor text readability by matching `mA` row to `RPM` row typography.
  - `mA` row now uses `DrawTextUi(..., scale=2)` (same font style and same text size as RPM).
  - replaced prior `DrawTextUi125` path used for mA rendering.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Repositioned motor `mA` row for alignment.
  - moved left by one character width (`x: 46 -> 34`, -12 px at scale-2).
  - moved up by roughly two-thirds of a row (`y: 76 -> 71`, -5 px).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Corrected rollback for motor current row placement.
  - restored `mA` row from shifted position back to the prior center location (`x: 34 -> 46`, `y: 71 -> 76`).
  - this matches the state before the temporary left/up alignment experiment.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted motor `mA` row as requested.
  - moved left by one character width (`x: 46 -> 34`, -12 px at scale-2).
  - moved up by approximately two-thirds text height (`y: 76 -> 67`, -9 px).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Replaced motor wear row layout per request.
  - removed legacy `WR` row.
  - kept `mA` row at `x=34,y=67`.
  - added `ANOM:` row at `x=22,y=102` (exactly +35 px, i.e., 2.5 scale-2 row heights from mA row).
  - added `WEAR:` row at `x=22,y=116` (one additional scale-2 row below ANOM).
  - both `ANOM` and `WEAR` use same font/text height as `mA` (`DrawTextUi`, scale 2).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Repositioned `ANOM` and `WEAR` rows.
  - both rows moved left by one character width (`x: 22 -> 10`).
  - both rows moved down by 1.5 scale-2 rows (`+21 px`):
    - `ANOM y: 102 -> 123`
    - `WEAR y: 116 -> 137`
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added extra vertical padding between `ANOM` and `WEAR` rows.
  - moved `WEAR` row down by 2 px (`y: 137 -> 139`) while keeping ANOM unchanged.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added left-of-human blood glucose indicator.
  - new green text overlay: `BG 98 mg/dL`.
  - placement: `x=124`, `y=168` (left side of center human graphic).
  - rendering style: `DrawTextUi` scale 1 with green foreground (`RGB565(124,255,124)`).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Repositioned and enlarged blood sugar indicator.
  - moved from left side to centered below human figure.
  - new placement computed from center geometry (`x = MAIN_CX - 0.75*text_width`, `y = MAIN_CY + 56`).
  - increased text size by ~50% via `DrawTextUi125` while keeping green color.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated center blood sugar text formatting/placement.
  - removed `BG` prefix; text now `98 mg/dL`.
  - increased size to scale 3 (100% larger than previous 1.5x style).
  - moved up by roughly two rows and centered in middle segment using `SECTION2_CX` anchor.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Reduced center glucose text size.
  - switched rendering from scale `3` to scale `2` and recomputed centering with scale-2 text width.
  - text content and vertical placement unchanged.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed glucose text clipping caused by pump text background redraw.
  - extracted glucose overlay into dedicated `DrawGlucoseIndicator()` helper.
  - moved glucose rendering to late-frame order (after `DrawMedicalOverlayData`) so pump background restore cannot overdraw it.
  - preserved glucose content/style/position while changing z-order only.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Reworked glucose behavior to be slow and stable.
  - glucose now constrained to `96..106 mg/dL`.
  - value changes by `1 mg/dL` steps every `90..180` seconds (random), yielding ~15-30 minute full-range trend movement.
  - added direction state with occasional reversals plus hard boundary reversal at 96/106.
  - moved glucose text to `y=252` to keep it outside high-frequency gyro refresh blit area and eliminate rapid flashing artifacts.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated orientation ball color for normal posture.
  - right-side-up ball color changed from light green to medium dark blue (`RGB565(46,102,190)`).
  - upside-down color threshold/path remains unchanged (light orange when `gAccelZmg < -200`).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Moved center-segment glucose `mg/dL` row up by two row heights in LCD renderer (`src/gauge_render.c`, `bg_y: 252 -> 224`).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Wired pump state to motor RPM scheduler in `src/gauge_render.c`:
  - motor STOP phase: `0 RPM` for random `10..20s`
  - motor RUN phase: non-zero RPM for random `5..10s`
  - pump status now follows motor phase (`PUMP IDLE` when stopped, `PUMP ACTIVE` when running)
  - pump `RATE` now uses randomized dosing value (`1..95 ML/H`) generated each RUN phase
  - current (`mA`) remains mapped from RPM in `0..95mA`
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Calibrated pump reservoir simulation to insulin dosing model in `src/gauge_render.c`:
  - reservoir starts at `92%` of a `3.0 mL` cartridge on boot
  - dose modeled in `U/h` (stored as milli-units per hour), converted with U100 rule (`100 U = 1 mL`)
  - displayed flow now uses precise `mL/h` formatting (`RATE:x.xxx ML/H`)
  - fill percentage now drains from computed dose flow instead of SOC placeholder
  - motor/pump run-idle windows remain wired (`STOP 10..20s`, `RUN 5..10s`), with pulse-compensated drain to preserve realistic average dose trend toward ~3-day depletion
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated motor simulation to better match insulin-pump delivery behavior in `src/gauge_render.c`:
  - replaced long run/idle windows with short micro-dosing motor pulses (`0.8..1.2s`)
  - pulse interval is computed from basal dose rate (`U/h`) and fixed micro-bolus volume (`0.02 U` per pulse on U100)
  - basal target remains in pump-style `0.025 U/h` increments and retunes every `20..40 min`
  - current demo profile keeps basal centered near `4 U/h` (`3..5 U/h`) to align with 3-day 3mL depletion behavior
  - reservoir drain now occurs from each delivered pulse (discrete delivery model), not synthetic time scaling
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Removed stray text below `ANOM` and updated left-side bargraph to reservoir fill behavior:
  - deleted `WEAR` row under `ANOM`
  - added explicit background restore band below `ANOM` to clear stale/random text artifacts
  - rewired left bargraph from temperature-driven level to `FILL%` level (`gUiReservoirPct`)
  - changed bargraph label to `FILL %` and made active bars red when fill is below `20%`
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Optimized glucose line redraw behavior in `src/gauge_render.c`:
  - `mg/dL` row is now refreshed only when glucose value changes
  - added cached previous glucose value (`gPrevGlucoseMgdl`) and reset hooks on renderer reinit paths to ensure correct first redraw
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Optimized glucose line redraw behavior in `src/gauge_render.c`:
  - `mg/dL` row is now refreshed only when glucose value changes
  - added cached previous glucose value (`gPrevGlucoseMgdl`) and reset hooks on renderer reinit paths to ensure correct first redraw
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Restored `WEAR` row under `ANOM` in motor overlay (`src/gauge_render.c`) while keeping prior anti-artifact background clear.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed disappearing `mg/dL` row by removing pump-region background refresh overlap with glucose text band:
  - adjusted pump blit region from `y=228..281` to `y=240..281` in `src/gauge_render.c`
  - keeps `mg/dL` redraw-on-change optimization while preventing erase by later draw passes
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Restored lower-left temperature `C/F` text in blue box and increased text size ~20% without changing fill-driven bargraph behavior:
  - left bargraph remains wired to `FILL%` level and low-fill red threshold
  - bottom-left label text switched back to `T xx.xC xxx.xF`
  - added `DrawTextUi120()` helper for ~20% larger temp label rendering
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed intermittent `mg/dL` disappearance and added spacing between anomaly rows:
  - narrowed left bargraph background-restore span from `BAR_X0 + 188` to `BAR_X0 + 170` so it no longer clips into the `mg/dL` text zone
  - moved `WEAR` row down for added vertical padding under `ANOM` (`y: 139 -> 143`)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed `mg/dL` row being erased by center-area background refresh:
  - in `DrawHumanOrientationPointer()`, split human-circle background restore into two blits and skipped the glucose text band (`y=220..242`)
  - preserves one-write-on-value-change behavior for `mg/dL` without center blanking side effects
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Recalibrated dosing so `ML/H` tracks motor `RPM` and `mA` behavior while preserving insulin pump math:
  - `RATE` now derived from instantaneous motor RPM (`ml/h = rpm * 0.002`), so idle motor shows `RATE:0.00 ML/H`
  - `mA` remains proportional to RPM via existing mapping, so dose/rpm/current now move together
  - basal target remains in `U/h` with `0.025 U/h` increments and uses U100 conversion (`100 U/mL`) for target volume pacing
  - pulse interval is now computed from delivered pulse volume and target `U/h`, so long-term reservoir depletion remains aligned with basal dosing profile
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added center-segment digital tach arc in `DrawHumanOrientationPointer()`:
  - 270-degree bargraph circle with bottom 90 degrees open
  - centered on the ball-circle center, radius set to 80% of ball-circle radius (matching requested 80% diameter scaling intent)
  - zone colors by sweep order: first ~120° green, next ~90° yellow, final ~60° red
  - active bar count driven by current motor RPM (`gUiRpmTenths`)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Updated center RPM tach arc geometry in `DrawHumanOrientationPointer()`:
  - vertically flipped arc orientation (open section moved opposite side)
  - doubled bar thickness (radial span and line thickness increased 2x)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok
