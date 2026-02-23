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
