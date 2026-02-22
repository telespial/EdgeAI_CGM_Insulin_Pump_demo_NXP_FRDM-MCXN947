# Project State

Last updated: 2026-02-22

## Current Status
- Project framework scaffold created.
- Build/flash workflow scripts added.
- Git repository initialized locally.

## Hardware Setup
- Board: FRDM-MCXN947
- Probe: MCU-Link CMSIS-DAP (use `#1`)
- Debug USB: J17

## Build/Flash Config
- Build Command:
- Flash Command: /usr/local/LinkServer/LinkServer flash --probe '#1' MCXN947:FRDM-MCXN947 load --addr 0x0 {BIN_PATH}
- Binary:
- Status File: STATUS.md

## Update 2026-02-22
- Change: Initial standalone framework scaffold with docs/scripts/source layout.
- Result: ok
