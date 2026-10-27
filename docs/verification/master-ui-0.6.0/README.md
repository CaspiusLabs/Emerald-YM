# Console-style MASTER UI 0.6.0 — desktop evidence

This is a new UI development build, identity `FM-1_68000600`. No firmware was installed and no physical MIDI/audio ports were opened. The device remains on previously tested 0.5.1. The public 0.4.3 release and all previous package/tag evidence are unchanged.

[Package/hash/memory report](package-verification.json), [raw build log](build.log), [raw desktop tests](desktop-tests.log). Build and the complete `test-ym-windows.ps1` suite passed, including no-soft-double, flash-off/no-XIP-call checks, UAC drift/descriptors, DSP determinism/PSG/FX/settings, actual UI/LED and OTA/loader/mock installer tests. The optional inherited JavaScript installer comparison was skipped, not passed.

| Region | Used | Limit |
|---|---:|---:|
| Application image | 107792 B | build-checked |
| Data + BSS | 39120 B | 98304 B |
| Pool | 101888 B | 344064 B |
| Audio hot code/tables | 21604 B | inside RAMTEXT |
| Total RAMTEXT | 24492 B | 24576 B |

Relative to 0.5.1, application grows 732 B; all RAM/pool/RAMTEXT totals are unchanged. The narrow RAMTEXT margin remains 84 B. No linker regions, startup copy or protective 85% threshold were changed.

The actual renderer tests verify a complete 0..1023 MASTER ADC sweep, top/bottom red-handle positions, monotonic motion, visual hysteresis in both directions, no redraw for small ADC jitter, no writes outside the 22x108 moving tile, no idle writes and no repeated full-screen clear. Knob/transport/instrument/FX/SAVE/overload and LED behavior retain their existing tests. All LCD blit tiles are checked against the 144-row canvas height and 240x240 screen bounds.

The UI mirrors the same filtered physical pot value used by the existing master gain calculation. It does not alter that calculation, add ADC reads, persist MASTER, add a touchscreen interaction or change USB/MIDI/status. The 0-10 scale is control position, not dB or signal amplitude. Physical knob direction/calibration, LCD smoothness during simultaneous USB recording and runtime CPU/latency remain to be verified on the actual FM-1. Earlier 0.5.x measurements are not relabelled as 0.6.0 results.

## Native previews

These PNGs are 240x240 captures from the actual C renderer test, not a design mockup or device photograph. Layout is adapted from the owner's supplied reference to fit the real LCD; all graphics are code-drawn, with no new framebuffer or external bitmap asset.

![Playing layout](ui-preview.png)

[MASTER minimum](master-min.png) · [MASTER midpoint](master-mid.png) · [MASTER maximum](master-max.png) · [Save error](save-error.png) · [Overload](overload.png).
