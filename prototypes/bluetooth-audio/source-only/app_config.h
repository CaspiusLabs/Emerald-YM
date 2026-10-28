/* SPDX-License-Identifier: GPL-3.0-only
 * Offline source-only profile. SDK services are audited, never installed.
 * Reference RF calibration is opt-in; no demo GPIO/power board code is used. */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H
#define __FLASH_SIZE__ (1024 * 1024)
#define __SDRAM_SIZE__ 0
#define CONFIG_BT_ENABLE 1
#define CONFIG_CPU_WL82 1
#define __GCC_PI32V2__ 1
#define CONFIG_FREE_RTOS_ENABLE 1
#define CONFIG_THREAD_ENABLE 1
#define CONFIG_NO_SDRAM_ENABLE 1
#define CONFIG_SFC_ENABLE 1
#define CONFIG_AUDIO_ENABLE 1
#define CONFIG_SBC_ENC_ENABLE 1
#define CONFIG_VIRTUAL_DEV_ENC_ENABLE 1
#define CONFIG_DEBUG_LIB(x) 0
/* SDK demo_edr compile defaults, audit only; not an FM-1 flash partition. */
#define VM_MAX_PAGE_ALIGN_SIZE_CONFIG (64 * 1024)
#define VM_MAX_SECTOR_ALIGN_SIZE_CONFIG (64 * 1024)
#define APP_PRIVATE_PROFILE_CFG 1
#define TCFG_USER_BT_CLASSIC_ENABLE 1
#define TCFG_USER_EMITTER_ENABLE 1
#define TCFG_USER_BLE_ENABLE 0
#define TCFG_BT_SUPPORT_AAC 0
#define TCFG_BD_NUM 1
#define CONFIG_BT_RX_BUFF_SIZE (17 * 1024 + 400)
#define CONFIG_BT_TX_BUFF_SIZE (10 * 1024 + 512)
#endif
