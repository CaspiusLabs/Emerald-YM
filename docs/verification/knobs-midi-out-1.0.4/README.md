# K1–K4 during MIDI OUT — exact stable 1.0.4

This documents a completed, owner-authorized physical test on the installed stable **1.0.4**, not a new build. Documentation VERSION **1.0.6** adds the report; firmware, release assets and tag remain unchanged. Source `7bb4efdd02499664296e96f134371c68403b22de`; package SHA-256 `f16da6faa7607541c94ac4bfbb0bb626e95eac0956e990a75317caf5b73a411f`, identity `FM-1_68010004`. Device identity was queried; the package hash identifies the known installed release, not a new USB readback of firmware bytes.

## Procedure

The owner selected **HOME dim/endless**, **EDIT bright/USB MIDI OUT** and paused playback. Initial displayed values were **K1 130 BPM, K2 100%, K3 100%, K4 0**. A physical PLAY started a baseline, followed by separately marked K1, K2, K3 and K4 instruction windows. In each the owner turned only that knob in both directions, slowly and with quicker movements, for approximately 15–20 seconds, then returned to its initial value. Requested ranges were approximately K1 100–160 BPM, K2 20–100%, K3 0–100%, K4 −12 to +12. Final restoration of all four values was explicitly owner-confirmed before the requested final PLAY Stop.

The host opened only the unique verified Emerald MIDI pair and audio endpoint, sent the documented identity/status queries, received MIDI events, and recorded stereo USB audio. No host notes, Clock/transport, parameter CCs, firmware or preset writes were sent. No unrelated MIDI interface was opened. The capture continued through the controlled final Stop and cleanup.

Phase markers timestamp when the capture observed each instruction change. **Window lengths below include instruction/response waits; they are not exact physical rotation durations or encoder telemetry.** K1's tempo range is inferred from 24-interval Windows MIDI arrival windows; it is not electrical timing or an exact display-value log. K2–K4 movement/restoration is owner-reported.

## Results

| Instruction window | Window length | Unexpected Stop | USB audio |
|---|---:|---:|---|
| K1 TEMPO | 63.37 s | **0** | Signal, no clipping; observed Clock tempo windows about 103.31–172.61 BPM |
| K2 DENSITY | 53.16 s | **0** | Signal, no clipping |
| K3 MUTATION | 61.37 s | **0** | Signal, no clipping |
| K4 TRANSPOSE | 45.10 s | **0** | Signal, no clipping |

- Music: **321.78 seconds**, with one Start and only the requested final Stop. No unexpected Stop during knobs or restoration; Clock continued.
- Diagnostics: **806 queries / 806 valid replies**, zero timeouts; maximum observed reply latency **10.29 ms**.
- Sampled music CPU: **47.47% average**, **72% lifetime maximum**, worst block **4193 us**. XRUN/overload/boot-fault increases: **0**. The maximum fields are since boot, not reset for each phase.
- USB capture: **403.53 seconds**, stereo PCM16/44.1 kHz. Every knob window contains signal and zero clipped samples; phase RMS/peak values are in the summary. This does not measure separate USB packet-loss counters.
- No held outgoing notes or duplicate Note Ons at the end. Original **130/100/100/0** settings were owner-confirmed restored. Device left paused, HOME dim/endless and EDIT OUT.

## Evidence and limits

[Summary and phase metrics](summary.json) · [Raw MIDI/status report](report.json.gz) · [Phase markers](phase-markers.json) · [Owner feedback](owner-feedback.json) · [Capture log](capture.log.gz). Compressed raw files are byte-preserving copies. The large WAV remains local in ignored `build/knob-midi-1.0.4/run-01/usb-audio.wav`; the raw report preserves its PCM SHA-256.

**The earlier unexplained ~2.52-second MIDI Stop remains unresolved and its record remains failed.** This targeted test did not reproduce it; it neither proves that all possible knob movements are safe nor establishes what caused that event. Keep the [earlier trace and context](../stable-1.0.4/README.md#earlier-short-trace-anomaly), the full-song test and this endless-mode knob test separate. No external-synth, electrical-jitter, new TRS/recovery or long-session guarantee is claimed. Publication/branch cleanup involves no new hardware access or firmware change.
