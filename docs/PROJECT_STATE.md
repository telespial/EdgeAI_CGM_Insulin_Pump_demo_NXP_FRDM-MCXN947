# Project State

Last updated: 2026-02-24

## Restore Point
- Golden: `GOLDEN-2026-02-24-R2`
- Failsafe: `FAILSAFE-2026-02-24-R2`
- Status: active

## Current Status
- Project framework scaffold created.
- Build/flash workflow scripts added.
- Git repository initialized locally.

## Update 2026-02-23
- Change: Completed CGM alignment step 1 by adding stage-by-stage traceability against the dermal CGM review.
  - new doc: `docs/CGM_TRACEABILITY.md`
  - each review stage now tagged as `implemented`, `simulated`, or `missing` with code/doc evidence references
  - TODO step 1 marked complete in `docs/TODO.md`
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 2 by defining the runtime CGM tuple contract.
  - new doc: `docs/CGM_RUNTIME_CONTRACT.md`
  - defined canonical tuple fields:
    - `glucose_mgdl`
    - `trend_mgdl_min_x100` (fixed-point)
    - `sqi_pct`
    - `sensor_flags`
    - `prediction_15m_mgdl`
    - `prediction_30m_mgdl`
  - included units/ranges/sentinel behavior, update cadence, stale rules, and initial sensor flag bit assignments
  - TODO step 2 marked complete in `docs/TODO.md`
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 3 by defining sensor-status taxonomy and trigger logic.
  - new doc: `docs/CGM_SENSOR_FLAGS.md`
  - documented set/clear logic, debounce windows, and precedence for:
    - `CGM_FLAG_SATURATION`
    - `CGM_FLAG_DROPOUT`
    - `CGM_FLAG_IMPLAUSIBLE_ROC`
    - `CGM_FLAG_TEMP_OUT_OF_RANGE`
    - `CGM_FLAG_CAL_STALE`
    - `CGM_FLAG_DRIFT_WARN`
  - updated `docs/CGM_RUNTIME_CONTRACT.md` to reference detailed flag logic
  - TODO step 3 marked complete in `docs/TODO.md`
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 4 by publishing the 30-second cadence reference pipeline artifact.
  - new doc: `docs/CGM_30S_PIPELINE_REFERENCE.md`
  - includes:
    - end-to-end block diagram
    - equations-level pseudocode at 1 Hz internal / 30 s publish cadence
    - candidate filter constants for anti-alias, fast LP, derivative smoothing, impulse reject, notch
    - SQI feature set and threshold strategy
    - drift random-walk and conservative lag correction model structure
    - contract mapping to `cgm_tuple_t`
  - TODO step 4 marked complete in `docs/TODO.md`
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 5 by implementing raw-to-engineering preprocessing in firmware.
  - added preprocessing module:
    - `src/cgm_preprocess.h`
    - `src/cgm_preprocess.c`
  - preprocessing stages implemented:
    - ADC-domain conversion with offset/gain and quadratic linearization
    - reference-voltage compensation
    - anti-alias low-pass filtering
    - decimation
    - impulse-noise rejection (median-3)
    - configurable notch filtering
  - integrated into renderer glucose path (`src/gauge_render.c`) using a synthetic raw stream adapter so UI/dose logic now consumes preprocessed glucose output
  - updated SDK overlay sources to compile new module (`sdk_example/mcuxsdk_examples_overlay/demo_apps/edgeai_medical_device_demo/CMakeLists.txt`)
  - updated traceability status for stage 1 in `docs/CGM_TRACEABILITY.md`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Promoted current built/flashed runtime as the active golden/failsafe baseline and synchronized project docs.
  - built and flashed current firmware via `./scripts/build_and_flash.sh`
  - staged restore artifacts:
    - `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R3.bin`
    - `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R3.bin`
  - updated state/runbook/start/hardware/todo/readme/status documentation to reference R3 restore baseline
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Finalized higher-band label naming for consistent style.
  - `ACTIVE ACTIVITY` renamed to `HIGH ACTIVITY`
  - final/top band renamed from `HEAVY ACTIVITY` to `EXT ACTIVITY`
  - keeps post-`MOD` labels in compact same-format phrasing
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Renamed ACTIVE-stage alert label for wording consistency.
  - updated headline text from `ACTIVE ACTIVITY` to `HIGH ACTIVITY`
  - keeps naming progression aligned with `MOD ACTIVITY` and `HEAVY ACTIVITY`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Prevented long activity labels from overflowing the alert box.
  - added dynamic label-fit logic in `DrawAiAlertOverlay()`:
    - headline renders at scale-2 when it fits
    - automatically falls back to scale-1 if width exceeds alert box budget
  - fixes overflow for strings such as `MOD ACTIVITY` and other long labels
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Fixed alert headline overflow for long state labels (e.g., `MODERATE ACTIVITY`).
  - added width-aware auto-fit in `DrawAiAlertOverlay()`:
    - default headline scale remains 2
    - automatically falls back to scale 1 when text width exceeds alert box width budget
  - applies to both normal and warning/fault alert render paths
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Tuned REST coverage to occupy the first three ticks of the activity arc bargraph.
  - activity arc uses `54` ticks (5-degree segments over 270 degrees)
  - set LIGHT entry threshold to `6%` (`~3/54`) so `HUMAN REST` spans the first three ticks
  - kept LIGHT split behavior (`HUMAN MOVEMENT` / `HUMAN ACTIVE`) unchanged
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Made LIGHT-band triggering much earlier and advanced HUMAN ACTIVE transition.
  - LIGHT entry threshold reduced from `10` to `3` (~75% earlier trigger)
  - LIGHT split threshold reduced from `22` to `14` so `HUMAN ACTIVE` appears earlier
  - resulting LIGHT-band labels:
    - `HUMAN MOVEMENT` for `3..13`
    - `HUMAN ACTIVE` for `14..29`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Reduced HUMAN MOVEMENT trigger threshold so it engages earlier.
  - lowered LIGHT entry threshold in `ActivityStageFromPct()` from `13` to `10`
  - result: earlier transition from `HUMAN REST` to LIGHT-band labels (`HUMAN MOVEMENT` / `HUMAN ACTIVE`)
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Split LIGHT stage into two separate alert-scale labels (instead of LIGHT ACTIVITY wording).
  - lower LIGHT sub-threshold now shows `HUMAN MOVEMENT`
  - upper LIGHT sub-threshold now shows `HUMAN ACTIVE`
  - removed prior two-line LIGHT rendering and restored single-line threshold label behavior
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Split LIGHT activity headline into two-part human-movement wording in alert box.
  - for LIGHT stage, alert now renders two lines:
    - `HUMAN MOVEMENT`
    - `LIGHT ACTIVITY`
  - other stages continue using single-line headline labels
  - extended alert headline cache key size and logic to support two-line state redraw correctness
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Tuned human-activity stage sensitivity to make moderate/heavy states easier to enter.
  - adjusted `ActivityStageFromPct()` thresholds in `src/gauge_render.c` from:
    - `HEAVY 78`, `ACTIVE 58`, `MODERATE 38`, `LIGHT 16`
    - to `HEAVY 62`, `ACTIVE 46`, `MODERATE 30`, `LIGHT 13`
  - intent: approximately 20% easier transition into higher activity bands with no change to sensor-fusion inputs
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Reverted alert box width and reduced alert headline visual size.
  - restored alert width geometry to prior values (`ALERT_X0=147`, `ALERT_X1=325`)
  - switched headline draw path from shadowed text to crisp text at same scale for approximately 10% smaller perceived size
  - retained headline-change redraw caching fix to avoid stale glyph artifacts
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Improved alert box layout and state redraw stability.
  - increased alert-box width by ~5% (`ALERT_X0: 147 -> 142`, `ALERT_X1: 325 -> 330`)
  - added cached alert-headline tracking so headline changes force redraw and prevent stale/stray glyph artifacts
  - targeted issue: stray character appearing next to warning box during state transitions
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Refined alert headline phrasing to explicit human-state wording.
  - mapped alert labels to:
    - `HUMAN REST`
    - `LIGHT ACTIVITY`
    - `MODERATE ACTIVITY`
    - `ACTIVE ACTIVITY`
    - `HEAVY ACTIVITY`
  - implemented with new activity-headline string mapping in `src/gauge_render.c`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Updated alert box headline text to human-state mapping across all activity stages.
  - replaced fixed/system status headline with `HUMAN <state>` text derived from runtime activity stage
  - states now displayed in alert headline: `REST`, `LIGHT`, `MODERATE`, `ACTIVE`, `HEAVY`
  - applied in `src/gauge_render.c` `DrawAiAlertOverlay()` for both normal and warning/fault render paths
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Rebuilt and reflashed current medical demo firmware on FRDM-MCXN947.
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Reformatted CGM alignment TODO into explicit numbered execution steps.
  - updated `docs/TODO.md` from unordered checklist format to `1..12` numbered checklist format
  - preserved task content while improving planning readability and review handoff
- Result: ok

## Update 2026-02-23
- Change: Finalized medical-demo repo cleanup to eliminate package-demo crossover risk.
  - removed legacy package-demo app sources/overlay paths from active tree
  - retained medical-demo app paths/scripts as the sole build/flash target
  - confirmed repo scan has no package-demo keyword references in active files
- Verification:
  - `./scripts/preflight.sh` PASS
  - `./scripts/build_and_flash.sh` PASS (build + LinkServer flash, probe `#1`)
  - repo keyword scan PASS (`edgeai_package_transport*` not found in active files)
- Result: ok

## Update 2026-02-23
- Change: Replaced legacy TODO backlog with a direct CGM-alignment task list derived from `docs/CGM_DERMAL_ALGORITHM_REVIEW.md`.
  - cleared prior mixed historical checklist in `docs/TODO.md`
  - added explicit execution steps for CGM data contract, preprocessing, filtering, compensation, SQI/fault gating, prediction/alerts, UI semantics, and validation protocol
- Result: ok

## Update 2026-02-23
- Change: Completed hygiene and operations alignment pass for medical demo baseline.
  - sanitized project command-log wording to remove internal/root path naming in `docs/COMMAND_LOG.md`
  - synced stale TODO status to reflect implemented display/runtime items in `docs/TODO.md`
  - added CI workflow `/.github/workflows/build.yml` with:
    - `./scripts/preflight.sh`
    - shell syntax checks for core scripts/tools
    - host compile smoke test (`src/main.c` + `src/medical_lcd_layout.c` + `src/medical_display_stub.c`)
- Verification:
  - `./scripts/preflight.sh` PASS
  - `./scripts/build_and_flash.sh` PASS (build + LinkServer flash, probe `#1`)
  - local CI-equivalent run PASS (shell syntax + host compile/run)
- Result: ok

## Update 2026-02-23
- Change: Completed full documentation cleanup to remove legacy naming tied to earlier non-medical wording.
  - updated `README.md`, `docs/TODO.md`, `docs/PROJECT_STATE.md`, and SDK overlay readme text for medical-demo wording consistency
  - normalized prior command-log wording in `docs/COMMAND_LOG.md` where legacy naming appeared
- Change: Promoted current baseline as restore points:
  - Golden: `GOLDEN-2026-02-23-R2`
  - Failsafe: `FAILSAFE-2026-02-23-R2`
- Verification:
  - Documentation consistency scan across markdown files: PASS (no legacy legacy wording remains)
- Result: ok

## Update 2026-02-23
- Change: Shifted all settings rows with buttons left by ~3 character widths (18 px), keeping non-button rows unchanged.
  - Updated X-start constants in `src/gauge_render.h` for:
    - `MODE`, `RUN`, `SENS`, `AI`, `OPEN LIMITS`, `CLEAR FLASH`, and `LOG HZ` controls.
- Change: Added recorder memory usage indicator to the `FLASH` row.
  - Added `ExtFlashRecorder_GetUsageInfo(...)` API in `src/ext_flash_recorder.h/.c`.
  - Settings popup now renders `% USED` text next to a narrower `CLEAR FLASH` button in `src/gauge_render.c`.
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - Build output: `mcuxsdk_ws/build/<generated_firmware>.bin`
  - Flash target: `MCXN947:FRDM-MCXN947` on probe `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Shifted only the `LOOSE / NORM / STRICT` settings row right to avoid covering the `SENS` label.
  - Updated `GAUGE_RENDER_SET_TUNE_X0` in `src/gauge_render.h` from `96` to `180`.
  - No other settings-row geometry constants were changed.
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - Build output: `mcuxsdk_ws/build/<generated_firmware>.bin`
  - Flash target: `MCXN947:FRDM-MCXN947` on probe `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Removed colon from AI settings row label text:
  - `AI:` -> `AI` in `src/gauge_render.c` so settings now reads `AI MCU NPU`.
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - Build output: `mcuxsdk_ws/build/<generated_firmware>.bin`
  - Flash target: `MCXN947:FRDM-MCXN947` on probe `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Fixed settings-row layout so `AI: MCU NPU` renders cleanly without label overlap:
  - moved AI row button start x-coordinate to align with other settings rows (`GAUGE_RENDER_SET_AI_X0: 144 -> 184` in `src/gauge_render.h`).
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - Build output: `mcuxsdk_ws/build/<generated_firmware>.bin`
  - Flash target: `MCXN947:FRDM-MCXN947` on probe `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Hardware Setup
- Board: FRDM-MCXN947
- Probe: MCU-Link CMSIS-DAP (use `#1`)
- Debug USB: J17

## Build/Flash Config
- Build Command: ./tools/build_frdmmcxn947.sh debug
- Flash Command: /usr/local/LinkServer/LinkServer flash --probe '#1' MCXN947:FRDM-MCXN947 load --addr 0x0 {BIN_PATH}
- Binary: mcuxsdk_ws/build/<generated_firmware>.bin
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
    - output binary: `mcuxsdk_ws/build/<generated_firmware>.bin`
  - Flash PASS:
    - `./tools/flash_frdmmcxn947.sh`
    - runner: `linkserver`, probe `#1`, target `MCXN947:FRDM-MCXN947`
- Notes:
  - `west update` in setup reported non-critical failures for unrelated repos; required demo build/flash path still completed successfully.
- Result: ok

## Update 2026-02-22
- Change: Restored reference visual baseline for LCD rendering behavior in medical firmware:
  - same `NXP EDGEAI` placement/color/size as reference baseline
  - same elapsed-time placement/color/size as reference baseline
  - right-third timeline buttons (`PLAY/STOP`, `REC`, `LIVE`), graph, and terminal kept in reference style/flow
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
- Change: Applied requested medical UI refinements on top of reference-style baseline:
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

## Update 2026-02-23
- Change: Refined center tach arc appearance:
  - increased arc diameter by 10% over prior setting (radius factor `0.80 -> 0.88` of ball-circle radius)
  - sharpened bars by reducing line thickness (`4 -> 3`) and tightening radial span (`-10 -> -8`)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Increased center tach arc diameter by an additional 10% (radius factor `0.88 -> 0.97` of ball-circle radius).
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Reworked displayed error/reason/status coding to activity staging derived from motion/environment sensors:
  - activity model now fuses accelerometer dynamics, gyro magnitude, and barometric pressure-rate trend
  - stage bands: `REST`, `LIGHT`, `MODERATE`, `ACTIVE`, `HEAVY`
  - reason/detail text rewritten to activity codes (`ACT-L0..ACT-L4`) plus movement/vertical trend context
  - terminal status/system rows now report activity state and activity code instead of prior alert-reason strings
- Change: Circular 270-degree center bargraph now represents activity intensity (0-100%) instead of motor RPM while keeping requested color zones and geometry.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS (after one compile-fix iteration)
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added barometric pressure to the right-hand scope graph overlay:
  - plotted `gTraceBaro` as an additional line in `TRACE_BARO_COLOR`
  - added `BP` in the scope legend row
- Change: Fixed blue orientation-ball trail artifact at the bottom of the center circle:
  - full center-circle background region is now restored each frame before ball redraw
  - glucose `mg/dL` text is redrawn each frame so the full restore does not blank it
- Change: Flash recorder barometric logging/playback path verified:
  - `ExtFlashRecorder_AppendSampleEx(... baro_dhpa ...)` and playback restore path were already wired and retained
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Implemented automatic transport-mode distinction to separate human effort from vehicle motion:
  - added auto classifier outputs: `FOOT`, `SKATE`, `SCOOT`, `BIKE`, `CAR`, `AIR`
  - classifier uses fused accel dynamics, gyro magnitude, and barometric pressure-rate trend
  - added transport confidence output (`gTransportConfidencePct`)
- Change: Human activity scoring is now transport-aware:
  - vehicle-like modes (`CAR`, `AIR`) downscale effort contribution to avoid vibration-driven false high activity
  - active personal mobility modes (`BIKE`, `SCOOT`, `SKATE`) apply lighter downscaling to preserve real rider effort
- Change: Terminal now shows auto transport context and confidence inline with activity (`TRN ... ACT ...`).
- Operation mode:
  - fully automatic by default; no user setting required for this behavior.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Fixed activity arc baseline so resting state maps to early green region instead of yellow.
  - corrected activity accel feature to primarily use accel-magnitude deviation from `1g` (rest baseline) rather than raw gravity-contaminated magnitude
  - added guard for linear-accel streams that still include gravity
  - tightened rest clamp (`low accel + low gyro + low baro trend`) to suppress false motion
  - adjusted transport gating so very low-motion rest no longer auto-classifies as `CAR`
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Added demo insulin recommendation controller and wired it into runtime dosing behavior.
  - recommendation uses fused inputs: transport-aware activity stage, glucose value, glucose trend (`dBG`), and insulin-on-board (`IOB`) with time decay
  - recommendation is quantized to 0.025 U/h increments and smoothed before being applied to pulse scheduling
  - terminal now shows recommendation telemetry when help popup is closed: `DOS`, `IOB`, and `dBG`
- Change: Updated on-screen `?` help popup content (both pages) to document the new transport/activity and dose recommendation system behavior.
- Change: Rewrote `README.md` with detailed project documentation:
  - safety note, system architecture, runtime semantics, transport/activity model, dose recommendation model, record/replay details, controls, and build/flash instructions
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Dated project documentation files and established current release as active restore baseline.
  - added `Last updated: 2026-02-23` to core docs and README
  - updated `STATUS.md` with explicit restore markers
  - set restore-point identifiers in project state:
    - `GOLDEN-2026-02-23`
    - `FAILSAFE-2026-02-23`
- Result: ok

## Update 2026-02-23
- Change: Flashed current medical demo firmware image to FRDM-MCXN947.
- Verification:
  - `./tools/flash_frdmmcxn947.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Shifted all settings row labels left by 3 character widths for improved separation from row buttons.
  - updated `DrawSettingsPopup()` label anchor (`label_col_right`) from `172` to `154`
  - this applies uniformly to all settings labels while leaving row option content unchanged
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - build: PASS
  - flash: PASS (`MCXN947:FRDM-MCXN947`, probe `#1` `UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Fixed AI settings row label placement/format so the left-side row label is explicitly `AI:` before the `MCU` and `NPU` buttons.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Synchronized all project docs to current medical-demo baseline.
  - updated `README.md` and `STATUS.md`
  - updated `docs/START_HERE.md`, `docs/OPS_RUNBOOK.md`, `docs/HARDWARE_SETUP.md`, and `docs/TODO.md`
  - aligned docs with settings UI wording `AI: MCU/NPU`
- Result: ok

## Update 2026-02-23
- Change: Flashed current medical demo firmware image to FRDM-MCXN947.
- Verification:
  - `./tools/flash_frdmmcxn947.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Replaced AI mode wording from ON/OFF to MCU/NPU across the runtime UI and settings popup.
  - top mode pill now shows `AI MCU` (left selection) or `AI NPU` (right selection)
  - settings popup AI buttons now read `AI: MCU` on the left and `AI: NPU` on the right
- Change: Updated selection event log output in touch handler:
  - left selection prints `AI_SET,MCU`
  - right selection prints `AI_SET,NPU`
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (LinkServer, probe `#1`)
- Result: ok

## Update 2026-02-23
- Change: Adjusted AI settings row format to match other settings style.
  - kept row label as `AI`
  - changed option button text to `MCU` (left) and `NPU` (right)
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
- Result: ok

## Update 2026-02-23
- Change: Flashed current medical demo firmware image to FRDM-MCXN947.
- Verification:
  - `./tools/flash_frdmmcxn947.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Restored runtime target after wrong-project flash by rebuilding/flashing the medical demo.
- Verification:
  - `./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`, target `MCXN947:FRDM-MCXN947`)
- Result: ok

## Update 2026-02-23
- Change: Removed legacy package-demo naming from medical project code/build paths to prevent project confusion.
  - renamed runtime source file to `src/edgeai_medical_device_demo.c`
  - renamed MCUX overlay app path to `demo_apps/edgeai_medical_device_demo`
  - updated build/install scripts and environment variable names to medical-demo identifiers
  - updated generated model name defaults from package naming to medical naming
- Verification:
  - `EDGEAI_WEST_BUILD_ARGS='-p always' ./tools/build_frdmmcxn947.sh debug` PASS
  - output: `mcuxsdk_ws/build/edgeai_medical_device_demo_cm33_core0.bin`
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`, target `MCXN947:FRDM-MCXN947`)
- Result: ok

## Update 2026-02-23
- Change: Added CGM algorithm review document to medical demo docs.
  - new file: `docs/CGM_DERMAL_ALGORITHM_REVIEW.md`
  - content covers dermal CGM preprocessing, filtering, calibration/compensation, lag handling, SQI/fault detection, prediction/alerts, and validation metrics
- Result: ok

## Update 2026-02-23
- Change: Restored board runtime to medical demo after accidental package-demo flash.
- Verification:
  - `EDGEAI_WEST_BUILD_ARGS='-p always' ./tools/build_frdmmcxn947.sh debug` PASS
  - `./tools/flash_frdmmcxn947.sh` PASS (probe `#1`, target `MCXN947:FRDM-MCXN947`)
- Result: ok

## Update 2026-02-23
- Change: Added explicit demo-CGM simulation wording in help popup.
  - updated page-1 help text in `src/gauge_render.c` to state CGM data and dosing response are simulated
- Change: Added flash fail-fast guard and safe wrapper.
  - `tools/flash_frdmmcxn947.sh` now blocks flash if expected medical artifact is missing or build dir app path does not match project
  - new wrapper: `scripts/flash_safe.sh`
  - `scripts/flash.sh` now delegates to `scripts/flash_safe.sh`
- Verification:
  - `EDGEAI_WEST_BUILD_ARGS='-p always' ./tools/build_frdmmcxn947.sh debug` PASS
  - `./scripts/flash_safe.sh` PASS (probe `#1`, target `MCXN947:FRDM-MCXN947`)
- Result: ok

## Update 2026-02-23
- Change: Tuned activity stage entry so `HUMAN REST` spans first ~5 bargraph ticks.
  - `src/gauge_render.c`: `ActivityStageFromPct()` LIGHT threshold set to `pct >= 10` (REST below 10)
- Verification:
  - `./scripts/build_and_flash.sh` PASS (probe `#1`, target `MCXN947:FRDM-MCXN947`)
- Result: ok

## Update 2026-02-23
- Change: Unified alert headline rendering style for warning/fault states (including `HIGH ACTIVITY`) with normal alerts.
  - `src/gauge_render.c` in `DrawAiAlertOverlay()` now renders warning/fault headline at the same Y offset/scale path as normal alerts.
  - removed secondary detail line from warning/fault alert box to keep single-line headline presentation.
- Verification:
  - `./scripts/build_and_flash.sh` PASS (probe `#1`, target `MCXN947:FRDM-MCXN947`)
- Result: ok

## Update 2026-02-23
- Change: Committed and published R3 restore baseline.
  - commit: `067e438` on `main`
  - pushed tags:
    - `GOLDEN-2026-02-23-R3`
    - `FAILSAFE-2026-02-23-R3`
- Result: ok

## Update 2026-02-23
- Change: Verification test run requested after step-5 preprocessing integration.
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 6 by implementing a dermal-tuned low-lag filtering/trend path with adaptive SQI gating.
  - `src/cgm_preprocess.c`:
    - added SQI estimation from preprocessing residual-noise EMA
    - added SQI-adaptive glucose smoothing alpha (`filter_alpha_min..max`)
    - added explicit trend derivative output (`mg/dL/min`) with adaptive trend smoothing
  - `src/gauge_render.c`:
    - switched runtime trend consumption to preprocessor trend output
    - removed legacy trend-decay recompute path in dose loop
    - exposed `SQI` in terminal status line with `DOS/IOB/dBG`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Promoted current step-6 firmware/docs state as active golden restore baseline `R4` only.
  - staged artifact:
    - `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R4.bin`
  - failsafe baseline intentionally remains:
    - `FAILSAFE-2026-02-23-R3`
    - `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R3.bin`
  - updated active restore references in:
    - `README.md`, `STATUS.md`
    - `docs/START_HERE.md`, `docs/OPS_RUNBOOK.md`, `docs/HARDWARE_SETUP.md`, `docs/TODO.md`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
- Result: ok

## Update 2026-02-23
- Change: Published step-6 implementation and golden-only restore promotion.
  - commit: `d7e2deb` on `main`
  - pushed tag: `GOLDEN-2026-02-23-R4`
  - failsafe tag intentionally unchanged: `FAILSAFE-2026-02-23-R3`
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 7 by implementing calibration and compensation path in preprocessing.
  - `src/cgm_preprocess.h/.c` updates:
    - runtime factory-calibration hooks: `CgmPreprocess_SetCalibration(...)`, `CgmPreprocess_ResetCalibrationAge(...)`
    - temperature compensation retained in calibrated glucose conversion path
    - aging/drift compensation state added (`drift_state_mgdl`) with bounded adaptive correction
    - sensitivity-change detection added using short/long EMA ratio with hold counter
    - output telemetry fields added: calibration age, drift compensation, calibration-stale/drift-warn/sensitivity-change flags
  - `src/gauge_render.c` integration:
    - consumes updated preprocessing outputs during glucose/trend path
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 8 with conservative lag/kinetics handling.
  - `src/cgm_preprocess.h/.c` updates:
    - added optional lag/kinetics configuration and state (`lag_enable`, `lag_tau_s`, `lag_gain_min/max`, bounded lag compensation)
    - implemented low-gain lag correction with SQI-adaptive gain and compensation clamp
    - trend derivative now computed from kinetics-corrected glucose signal
    - exposed lag diagnostics in preprocessing output (`lag_comp_mgdl`, `lag_gain_applied`)
  - behavior remains conservative by default (no aggressive look-forward extrapolation)
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 9 by adding SQI/fault gating as a runtime control path.
  - `src/cgm_preprocess.h/.c` updates:
    - added runtime `sensor_flags` output and bit taxonomy constants
    - added flag derivation for saturation, dropout, implausible ROC, temp out-of-range, calibration stale, and drift warning
    - added `prediction_blocked` and `hold_last` outputs to gate downstream logic when SQI/fault state is poor
  - `src/gauge_render.c` integration:
    - consumes preprocessing `sensor_flags`, `prediction_blocked`, and `hold_last`
    - applies dropout hold-last guard for glucose/trend presentation path
    - blocks/de-rates dose recommendation updates when prediction path is gated
    - exposes flags in terminal line (`Fxx`) with SQI for runtime visibility
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Ran additional runtime verification cycle on current CGM step-9 firmware.
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Promoted current firmware/docs runtime to golden restore baseline `R5` (failsafe remains `R3`).
  - staged artifact:
    - `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
  - active restore points:
    - `GOLDEN-2026-02-23-R5`
    - `FAILSAFE-2026-02-23-R3`
  - synchronized baseline references in:
    - `README.md`, `STATUS.md`
    - `docs/START_HERE.md`, `docs/OPS_RUNBOOK.md`, `docs/TODO.md`
- Verification:
  - source binary copied from latest successful build:
    - `mcuxsdk_ws/build/edgeai_medical_device_demo_cm33_core0.bin`
- Result: ok

## Update 2026-02-23
- Change: Completed CGM alignment step 10 with prediction and alert runtime behavior.
  - `src/gauge_render.c` updates:
    - added short-horizon prediction outputs (`P15`, `P30`) from current glucose and trend slope
    - added SQI/fault-gated predictive hypo/hyper checks (warn/fault bands)
    - implemented debounce and hysteresis for both hypo and hyper paths to reduce alert chatter
    - integrated predictive alert state into overlay/terminal AI status selection
    - integrated predictive alert direction/severity into dose recommendation scaling
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-23
- Change: Added detailed CGM AI explainer documentation for runtime behavior and industry-standard methods.
  - new doc: `docs/CGM_AI_ALGORITHM_EXPLAINER.md`
  - includes:
    - end-to-end runtime pipeline explanation from preprocessing to alerting
    - exact current prediction equations/thresholds/debounce/hysteresis from firmware
    - UI/alert semantics and dose interaction behavior
    - common CGM-market medical concepts and software/math patterns
    - clear separation between demo-specific behavior and production-grade expectations
- Result: ok

## Update 2026-02-24
- Change: Built/flashed current firmware and promoted both golden and failsafe restore baselines to `R1` for 2026-02-24.
  - built and flashed via `./scripts/build_and_flash.sh`
  - staged restore artifacts:
    - `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
    - `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R1.bin`
  - active restore points:
    - `GOLDEN-2026-02-24-R1`
    - `FAILSAFE-2026-02-24-R1`
  - synchronized active baseline references in:
    - `README.md`, `STATUS.md`
    - `docs/START_HERE.md`, `docs/OPS_RUNBOOK.md`, `docs/HARDWARE_SETUP.md`, `docs/TODO.md`
  - included CGM documentation update:
    - `docs/CGM_AI_ALGORITHM_EXPLAINER.md`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Completed CGM alignment step 11 by aligning LCD/terminal semantics to the runtime CGM contract.
  - `src/gauge_render.c` updates:
    - added explicit simulation labeling in CGM UI paths (`SIM CGM`)
    - added on-screen prediction visibility (`P15`/`P30`) under glucose display
    - added terminal SQI confidence coding (`HI`/`MED`/`LOW`/`BAD`) and active sensor flag display (`Fxx`)
    - aligned terminal summary lines to contract fields (predictions + trend with simulation marker)
  - compile hygiene:
    - marked now-unused `FormatDewAltCompact()` helper with `__attribute__((unused))` to satisfy `-Werror`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Increased on-screen simulated CGM summary text size in glucose panel.
  - `src/gauge_render.c`:
    - changed glucose-panel simulated summary text from scale `1` to scale `2`
    - updated label string from `SIM CGM ...` to `SIM ...` to preserve fit at larger size
    - build fix: marked `FormatDewAltCompact()` as `__attribute__((unused))` after prior line replacement to satisfy `-Werror`
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Adjusted simulated CGM summary text to fit within middle LCD segment.
  - `src/gauge_render.c`:
    - set center-panel `SIM P15/P30` summary render scale from `2` back to `1` to prevent overflow in middle segment
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Increased simulated CGM summary text slightly while preserving middle-segment fit.
  - `src/gauge_render.c`:
    - center `SIM P15/P30` line now uses scale-1 pseudo-1.25x render (double-draw with +1px offset)
    - keeps horizontal fit in the middle segment while improving readability
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Increased center simulated CGM summary text a little more while preserving middle-segment fit.
  - `src/gauge_render.c`:
    - `SIM P15/P30` line now uses 3-pass pseudo-scale render (base + x+1 + y+1 at scale-1)
    - improves readability relative to single-pass scale-1 without full scale-2 overflow
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Improved center simulated CGM text appearance with cleaner font rendering.
  - `src/gauge_render.c`:
    - switched `SIM P15/P30` line from pseudo-thick multi-pass draw to `DrawTextUiCrisp(...)`
    - keeps same size/fit but improves legibility and visual cleanliness
- Verification:
  - `./scripts/build_and_flash.sh` PASS
  - target: `MCXN947:FRDM-MCXN947`
  - probe: `#1` (`UYLKOJI11H2B3`)
- Result: ok

## Update 2026-02-24
- Change: Reduced center mg/dL + SIM flicker by throttling redraw to 5-second cadence with cache invalidation on static/dashboard refresh paths.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Fixed center-text blanking: mg/dL and SIM lines now stay visible continuously while displayed values refresh every 5 seconds.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Fixed center mg/dL/SIM overdraw artifacts by restoring the center text strip every frame before drawing the two lines; retained 5-second value refresh cadence.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Moved center mg/dL + SIM lines upward to keep them fully outside the pump/elapsed clear bands; this isolates center text from y=240 region wipes.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Enabled deterministic full-frame redraw compositing each refresh to eliminate section overdraw/dirty-region flicker; center CGM text remains final overlay.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Reverted full-frame redraw mode due to whole-screen flashing and missing fields; restored incremental rendering while keeping center CGM text protections.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Adjusted center CGM text rendering to remove black bars/boxes: switched strip clear to background blit and mg/dL text to crisp draw (no shadow).
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Refined center CGM redraw to professional minimal dirty-rect behavior: only clears a few pixels around old/new text bounds before redraw; removed wide strip clearing.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Isolated center mg/dL/SIM region from activity/ball renderer by adding a protected guard rectangle in orientation ring background restore and segment draw paths.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Cleared residual ball/activity artifacts in center text zone by forcing per-frame background restore of the protected guard rectangle before overlay text redraw.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Updated rendering strategy per request: ball/ring redraws normally, and center CGM text is re-drawn as final persistent overlay each frame with minimal dirty-clear only when text changes.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Adopted permanent center-text overlay strategy: render ball/ring normally, then every frame restore a tight text-width background box and redraw mg/dL+SIM immediately (no broad clears, no guard carveouts).
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Replaced center text clear+draw flow with masked opaque glyph redraw (fg+bg per character cell) to avoid broad clears and reduce flicker/tearing.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Implemented true masked center text redraw using pump-background-per-glyph cells (no black fill box) and fixed declaration-order compile issue.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Removed center-text background-cell rewrite path; mg/dL and SIM now render as transparent final overlay only to avoid black box and bargraph masking.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Test mode: removed all center-text background restoration/masking; mg/dL and SIM are now pure transparent final overlays to isolate remaining 1 Hz flicker source.
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Removed activity bargraph/orientation ring draw from the runtime frame path for test isolation.
  - disabled activity renderer invocation in `GaugeRender_DrawFrame()`
  - retained renderer function source but marked `DrawHumanOrientationPointer()` as intentionally unused for clean `-Werror` builds
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Restored activity ball/orientation ring rendering in runtime frame path.
  - re-enabled `DrawHumanOrientationPointer(style)` in `GaugeRender_DrawFrame()`
  - removed temporary `__attribute__((unused))` from `DrawHumanOrientationPointer()`
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Restored orientation ball marker rendering inside `DrawHumanOrientationPointer()`.
  - removed temporary test-disable block that suppressed `ball_x/ball_y/ball_color`
  - ball now draws as a filled marker with outline + highlight on top of the activity arc
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Stabilized MG/DL + SIM rendering with activity ball/bargraph active.
  - `DrawGlucoseIndicator()` now restores only a tight background band behind center text before redraw (prevents text smearing)
  - `DrawHumanOrientationPointer()` now skips arc segments and ball draw that intersect the center text guard zone (prevents overdraw/flicker in text rows)
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Adjusted center-text/ball interaction to address disappearing ball and persistent flashing.
  - removed orientation-layer center guard clipping so ball never disappears when crossing text region
  - changed `DrawGlucoseIndicator()` band clear policy to clear only when displayed values change (still redraws text each frame)
  - outcome goal: no text smear on value updates and reduced frame-to-frame flash in center text rows
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Switched elapsed runtime clock source to MCU hardware RTC (IRTC).
  - added `fsl_irtc` driver integration to project build
  - initialized IRTC at boot and captured RTC epoch on each elapsed-clock reset event
  - runtime elapsed display now computes `elapsed_sec` from RTC wall-time delta; timebase path remains fallback if RTC read fails
  - retained existing UI reset semantics for playback/record transitions
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Rolled back the recent MCU IRTC integration due to boot freeze and restored prior runtime/elapsed clock implementation.
  - restored `src/edgeai_medical_device_demo.c` to HEAD baseline (pre-IRTC integration)
  - restored `sdk_example/mcuxsdk_examples_overlay/demo_apps/edgeai_medical_device_demo/CMakeLists.txt` to HEAD baseline (removed explicit IRTC driver add)
  - this returns elapsed/logging flow to the previous known-good logic path
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Restored the previously working counter-read elapsed-time path (for runtime clock + logging cohesion).
  - reintroduced OSTIMER source calibration against delay window for both raw and decoded counters
  - runtime now selects the closer source (`OSTIMER_GetCurrentTimerRawValue` vs `OSTIMER_GetCurrentTimerValue`) and uses matched `s_timebase_hz`
  - no MCU IRTC integration in this change; existing runtime/logging architecture preserved
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Prevented elapsed clock display from counting up/down due small timing jitter.
  - added bounded anti-backstep guard (<=3 s) in normal, record, and playback display update paths
  - keeps existing mode-reset behavior intact (`runtime_displayed_sec = UINT32_MAX` still allows intentional resets)
- Command: ./scripts/build_and_flash.sh
- Result: ok

## Update 2026-02-24
- Change: Restored device firmware to active Golden restore point.
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
- Result: ok

## Update 2026-02-24
- Change: Restored one step back to previous Golden firmware image.
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
- Result: ok

## Update 2026-02-24
- Change: Restored one more step back to earlier Golden firmware image.
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R3.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R3.bin`
- Result: ok

## Update 2026-02-24
- Change: Restored one more step back to failsafe firmware baseline.
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R3.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R3.bin`
- Result: ok

## Update 2026-02-24
- Change: Returned from failsafe baseline back to Golden restore track (latest Golden).
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
- Result: ok

## Update 2026-02-24
- Change: Rolled back two more Golden restore points.
  - step 1 flashed: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
  - step 2 flashed: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R4.bin`
  - commands:
    - `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
    - `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R4.bin`
- Result: ok

## Update 2026-02-24
- Change: Rolled back three more restore points.
  - step 1 flashed: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R3.bin`
  - step 2 flashed: `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R4.bin`
  - step 3 flashed: `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R3.bin`
  - commands:
    - `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R3.bin`
    - `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R4.bin`
    - `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-23-R3.bin`
- Result: ok

## Update 2026-02-24
- Change: Moved forward to most recent Golden restore point.
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
- Result: ok

## Update 2026-02-24
- Change: Restored firmware to Golden point `R5` (pre-rollback stable target).
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-23-R5.bin`
- Result: ok

## Update 2026-02-24
- Change: Flashed most recent failsafe restore image for validation.
  - flashed artifact: `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R1.bin`
  - command: `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R1.bin`
- Result: ok

## Update 2026-02-24
- Change: Promoted current source-built runtime as new restore baseline `R2` and flashed active Golden `R2`.
  - built+flashed current source: `./scripts/build_and_flash.sh`
  - staged artifacts:
    - `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R2.bin`
    - `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R2.bin`
  - activated on hardware:
    - `./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R2.bin`
- Result: ok

## Update 2026-02-24
- Change: Restored runtime behavior path where center `mg/dL` + `SIM` lines are present while activity ball/bargraph are active by rebuilding/flashing from current source (not a saved restore artifact).
  - note: the exact historical point before ball-removal tests was a runtime iteration and was not captured as a standalone failsafe/golden `.bin`.
  - command: `./scripts/build_and_flash.sh`
- Result: ok
