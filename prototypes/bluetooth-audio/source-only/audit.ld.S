/* SPDX-License-Identifier: GPL-3.0-only
 * Offline isolated link audit, NOT an FM-1 boot image or production layout.
 * Real SDK registration tables; no fabricated callback or hardware addresses.
 * Region sizes match Emerald; no RAMTEXT expansion or changed production map. */
MEMORY {
    XIP (rx) : ORIGIN = 0x02000120, LENGTH = 0x8DFBC
    RAMTEXT (rwx) : ORIGIN = 0x01C00000, LENGTH = 0x6000
    RAM (rw) : ORIGIN = 0x01C08000, LENGTH = 96K
    POOL (rw) : ORIGIN = 0x01C20000, LENGTH = 0x54000
}
SECTIONS {
    .text : {
        text_rodata_begin = .;
        #include "btstack_source_only.ld"
        #include "driver/cpu/wl82/system.ld"
        #include "driver/cpu/wl82/system_data.ld"
        #include "server/media.ld"
        *(.text* .rodata*)
        *(.classic*_code .classic*_const .hci*_code .hci*_const)
        *(.link*_code .link*_const .lmp*_code .lmp*_const)
        *(.bt_rf_code .bt_rf_const .sbc_eng_code .sbc_eng_const)
        text_rodata_end = .;
    } > XIP
    .sdk_ram_code : {
        ram_text_rodata_begin = .;
        *(.vm .spi_code .flushinv_icache .volatile_ram_code)
        ram_text_rodata_end = .;
    } > RAMTEXT AT > XIP
    .data : {
        *(.data* .volatile_ram .non_volatile_ram .os_data .os_port_data)
        #include "btctrler/btctler_lib_data.ld"
        #include "btstack/btstack_lib_data.ld"
    } > RAM AT > XIP
    .bss (NOLOAD) : {
        *(.bss* COMMON .mem_heap .os_bss .os_port_bss)
        /* SDK user stacks have no SHF_WRITE bit in input objects. Explicit
         * RAM placement is essential; orphan placement can put them in XIP. */
        . = ALIGN(8); KEEP(*(.cpu0_ustack .cpu1_ustack))
        #include "btctrler/btctler_lib_bss.ld"
        #include "btstack/btstack_lib_bss.ld"
        cpu0_sstack_begin = .; KEEP(*(.cpu0_sstack)) cpu0_sstack_end = .;
        cpu1_sstack_begin = .; KEEP(*(.cpu1_sstack)) cpu1_sstack_end = .;
    } > RAM
    .heap (NOLOAD) : { . = ALIGN(32); HEAP_BEGIN = .; } > POOL
    HEAP_END = ORIGIN(POOL) + LENGTH(POOL);
    RAM0_SIZE = LENGTH(RAM); SDRAM_SIZE = 0; CACHE_RAM_SIZE = 0;
    FREE_DACHE_WAY = 0; FREE_IACHE_WAY = 0;
    text_size = SIZEOF(.text);
    data_size = SIZEOF(.data); data_vma = ADDR(.data); data_lma = LOADADDR(.data);
    bss_size = SIZEOF(.bss); bss_begin = ADDR(.bss);
    ram0_data_size = SIZEOF(.data); ram0_bss_size = SIZEOF(.bss);
    _ram0_data_size = SIZEOF(.data); _ram0_data_vma = ADDR(.data); _ram0_data_lma = LOADADDR(.data);
    _ram0_bss_size = SIZEOF(.bss); _ram0_bss_vma = ADDR(.bss);
    .cache_data (NOLOAD) : { cache_ram_data_vma = .; cache_ram_data_lma = .; } > RAM
    cache_ram_data_size = SIZEOF(.cache_data);
    .cache_bss (NOLOAD) : { cache_ram_bss_vma = .; } > RAM
    cache_ram_bss_size = SIZEOF(.cache_bss);
}
