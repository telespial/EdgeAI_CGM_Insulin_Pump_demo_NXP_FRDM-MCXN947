# TODO

Last updated: 2026-02-23

## CGM Alignment Steps

0. [x] Promote current firmware/docs baseline as restore points:
   - `GOLDEN-2026-02-23-R3`
   - `FAILSAFE-2026-02-23-R3`
   - artifacts staged in `failsafe/`

1. [ ] Create a traceability matrix mapping current runtime behavior to each stage in `docs/CGM_DERMAL_ALGORITHM_REVIEW.md` and tag each stage as implemented/simulated/missing.
2. [ ] Define and document the runtime CGM tuple contract: `glucose_mgdl`, `trend_mgdl_min`, `sqi_pct`, `sensor_flags`, `prediction_15m`, `prediction_30m`.
3. [ ] Define and document sensor-status flag taxonomy and trigger logic: clipping/saturation, dropout, implausible rate-of-change, temperature out-of-range, calibration stale/drift warning.
4. [ ] Produce a 30-second cadence reference pipeline artifact including block diagram, equations-level pseudocode, candidate filter constants, SQI feature definitions/threshold strategy, and drift/lag model structure.
5. [ ] Implement raw-to-engineering preprocessing: conversion/linearization, offset/gain/reference compensation, anti-alias and decimation, notch and impulse-noise rejection.
6. [ ] Implement dermal-tuned filtering/trend path emphasizing low lag: shorter-time-constant smoothing, trend derivative output (`mg/dL/min`), adaptive filter gating based on SQI.
7. [ ] Implement calibration and compensation path: factory calibration mapping hooks, temperature compensation, aging/drift compensation, sensitivity-change detection.
8. [ ] Implement conservative lag/kinetics handling: optional low-gain lag correction only and no aggressive look-forward behavior by default.
9. [ ] Implement SQI/fault gating as a first-class runtime control: block/de-rate prediction and alert outputs when SQI is low, hold-last-value/guarded fallback on dropouts.
10. [ ] Implement trend/prediction/alerts behavior: short-horizon prediction (15m/30m), predictive hypo/hyper checks gated by SQI, debounce and hysteresis to prevent alert chatter.
11. [ ] Align LCD/terminal semantics with CGM contract: show SQI/confidence and sensor flags on-screen, clearly mark simulated fields until live sensor path is active.
12. [ ] Define validation protocol and pass/fail thresholds for lag estimation, trend accuracy by range, alert false-positive/chatter rate, and replay robustness under dropout/noise/motion artifacts.
