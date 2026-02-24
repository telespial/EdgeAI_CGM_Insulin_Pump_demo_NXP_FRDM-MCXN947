# EdgeAI Medical Device Demo (NXP FRDM-MCXN947)

Last updated: 2026-02-23

Embedded medical-device style Edge AI demo for FRDM-MCXN947.

This project renders a full infusion-pump cockpit UI on the LCD, streams live sensors, supports record/replay from external flash, and runs an on-device inference layer for:
- motor and pump behavior visualization
- transport-aware human activity estimation
- glucose trend simulation
- insulin dose recommendation (demo logic)

## Documentation Sync (2026-02-23)

- Core docs were synchronized to the current medical-demo baseline.
- Settings UI documentation now reflects `AI: MCU/NPU` row format.
- Build/flash flow remains project-local and unchanged.

## Safety Notice

This repository is a development/demo system only. It is **not** a certified medical device and must not be used for real therapy decisions.

## What The Demo Does

### 1. On-screen Medical Cockpit
The LCD combines:
- motor zone (RPM, current, anomaly, wear)
- pump zone (state, flow rate, fill)
- human zone (orientation ball + activity arc)
- terminal/graph zone (live telemetry, mode/status)
- elapsed time + branding + controls (`PLAY/STOP`, `REC`, `LIVE`, `*`, `?`)

### 2. Live Sensor Fusion
The runtime fuses:
- accelerometer
- gyroscope
- barometric pressure
- board/aux temperature and humidity

These drive UI status and model behavior.

### 3. Auto Transport + Human Effort
The model outputs transport and effort separately:
- transport classes: `FOOT`, `SKATE`, `SCOOT`, `BIKE`, `CAR`, `AIR`
- effort stages: `REST`, `LIGHT`, `MODERATE`, `ACTIVE`, `HEAVY`

The circular activity bargraph is driven by effort intensity (0-100%).

### 4. Demo Insulin Recommendation Logic
The dose recommendation signal (`DOS`) is computed from:
- transport-aware effort
- glucose level and glucose trend (`dBG`)
- insulin-on-board (`IOB`) decay model

Behavior summary:
- rising BG trend / higher BG tends to increase recommendation
- falling BG trend / lower BG tends to reduce recommendation
- higher IOB applies safety down-scaling
- vehicle-like transport (car/air) reduces false effort inflation

### 5. Record / Replay in External Flash
The recorder stores and replays time-series including:
- accel/gyro/mag
- temperature/humidity
- barometric pressure
- anomaly metadata
- timestamps

## Runtime Display Semantics

### Terminal Highlights
- `TRN ...` = transport mode and confidence
- `ACT ...` = effort level/intensity
- `DOS ...` = recommended basal-equivalent dose (`U/h`)
- `IOB ...` = insulin-on-board estimate
- `dBG ...` = glucose trend (`mg/dL/min`)

### Pump/Motor Areas
- `RPM` and `mA` are linked
- pump `RATE` is tied to motor pulse activity
- `FILL` drains from modeled dose delivery over time

## Build and Flash

### Prerequisites
- Linux host with arm-none-eabi toolchain and LinkServer
- FRDM-MCXN947 connected over MCU-Link

### Commands
From project root:

```bash
./scripts/preflight.sh
./tools/build_frdmmcxn947.sh debug
./tools/flash_frdmmcxn947.sh
```

Alternative wrapper scripts:

```bash
./scripts/build.sh
./scripts/flash.sh
./scripts/build_and_flash.sh
```

## Restore Baseline

Current restore points:
- `GOLDEN-2026-02-24-R1`
- `FAILSAFE-2026-02-24-R1`

Staged restore artifacts:
- `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R1.bin`
- `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R1.bin`

Failsafe restore command:

```bash
./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R1.bin
```

## Controls

- `PLAY/STOP` timeline area: replay/live timeline control
- `REC` area: recording workflow
- `*` button: settings popup
- `?` button: on-screen help pages

## Key Source Files

- `src/gauge_render.c`
  LCD composition, overlays, activity/transport model, dose recommendation UI
- `src/main.c` and runtime app module in `src/`
  sensor polling, runtime orchestration, record/replay glue
- `src/ext_flash_recorder.c`
  external NOR flash record/replay persistence
- `src/power_data_source.c`
  replay/live data source handling

## Project Layout

- `src/` firmware sources
- `docs/` project state, runbook, command log, TODO, assets
- `tools/` build/flash/bootstrap helpers
- `scripts/` workflow wrappers
- `sdk_example/` MCUX overlay content
- `mcuxsdk_ws/` local generated build workspace

## Board Target

- Target board: `FRDM-MCXN947`
- Probe: `MCU-Link` (probe `#1` expected)

## License

MIT (`LICENSE`).
