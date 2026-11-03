/* SPDX-License-Identifier: GPL-3.0-only
 * Pinned SDK VM file ABI, verified against vm.c.o IR at build time.
 * The stack owns the single cursor and must serialize whole file sessions.
 * Record I/O uses the existing task-serialized session syscfg backend. */
#include "system/includes.h"
#include "syscfg/syscfg_id.h"

enum { BT_VM_FILE_LIMIT = 64 };
static int file_id, file_size, position;
extern const unsigned char *eym_session_mac(void);

static int vm_open_ram(int id, int size)
{
    if (file_id || !eym_session_mac() || id <= 0 || id > 255 ||
        id == CFG_BT_MAC_ADDR || size <= 0 || size > BT_VM_FILE_LIMIT)
        return 0;
    file_id = id;
    file_size = size;
    position = 0;
    return 1;
}

static void load_record(unsigned char *record)
{
    int i;
    for (i = 0; i < BT_VM_FILE_LIMIT; ++i) record[i] = 0xff;
    syscfg_read((u16)file_id, record, BT_VM_FILE_LIMIT);
}

static int vm_read_ram(unsigned char *dst, int length)
{
    unsigned char record[BT_VM_FILE_LIMIT];
    int i;
    if (!file_id || !dst || length <= 0 || length > file_size - position) return 0;
    load_record(record);
    for (i = 0; i < length; ++i) dst[i] = record[position + i];
    position += length;
    return length;
}

static int vm_write_ram(unsigned char *src, int length)
{
    unsigned char record[BT_VM_FILE_LIMIT];
    int i;
    if (!file_id || !src || length <= 0 || length > file_size - position) return 0;
    load_record(record);
    for (i = 0; i < length; ++i) record[position + i] = src[i];
    if (syscfg_write((u16)file_id, record, (u16)file_size) != file_size) return 0;
    position += length;
    return length;
}

static int vm_seek_ram(int from, int offset)
{
    int base = from ? position : 0;
    if (!file_id || offset < -base || offset > file_size - base) return -1;
    position = base + offset;
    return 0;
}

static int vm_close_ram(void)
{
    file_id = 0;
    return 1;
}

static struct {
    int (*open)(int, int);
    int (*read)(unsigned char *, int);
    int (*write)(unsigned char *, int);
    int (*seek)(int, int);
    int (*close)(void);
} callbacks = {vm_open_ram, vm_read_ram, vm_write_ram, vm_seek_ram, vm_close_ram};

void *bt_vm_interface(void)
{
    return &callbacks;
}
