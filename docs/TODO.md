# TODO

Last updated: 2026-02-24

## CGM Alignment Steps

0. [x] Promote current firmware/docs baseline as restore points:
   - `GOLDEN-2026-02-24-R3`
   - `FAILSAFE-2026-02-24-R3`
   - artifacts staged in `failsafe/`

1. [x] Create a traceability matrix mapping current runtime behavior to each stage in `docs/CGM_DERMAL_ALGORITHM_REVIEW.md` and tag each stage as implemented/simulated/missing.
   - completed in `docs/CGM_TRACEABILITY.md`
2. [x] Define and document the runtime CGM tuple contract: `glucose_mgdl`, `trend_mgdl_min_x100`, `sqi_pct`, `sensor_flags`, `prediction_15m`, `prediction_30m`.
   - completed in `docs/CGM_RUNTIME_CONTRACT.md`
3. [x] Define and document sensor-status flag taxonomy and trigger logic: clipping/saturation, dropout, implausible rate-of-change, temperature out-of-range, calibration stale/drift warning.
   - completed in `docs/CGM_SENSOR_FLAGS.md`
4. [x] Produce a 30-second cadence reference pipeline artifact including block diagram, equations-level pseudocode, candidate filter constants, SQI feature definitions/threshold strategy, and drift/lag model structure.
   - completed in `docs/CGM_30S_PIPELINE_REFERENCE.md`
5. [x] Implement raw-to-engineering preprocessing: conversion/linearization, offset/gain/reference compensation, anti-alias and decimation, notch and impulse-noise rejection.
   - implemented in `src/cgm_preprocess.c` and integrated in `src/gauge_render.c`
6. [x] Implement dermal-tuned filtering/trend path emphasizing low lag: shorter-time-constant smoothing, trend derivative output (`mg/dL/min`), adaptive filter gating based on SQI.
   - implemented in `src/cgm_preprocess.c` and integrated in `src/gauge_render.c`
7. [x] Implement calibration and compensation path: factory calibration mapping hooks, temperature compensation, aging/drift compensation, sensitivity-change detection.
   - implemented in `src/cgm_preprocess.c/.h` and integrated in `src/gauge_render.c`
8. [x] Implement conservative lag/kinetics handling: optional low-gain lag correction only and no aggressive look-forward behavior by default.
   - implemented in `src/cgm_preprocess.c/.h` (low-gain lag state with SQI-adaptive gating and bounded correction)
9. [x] Implement SQI/fault gating as a first-class runtime control: block/de-rate prediction and alert outputs when SQI is low, hold-last-value/guarded fallback on dropouts.
   - implemented in `src/cgm_preprocess.c/.h` and integrated in `src/gauge_render.c` (runtime sensor flags, prediction block/de-rate, and dropout hold-last guard)
10. [x] Implement trend/prediction/alerts behavior: short-horizon prediction (15m/30m), predictive hypo/hyper checks gated by SQI, debounce and hysteresis to prevent alert chatter.
    - implemented in `src/gauge_render.c` (P15/P30 runtime model, SQI-gated hypo/hyper warning/fault logic, debounce+hysteresis, and prediction-driven alert/dose integration)
11. [x] Align LCD/terminal semantics with CGM contract: show SQI/confidence and sensor flags on-screen, clearly mark simulated fields until live sensor path is active.
    - implemented in `src/gauge_render.c` (explicit `SIM` labeling, on-screen P15/P30 display, terminal SQI confidence code + sensor flags, and simulated-contract CGM summary lines)
12. [x] Define validation protocol and pass/fail thresholds for lag estimation, trend accuracy by range, alert false-positive/chatter rate, and replay robustness under dropout/noise/motion artifacts.
    - completed in `docs/CGM_VALIDATION_PROTOCOL.md`
