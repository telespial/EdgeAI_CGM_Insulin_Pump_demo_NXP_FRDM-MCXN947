# CGM Traceability Matrix

Last updated: 2026-02-23

## Scope
This matrix maps the current demo runtime to the algorithm stages in `docs/CGM_DERMAL_ALGORITHM_REVIEW.md`.

Status legend:
- `implemented`: present in runtime logic
- `simulated`: present as demo/simplified behavior, not a production CGM path
- `missing`: not implemented in current runtime

## Stage Mapping

| Review stage | Status | Current evidence | Gap summary |
|---|---|---|---|
| 1) Raw electrochemical signal to engineering units | simulated | Preprocessing pipeline now implemented (`src/cgm_preprocess.c`) and integrated into glucose path (`src/gauge_render.c`), including conversion/linearization, reference compensation, anti-aliasing, decimation, notch, and impulse rejection; input is still synthetic/demo raw stream. | Replace synthetic raw generator with live AFE sensor source and validate coefficient tuning on hardware data. |
| 2) Filtering tuned for fast dynamics | simulated | Adaptive low-lag filtering/trend path implemented in preprocessing (`src/cgm_preprocess.c`) and wired to runtime trend output (`src/gauge_render.c`), including SQI-driven alpha selection; upstream raw input remains synthetic. | Replace synthetic raw generator with live sensor stream and retune constants against captured dermal datasets. |
| 3) Calibration and compensation | missing | No factory calibration map, temperature compensation, or drift compensation in CGM path; help text explicitly states demo simulation (`src/gauge_render.c:2278`). | Need calibration hooks, temperature and aging/drift compensation, and sensitivity-change handling. |
| 4) Lag and kinetics handling | missing | No explicit kinetics/lag state model or conservative lag correction path found in runtime. | Need optional low-gain lag compensation and explicit kinetics model parameters. |
| 5) SQI and fault detection | missing | No CGM SQI/confidence field or sensor flags in runtime tuple; current alert framework is generic anomaly/power state (`src/power_data_source.h:55`). | Need SQI score, sensor-status flags (clipping/dropout/ROC plausibility/temp-range/drift), and guarded fallback behavior. |
| 6) Trend, prediction, and alert logic | simulated | Trend (`dBG`) and dose recommendation are demo-derived from simulated glucose/IOB/activity (`src/gauge_render.c:1353`, `src/gauge_render.c:2587`). | Need CGM-centric 15/30 min prediction plus SQI-gated predictive hypo/hyper alerting with explicit debounce/hysteresis policy. |
| 7) Performance and validation metrics | missing | No validation protocol artifact or pass/fail metric table in docs/runtime. | Need defined metrics and thresholds (MARD/RMSE/lag/chatter/robustness) with replay test procedure. |
| 8) Advanced/optional transforms | missing | No FFT/wavelet diagnostic path in current CGM implementation scope. | Optional; decide later based on compute budget after core path is stable. |

## Current Runtime Notes

- The UI and help explicitly identify CGM and dosing response as simulated demo behavior (`src/gauge_render.c:2278`, `src/gauge_render.c:2279`).
- Terminal line already exposes `dBG` and dose context, which is a useful UI target when replacing simulated internals with a formal CGM tuple (`src/gauge_render.c:2587`).

## Immediate Follow-On

Proceed with TODO step 2 and define a formal runtime tuple contract:
- `glucose_mgdl`
- `trend_mgdl_min`
- `sqi_pct`
- `sensor_flags`
- `prediction_15m`
- `prediction_30m`
