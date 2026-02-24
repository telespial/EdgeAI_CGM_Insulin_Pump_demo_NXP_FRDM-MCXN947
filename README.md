# EdgeAI CGM Insulin Pump Demo (NXP FRDM-MCXN947)

Last updated: 2026-02-24

Firmware demo for an embedded CGM + insulin-pump UX on FRDM-MCXN947 with a model-assisted prediction layer, replay benchmarking, and safety-gated alerting.

## Safety Notice

This repository is an engineering prototype and evaluation demo. It is **not** a cleared or approved medical device and must not be used for clinical treatment decisions.

## What This Project Demonstrates

- Real-time embedded UI for a pump-style cockpit.
- CGM preprocessing and short-horizon prediction (`+15m`, `+30m` display).
- Prediction-driven hypo/hyper warning integration.
- Replay-driven benchmarking with on-device scoring.
- Clear separation between host firmware controls and model inference logic.

## Embedded Intelligence Layer (EIL) Architecture

The Embedded Intelligence Layer is the model/inference subsystem that sits on top of deterministic host firmware.

### Host Firmware Responsibilities

- Sensor ingestion and data integrity checks.
- Timebase, replay control, storage, and deterministic scheduling.
- UI rendering and user interaction.
- Hard gating and safe fallback behavior (AI enable/disable, prediction validity gates).

### EIL Responsibilities

- Feature preparation from CGM stream state.
- Model inference for short-horizon predictions.
- Confidence-linked prediction behavior.
- Prediction signal handoff to warning/overlay logic.

### Why this split improves ease-of-use and safety

- Firmware remains debuggable and deterministic even when models change.
- Model upgrades can be evaluated without rewriting control/timing code.
- Safety checks remain in firmware, independent of model internals.
- Operators can disable AI while preserving core UI/runtime functionality.

## Model Summary (Current Runtime)

- Runtime focus: CGM prediction support.
- Primary scored horizon: `+15m`.
- Display-only secondary horizon: `+30m`.
- On-screen center format:
  - `PRED 15M:xxx 30M:xxx`
- AI off placeholder:
  - `PRED 15M:--- 30M:---`

Scoring policy (current baseline):
- prediction score uses `+15m` only
- hit tolerance: `±10%`
- `+30m` remains displayed but excluded from hit-rate scoring until a dedicated +30 model policy is promoted

## Runtime Flow

1. Boot and initialize runtime + display.
2. Load/replay dataset path and CGM processing chain.
3. Run model inference on schedule.
4. Render predictions and warning state.
5. Evaluate prediction quality over replay and show score/MAE indicators.

## Build and Flash

### Prerequisites

- Linux host
- Arm GNU toolchain
- LinkServer installed and accessible in `PATH`
- FRDM-MCXN947 connected through MCU-Link

### Standard Commands

```bash
./scripts/preflight.sh
./scripts/build.sh
./scripts/flash.sh
```

One-step:

```bash
./scripts/build_and_flash.sh
```

Direct board tooling:

```bash
./tools/build_frdmmcxn947.sh debug
./tools/flash_frdmmcxn947.sh
```

## Active Restore Baseline

- Golden: `GOLDEN-2026-02-24-R6`
- Failsafe: `FAILSAFE-2026-02-24-R6`

Artifacts:
- `failsafe/edgeai_medical_device_demo_cm33_core0_golden_2026-02-24-R6.bin`
- `failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R6.bin`

Failsafe restore:

```bash
./tools/flash_failsafe.sh failsafe/edgeai_medical_device_demo_cm33_core0_failsafe_2026-02-24-R6.bin
```

## Controls

- Timeline area: `LIVE`, `PLAY/STOP`
- `REC` control: recording workflow
- `*`: settings
- `?`: help

## Key Files

- `src/edgeai_medical_device_demo.c`
  runtime orchestration, replay/timing, firmware control flow
- `src/cgm_preprocess.c`
  CGM preprocessing + prediction interface
- `src/gauge_render.c`
  LCD composition, overlays, prediction text, score visuals
- `src/ext_flash_recorder.c`
  external flash recording/replay persistence
- `docs/PROJECT_STATE.md`
  authoritative change and verification history
- `docs/OPS_RUNBOOK.md`
  operational command baseline

## Repository Layout

- `src/` firmware source
- `docs/` state/runbook/history/TODO
- `scripts/` common workflow commands
- `tools/` board/toolchain helpers
- `failsafe/` staged restore binaries
- `model/` model export artifacts
- `mcuxsdk_ws/` generated build workspace

## License

MIT (`LICENSE`)
