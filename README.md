# EdgeAI Medical Device Demo (NXP FRDM-MCXN947)

Standalone framework for an MCXN947-based medical device edge AI demo.

## Current LCD Layout Milestone

Implemented first-pass on-screen layout scaffolding in `src/` with project-provided background image:

- New LCD layout module: `src/medical_lcd_layout.c` / `src/medical_lcd_layout.h`
- Background asset generated from `docs/pump.png`: `src/pump_bg.h`
- Platform draw abstraction: `src/medical_display_hal.h`
- Temporary host stub for compile/smoke test: `src/medical_display_stub.c`

Current layout intent:
- `NXP EDGEAI` title remains at top-center area.
- Elapsed time remains in bottom-center area.
- Right-hand third keeps graph panel + terminal panel.
- Left/center area now draws first-pass medical visuals:
  - motor block
  - chemical pump block
  - human figure/context block

This is a graphics-first milestone only; sensor and inference integration is next.

## Quick Start

1. Run preflight checks:
   `./scripts/preflight.sh`
2. Configure build/flash values in `docs/PROJECT_STATE.md`.
3. Build:
   `./scripts/build.sh`
4. Flash:
   `./scripts/flash.sh`

## Structure

- `src/`: Demo source code
- `docs/`: Operational docs and logs
- `scripts/`: Build/flash/state helpers
- `tools/`: Machine bootstrap helpers
- `sdk_example/`: SDK overlay area
- `mcuxsdk_ws/`: Local build workspace

## Board

- Target: `FRDM-MCXN947`
- Probe: `MCU-Link` (`#1` recommended)

## License

MIT (see `LICENSE`).
