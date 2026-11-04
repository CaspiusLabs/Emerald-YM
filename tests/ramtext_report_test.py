#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Host-only RAMTEXT report parser regression tests, with synthetic objdump data."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import ramtext_report as report

TABLE = """SYMBOL TABLE:
01c00000 g       .ram_text 00000000 _rt_start
01c00000 l     F .ram_text 00000080 flash_entry
01c00100 g       .ram_text 00000000 _rt_end
01c00104 g       .audio_hot 00000000 _audio_start
01c00104 l     F .audio_hot 00000060 engine_tick
01c00164 l     F .audio_hot 00000020 psg_sample
01c00200 g       .audio_hot 00000000 _fx_start
01c00200 l     F .audio_hot 000000c0 fx_echo
01c002c0 g       .audio_hot 00000000 _fx_end
01c00300 g       .audio_hot 00000000 _audio_end
02000120 g     F .text 00000020 xip_control
"""


class RAMTEXTReportTests(unittest.TestCase):
    def test_section_accounting_and_sizes(self):
        result = report.summarize(TABLE, baseline=1024, goal=256)
        self.assertEqual(result["ramtext_used_bytes"], 768)
        self.assertEqual(result["ramtext_free_bytes"], 24576 - 768)
        self.assertEqual(result["flash_driver_bytes"], 256)
        self.assertEqual(result["flash_to_audio_padding_bytes"], 4)
        self.assertEqual(result["audio_hot_bytes"], 508)
        self.assertEqual(result["fx_code_bytes"], 192)
        self.assertEqual(result["reclaimed_vs_stable_bytes"], 256)
        self.assertTrue(result["target_met"])
        self.assertEqual(result["top_named_symbols"][0]["name"], "fx_echo")
        self.assertEqual(result["named_symbol_count"], 4)

    def test_explicit_goal_and_baseline(self):
        self.assertFalse(report.summarize(TABLE, baseline=1024, goal=257)["target_met"])
        self.assertEqual(report.summarize(TABLE, baseline=768, goal=0)["delta_vs_stable_bytes"], 0)
        with self.assertRaises(ValueError):
            report.summarize(TABLE, goal=-1)

    def test_missing_symbols_or_invalid_bounds_fail_closed(self):
        with self.assertRaises(ValueError):
            report.summarize(TABLE.replace(" _fx_end", " _fx_missing"))
        with self.assertRaises(ValueError):
            report.summarize(TABLE.replace("01c00300 g", "01c07000 g"))
        with self.assertRaises(ValueError):
            report.summarize(TABLE, baseline=25000)


if __name__ == "__main__":
    unittest.main()
