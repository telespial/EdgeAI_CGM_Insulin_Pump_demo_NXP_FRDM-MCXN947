# EdgeAI Medical Device Demo (NXP FRDM-MCXN947)

Standalone framework for an MCXN947-based medical device edge AI demo.

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
