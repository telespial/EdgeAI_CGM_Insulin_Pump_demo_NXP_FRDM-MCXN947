# TODO

Last updated: 2026-02-23

## CGM Alignment Steps

- [ ] Create a traceability matrix mapping current runtime behavior to each stage in `docs/CGM_DERMAL_ALGORITHM_REVIEW.md` and tag every stage as implemented/simulated/missing.
- [ ] Define and document the runtime CGM tuple contract:
  - `glucose_mgdl`
  - `trend_mgdl_min`
  - `sqi_pct`
  - `sensor_flags`
  - `prediction_15m`
  - `prediction_30m`
- [ ] Define and document sensor-status flag taxonomy and trigger logic:
  - clipping/saturation
  - dropout
  - implausible rate-of-change
  - temperature out-of-range
  - calibration stale/drift warning
- [ ] Produce a 30-second cadence reference pipeline artifact with:
  - block diagram
  - equations-level pseudocode
  - candidate filter constants
  - SQI feature definitions and threshold strategy
  - drift/lag model structure
- [ ] Implement raw-to-engineering preprocessing path:
  - conversion/linearization
  - offset/gain/reference compensation
  - anti-alias and decimation
  - notch and impulse-noise rejection
- [ ] Implement dermal-tuned filtering/trend path emphasizing low lag:
  - shorter-time-constant smoothing
  - trend derivative output (`mg/dL/min`)
  - adaptive filter gating based on SQI
- [ ] Implement calibration and compensation path:
  - factory calibration mapping hooks
  - temperature compensation
  - aging/drift compensation
  - sensitivity-change detection
- [ ] Implement conservative lag/kinetics handling:
  - optional low-gain lag correction only
  - no aggressive look-forward behavior by default
- [ ] Implement SQI/fault gating as a first-class runtime control:
  - block/de-rate prediction and alert outputs when SQI is low
  - hold-last-value/guarded fallback on dropouts
- [ ] Implement trend/prediction/alerts behavior:
  - short-horizon prediction (15m/30m)
  - predictive hypo/hyper checks gated by SQI
  - debounce and hysteresis to prevent alert chatter
- [ ] Align LCD/terminal semantics with CGM contract:
  - show SQI/confidence and sensor flags on-screen
  - clearly mark simulated fields until live sensor path is active
- [ ] Define validation protocol and pass/fail thresholds for:
  - lag estimation
  - trend accuracy by range
  - alert false-positive/chatter rate
  - replay robustness under dropout/noise/motion artifacts
