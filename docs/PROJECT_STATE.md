# Project State

Last updated: 2026-02-22

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
