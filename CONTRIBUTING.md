# Contributing

The canonical repository is [CaspiusLabs/Emerald-YM](https://github.com/CaspiusLabs/Emerald-YM). Clone it and work on a feature branch. Keep documentation, issue descriptions and commit messages in English. See [AGENTS.md](AGENTS.md) for implementation constraints.

Keep changes focused, explain the resulting behavior, and state which checks actually ran. Build and desktop tests do not flash or open MIDI. Physical testing and installation are separate actions; never target unrelated MIDI devices.

`VERSION` controls the firmware version. Documentation changes may retain 0.3.1; changed firmware behavior requires a new version, preserved package and matching source tag. Never replace the v0.1.4 package or tag. Avoid rewriting published history or moving release tags.

Keep desktop tests, flash simulations, user feedback and physical FM-1 measurements separate. Preserve raw failed/interrupted evidence and report what it means. Include version, exact package hash, actions and status counters in bug reports; do not include credentials or unrelated device data.

Do not commit `.runtime`, private credentials, temporary build outputs or proprietary compiler binaries. Preserve third-party notices. This repository's inherited web/host code is not automatically a supported Emerald feature.
