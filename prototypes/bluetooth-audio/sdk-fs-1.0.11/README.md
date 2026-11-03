# SDK filesystem dependency audit

Development 1.0.11 adds an optional deletion policy to the RAM Bluetooth VM
profile in PR #9. Both SDK file deletion entry points return the SDK's existing
failure value (-1) before touching flash. The build verifies that the rebuilt
private filesystem member has no sfc_erase_zone import.

The isolated reference SDK link and production app audit passed. Relative to
1.0.10, the SDK XIP span fell from 206,480 B to 206,416 B. Its RAMTEXT span
remains 9,521 B, so this change recovers **0 B of RAMTEXT**. The combined link
still exceeds RAMTEXT by **9,455 B**. The SDK map shows that flash_fs_api.c.o
still pulls vm.c.o through vm_defrag_for_update. This is a separate dependency
from the file deletion calls.

Use the commands in the [1.0.10 report](../bt-vm-ram-1.0.10/README.md) and add
`--deny-sdk-file-delete` to both Python commands. This is an offline profile with
file deletion deliberately disabled.

[SDK report](sdk-report.json.gz), [combined diagnostics](combined-link.txt.gz)
and [production audit](production-audit.json.gz).
