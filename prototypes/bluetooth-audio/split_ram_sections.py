#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Split explicit SDK RAM function sections for GC, retaining RAM placement."""
import re

PREFIXES = ('.spi_code', '.volatile_ram_code', '.flushinv_icache', '.vm')


def split_sections(text):
    changes = []
    def replace(line):
        if not line.startswith('define '):
            return line
        name = re.search(r'@([A-Za-z_.$][A-Za-z_.$0-9]*)\(', line)
        section = re.search(r' section "([^"]+)"', line)
        if not name or not section or section[1] not in PREFIXES:
            return line
        old = section[0]
        new = ' section "' + section[1] + '.' + name[1] + '"'
        changes.append(dict(function=name[1], before=section[1], after=section[1]+'.'+name[1]))
        return line.replace(old, new, 1)
    result = ''.join(replace(line) for line in text.splitlines(keepends=True))
    # Reversibility proves this transformation changed only section metadata,
    # never instructions, function signatures, globals or call destinations.
    restored = result
    for change in changes:
        restored = restored.replace(' section "'+change['after']+'"', ' section "'+change['before']+'"')
    if restored != text:
        raise ValueError('Unexpected IR change outside section annotations')
    return result, changes
