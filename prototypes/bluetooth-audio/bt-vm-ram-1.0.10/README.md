# Bluetooth VM RAM backend — development 1.0.10

The optional RAM file backend replaces the Bluetooth VM factory in the isolated
reference SDK link. The combined link still exceeds RAMTEXT by **9,455 B**.
The SDK file system still references `sfc_erase_zone` from the vendor VM object,
which keeps a separate path to flash services alive.

The build checks the five-callback layout and order against the pinned SDK IR.
It namespaces the original factory in a private archive copy because the file
system and OTA code can reference other functions in the same object. The
isolated link verifies that the new factory and RAM read/write callbacks survive
and that the legacy factory is collected.

The backend reuses the task-mutex-protected session record store. It implements
one open file and cursor, records up to 64 bytes, erased bytes as `0xff`, and
bounded absolute and relative seek. A failed write leaves the cursor unchanged.
This prototype assumes that the SDK serializes complete file sessions and that
the session configuration has already been initialized with a provisioned MAC.
The callback lifecycle and those assumptions still need target verification.
Session records are volatile.

A broader trial split RAM function sections in the SDK cache, power and RTOS
modules. The resulting combined overflow grew to 9,583 B. The narrower cache
and RTOS split returned to 9,455 B. Both trials were dropped because they saved
no combined RAMTEXT. The earlier per-function split remains in the audit.

The desktop check of the callbacks used the actual RAM record store with host
adapters for syscfg and identity. It covered open limits, erased data, partial
writes, seek limits, rejected writes and record isolation. The existing desktop
queue and session store suites also passed. The isolated target SDK link used
reference RF values. The production app audit checks the existing 1.0.4 source
and records its memory separately from the failed combined preflight.

From the repo root, with the pinned SDK and the production audit objects
provisioned as described in the [1.0.9 audit](../platform-integration-1.0.9/README.md):

```powershell
.runtime/python/python.exe prototypes/bluetooth-audio/build_source_only.py --lean-virtual --session-config --split-ram-sections --ram-bt-vm --reference-rf --combined-preflight
.runtime/python/python.exe prototypes/bluetooth-audio/audit_current.py --lean --session-config --split-ram-sections --ram-bt-vm
cmd /c prototypes\bluetooth-audio\test-windows.cmd
```

[Reference SDK report](sdk-report.json.gz) and [combined linker diagnostics](combined-link.txt.gz)
record the results. The combined profile remains an offline experiment with
startup, flash/cache ownership and RTOS integration unresolved.

The [production comparison](production-audit.json.gz) reports SDK spans of
9,521 B RAMTEXT, 55,370 B RAM, 10,368 B POOL and 206,480 B XIP.
The conservative RAMTEXT total is 34,029 B against a 24,576 B limit.
