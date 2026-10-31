#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Rename one defined ELF32-LE symbol in a private offline archive member.

The old toolchain objcopy cannot rename symbols. Relocations keep their symbol
indices and executable/data sections are byte-identical. No library is shipped.
"""
import struct


def rename_defined_symbol(raw, old, new):
    if raw[:6] != b'\x7fELF\x01\x01':
        raise ValueError('Expected ELF32 little endian')
    if struct.unpack_from('<H', raw, 16)[0] != 1:
        raise ValueError('Expected relocatable object')
    shoff = struct.unpack_from('<I', raw, 32)[0]
    stride, count = struct.unpack_from('<HH', raw, 46)
    if stride != 40:
        raise ValueError('Unexpected section header size')
    headers = [struct.unpack_from('<10I', raw, shoff+i*stride) for i in range(count)]
    found = []
    for h in headers:
        if h[1] != 2:
            continue
        if h[9] != 16 or h[6] >= count:
            raise ValueError('Unexpected symbol table')
        strings_h = headers[h[6]]
        if strings_h[1] != 3:
            raise ValueError('Missing symbol string table')
        strings = raw[strings_h[4]:strings_h[4]+strings_h[5]]
        for pos in range(h[4], h[4]+h[5], 16):
            name, _, _, info, _, section = struct.unpack_from('<IIIBBH', raw, pos)
            end = strings.find(b'\0', name)
            symbol = strings[name:end]
            if symbol == new.encode('ascii'):
                raise ValueError('Namespace target already exists')
            if symbol == old.encode('ascii'):
                if not section or info >> 4 != 1:
                    raise ValueError('Expected one defined global symbol')
                found.append((pos, h[6], strings))
    if len(found) != 1:
        raise ValueError('Expected exactly one symbol to rename')
    pos, index, strings = found[0]
    result = bytearray(raw)
    # Relocate just the string table to appended storage. All other offsets and
    # symbol/relocation indices remain unchanged, including shared suffix names.
    new_offset = len(result)
    result.extend(strings + new.encode('ascii') + b'\0')
    struct.pack_into('<I', result, pos, len(strings))
    struct.pack_into('<II', result, shoff+index*stride+16, new_offset,
                     len(strings)+len(new)+1)
    for h in headers:
        if h[1] != 8 and h[2] & 2:
            assert result[h[4]:h[4]+h[5]] == raw[h[4]:h[4]+h[5]]
    return bytes(result)
