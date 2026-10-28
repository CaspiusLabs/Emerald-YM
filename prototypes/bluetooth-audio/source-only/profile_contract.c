/* SPDX-License-Identifier: GPL-3.0-only */
#include "app_config.h"
#include "system/includes.h"
#include "bt_common.h"
#if !TCFG_USER_BT_CLASSIC_ENABLE || !TCFG_USER_EMITTER_ENABLE || TCFG_USER_BLE_ENABLE || TCFG_BT_SUPPORT_AAC
#error "The audit must select classic emitter/SBC without BLE or AAC"
#endif
#if USER_SUPPORT_PROFILE_A2DP || USER_SUPPORT_PROFILE_AVCTP || USER_SUPPORT_PROFILE_HFP || USER_SUPPORT_PROFILE_HFP_AG || USER_SUPPORT_PROFILE_SPP || USER_SUPPORT_PROFILE_HID || USER_SUPPORT_PROFILE_PNP || USER_SUPPORT_PROFILE_PBAP || USER_SUPPORT_PROFILE_PAN || USER_SUPPORT_DUAL_A2DP_SOURCE
#error "Unexpected sink or additional profile in the source-only audit"
#endif
