# Emerald YM 0.7.0 — beta

By Caspius Labs. GLO toggles USB ONLY: physical speaker/headphone audio fades to silence, USB audio/MIDI/diagnostics continue. GLO lights and the screen shows USB ONLY. Press again to restore local output. MASTER controls USB level. Mode is session-only; reboot restores normal output.

Exact tested package, no rebuild. Source tag v0.7.0: b6be50a131f437fe5f09a6261552d24690eb907e. SHA-256: b575995cb7c829acc97a3672579212767346194811c18a66323621b95d7b64b6.

Physical USB ONLY test: 30 s continuous MIDI/status and 32.34 s stereo PCM16/44.1 kHz recording; average audio CPU 47.77%, maximum 73%, XRUNS/overload/failed boots 0. Owner confirmed speaker switching, LED and display. Headphone muting not separately physically tested. No packet-level or long-session guarantee; older V15/Dinghy round trips are not inherited.

[Manual](https://github.com/CaspiusLabs/Emerald-YM/blob/main/docs/MANUAL.md). Back up other firmware data before switching; Emerald does not import it. GPL-3.0-only with component notices. Locally built, no GitHub attestation.
