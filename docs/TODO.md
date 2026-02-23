# TODO

Last updated: 2026-02-23

## Priority 0 (LCD First Slice)
- [x] Create first-pass LCD layout with same background image.
- [x] Keep `NXP EDGEAI` and elapsed-time positions stable.
- [x] Keep graph + terminal in right-hand screen third.
- [x] Add medical visuals in main canvas:
  - motor block
  - chemical pump block
  - human figure/context block
- [x] Replace background with project image `docs/pump.png`.
- [x] Anchor active labels around real background icon locations (motor/pump/human) instead of placeholder icon boxes.
- [x] Add human gyro ring pointer around center circle area.
- [x] Remove legacy gyro sphere and accel box/ball visuals from LCD path.
- [x] Add motor on-region active labels (`RPM`, `current`, `load`, `anomaly` bar).
- [x] Add pump on-region active labels (`rate`, `fill`, `state`) and fill-level visualization.
- [x] Match package demo style/placement for `NXP EDGEAI`, elapsed time, and right-third controls/graph/terminal.
- [x] Remove overlay panel boxes/borders; keep medical labels transparent over icon regions only.
- [ ] Tune overlay coordinates/contrast on real LCD hardware for final readability.

## Priority 1 (Display Integration)
- [ ] Replace stub display HAL with FRDM-MCXN947 LCD driver bindings.
- [ ] Wire terminal fields to live telemetry channels (motor/temp/activity/alerts).
- [ ] Wire graph panel to selected runtime channels with scrolling buffer.

## Priority 2 (Telemetry + Intelligence)
- [ ] Define medical-device telemetry schema.
- [ ] Add sensor drivers and simulation fallback.
- [ ] Add anomaly/inference pipeline in `src/`.

## Priority 3 (Quality)
- [x] Add build target integration for SDK workspace.
- [ ] Add CI build checks.
