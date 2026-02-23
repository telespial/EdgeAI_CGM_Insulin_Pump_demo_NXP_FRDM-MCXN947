# Operations Runbook

Last updated: 2026-02-23

## One-Time Bootstrap (Ubuntu User-Local)

`./tools/bootstrap_ubuntu_user.sh`

## Preflight

`./scripts/preflight.sh`

## Build

`./scripts/build.sh`

## Flash

`./scripts/flash.sh`

## Build + Flash

`./scripts/build_and_flash.sh`

## State Updates

`./scripts/update_state.sh --change "<summary>" [--binary <path>] [--command "<cmd>"] [--result <ok|fail>]`

## Probe Check

`/usr/local/LinkServer/LinkServer probe '#1' dapinfo`
