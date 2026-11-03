# SDK VM update dependency audit

Development 1.0.12 adds an opt-in offline policy in PR #9. The SDK function
`vm_update_defrag` returns an existing negative SDK error (-1) before accessing
flash. The build checks the pinned function signature and both call sites, then
verifies that the rebuilt private archive member has no `vm_defrag_for_update`
import. This profile deliberately rejects VM update preparation.

The isolated reference SDK link and production app audit passed. Compared with
1.0.11, **RAMTEXT savings were 0 B**. The isolated SDK RAMTEXT span is 9,521 B.
The combined link still exceeds RAMTEXT by **9,455 B**. The SDK map identifies
`vm_backup_for_update` in the same `flash_fs_api.c.o` member as the remaining
archive extraction dependency on `vm.c.o`.

Use the commands in the [1.0.11 report](../sdk-fs-1.0.11/README.md) and add
`--deny-sdk-vm-update` to both Python commands.

[SDK report](sdk-report.json.gz), [SDK map](sdk.map.gz),
[combined diagnostics](combined-link.txt.gz), [production audit](production-audit.json.gz).
