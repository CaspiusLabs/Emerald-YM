# Main panel grid — development 0.11.3

Owner-requested layout correction, on a separate branch based on unmerged PR #13 (which depends on PR #12). Native rendering still uses changed-only complete tiles and the original 240x144 canvas; no extra framebuffer or DSP/USB/LED behavior change.

- Six-pixel horizontal gutter between the volume rail and right grid, and between the two columns. Six-pixel vertical gutters between transport and each following card row.
- Two 86x44 cards per row, at x=54/146 and y=78/128/178. The transport is 178x24 at x=54, y=48. Both bottom cards now align with the upper cards.
- Instrument and effect bars use the same 76x3 geometry at local x=5, y=39, below both text lines. The instrument title retains its full name; the redundant SEL prefix is removed to fit the narrower card.
- Extend the rail's lower tile from 40x32 to 40x38 so its bottom aligns with the cards at y=222. The slider/ADC mapping and output toggle are unchanged.

![Native 0.11.3 main-screen render, not a photograph](ui-preview.png)

Build and full desktop tests pass, including existing bounds/changed-tile checks and assertions for six clear gap pixels, equal lower-card widths, and the instrument bar's below-text position and 50% fill. The preview is generated from the actual firmware renderer and visually reviewed. See [summary and package hash](summary.json), [build log](build.txt.gz), [desktop tests](tests.txt.gz). Optional inherited JavaScript package comparison skipped as before.

No device accessed or flashed; installed exact 0.11.1 and public 0.10.0 remain unchanged. Physical screen/performance validation is pending. No release/tag or automatic merge. This PR targets main and includes unmerged dependencies; review/merge after PR #12 and PR #13.
