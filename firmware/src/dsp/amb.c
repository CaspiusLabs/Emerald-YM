/* SPDX-License-Identifier: GPL-3.0-only */
/* The instrument (amb.h). One compilation unit, -O2. */
#include "amb.h"
#include "fastmath.h"

#ifdef EN_HOST
#define EN_POOL
#else
#define EN_POOL __attribute__((section(".pool")))
#endif
#define BARRIER() __asm__ volatile("" ::: "memory")

const char *const EN_NOTE_NAME[12] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
/* (Ionian and Aeolian shown as Major and Minor) */
const char *const EN_MODE_NAME[EN_NMODES] = {"Major", "Dorian", "Phrygian", "Lydian", "Mixolyd.", "Minor",
                                             "Pent maj", "Pent min"};
#define B(a, b, c, d, e, f, g) (1u << (a) | 1u << (b) | 1u << (c) | 1u << (d) | 1u << (e) | 1u << (f) | 1u << (g))
const uint16_t EN_MODE_POOL[EN_NMODES] = {
    B(0, 2, 4, 5, 7, 9, 11), B(0, 2, 3, 5, 7, 9, 10), B(0, 1, 3, 5, 7, 8, 10), B(0, 2, 4, 6, 7, 9, 11),
    B(0, 2, 4, 5, 7, 9, 10), B(0, 2, 3, 5, 7, 8, 10), B(0, 2, 4, 7, 9, 9, 9),  B(0, 3, 5, 7, 10, 10, 10)};
#undef B
const uint8_t EN_LOOP_LEN[EN_NLOOP] = {13, 17, 23, 29};
const uint8_t EN_DRUM_N[EN_NDRUM] = {16, 16, 16, 16, 16};
const char *const EN_SEC_NAME[EN_NSEC] = {"Full", "Break", "Groove", "Thin"};

static const en_param_t PARAMS[P_NPARAMS] = {
    {"Chord", 0, 100, 70},    {"Pings", 0, 100, 35},    {"Bass", 0, 100, 60},     {"Drums", 0, 100, 65},
    {"Density", 0, 100, 30},  {"Evolve", 0, 100, 40},   {"Drift", 0, 6, 4},       {"Hits", 0, 100, 50},
    {"Echo", 0, 100, 55},     {"Echo time", 0, 5, 1},   {"Reverb", 0, 100, 35},   {"Feedback", 0, 100, 60},
    {"Cutoff", 0, 100, 45},   {"Decay", 0, 100, 35},    {"Ping tone", 0, 100, 40}, {"Motion", 0, 100, 50},
    {"Tune", -50, 50, 0},     {"MIDI out", 0, 1, 1},    {"Tempo", 90, 140, 122},  {"Root", 0, 11, 5},
    {"Mode", 0, EN_NMODES - 1, MODE_DORIAN}, {"Ping oct", -2, 2, 0}, {"Chord voice", 0, EN_NCV - 1, 0},
    {"Bass oct", -1, 1, 0},   {"Groove", 0, 4, 1},      {"Fifth", 0, 100, 20},
    {"Swing", 0, 100, 10},    {"Hats", 0, 100, 50},
    {"Stabs", 0, 100, 30},    {"Crackle", 0, 100, 35},  {"Duck", 0, 100, 50},     {"Size", 0, 100, 55},
    {"Cloud", 0, 100, 30},
    {"Ping voice", 0, EN_NPV - 1, 0}, {"Bass voice", 0, EN_NBV - 1, 0}, {"Kit", 0, EN_NKIT - 1, 0},
    {"Attack", -50, 50, 0},   {"Colour", -50, 50, 0},   {"Resonance", -50, 50, 0}, {"Env amount", -50, 50, 0},
    {"Tone", -50, 50, 0},     {"Ratio", -50, 50, 0},    {"Ping len", -50, 50, 0}, {"Bend", -50, 50, 0},
    {"Bite", -50, 50, 0},     {"Shape", -50, 50, 0},    {"Bass len", -50, 50, 0}, {"Slide", -50, 50, 0},
    {"Kick tune", -12, 12, 0}, {"Kick len", -50, 50, 0}, {"Punch", -50, 50, 0},   {"Hat tone", -50, 50, 0},
    {"Chord alg", 0, 9, 0},   {"Chord ratio", -50, 50, 0}, {"Chord fb", -50, 50, 0}, {"Chord mod", -50, 50, 0},
    {"Ping alg", 0, 9, 0},    {"Ping ratio", -50, 50, 0}, {"Ping fb", -50, 50, 0}, {"Ping mod", -50, 50, 0},
    {"Bass alg", 0, 9, 0},    {"Bass ratio", -50, 50, 0}, {"Bass fb", -50, 50, 0}, {"Bass mod", -50, 50, 0},
    {"Root 1", 0, 11, 0}, {"Root 2", 0, 11, 0}, {"Root 3", 0, 11, 0}, {"Root 4", 0, 11, 0},
    {"Root 5", 0, 11, 0}, {"Root 6", 0, 11, 0}, {"Root 7", 0, 11, 0}, {"Root 8", 0, 11, 0},
    {"Kind 1", 0, EN_NCQ - 1, 0}, {"Kind 2", 0, EN_NCQ - 1, 0}, {"Kind 3", 0, EN_NCQ - 1, 0}, {"Kind 4", 0, EN_NCQ - 1, 0},
    {"Kind 5", 0, EN_NCQ - 1, 0}, {"Kind 6", 0, EN_NCQ - 1, 0}, {"Kind 7", 0, EN_NCQ - 1, 0}, {"Kind 8", 0, EN_NCQ - 1, 0},
    {"Type 1", 0, EN_NCX - 1, 0}, {"Type 2", 0, EN_NCX - 1, 0}, {"Type 3", 0, EN_NCX - 1, 0}, {"Type 4", 0, EN_NCX - 1, 0},
    {"Type 5", 0, EN_NCX - 1, 0}, {"Type 6", 0, EN_NCX - 1, 0}, {"Type 7", 0, EN_NCX - 1, 0}, {"Type 8", 0, EN_NCX - 1, 0},
    {"Play", 0, 6, 1},
    {"Kick steps", -32768, 32767, 0}, {"Shaker steps", -32768, 32767, 0}, {"Open steps", -32768, 32767, 0},
    {"Rim steps", -32768, 32767, 0},  {"Clap steps", -32768, 32767, 0},
    {"Kick acc", -32768, 32767, 0},   {"Shaker acc", -32768, 32767, 0},   {"Open acc", -32768, 32767, 0},
    {"Rim acc", -32768, 32767, 0},    {"Clap acc", -32768, 32767, 0},
    {"Kick chance", 0, 100, 100}, {"Shaker chance", 0, 100, 100}, {"Open chance", 0, 100, 100}, {"Rim chance", 0, 100, 100},
    {"Clap chance", 0, 100, 5},
    {"Accent", 0, 100, 50}, {"Drum play", 0, 1, 0},
    {"Noise colour", -50, 50, 0}, {"Grain", -50, 50, 0}, {"Room", -50, 50, 0}, {"Noise drift", -50, 50, 0},
    {"Noise", 0, EN_NNCHAR - 1, 0},
    {"Throws", 0, 100, 50},   {"Ghost", 0, 100, 50},    {"Notes", 0, 100, 100},   {"Hats level", 0, 100, 50},
};
const en_param_t *en_param_info(int i) { return (i >= 0 && i < P_NPARAMS) ? &PARAMS[i] : &PARAMS[0]; }

/* the pieces: eight recipes for the whole machine (the FM-1's PRESETS, the web app's cards). Tune and
 * MIDI out are taken from here by neither: they belong to the player, not the piece */
/*                  Cho Pin Bas Dru  Den Evo Dri Hit  Ech ET Rev Fdb  Cut Dec PTo Mot  Tun MI Tem Rt Md Oc
 *                  CV BO Grv Fif Swi Hat  Stb Crk Dck Siz  Clo PV BV Kit
 *                  then the sound pages' sixteen, all 0: the voices as they are; then the noise's five (its controls, its character);
 *                  then Throws, Ghost, Notes, Hats level (50, 50, 100, 50: the pieces as they were)
 * (Dri: Drift, 2 4 8 16 32 64 bars or Hold; ET: Echo time, 1/8 1/8. 1/4 1/4. 1/2 1 bar; Grv: the knob's, Drone,
 *  Offbeat, Dotted, Sync, Roller (GROOVE_NAME); CV, PV, BV, Kit: the voices, EN_CV_NAME ...) */
static const struct {
    const char *name;
    int16_t v[P_NPARAMS];
} PIECES[EN_NPIECES] = {
    {"Basic",      {76, 30, 60, 65, 30, 40, 4, 50, 55, 1, 35, 60, 45, 35, 40, 50, 0, 1, 122, 5, 1, 0,
                    0, 0, 1, 20, 10, 50, 30, 35, 50, 55, 30, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    0, 0, 0, 0, 0,
                    50, 50, 100, 50}},
    {"Chamber",    {68, 20, 55, 60, 20, 45, 6, 40, 70, 1, 45, 75, 50, 55, 30, 60, 0, 1, 118, 9, 5, 0,
                    2, 0, 0, 10, 0, 40, 15, 45, 45, 70, 45, 2, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    -5, -10, 10, 0, 3,
                    50, 50, 100, 50}},
    {"Steppers",   {60, 35, 70, 70, 35, 40, 3, 60, 50, 3, 30, 55, 55, 25, 50, 45, 0, 1, 126, 2, 1, 0,
                    0, 0, 2, 35, 15, 60, 45, 25, 60, 50, 20, 1, 1, 2,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    5, 10, -10, 0, 0,
                    50, 50, 100, 50}},
    {"Fog",        {64, 34, 50, 50, 40, 55, 5, 30, 75, 2, 55, 80, 46, 70, 25, 70, 0, 1, 112, 7, 5, 0,
                    1, -1, 0, 15, 0, 30, 20, 60, 35, 80, 60, 0, 0, 3,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    -10, 0, 15, 10, 1,
                    50, 50, 100, 50}},
    {"Concrete",   {65, 25, 65, 75, 25, 35, 4, 75, 50, 1, 25, 60, 60, 20, 60, 40, 0, 1, 130, 0, 7, 0,
                    6, 0, 4, 30, 20, 75, 50, 20, 65, 45, 15, 3, 3, 2,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    0, 15, 0, 10, 4,
                    50, 50, 100, 50}},
    {"Night Drive", {65, 45, 60, 65, 45, 50, 3, 55, 60, 4, 35, 65, 50, 40, 45, 55, 0, 1, 124, 10, 1, 0,
                    1, 0, 3, 25, 25, 55, 35, 30, 50, 60, 35, 4, 4, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    0, -10, 0, 0, 1,
                    50, 50, 100, 50}},
    {"Pressure",   {78, 20, 54, 74, 20, 30, 6, 65, 45, 1, 20, 55, 48, 30, 35, 35, 0, 1, 128, 4, 2, 0,
                    5, 0, 1, 10, 5, 65, 25, 32, 70, 40, 20, 3, 4, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    -10, -20, -25, -10, 0,
                    50, 50, 100, 50}},
    {"Haze",       {56, 42, 52, 45, 50, 60, 4, 35, 80, 3, 60, 85, 45, 80, 25, 80, 0, 1, 116, 3, 0, 1,
                    3, 0, 0, 20, 10, 35, 30, 62, 30, 85, 70, 2, 0, 4,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 100, 100, 100, 5, 50, 0,
                    5, 10, 10, 15, 2,
                    50, 50, 100, 50}},
};

const char *en_piece_name(int i) { return PIECES[(unsigned)i < EN_NPIECES ? i : 0].name; }
void en_piece(int i, int16_t *par, uint16_t *pool)
{
    int p;
    i = (unsigned)i < EN_NPIECES ? i : 0;
    for (p = 0; p < P_NPARAMS; p++)
        par[p] = PIECES[i].v[p];
    *pool = EN_MODE_POOL[PIECES[i].v[P_MODE]];
}

const char *const EN_CV_NAME[EN_NCV] = {"Saws", "Keys", "Organ", "Pad", "Bell", "Tonewheel", "Noise"};
const char *const EN_PV_NAME[EN_NPV] = {"Blip", "Marimba", "Glass", "Wood", "Drop"};
const char *const EN_BV_NAME[EN_NBV] = {"Sub", "Punch", "Rubber", "Pluck", "Growl"};
const char *const EN_KIT_NAME[EN_NKIT] = {"Tight", "Deep", "Punch", "Soft", "Dusty"};
const char *const EN_CQ_NAME[EN_NCQ] = {"Empty", "Major", "Minor", "Sus2", "Sus4", "Dim", "Aug"};
const char *const EN_CX_NAME[EN_NCX] = {"Triad", "6", "7", "Maj7", "9", "11", "Add9"};
static const char *const ETIME_NAME[6] = {"1/8", "1/8.", "1/4", "1/4.", "1/2", "1 bar"};
static const float ETIME_BEATS[6] = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 4.0f};
/* the Groove knob (P_GROOVE): the five patterns from sparse to busy (GROOVE_PAT: the engine's GROOVE below); Notes
 * (P_BNOTES) how many of its notes play (the rest left out at random, bar by bar); Energy (PERFORM) thins it further,
 * or fills a thin one */
#define NGROOVE 5
static const uint8_t GROOVE_PAT[NGROOVE] = {0, 1, 2, 4, 3};
static const char *const GROOVE_NAME[NGROOVE] = {"Drone", "Offbeat", "Dotted", "Sync", "Roller"};
/* the bass's grooves (bit i: a note on sixteenth i) and how long each note holds, in sixteenths (Offbeat's
 * held past the next kick, and Roller's nearly to its next note: the kick lands on a bass the duck shapes, not
 * in silence) */
static const uint16_t GROOVE[5] = {0x0001u, 0x4444u, 0x0909u, 0x4949u, 0x0481u};
static const float GROOVE_GATE[5] = {15.0f, 3.25f, 2.2f, 1.8f, 2.6f};
static const float GROOVE_LVL[5] = {1.0f, 0.5f, 1.0f, 0.9f, 1.0f};   /* (the longer ones lower: Offbeat's held more
                                                                      * than twice as long, and under the kick) */
static const uint8_t DRIFT_BARS[7] = {2, 4, 8, 16, 32, 64, 0};

uint32_t en_groove(int g) { return GROOVE[(unsigned)g < 5u ? g : 0]; }
int en_groove_pat(int knob) { return GROOVE_PAT[(unsigned)knob < NGROOVE ? knob : 0]; }
int en_drift_bars(int v) { return DRIFT_BARS[(unsigned)v < 7u ? v : 6]; }

static char *itoa_s(int v, char *b)
{
    char t[8];
    int n = 0, neg = v < 0;
    if (neg)
        v = -v;
    do {
        t[n++] = (char)('0' + v % 10);
        v /= 10;
    } while (v && n < 6);
    if (neg)
        *b++ = '-';
    while (n)
        *b++ = t[--n];
    *b = 0;
    return b;
}

void en_param_text(int i, int v, char *b)
{
    static const char *const ONOFF[2] = {"Off", "On"};
    const char *s = 0;
    if (i == P_ETIME)
        s = ETIME_NAME[(unsigned)v < 6u ? v : 0];
    else if (i == P_MIDI)
        s = ONOFF[v ? 1 : 0];
    else if (i == P_CVOICE)
        s = EN_CV_NAME[(unsigned)v < EN_NCV ? v : 0];
    else if (i == P_PVOICE)
        s = EN_PV_NAME[(unsigned)v < EN_NPV ? v : 0];
    else if (i == P_BVOICE)
        s = EN_BV_NAME[(unsigned)v < EN_NBV ? v : 0];
    else if (i == P_KIT)
        s = EN_KIT_NAME[(unsigned)v < EN_NKIT ? v : 0];
    else if (i >= P_CR0 && i <= P_CR7)
        s = EN_NOTE_NAME[(unsigned)v < 12u ? v : 0];
    else if (i >= P_CQ0 && i <= P_CQ7)
        s = EN_CQ_NAME[(unsigned)v < EN_NCQ ? v : 0];
    else if (i >= P_CX0 && i <= P_CX7)
        s = EN_CX_NAME[(unsigned)v < EN_NCX ? v : 0];
    else if (i == P_PMODE)
        s = v & EN_PLOOP ? "Loop" : "Endless";
    else if (i == P_NCHAR)
        s = EN_NCHAR_NAME[(unsigned)v < EN_NNCHAR ? v : 0];
    else if (i == P_DMODE)
        s = v ? "Pattern" : "Auto";
    else if (i == P_GROOVE)
        s = GROOVE_NAME[(unsigned)v < NGROOVE ? v : 0];
    else if (i == P_MODE)
        s = EN_MODE_NAME[(unsigned)v < EN_NMODES ? v : 0];
    else if (i == P_ROOT)
        s = EN_NOTE_NAME[(unsigned)v < 12u ? v : 0];
    else if (i == P_DRIFT && !en_drift_bars(v))
        s = "Hold";
    if (s) {
        int k = 0;
        while (s[k] && k < 9) {
            b[k] = s[k];
            k++;
        }
        b[k] = 0;
        return;
    }
    if (i == P_DRIFT) {
        b = itoa_s(en_drift_bars(v), b);
        *b++ = ' ';
        *b++ = 'b';
        *b++ = 'a';
        *b++ = 'r';
        *b++ = 's';
        *b = 0;
        return;
    }
    if (i == P_FEEDBACK) {                       /* as the loop's gain: 45% .. 112% (past 100: the clip holds it) */
        b = itoa_s(45 + (v * 67 + 50) / 100, b);
        *b++ = '%';
        *b = 0;
        return;
    }
    if (i == P_SWING) {                          /* as a swing percentage: 50% (straight) .. 70% */
        b = itoa_s(50 + v / 5, b);
        *b++ = '%';
        *b = 0;
        return;
    }
    if ((i == P_CALG || i == P_PALG || i == P_BALG)) {
        if (!v) {
            b[0] = 'V', b[1] = 'o', b[2] = 'i', b[3] = 'c', b[4] = 'e', b[5] = 0;   /* the voice's own */
        } else if (v == 9) {
            b[0] = '2', b[1] = '-', b[2] = 'o', b[3] = 'p', b[4] = 0;
        } else {
            itoa_s(v, b);
        }
        return;
    }
    if ((i == P_TUNE || i == P_POCT || i == P_BASSOCT || (i >= P_CATT && i < P_CR0) || (i >= P_NCOL && i <= P_NDRIFT)) && v > 0)
        *b++ = '+';
    itoa_s(v, b);
}

/* ------------------------------------------------------- the pure parts --- */
uint32_t en_euclid(int k, int n, int rot)
{
    /* Bjorklund's algorithm (as Toussaint's "The Euclidean Algorithm Generates Traditional Musical
     * Rhythms"): k groups "x" and n - k groups ".", the remainders folded onto the heads until at most
     * one remains. Groups are bit strings, LSB first: (bits, length). */
    uint32_t gb[32], m = 0;
    uint8_t gl[32];
    int ng, heads, tails, i, pos = 0;
    if (n <= 0 || n > 32 || k <= 0)
        return 0;
    if (k > n)
        k = n;
    for (i = 0; i < n; i++) {
        gb[i] = i < k ? 1u : 0u;
        gl[i] = 1;
    }
    ng = n;
    heads = k;
    tails = n - k;
    while (tails > 1 && heads > 0) {
        int pairs = heads < tails ? heads : tails;
        for (i = 0; i < pairs; i++) {           /* a tail onto each head */
            gb[i] |= gb[ng - pairs + i] << gl[i];
            gl[i] = (uint8_t)(gl[i] + gl[ng - pairs + i]);
        }
        if (heads > tails) {                    /* the unpaired heads become the new tails */
            tails = heads - pairs;
            heads = pairs;
        } else {
            tails -= pairs;
            heads = pairs;
        }
        ng = heads + tails;
    }
    for (i = 0; i < ng; i++) {
        uint32_t j;
        for (j = 0; j < gl[i]; j++)
            if (gb[i] >> j & 1u)
                m |= 1u << (((pos + (int)j + rot) % n + n) % n);
        pos += gl[i];
    }
    return m;
}

int en_scale(uint32_t pool, int *steps)
{
    int i, m = 0;
    pool |= 1u;
    for (i = 0; i < 12; i++)
        if (pool >> i & 1u)
            steps[m++] = i;
    return m;
}

/* the chord's tones in stacking order: the root, then every other scale step (thirds in a seven-note
 * scale, fourths in a pentatonic), then what's left; up to max of them, distinct */
static int stack(uint32_t pool, int root, int deg, int *pcs, int max)
{
    static const uint8_t OFF[7] = {0, 2, 4, 6, 1, 3, 5};
    int st[12], m = en_scale(pool, st), n = 0, i, j;
    deg = ((deg % m) + m) % m;
    for (i = 0; i < 7 && n < max; i++) {
        int pc = (root + st[(deg + OFF[i]) % m]) % 12, dup = 0;
        for (j = 0; j < n; j++)
            dup |= pcs[j] == pc;
        if (!dup)
            pcs[n++] = pc;
    }
    return n;
}

int en_chord_tones(uint32_t pool, int root, int deg, int *pcs)
{
    int st[12];
    return stack(pool, root, deg, pcs, en_scale(pool, st) >= 6 ? 4 : 3);
}

void en_voicing(uint32_t pool, int deg, int shape, int *iv)
{
    int t[5], n = stack(pool, 0, deg, t, 5), i, j, r = t[0], d[5];
    for (i = 0; i < 5; i++)                      /* intervals above the chord's root; a short pool repeats */
        d[i] = (t[i < n ? i : i % n] - r + 12) % 12;
    if (shape) {                                 /* root, 7th, 10th, 14th: a 9th without its fifth */
        iv[0] = 0;
        iv[1] = d[3];
        iv[2] = d[1] + 12;
        iv[3] = d[n >= 5 ? 4 : 2] + 12;
    } else {                                     /* root, 5th, 7th, 10th: the open 7th */
        iv[0] = 0;
        iv[1] = d[2];
        iv[2] = d[3];
        iv[3] = d[1] + 12;
    }
    for (i = 1; i < 4; i++) {                    /* ascending, no two alike */
        for (j = i; j > 0 && iv[j] < iv[j - 1]; j--) {
            int x = iv[j];
            iv[j] = iv[j - 1];
            iv[j - 1] = x;
        }
    }
    for (i = 1; i < 4; i++)
        while (iv[i] <= iv[i - 1])
            iv[i] += 12;
}

const char *en_chord_name(const int *pcs, int n, char *buf)
{
    uint32_t iv = 0;
    int i, k = 0, third, fifth;
    const char *q = "", *r = EN_NOTE_NAME[pcs[0] % 12];
    for (i = 1; i < n; i++)
        iv |= 1u << ((pcs[i] - pcs[0] + 12) % 12);
    third = iv >> 4 & 1u ? 4 : iv >> 3 & 1u ? 3 : 0;
    fifth = iv >> 7 & 1u ? 7 : iv >> 6 & 1u ? 6 : iv >> 8 & 1u ? 8 : 0;
    if (third == 4) {
        q = fifth == 8 ? "aug" : iv >> 11 & 1u ? "M7" : iv >> 10 & 1u ? "7" : iv >> 9 & 1u ? "6"
            : iv >> 2 & 1u ? "add9" : "";
    } else if (third == 3) {
        if (fifth == 6)
            q = iv >> 10 & 1u ? "m7b5" : iv >> 9 & 1u ? "dim7" : "dim";
        else
            q = iv >> 10 & 1u ? "m7" : iv >> 11 & 1u ? "mM7" : iv >> 9 & 1u ? "m6" : iv >> 2 & 1u ? "madd9" : "m";
    } else {
        q = iv >> 5 & 1u ? (iv >> 10 & 1u ? "7sus4" : "sus4") : iv >> 2 & 1u ? "sus2" : "5";
    }
    while (*r)
        buf[k++] = *r++;
    while (*q)
        buf[k++] = *q++;
    buf[k] = 0;
    return buf;
}

/* ------------------------------------------------------------ commands --- */
enum { C_SET, C_POOL, C_PLAY, C_NEXT, C_FREEZE, C_SEED, C_PANIC, C_CLOCK, C_REGROW, C_MUTE, C_PERF, C_FX, C_PINGS, C_MACRO, C_PROT, C_REWALK, C_PNUDGE };
#define QN 256u                                 /* (a piece is every parameter at once: room for four) */
_Static_assert(QN >= 2u * (P_NPARAMS + 8), "the command queue can't hold a piece's parameters twice over");
static uint32_t q[QN];
static volatile uint32_t q_w, q_r;
static void post(uint32_t c, int a, int b)
{
    uint32_t w = q_w;
    if (w - q_r >= QN)
        return;                                   /* full: dropped (never, at the UI's rate) */
    q[w % QN] = c << 24 | ((uint32_t)a & 0xFFu) << 16 | ((uint32_t)b & 0xFFFFu);
    BARRIER();
    q_w = w + 1;
}
void en_set(int p, int v) { post(C_SET, p, v); }
void en_pool(uint32_t pool) { post(C_POOL, 0, (int)(pool & 0xFFFu)); }
void en_play(int on) { post(C_PLAY, on, 0); }
void en_next(void) { post(C_NEXT, 0, 0); }
void en_freeze(int on) { post(C_FREEZE, on, 0); }
void en_seed(uint32_t s) { post(C_SEED, 0, (int)(s & 0xFFFFu)); }
void en_panic(void) { post(C_PANIC, 0, 0); }
void en_clock(int msg) { post(C_CLOCK, msg, 0); }
void en_regrow(void) { post(C_REGROW, 0, 0); }
void en_mute(uint32_t m) { post(C_MUTE, (int)(m & 0xFFu), 0); }
void en_perf(uint32_t f) { post(C_PERF, (int)(f & 0xFFu), 0); }
void en_fx(uint32_t m) { post(C_FX, 0, (int)(m & 0xFFFFu)); }
void en_macro(int i, int v) { post(C_MACRO, i, v + 50); }   /* (sent as 0..100) */
void en_prog_rotate(void) { post(C_PROT, 0, 0); }
void en_prog_rewalk(void) { post(C_REWALK, 0, 0); }
void en_ping_nudge(void) { post(C_PNUDGE, 0, 0); }
const char *const EN_FX_NAME[EN_NFX] = {"Throw", "Time x2", "Tape stop", "Swell", "Low-pass", "High-pass", "Wah",
                                        "Phone", "Verb", "Crash", "Freeze", "Wash", "Stutter", "Gate", "Siren", "Riser"};

/* -------------------------------------------------------------- state --- */
volatile uint8_t en_playing, en_frozen, en_ext;
volatile uint8_t en_chord[EN_NPAD], en_chord_root, en_chord_n;
volatile float en_drift, en_pad_level, en_bass_level;
volatile uint8_t en_loop_pos[EN_NLOOP];
volatile int8_t en_loop_note[EN_NLOOP][EN_LOOP_MAX];
volatile float en_loop_glow[EN_NLOOP];
volatile uint8_t en_drum_pos[EN_NDRUM];
volatile uint32_t en_drum_pat[EN_NDRUM];
volatile float en_drum_glow[EN_NDRUM];
volatile uint32_t en_bars;
volatile float en_arc;
volatile uint8_t en_pulse_rest;
volatile uint8_t en_pad_note[EN_NPAD];
volatile float en_pad_amp[EN_NPAD];
volatile uint8_t en_section, en_phrase_bar;
volatile uint32_t en_stab_pat, en_throws, en_stabs;
volatile float en_stab_vel, en_echo_ms, en_echo_g, en_echo_in, en_duck;
volatile uint8_t en_stab_thrown;
volatile uint16_t en_lane[6];
volatile int16_t en_scope[EN_SCOPE_N];
volatile uint32_t en_scope_w;
volatile int16_t en_spec[EN_SPEC_N];
volatile uint32_t en_spec_w;
static float scope_acc, spec_acc;
static uint32_t scope_ph;
static uint32_t mutes, perf, fxm, fxm_was;      /* performance: the layers muted, the moves held; the effects held */
static float perf_sw, perf_echo, mb_c[4], mb_s[4];   /* (mb: Open's and Shut's filter on the music, the kick left out) */                /* the filter pushed open (+) or shut (-), the echo pushed */

static int16_t par[P_NPARAMS];
/* the noise's controls (P_NCOL .. P_NCHAR: render_crackle reads them) */
typedef struct {
    float colour, grain, room, drift;           /* dark .. bright; smooth .. granular; dry .. roomy; steady .. restless */
    uint8_t character;                          /* NCHAR[] */
} noise_ctl_t;
static noise_ctl_t nctl;
static float throw_mul = 1.0f, ping_dry = 0.5f, hats_lvl = 1.0f;   /* Throws, Ghost, Hats level (apply) */
static int gpat(void) { return GROOVE_PAT[(unsigned)par[P_GROOVE] < NGROOVE ? par[P_GROOVE] : 0]; }   /* the pattern */
static int8_t mac[EN_NMAC];                     /* the master actions, -50..50 */
static int8_t pofs[P_NPARAMS];
static float dub_up;                            /* Dub turned up, 0..1: the echo darker, feeding back past its knob */                  /* what they add to the piece's parameters */
static int pe(int p)                            /* a parameter as heard: the piece's, moved by the master actions */
{
    int v = par[p] + pofs[p];
    return v < PARAMS[p].lo ? PARAMS[p].lo : v > PARAMS[p].hi ? PARAMS[p].hi : v;
}
static uint32_t pool = 0xAADu;                  /* Ionian until told */
static int nsteps, steps[12];                   /* the scale */
static int deg;                                 /* the chord's scale degree */
static int chord_pcs[4], chord_nt;
static float tunefac = 1.0f;
static uint32_t rng = 0x9E3779B9u;

static uint32_t rnd(void)
{
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}
static float frand(void) { return (float)(rnd() >> 8) * (1.0f / 16777216.0f); }
static int irand(int n) { return n > 0 ? (int)((rnd() >> 8) % (uint32_t)n) : 0; }

static void midi(uint32_t st, uint32_t d1, uint32_t d2)
{
    if (par[P_MIDI])
        en_midi_out(st, d1, d2);
}

static float rel(int p) { return (float)par[p] * 0.02f; }   /* a sound page's value, -1 .. +1 */

static float mtof(float n) { return 440.0f * tunefac * fm_exp2f((n - 69.0f) * (1.0f / 12.0f)); }

/* a soft clip, cheap: x - 4/27 x^3, flat at +-1.5 (to +-1) */
static inline float sat(float x)
{
    if (x > 1.5f)
        return 1.0f;
    if (x < -1.5f)
        return -1.0f;
    return x - 0.148148148f * x * x * x;
}

/* a sine table: 1024 points a turn, plus one */
#define SN 1024
static float sint[SN + 1];
static inline float sine(float ph)              /* ph in turns, any value >= 0 */
{
    float x = (ph - (float)(int)ph) * (float)SN;
    int i = (int)x;
    float f = x - (float)i;
    return sint[i] + (sint[i + 1] - sint[i]) * f;
}

static inline float blep_saw(float t, float dt)
{
    float y = 2.0f * t - 1.0f;
    if (t < dt) {
        float x = t / dt;
        y -= x + x - x * x - 1.0f;
    } else if (t > 1.0f - dt) {
        float x = (t - 1.0f) / dt;
        y -= x * x + x + x + 1.0f;
    }
    return y;
}

static void svf_coefs(float *c, float f, float k)   /* TPT SVF (Zavalishin): a1, a2, a3, k */
{
    float g = fm_tanf(FM_PI * fm_clampf(f, 40.0f, 16000.0f) * (1.0f / EN_SR));
    c[0] = 1.0f / (1.0f + g * (g + k));
    c[1] = g * c[0];
    c[2] = g * c[1];
    c[3] = k;
}

/* ---------------------------------------------------------- the FM core --- */
/* Four sine operators, as the 4-op FM synths have them (the DX21 / TX81Z family's eight algorithms; op 4 can
 * feed back on itself), and a ninth "algorithm", two operators only (2 into 1), for the light voices. Each
 * operator: a ratio to the note, a level (a modulator's: its index, in turns; a carrier's: its share of the
 * sound) and a decay to a sustain (a share of its level). The levels are worked out at the control rate
 * (every 32 samples), the operators every sample. The algorithms, 4 -> 3 meaning 4 modulates 3:
 *   1: 4 -> 3 -> 2 -> 1            2: (3 + 4) -> 2 -> 1         3: (3 -> 2) + 4 -> 1
 *   4: (4 -> 3) + 2 -> 1           5: 4 -> 3, 2 -> 1 (two pairs) 6: 4 -> 1, 2 and 3
 *   7: 4 -> 3, and 2 and 1         8: all four, side by side     9: 2 -> 1 (two operators) */
#define FM_NALG 9
#define FM_ALG2 8                                /* the two-operator one */
static const uint8_t FM_CARR[FM_NALG] = {1, 1, 1, 1, 5, 7, 7, 15, 1};   /* the carriers (bit k: op k + 1) */
typedef struct {
    float ratio, level, dec, sus;               /* the ratio, the level, the decay (s) to the sustain (0..1 of the level) */
} fmop_t;
typedef struct {
    uint8_t alg;                                /* 0..8 */
    float fb;                                   /* op 4's feedback (turns) */
    fmop_t op[4];
} fmpatch_t;
typedef struct {
    float ph[4], inc[4], e[4], kd[4], lv[4], fb1, fb2;
} fmvoice_t;

/* a note: the operators' rates (rmul: op 2's ratio moved), their envelopes from the top (dmul: the modulators'
 * decays stretched) */
static void fm_note(fmvoice_t *v, const fmpatch_t *p, float f, float rmul, float qmul, float dmul)
{
    int k;
    for (k = 0; k < 4; k++) {
        float r = p->op[k].ratio * (k == 1 ? rmul : k >= 2 ? qmul : 1.0f), d = p->op[k].dec * (k ? dmul : 1.0f);
        v->inc[k] = f * r * (1.0f / EN_SR);
        v->e[k] = 1.0f;
        v->kd[k] = d > 0.0f ? fm_expf(-32.0f / (d * EN_SR)) : 1.0f;
    }
}

static void fm_tune(fmvoice_t *v, const fmpatch_t *p, float f, float rmul, float qmul)   /* a slide: the rates only */
{
    int k;
    for (k = 0; k < 4; k++)
        v->inc[k] = f * p->op[k].ratio * (k == 1 ? rmul : k >= 2 ? qmul : 1.0f) * (1.0f / EN_SR);
}

/* the control rate: the envelopes a step on, the levels (ms: the modulators' scaled, as Colour and Tone have it) */
static void fm_ctl(fmvoice_t *v, const fmpatch_t *p, int alg, float ms)
{
    int k;
    for (k = 0; k < 4; k++) {
        float s = p->op[k].sus;
        v->e[k] = s + (v->e[k] - s) * v->kd[k];
        v->lv[k] = p->op[k].level * v->e[k] * ((FM_CARR[alg] >> k & 1u) ? 1.0f : ms);
    }
}

/* one sample (dk: the pitch now, a multiple of the note's; fb: op 4's feedback) */
static inline float fm_sample(fmvoice_t *v, int alg, float fb, float dk)
{
    float o2, o3, o4, out, *ph = v->ph, *l = v->lv;
    int k;
    if (alg == FM_ALG2) {
        o2 = sine(ph[1] + 8.0f + fb * (v->fb1 + v->fb2)) * l[1];
        v->fb2 = v->fb1;
        v->fb1 = o2;
        out = sine(ph[0] + o2 + 8.0f) * l[0];
        ph[0] += v->inc[0] * dk;
        ph[1] += v->inc[1] * dk;
        for (k = 0; k < 2; k++)
            if (ph[k] >= 1.0f)
                ph[k] -= 1.0f;
        return out;
    }
    o4 = sine(ph[3] + 8.0f + fb * (v->fb1 + v->fb2)) * l[3];   /* (the feedback: the last two, averaged) */
    v->fb2 = v->fb1;
    v->fb1 = o4;
    switch (alg) {
    case 0: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + o3 + 8.0f) * l[1]; out = sine(ph[0] + o2 + 8.0f) * l[0]; break;
    case 1: o3 = sine(ph[2] + 8.0f) * l[2]; o2 = sine(ph[1] + o3 + o4 + 8.0f) * l[1]; out = sine(ph[0] + o2 + 8.0f) * l[0]; break;
    case 2: o3 = sine(ph[2] + 8.0f) * l[2]; o2 = sine(ph[1] + o3 + 8.0f) * l[1]; out = sine(ph[0] + o2 + o4 + 8.0f) * l[0]; break;
    case 3: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + o2 + o3 + 8.0f) * l[0]; break;
    case 4: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + o2 + 8.0f) * l[0] + o3; break;
    case 5: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + o4 + 8.0f) * l[1]; out = sine(ph[0] + o4 + 8.0f) * l[0] + o2 + o3; break;
    case 6: o3 = sine(ph[2] + o4 + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + 8.0f) * l[0] + o2 + o3; break;
    default: o3 = sine(ph[2] + 8.0f) * l[2]; o2 = sine(ph[1] + 8.0f) * l[1]; out = sine(ph[0] + 8.0f) * l[0] + o2 + o3 + o4; break;
    }
    for (k = 0; k < 4; k++) {
        ph[k] += v->inc[k] * dk;
        if (ph[k] >= 1.0f)
            ph[k] -= 1.0f;
    }
    return out;
}

/* the FM voices' patches: the chord's Keys (an electric piano: a body pair, and a tine pair at 14) and Bell
 * (3.5 against 1, 7 against 2); the basses: Punch (a stack, a thump in the first 90 ms), Rubber (two pairs, an
 * octave under, op 4 feeding back), Pluck (2 ops at 3, gone in 50 ms), Growl (2 ops at 1, a bite) */
static const fmpatch_t FM_KEYS = {4, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.35f, 0.15f}, {1.0f, 0.4f, 0.0f, 1.0f},
                                            {14.0f, 0.13f, 0.05f, 0.0f}}};
static const fmpatch_t FM_BELL = {4, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {3.5f, 0.38f, 0.5f, 0.12f}, {2.0f, 0.45f, 0.0f, 1.0f},
                                            {7.07f, 0.16f, 0.18f, 0.0f}}};
static const fmpatch_t FM_PUNCH = {0, 0.12f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.06f, 0.08f}, {2.0f, 0.22f, 0.12f, 0.1f},
                                             {1.0f, 0.12f, 0.2f, 0.25f}}};
static const fmpatch_t FM_RUBBER = {4, 0.3f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.3f, 0.25f, 0.3f}, {0.5f, 0.6f, 0.0f, 1.0f},
                                              {1.0f, 0.18f, 0.3f, 0.4f}}};
static const fmpatch_t FM_PLUCK = {FM_ALG2, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {3.0f, 0.55f, 0.035f, 0.0f}, {2.0f, 0.2f, 0.05f, 0.0f},
                                                   {1.0f, 0.1f, 0.1f, 0.0f}}};
static const fmpatch_t FM_GROWL = {FM_ALG2, 0.0f, {{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.44f, 0.07f, 0.09f}, {2.0f, 0.2f, 0.1f, 0.1f},
                                                   {1.0f, 0.1f, 0.1f, 0.0f}}};
static float fm_q(int p) { return fm_exp2f((float)par[p] * 0.02f); }           /* FM page: the upper ratios, x0.5 .. x2 */
static float fm_d(int p) { return fm_exp2f(1.5f * (float)par[p] * 0.02f); }    /* the modulators' decay */
static float fm_fb(int p, const fmpatch_t *pt) { return fm_clampf(pt->fb + 0.3f * (float)par[p] * 0.02f, 0.0f, 0.7f); }
static int fm_alg_of(int p, const fmpatch_t *pt) { return par[p] ? par[p] - 1 : pt->alg; }

/* ------------------------------------------------- slow random motion --- */
#define NMOD 9                                  /* 0..4 the stab's, 5..8 the echo's */
static struct {
    float v, tgt, rate;
    uint32_t left;                              /* samples until a new target */
} mod[NMOD];

static void mods_step(uint32_t n)
{
    int i;
    for (i = 0; i < NMOD; i++) {
        if (mod[i].left <= n) {
            float secs = 7.0f + 33.0f * frand();
            mod[i].tgt = frand() * 2.0f - 1.0f;
            mod[i].left = (uint32_t)(secs * EN_SR);
            mod[i].rate = (float)n / (secs * EN_SR * 0.5f);
        } else {
            mod[i].left -= n;
        }
        mod[i].v += (mod[i].tgt - mod[i].v) * fm_minf(mod[i].rate, 1.0f);
    }
}
static float motion, evolve;                    /* 0..1: Motion, Evolve */
static float arcm;                              /* the sweep, -1 dark .. +1 open, times Motion */

/* -------------------------------------------------------------- chord --- */
/* The stab: four voices struck together, one envelope, one filter. Saws: two a voice, 7 cents apart;
 * Keys: an FM electric piano, brighter as it's struck. Then a resonant low-pass a side that opens with
 * each stab (its own, quicker envelope) and with the sweep, and a soft clip. */
typedef struct {
    float ph[2], inc[2];
    uint8_t note, midi_note;
} stab_t;
static stab_t stab[EN_NPAD];
static float chord_vol, cut_base = 800.0f, st_dec = 0.9995f, st_fdec = 0.99f, st_t60 = 0.3f;
static float st_env, st_amp, st_fenv, st_throw, throw_dec;
static float st_s[4], st_c[4], st_h[2];
static int32_t st_midi_left;
static int shape, root_note = 55;               /* the voicing's shape, the chord's root as voiced (MIDI) */
static const float STAB_PAN[EN_NPAD] = {-0.5f, -0.15f, 0.2f, 0.55f};
static float stab_gl[EN_NPAD], stab_gr[EN_NPAD];
static int32_t gate_ctr;                        /* samples since the sixteenth began (the grid's Gate) */
static float fx_gate, gate_g, gate_dec;
/* the chord's voices: how it sounds (CV_*), how quickly it strikes (the amplitude's smoothing a sample:
 * 0.035 ~0.6 ms .. 0.00012 ~190 ms), its length (times Decay's), how far its own envelope opens the filter
 * (octaves), its modulator's ratio and index (FM voices, in turns: the index at the strike, and at rest),
 * and a level to match the others */
enum { CV_SAWS, CV_KEYS, CV_ORGAN, CV_PAD, CV_BELL, CV_WHEEL, CV_NOISE };
static const struct {
    float att, dec, fenv, ratio, idx_hi, idx_lo, gain;
} CV[EN_NCV] = {
    /* (1.3.0: each strike opens further, and closes sooner) */
    {0.035f, 1.0f, 2.5f, 0.0f, 0.0f, 0.0f, 1.0f},        /* Saws: two saws a voice, 7 cents apart */
    {0.035f, 1.3f, 1.4f, 1.0f, 0.41f, 0.06f, 1.23f},     /* Keys: FM at the carrier's pitch, an electric piano */
    {0.02f, 2.5f, 0.7f, 0.0f, 0.0f, 0.0f, 0.75f},        /* Organ: four drawbars (1, 2, 3, 4) */
    {0.00012f, 4.0f, 0.6f, 0.0f, 0.0f, 0.0f, 0.72f},     /* Pad: the saws, swelling in, long */
    {0.05f, 1.8f, 1.1f, 3.5f, 0.35f, 0.05f, 1.17f},      /* Bell: FM at 3.5, the clang dying first */
    {0.03f, 3.0f, 0.4f, 0.0f, 0.0f, 0.0f, 0.67f},        /* Tonewheel: drawbars, a percussion and a click at the
                                                          * strike, a rotary speaker turning (left against right) */
    {0.05f, 0.6f, 2.6f, 0.0f, 0.0f, 0.0f, 3.6f},         /* Noise: noise and a little saw, the filter's pitch */
};
static int cvoice;                              /* the chord's voice now */
static fmvoice_t cfm[EN_NPAD];                  /* its FM voices' operators */
static const fmpatch_t *cv_patch(int cv) { return cv == 1 ? &FM_KEYS : cv == 4 ? &FM_BELL : 0; }
static float rotor, click;                      /* Tonewheel: the rotor's turn, the key click */

/* ------------------------------------------------------------------ bass --- */
static struct {
    float ph, note, target, env;
    int32_t gate;                               /* samples left of the note */
    uint8_t midi_note;
} bass;
static float bass_vol;
/* the bass's voices: Sub (a sine and a little of its octave, softly clipped), Round (its third harmonic
 * too, rounder), Growl (FM at 1, a bite that opens with each note and closes), Pulse (a sine clipped hard
 * towards a square) */
enum { BV_SUB, BV_PUNCH, BV_RUBBER, BV_PLUCK, BV_GROWL };
static const float BV_GAIN[EN_NBV] = {1.0f, 1.1f, 1.0f, 1.0f, 1.16f};
static const fmpatch_t *const BV_PATCH[EN_NBV] = {0, &FM_PUNCH, &FM_RUBBER, &FM_PLUCK, &FM_GROWL};
static fmvoice_t bfm;
static float bass_bite, bite_dec;

/* ----------------------------------------------------------------- pings --- */
#define NBELL 6
typedef struct {
    float amp, adec, att, gl, gr, drop;
    fmvoice_t fm;
    fmpatch_t pt;                               /* its own patch: the ping's voice, as its Tone and the loop have it */
    int32_t wait;
    uint8_t on, note, loop;
} bell_t;
static bell_t bell[NBELL];
static float bell_vol, btone;
static int8_t loop_note[EN_NLOOP][EN_LOOP_MAX];
static uint8_t loop_vel[EN_NLOOP][EN_LOOP_MAX], loop_pos[EN_NLOOP];
static const uint8_t LOOP_BASE[EN_NLOOP] = {72, 67, 79, 62};     /* each loop's register */
static const float LOOP_PAN[EN_NLOOP] = {-0.6f, 0.35f, 0.7f, -0.25f};
static const float RATIO[5] = {1.0f, 2.0f, 3.0f, 3.5f, 4.77f};

/* ----------------------------------------------------------------- drums --- */
typedef struct {
    float ph, inc, inc_end, amp, adec, x1, x2, att, mph[4], pend, clk, lp[2];   /* (lp: a hat's low-pass) */
    int32_t wait;                               /* swing: samples until a hit that waits (pend: its velocity) */
    uint8_t on, note;
} drum_t;
static drum_t drum[EN_NDRUM];
static uint32_t drum_pat[EN_NDRUM];             /* 0 the kick's (four on the floor), 1 the shaker's, 2 the open hat's */
static float pulse_vol, swing, hat_f = 0.9f, hat_open = 0.25f;
static float step_len, pulse_left;               /* a sixteenth, in samples; to the next MIDI clock pulse */
static uint32_t noise = 22222u, cnoise = 4321u;
static const uint8_t DRUM_GM[EN_NDRUM] = {36, 42, 46, 37, 39};  /* kick, closed hat, open hat, side stick, hand clap */
static const float HAT_INC[4] = {304.4f / EN_SR, 369.6f / EN_SR, 522.7f / EN_SR, 800.0f / EN_SR};
/* the shaker's steps, the most wanted first (the off sixteenths before the downbeats; never the open
 * hat's off beats) */
static uint8_t hat_order[12];
static const uint8_t HAT_ORDER[12] = {3, 11, 7, 15, 1, 9, 5, 13, 0, 8, 4, 12};
static int hat_k, hat_kb;
static float duck_x, duck_d, duck_depth, duck_rel;
static float duck_xb, duck_db, duck_relb;       /* the bass's duck: deeper, and slower to let go (it pumps) */
/* the kits: the kick (its pitch at the strike, times its note; how quickly it falls; its length; its click;
 * its drive) and the hats (their high-pass, times Hats'; how much noise against the metal; their length) */
static const struct {
    float start, fall, t60, click, drive, hat_f, noise, hat_t, gain;
} KIT[EN_NKIT] = {
    /* (1.3.0's sound pass: each kick from lower, longer, with half the click; a soft, rolling pulse, the
     * thump down from 100-200 Hz into the sub) */
    {2.75f, 0.0016f, 0.63f, 0.25f, 1.5f, 1.0f, 0.7f, 1.0f, 1.0f},  /* Tight */
    {2.0f, 0.0008f, 0.7f, 0.1f, 1.3f, 0.92f, 0.7f, 1.2f, 0.6f},    /* Deep: long, 808-ish (shorter and softer than
                                                                    * it was: over a bass that holds now) */
    {4.0f, 0.003f, 0.38f, 0.45f, 2.5f, 1.05f, 0.6f, 0.8f, 1.35f},  /* Punch: short and hard */
    {1.8f, 0.0012f, 0.75f, 0.0f, 1.0f, 0.82f, 0.85f, 1.3f, 0.86f}, /* Soft: a muffled thump */
    {2.6f, 0.0016f, 0.6f, 0.18f, 1.8f, 0.75f, 1.0f, 1.1f, 1.0f},   /* Dusty: darker, noisier hats */
};
static int kit;

/* ------------------------------------------------------------------ space --- */
static float echo_send, rev_send, echo_len, echo_len_t, echo_fb = 0.6f, ein_gain = 1.0f;
static float fb_cap = 2.0f;                     /* stopped, the feedback eases under 90%: the tails die */
static float fx_em = 1.0f, fx_ek = 0.001f, fx_ts = 1.0f;   /* the grid on the echo: its time's multiple, its easing; the tape stop */
static float rev_in_gain = 1.0f;                /* falls to 0 while frozen */
static float tape_lpc = 0.55f, tape_hpc = 0.08f; /* the echo's feedback band-pass */
static float crackle_vol, cloud_amt;
static float cr_wd, cr_ww, cr_wr;               /* Crackle's dust, wind and rumble: their share, by the knob */

/* ------------------------------------------------------- the arrangement --- */
static uint32_t sixteenth, bar, chord_bars;    /* bars since the start, since the chord moved */
static uint8_t sec, sec_next, fill;
static uint8_t lay_kick = 1, lay_hats = 1, lay_open = 1, lay_bass = 1, lay_stabs = 1;
static float arc, arc_tgt, arc_tau = 1.0f;     /* the sweep, its target, its time (samples) */
static uint8_t change_pending;

/* -------------------------------------------------------------- harmony --- */
static void scale_set(void)
{
    nsteps = en_scale(pool, steps);
}

static int bell_note(int l, int off)            /* a loop slot's note, from the chord's degree */
{
    int d = deg + off, oct = d / nsteps, s = steps[d % nsteps], base, root = par[P_ROOT], n;
    base = LOOP_BASE[l] - ((LOOP_BASE[l] - root) % 12 + 12) % 12;   /* the root at or below the register */
    n = base + s + 12 * oct + 12 * par[P_POCT];
    while (n > 103)
        n -= 12;
    while (n < 36)
        n += 12;
    return n;
}

/* the chord to the four voices: the voicing on the degree, its root moved the least from the last one,
 * in C3..B3 (as a chord memory: the shape kept, transposed whole; 1.3.0 up from F#2..F3, out of the low-mids) */
static int chord_slot = -1;                     /* the slot whose chord sounds (-1: one of the scale) */
static void voice_chord(void)
{
    int iv[4], i, pc, n;
    if (chord_slot >= 0 && par[P_CQ0 + chord_slot]) {   /* a slot's chord, as written */
        int r = par[P_CR0 + chord_slot];
        en_chord_iv(par[P_CQ0 + chord_slot], par[P_CX0 + chord_slot], iv);
        for (i = 0; i < 4; i++)
            chord_pcs[i] = (r + iv[i]) % 12;
        {                                        /* (in name order for the bass: the root, its third, its fifth) */
            int t[4];
            en_chord_iv(par[P_CQ0 + chord_slot], EN_CX_7, t);
            chord_pcs[1] = (r + t[3] - 12) % 12;
            chord_pcs[2] = (r + t[1]) % 12;
            chord_pcs[3] = (r + t[2]) % 12;
        }
        chord_nt = 4;
        deg = en_deg_of(pool, r - par[P_ROOT]);
    } else {
        chord_slot = -1;
        chord_nt = en_chord_tones(pool, par[P_ROOT], deg, chord_pcs);
        en_voicing(pool, deg, shape, iv);
    }
    en_chord_slot = (int8_t)chord_slot;
    pc = chord_pcs[0];
    n = root_note + ((pc - root_note) % 12 + 12) % 12;
    if (n - root_note > 6)
        n -= 12;
    while (n < 48)
        n += 12;
    while (n > 59)
        n -= 12;
    root_note = n;
    for (i = 0; i < EN_NPAD; i++) {
        stab[i].note = (uint8_t)(n + iv[i]);
        en_chord[i] = stab[i].note;
    }
    en_chord_root = (uint8_t)chord_pcs[0];
    en_chord_n = (uint8_t)chord_nt;
}

static int bass_note(int pc)                    /* E1..Eb2, an octave up or down (E0..Eb1: felt more than heard) */
{
    return 28 + ((pc - 4) % 12 + 12) % 12 + 12 * par[P_BASSOCT];
}

/* the chords' slots: how far the progression runs (to its last filled slot; 0: none, or Play at Random), a
 * slot's chord built from its kind and type, and a degree for a note above the root (the pings take the
 * scale's notes around it: nearest in the pool, the lower on a tie) */
volatile int8_t en_prog_pos = -1, en_chord_slot = -1;
volatile int8_t en_prog_slot[EN_NPROG][3];
volatile uint32_t en_prog_gen;
static int slot_gap(int i) { return !par[P_CQ0 + i] && !par[P_CR0 + i]; }
static int slot_walk(int i) { return !par[P_CQ0 + i] && par[P_CR0 + i]; }
static int prog_len(void)                       /* Loop: to the last slot that isn't a gap (0: none) */
{
    int i, n = 0;
    for (i = 0; i < EN_NPROG; i++)
        if (!slot_gap(i))
            n = i + 1;
    return n;
}
static void prog_publish(void)                  /* the slots, for the UI to take */
{
    int i;
    for (i = 0; i < EN_NPROG; i++) {
        en_prog_slot[i][0] = (int8_t)par[P_CR0 + i];
        en_prog_slot[i][1] = (int8_t)par[P_CQ0 + i];
        en_prog_slot[i][2] = (int8_t)par[P_CX0 + i];
    }
    en_prog_gen++;
}
void en_chord_iv(int q, int x, int *iv)
{
    static const int8_t THIRD[EN_NCQ] = {3, 4, 3, 2, 5, 3, 4}, FIFTH[EN_NCQ] = {7, 7, 7, 7, 7, 6, 8};
    int t = THIRD[(unsigned)q < EN_NCQ ? q : 0], f = FIFTH[(unsigned)q < EN_NCQ ? q : 0],
        s = q == EN_CQ_DIM ? 9 : 10, i, j;   /* the seventh: minor (a diminished seventh on Dim) */
    switch ((unsigned)x < EN_NCX ? x : 0) {
    case EN_CX_6: iv[0] = 0, iv[1] = f, iv[2] = 9, iv[3] = t + 12; break;
    case EN_CX_7: iv[0] = 0, iv[1] = f, iv[2] = s, iv[3] = t + 12; break;
    case EN_CX_MAJ7: iv[0] = 0, iv[1] = f, iv[2] = 11, iv[3] = t + 12; break;
    case EN_CX_9: iv[0] = 0, iv[1] = s, iv[2] = t + 12, iv[3] = 14; break;
    case EN_CX_11: iv[0] = 0, iv[1] = s, iv[2] = t + 12, iv[3] = 17; break;
    case EN_CX_ADD9: iv[0] = 0, iv[1] = f, iv[2] = t + 12, iv[3] = 14; break;
    default: iv[0] = 0, iv[1] = f, iv[2] = 12, iv[3] = t + 12; break;   /* the triad, open, its root doubled */
    }
    for (i = 1; i < 4; i++)                      /* the type on the sus's own note (sus2 with a 9, sus4 with an 11): */
        for (j = 0; j < i; j++)                  /* the fifth, an octave up, instead */
            if (iv[i] == iv[j])
                iv[i] = f + 12;
    for (i = 1; i < 4; i++)                      /* ascending, no two alike */
        for (j = i; j > 0 && iv[j] < iv[j - 1]; j--) {
            int y = iv[j];
            iv[j] = iv[j - 1];
            iv[j - 1] = y;
        }
    for (i = 1; i < 4; i++)
        while (iv[i] <= iv[i - 1])
            iv[i] += 12;
}
void en_walk_chord(uint32_t pool, int root, int deg, int *r, int *q, int *x)
{
    int pcs[4], n = en_chord_tones(pool, root, deg, pcs), t, f, s;
    t = (pcs[1] - pcs[0] + 12) % 12;
    f = (pcs[2 < n ? 2 : 0] - pcs[0] + 12) % 12;
    s = n >= 4 ? (pcs[3] - pcs[0] + 12) % 12 : -1;
    *r = pcs[0];
    *q = t == 2 ? EN_CQ_SUS2 : t == 5 ? EN_CQ_SUS4 : t == 3 ? (f == 6 ? EN_CQ_DIM : EN_CQ_MINOR)
       : (f == 8 ? EN_CQ_AUG : EN_CQ_MAJOR);
    *x = s == 10 || (s == 9 && *q == EN_CQ_DIM) ? EN_CX_7 : s == 11 ? EN_CX_MAJ7 : s == 9 ? EN_CX_6 : EN_CX_TRIAD;
}
const char *en_chord_label(int root, int q, int x, char *b)
{
    static const char *const QS[EN_NCQ] = {"", "", "m", "sus2", "sus4", "dim", "aug"};
    static const char *const XS[EN_NCX] = {"", "6", "7", "M7", "9", "11", "add9"};
    const char *r = EN_NOTE_NAME[(unsigned)root < 12u ? root : 0], *qs, *xs;
    int k = 0, sus;
    q = (unsigned)q < EN_NCQ ? q : 0;
    x = (unsigned)x < EN_NCX ? x : 0;
    if (!q) {
        b[0] = '-', b[1] = 0;
        return b;
    }
    qs = QS[q], xs = XS[x], sus = q == EN_CQ_SUS2 || q == EN_CQ_SUS4;
    while (*r)
        b[k++] = *r++;
    if (sus) {                                   /* the type before a sus: "C7sus4" */
        while (*xs)
            b[k++] = *xs++;
        while (*qs)
            b[k++] = *qs++;
    } else {
        while (*qs)
            b[k++] = *qs++;
        while (*xs)
            b[k++] = *xs++;
    }
    b[k] = 0;
    return b;
}
int en_deg_of(uint32_t pl, int rel)
{
    int st[12], m = en_scale(pl, st), i, best = 0, bd = 99;
    rel = ((rel % 12) + 12) % 12;
    for (i = 0; i < m; i++) {
        int d = (st[i] - rel + 12) % 12, e = d > 6 ? 12 - d : d;
        if (e < bd) {
            bd = e;
            best = i;
        }
    }
    return best;
}
static int prog_pos = -1;

/* to another degree: home most of all, then the subdominant, the seventh, the dominant; away from home,
 * home again more often than not. Never a diminished fifth (a half-diminished chord is no dub chord) */
/* the walk: the next degree after d (and before it, d2), in Play's character (0 Home, 1 Wander, 2 Free; Loop: as
 * Home). Away from home, back (Home 65%, Wander 35%, Free 15%); else a weighted choice, never the same degree,
 * never a diminished one: Home leans on the IV, then the VII and the V; Wander spreads wider; Free is even. Wander
 * and Free rarely step straight back to d2 (no ping-pong) */
const char *const EN_WALK_NAME[EN_NWALK] = {"Home", "Wander", "Free"};
#define PLAY_LOOP (par[P_PMODE] & EN_PLOOP)
static int walk_shape;                          /* the walk's voicing, now and then the other (Evolve) */
static int walk_prev;                           /* the degree before the walk's last */
static int walk_from(int d)
{
    static const float W7[3][7] = {{0.0f, 0.4f, 1.0f, 3.0f, 1.5f, 1.5f, 2.0f},
                                   {0.0f, 1.2f, 1.2f, 2.0f, 1.6f, 1.8f, 1.4f},
                                   {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}};
    static const float HOME[3] = {0.65f, 0.35f, 0.15f}, BACK[3] = {1.0f, 0.3f, 0.15f};
    const int st = (par[P_PMODE] & 3) < EN_NWALK ? par[P_PMODE] & 3 : EN_WALK_WANDER, d2 = walk_prev;
    float w[12], sum = 0.0f, r;
    int i;
    walk_prev = d;
    if (d != 0 && frand() < HOME[st] * (d2 == 0 && st ? 0.6f : 1.0f)) {   /* (just back from home: less often again) */
        i = 0;
    } else {
        for (i = 0; i < nsteps; i++) {
            int t[3], n = stack(pool, 0, i, t, 3);
            w[i] = i == d || i == 0 ? 0.0f : nsteps == 7 ? W7[st][i] : 1.0f;
            if (i == d2)
                w[i] *= BACK[st];
            if (n >= 3 && (t[2] - t[0] + 12) % 12 == 6)
                w[i] = 0.0f;
            sum += w[i];
        }
        r = frand() * sum;
        for (i = 0; i < nsteps; i++) {
            r -= w[i];
            if (r <= 0.0f && w[i] > 0.0f)
                break;
        }
        if (i >= nsteps)
            i = nsteps > 1 ? 1 : 0;
    }
    if (frand() < evolve * 0.1f)
        walk_shape ^= 1;                         /* now and then, the other voicing */
    return i;
}
static int walk_last;                           /* the degree the walk wrote last (it goes on from there) */
static void slot_walk_set(int k, int d)         /* slot k: the walk's degree d */
{
    par[P_CQ0 + k] = 0;
    par[P_CR0 + k] = (int16_t)(d + 1);
    par[P_CX0 + k] = (int16_t)walk_shape;
    walk_last = d;
}
static void walk_fill(void)                     /* Endless with every slot a gap (a fresh piece): eight from home */
{
    int k, d = 0;
    for (k = 0; k < EN_NPROG; k++)
        if (!slot_gap(k))
            return;
    walk_prev = 0;
    slot_walk_set(0, 0);
    for (k = 1; k < EN_NPROG; k++) {
        d = walk_from(d);
        slot_walk_set(k, d);
    }
    prog_publish();
}
static void slot_play(int k)                    /* slot k sounds: its chord as written, the walk's, or (a gap) held */
{
    if (par[P_CQ0 + k]) {
        chord_slot = k;
        voice_chord();
    } else if (par[P_CR0 + k]) {
        chord_slot = -1;
        deg = (par[P_CR0 + k] - 1) % (nsteps ? nsteps : 1);
        shape = par[P_CX0 + k] & 1;
        voice_chord();
    }
    chord_bars = 0;
}

static void chord_change(void)
{
    int k;
    if (PLAY_LOOP) {                            /* Loop: the slots in turn, a gap holding the chord before it */
        int n = prog_len();
        if (n) {
            prog_pos = (prog_pos + 1) % n;
            en_prog_pos = (int8_t)prog_pos;
            slot_play(prog_pos);
        }
        chord_bars = 0;
        return;
    }
    /* Endless: all up a slot (a gap stays a gap), the walk writes slot 8; slot 1 sounds */
    for (k = 0; k < EN_NPROG - 1; k++) {
        par[P_CR0 + k] = par[P_CR0 + k + 1];
        par[P_CQ0 + k] = par[P_CQ0 + k + 1];
        par[P_CX0 + k] = par[P_CX0 + k + 1];
    }
    slot_walk_set(EN_NPROG - 1, walk_from(walk_last));
    prog_pos = 0;
    en_prog_pos = 0;
    prog_publish();
    slot_play(0);
}

/* ------------------------------------------------------------- the loops --- */
static int rand_offset(void)                    /* scale steps above the chord's degree */
{
    if (frand() < 0.75f) {                      /* a chord tone, sometimes an octave up */
        int o = 2 * irand(chord_nt);
        return o + (frand() < 0.25f ? nsteps : 0);
    }
    return 1 + 2 * irand(3);                    /* a passing note: the 2nd, 4th or 6th */
}

static int loop_count(int l)
{
    int i, c = 0;
    for (i = 0; i < EN_LOOP_LEN[l]; i++)
        c += loop_note[l][i] >= 0;
    return c;
}

static int loop_target(void)                    /* 0..5 notes: Density, thinned in the dark */
{
    float d = (float)pe(P_DENSITY) * 0.05f * (1.0f + 0.5f * arcm);
    return (int)(fm_clampf(d, 0.0f, 5.0f) + frand());
}
static uint8_t loop_rest[EN_NLOOP];             /* this pass: silent (Evolve) */

static void loop_add(int l)
{
    int len = EN_LOOP_LEN[l], i, s;
    for (i = 0; i < 8; i++) {
        s = irand(len);
        if (loop_note[l][s] < 0) {
            loop_note[l][s] = (int8_t)rand_offset();
            loop_vel[l][s] = (uint8_t)(50 + irand(50));
            return;
        }
    }
}

static int loop_pick(int l)                     /* a random sounding slot, -1 none */
{
    int len = EN_LOOP_LEN[l], i, s = irand(len);
    for (i = 0; i < len; i++, s = (s + 1) % len)
        if (loop_note[l][s] >= 0)
            return s;
    return -1;
}

static void loop_publish(int l)
{
    int i;
    for (i = 0; i < EN_LOOP_MAX; i++)
        en_loop_note[l][i] = i < EN_LOOP_LEN[l] ? loop_note[l][i] : -1;
}

static void loop_regrow(int l)
{
    int i, n = loop_target();
    for (i = 0; i < EN_LOOP_MAX; i++)
        loop_note[l][i] = -1;
    for (i = 0; i < n; i++)
        loop_add(l);
    loop_publish(l);
}

/* a piece's own loops (en_pings): packed by the UI, unpacked here */
static uint8_t pings_pend[EN_PINGS_N], pings_have;
void en_pings_get(uint8_t *b)
{
    int l, i, k = 0;
    for (l = 0; l < EN_NLOOP; l++)
        for (i = 0; i < EN_LOOP_LEN[l]; i++, k++) {
            int nt = loop_note[l][i], v = (loop_vel[l][i] - 50) / 3;
            b[k] = (uint8_t)((nt < 0 || nt > 14 ? 15 : nt) | (v < 0 ? 0 : v > 15 ? 15 : v) << 4);
        }
}
void en_pings(const uint8_t *b)
{
    int i;
    for (i = 0; b && i < EN_PINGS_N; i++)
        pings_pend[i] = b[i];
    post(C_PINGS, b ? 1 : 0, 0);
}
static void pings_take(void)
{
    int l, i, k = 0;
    for (l = 0; l < EN_NLOOP; l++) {
        for (i = 0; i < EN_LOOP_MAX; i++)
            loop_note[l][i] = -1;
        for (i = 0; i < EN_LOOP_LEN[l]; i++, k++) {
            int nt = pings_pend[k] & 15;
            loop_note[l][i] = (int8_t)(nt == 15 ? -1 : nt);
            loop_vel[l][i] = (uint8_t)(50 + 3 * (pings_pend[k] >> 4));
        }
        loop_publish(l);
    }
}

static void loop_pass(int l)                    /* the end of a pass: grow, prune, evolve */
{
    int c = loop_count(l), t = loop_target(), s;
    if (evolve < 0.005f) {                      /* Evolve 0: the loops as they are (Density changed: to it) */
        t = (int)(fm_clampf((float)pe(P_DENSITY) * 0.05f, 0.0f, 5.0f) + 0.5f);
        for (; c < t; c++)
            loop_add(l);
        for (; c > t && (s = loop_pick(l)) >= 0; c--)
            loop_note[l][s] = -1;
        loop_rest[l] = frand() < 0.2f * fm_maxf(-arcm, 0.0f);   /* (Motion's dark: a pass sat out, the notes kept) */
        loop_publish(l);
        return;
    }
    for (; c < t; c++)
        loop_add(l);
    for (; c > t && (s = loop_pick(l)) >= 0; c--)
        loop_note[l][s] = -1;
    loop_rest[l] = frand() < evolve * 0.2f + 0.2f * fm_maxf(-arcm, 0.0f);
    if (frand() < evolve * 0.7f && (s = loop_pick(l)) >= 0) {
        if (frand() < 0.5f) {
            loop_note[l][s] = (int8_t)rand_offset();             /* another note */
        } else {
            int d = irand(EN_LOOP_LEN[l]);                       /* the same note, elsewhere */
            if (loop_note[l][d] < 0) {
                loop_note[l][d] = loop_note[l][s];
                loop_vel[l][d] = loop_vel[l][s];
                loop_note[l][s] = -1;
            }
        }
    }
    loop_publish(l);
}

/* each loop its own ping: index, decay and ratio a little apart */
static const float LOOP_IDX[EN_NLOOP] = {1.0f, 0.55f, 1.5f, 0.8f}, LOOP_DEC[EN_NLOOP] = {1.0f, 1.3f, 0.7f, 1.5f};
static const int8_t LOOP_RAT[EN_NLOOP] = {0, 0, 1, -1};
/* the pings' voices: the modulator's ratio (0: by Ping tone, RATIO), the index, its decay and the ping's
 * length (times the Blip's), a falling pitch (Drop: from this many times its note), a level */
static const struct {
    float ratio, idx, idec, t60, drop, gain;
} PV[EN_NPV] = {
    {0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},               /* Blip */
    {4.0f, 1.2f, 0.4f, 0.6f, 0.0f, 1.45f},              /* Marimba: a wooden bar's fourth partial */
    {3.5f, 0.8f, 2.0f, 1.7f, 0.0f, 0.79f},              /* Glass: a long, bright ring */
    {1.41f, 2.5f, 0.2f, 0.25f, 0.0f, 2.4f},             /* Wood: a knock */
    {1.0f, 0.6f, 1.0f, 0.8f, 1.0f, 1.29f},              /* Drop: the pitch falling from an octave up */
};

static void bell_voice(int l, int n, float vel, int32_t wait)
{
    int i, best = 0, ri, pv;
    float lo = 9.0f, t60, p;
    bell_t *v;
    for (i = 0; i < NBELL; i++) {               /* a free voice, or the quietest */
        float a = bell[i].on ? bell[i].amp : -1.0f;
        if (a < lo) {
            lo = a;
            best = i;
        }
    }
    v = &bell[best];
    if (v->on)
        midi(0x81, v->note, 0);
    v->on = 1;
    v->note = (uint8_t)n;
    v->loop = (uint8_t)l;
    pv = (unsigned)par[P_PVOICE] < EN_NPV ? par[P_PVOICE] : 0;
    ri = (int)(btone * 100.0f + 0.5f) * 5 / 101 + LOOP_RAT[l];
    v->drop = fm_clampf(PV[pv].drop + 1.5f * rel(P_PBEND), 0.0f, 3.0f);   /* Bend: from up to four times its note */
    v->amp = vel * PV[pv].gain;
    t60 = fm_clampf(0.9f - (float)(n - 60) * 0.02f, 0.25f, 1.1f) * LOOP_DEC[l] * PV[pv].t60   /* short: a ping */
          * fm_exp2f(1.5f * rel(P_PLEN));
    v->adec = fm_expf(-6.9f / (t60 * EN_SR));
    /* its patch: a carrier, the modulator (Ping tone: 0 a round blip, 100 a metallic clank: its index, in radians
     * as turns, and its decay; Ratio, the sound page's: its ratio), and two more operators for the 4-op algorithms
     * (Glass's own: two pairs) */
    {
        float idx = (0.2f + 5.0f * btone * btone) * (1.0f + 0.4f * fm_maxf(perf_sw, 0.0f)) * LOOP_IDX[l] * (0.6f + 0.4f * vel) * PV[pv].idx * (1.0f / FM_TWO_PI),
              td = (0.08f + 0.5f * btone) * PV[pv].idec * (1.0f / 6.9f);
        fmpatch_t *pt = &v->pt;
        pt->alg = pv == 2 ? 4 : FM_ALG2;
        pt->fb = 0.0f;
        pt->op[0] = (fmop_t){1.0f, 1.0f, 0.0f, 1.0f};
        pt->op[1] = (fmop_t){(PV[pv].ratio > 0.0f ? PV[pv].ratio : RATIO[ri < 0 ? 0 : ri > 4 ? 4 : ri]) * fm_exp2f(0.8f * rel(P_PRAT)),
                             idx, td, 0.0f};
        pt->op[2] = (fmop_t){2.0f, 0.45f, 0.0f, 1.0f};
        pt->op[3] = (fmop_t){pv == 2 ? 5.0f : 3.0f, idx * 0.3f, td * 0.6f, 0.0f};
        fm_note(&v->fm, pt, mtof((float)n), 1.0f, fm_q(P_PFRAT), fm_d(P_PFDEC));
        v->fm.ph[0] = v->fm.ph[1] = v->fm.ph[2] = v->fm.ph[3] = 0.0f;
        fm_ctl(&v->fm, pt, fm_alg_of(P_PALG, pt), 1.0f);
    }
    v->att = 0.0f;
    p = (LOOP_PAN[l] + 1.0f) * (FM_PI / 4.0f);
    v->gl = fm_cosf(p);
    v->gr = fm_sinf(p);
    v->wait = wait;
    en_loop_glow[l] = 1.0f;
    midi(0x91, (uint32_t)n, (uint32_t)(vel * 110.0f));
}

static void bell_ring(int l, int slot)
{
    int n = bell_note(l, loop_note[l][slot]);
    float vel = (float)loop_vel[l][slot] * 0.01f;
    bell_voice(l, n, vel, irand(220));           /* a hand on the grid: up to 5 ms late */
}

/* ------------------------------------------------------------- the stabs --- */
static int stab_k, stab_kb, stab_rot;
static uint32_t stab_home;                      /* the pattern, before this bar's gaps */

static void stabs_home(void)                    /* Stabs: one a bar .. six, off the beat */
{
    stab_kb = stab_k = 1 + (pe(P_STABS) * 5 + 50) / 100;
    stab_rot = 2;
    stab_home = en_euclid(stab_k, 16, stab_rot);
}

static void stabs_evolve(void)                  /* the pattern turns a step, or gains or loses a stab */
{
    if (frand() < 0.6f) {
        stab_rot = (stab_rot + (frand() < 0.5f ? 1 : 15)) % 16;
    } else {
        int k = stab_k + (frand() < 0.5f ? 1 : -1);
        if (k >= stab_kb - 1 && k <= stab_kb + 1 && k >= 1 && k <= 8)
            stab_k = k;
    }
    stab_home = en_euclid(stab_k, 16, stab_rot);
}

static void stabs_bar(void)                     /* this bar's stabs: the pattern, with gaps */
{
    uint32_t p = stab_home, out = 0;
    int i, first = 1;
    if (!lay_stabs)
        p = 0;
    else if (sec == SEC_GROOVE)                 /* a Groove: every other stab */
        for (i = 0; i < 16; i++)
            if (p >> i & 1u) {
                if (!first)
                    p &= ~(1u << i);
                first = !first;
            }
    first = 1;
    for (i = 0; i < 16; i++)
        if (p >> i & 1u) {
            if (first || frand() >= 0.2f * evolve)
                out |= 1u << i;
            first = 0;
        }
    en_stab_pat = out;
}

static void stab_fire(float vel, int throw_it)
{
    int i;
    if (st_midi_left > 0)
        for (i = 0; i < EN_NPAD; i++)
            midi(0x80, stab[i].midi_note, 0);
    for (i = 0; i < EN_NPAD; i++) {
        float f = mtof((float)stab[i].note);
        float dt = 0.00202f * fm_exp2f(2.0f * rel(P_CCOL));   /* +-3.5 cents (Colour: +-0.9 .. +-14) */
        stab[i].inc[0] = f * (1.0f - dt) * (1.0f / EN_SR);
        stab[i].inc[1] = CV[cvoice].ratio > 0.0f ? f * CV[cvoice].ratio * (1.0f / EN_SR)   /* FM: the modulator */
                                                 : f * (1.0f + dt) * (1.0f / EN_SR);
        if (cv_patch(cvoice))
            fm_note(&cfm[i], cv_patch(cvoice), f, 1.0f, fm_q(P_CFRAT), fm_d(P_CFDEC));
        stab[i].midi_note = stab[i].note;
        midi(0x90, stab[i].note, (uint32_t)(40.0f + 80.0f * vel));
    }
    st_env = vel;
    st_fenv = 1.0f;
    click = cvoice == CV_WHEEL ? 0.12f * vel : 0.0f;
    en_stab_vel = vel;
    en_stab_thrown = (uint8_t)(throw_it != 0);
    en_stabs++;
    st_midi_left = (int32_t)(fm_maxf(st_t60 * 0.5f, 0.06f) * EN_SR);
    if (throw_it) {
        st_throw = 1.0f;
        en_throws++;
    }
}

/* ---------------------------------------------------------------- drums --- */
static void hats_publish(void)
{
    uint32_t c = 0, o = 0;
    int i, k = pe(P_HITS), ko = k < 15 ? 0 : k < 40 ? 2 : 4;
    for (i = 0; i < hat_k && i < 12; i++)
        c |= 1u << hat_order[i];
    if (ko >= 2)
        o |= 1u << 2 | 1u << 10;
    if (ko >= 4)
        o |= 1u << 6 | 1u << 14;
    drum_pat[1] = c;
    drum_pat[2] = o;
    drum_pat[0] = 0x1111u;
    drum_pat[3] = k >= 70 ? 0x1010u : 0u;       /* the rim, with many hits: on two and four */
    drum_pat[4] = 0x1010u;                       /* the clap: two and four, now and then (clap_auto) */
    en_drum_pat[0] = drum_pat[0];
    en_drum_pat[1] = c;
    en_drum_pat[2] = o;
    en_drum_pat[3] = drum_pat[3];
    en_drum_pat[4] = drum_pat[4];
}

static void hats_home(void)                     /* Hits: the shaker's count, its steps home */
{
    int i;
    for (i = 0; i < 12; i++)
        hat_order[i] = HAT_ORDER[i];
    hat_kb = hat_k = (pe(P_HITS) * 12 + 50) / 100;
    hats_publish();
}

static void hats_evolve(void)                   /* a shaker step trades places, or one more or less */
{
    if (frand() < 0.6f) {
        int a = irand(11), t = hat_order[a];
        hat_order[a] = hat_order[a + 1];
        hat_order[a + 1] = (uint8_t)t;
    } else if (hat_kb) {
        int k = hat_k + (frand() < 0.5f ? 1 : -1);
        if (k >= hat_kb - 2 && k <= hat_kb + 2 && k >= 1 && k <= 12)
            hat_k = k;
    }
    hats_publish();
}

/* the kick's note: the key's root in F1..B1 (44..62 Hz); below F, its fifth (G1..B1), so the kick and the
 * sub agree. One note a piece: it doesn't follow the chords */
int en_kick_note(int root)
{
    int pc = (root % 12 + 12) % 12;
    return pc >= 5 ? 24 + pc : 24 + pc + 7;
}
#define kick_note en_kick_note

static void drum_fire(int d, float vel)
{
    drum_t *v = &drum[d];
    v->on = 1;
    v->amp = vel;
    v->att = 0.0f;
    switch (d) {
    case 0: {                                   /* kick: a sine falling fast from ~5x to its note */
        float f = mtof((float)(kick_note(par[P_ROOT]) + par[P_KTUNE])), pu = rel(P_KPUNCH);
        v->note = DRUM_GM[0];
        v->inc = f * KIT[kit].start * fm_exp2f(0.8f * pu) * (1.0f / EN_SR);
        v->inc_end = f * (1.0f / EN_SR);
        v->adec = fm_expf(-6.9f / (KIT[kit].t60 * fm_exp2f(1.2f * rel(P_KLEN)) * EN_SR));
        v->clk = KIT[kit].click * 2.0f * fm_maxf(1.0f + pu, 0.0f);   /* (Tight: 1, as before) */
        duck_x = duck_xb = 1.0f;
        break;
    }
    case 1:                                     /* the shaker: short; it chokes the open hat */
        v->note = DRUM_GM[1];
        v->adec = fm_expf(-6.9f / (0.07f * KIT[kit].hat_t * EN_SR));   /* (1.3.0: 45 ms to 70: a shaker) */
        drum[2].adec = fm_expf(-6.9f / (0.03f * EN_SR));
        break;
    case 2:                                     /* open hat: the "tss" off the beat */
        v->note = DRUM_GM[2];
        v->adec = fm_expf(-6.9f / (hat_open * KIT[kit].hat_t * EN_SR));
        break;
    case 3:                                     /* the rim: a short wooden tok, thrown into the echo */
        v->note = DRUM_GM[3];
        v->adec = fm_expf(-6.9f / (0.06f * EN_SR));
        v->clk = 1.0f;
        break;
    default:                                    /* the clap: three quick bursts and a tail, into the plate */
        v->note = DRUM_GM[4];
        v->ph = 0.0f;                            /* (its time, in samples) */
        v->adec = fm_expf(-6.9f / (0.22f * EN_SR));
        break;
    }
    en_drum_glow[d] = 1.0f;
    midi(0x99, v->note, (uint32_t)(vel * 100.0f));
    midi(0x89, v->note, 0);
}

static void drum_hit(int d, float vel, int off)  /* (Swing delays the whole off sixteenth now: clock_pulse) */
{
    int32_t w = 0;
    (void)off;
    if (w > 0) {
        drum[d].wait = w;
        drum[d].pend = vel;
    } else {
        drum_fire(d, vel);
    }
}

static int32_t drums_wait(void)                 /* samples to the next hit that waits; 0: none */
{
    int32_t m = 0;
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        if (drum[d].wait > 0 && (!m || drum[d].wait < m))
            m = drum[d].wait;
    return m;
}

static void drums_wait_step(int32_t k)          /* k samples went by: hits whose wait is over */
{
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        if (drum[d].wait > 0 && (drum[d].wait -= k) <= 0) {
            drum[d].wait = 0;
            drum_fire(d, drum[d].pend);
        }
}

/* ----------------------------------------------------------------- bass --- */
static void bass_hit(int first)
{
    int pc = chord_pcs[0], n, g = par[P_GROOVE];
    if ((!first || g == 0) && chord_nt >= 3 && frand() < (float)par[P_FIFTH] * 0.01f)
        pc = chord_pcs[2];                      /* the fifth (with Drone: a whole bar of it) */
    n = bass_note(pc);
    if (!first && frand() < evolve * 0.08f)
        n += 12;                                /* now and then, an octave up */
    if (bass.env < 0.05f)
        bass.note = (float)n;                   /* from silence: no slide */
    bass.target = (float)n;
    if (bass.gate > 0 || bass.midi_note)
        midi(0x82, bass.midi_note, 0);
    bass.gate = (int32_t)(fm_minf(GROOVE_GATE[gpat()] * fm_exp2f(rel(P_BLEN)), 15.8f) * step_len);
    bass_bite = 1.0f;
    {
        int bv = (unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0;
        if (BV_PATCH[bv])
            fm_note(&bfm, BV_PATCH[bv], mtof(bass.note), 1.0f, fm_q(P_BFRAT), fm_d(P_BFDEC));
    }
    bass.midi_note = (uint8_t)n;
    midi(0x92, (uint32_t)n, 90);
}

/* ------------------------------------------------------- the arrangement --- */
static void section_set(int s)
{
    float bl = 16.0f * step_len;
    sec = (uint8_t)s;
    en_section = (uint8_t)s;
    lay_kick = lay_hats = lay_open = lay_bass = lay_stabs = 1;
    switch (s) {
    case SEC_BREAK:                             /* the kick and the bass out; dark, then building */
        lay_kick = lay_bass = lay_hats = 0;
        lay_open = motion < 0.5f;
        arc_tgt = -1.0f;
        arc_tau = 0.7f * bl;
        break;
    case SEC_GROOVE:                            /* fewer stabs, the drums to the front */
        arc_tgt = -0.2f + 0.5f * frand();
        arc_tau = 3.0f * bl;
        break;
    case SEC_THIN:                              /* no bass, no shaker */
        lay_bass = lay_hats = 0;
        arc_tgt = -0.5f + 0.6f * frand();
        arc_tau = 3.0f * bl;
        break;
    default:
        arc_tgt = -0.1f + 1.1f * frand();
        arc_tau = 4.0f * bl;
        break;
    }
    en_pulse_rest = (uint8_t)!lay_kick;
}

static int section_pick(void)                   /* the next phrase: Motion leaves Full; back, mostly */
{
    float r = frand();
    if (motion < 0.02f)
        return SEC_FULL;
    if (sec != SEC_FULL)
        return r < 0.75f ? SEC_FULL : sec == SEC_BREAK ? SEC_THIN : SEC_BREAK;
    if (r >= 0.1f + 0.5f * motion)
        return SEC_FULL;
    r = frand();
    return r < 0.45f ? SEC_BREAK : r < 0.75f ? SEC_GROOVE : SEC_THIN;
}

static void bar_start(void)
{
    int pb = (int)(bar & 7u), db = en_drift_bars(par[P_DRIFT]);
    en_phrase_bar = (uint8_t)pb;
    if (pb == 0 && bar > 0)
        section_set(sec_next);
    if (sec == SEC_BREAK && pb == 2)            /* two bars in, the filter starts to open */
        arc_tgt = 1.0f, arc_tau = 2.2f * 16.0f * step_len;
    fill = 0;
    if (pb == 7) {
        sec_next = (uint8_t)section_pick();
        /* the last bar before the kick: a fill (the kick drops out halfway, the last stab is thrown) */
        if (sec_next != SEC_BREAK && (sec == SEC_BREAK || frand() < 0.5f * motion))
            fill = 1;
        /* Energy far down: the next phrase Thin (no bass, no shaker); far up: Full, with a fill into it (after the
         * draws, which stay as they were: at Energy 0 the music is the same) */
        if (mac[EN_MAC_ENERGY] <= -25)
            sec_next = SEC_THIN;
        else if (mac[EN_MAC_ENERGY] >= 25)
            sec_next = SEC_FULL, fill = 1;
    }
    if (!en_frozen && (change_pending || (db && bar > 0 && chord_bars >= (uint32_t)db))) {
        change_pending = 0;
        chord_change();
    }
    en_drift = db ? fm_clampf((float)chord_bars / (float)db, 0.0f, 1.0f) : 0.0f;
    chord_bars++;
    if ((bar & 3u) == 0u && bar > 0 && frand() < evolve * 0.6f)
        stabs_evolve();
    if (frand() < evolve * 0.25f)
        hats_evolve();
    stabs_bar();
    bar++;
}

/* a lane's steps as they play: the pattern page's (Play: Pattern), or the generated ones */
static uint32_t dpat(int d) { return par[P_DMODE] ? (uint32_t)(uint16_t)par[P_DPAT0 + d] : drum_pat[d]; }

/* the screen's lanes, the bar as it plays: what is muted or at level 0, gone at once */
static void lanes(void)
{
    const int kick = lay_kick && !(mutes & EN_M_KICK) && !(perf & EN_PF_KICKDROP) && par[P_DRUMS] > 0,
              hats = !(mutes & EN_M_HATS) && par[P_DRUMS] > 0;
    en_lane[0] = (uint16_t)(mutes & EN_M_CHORD || !par[P_CHORD] ? 0u : en_stab_pat);
    en_lane[1] = (uint16_t)(kick ? dpat(0) & (fill ? 0x00FFu : 0xFFFFu) : 0u);
    en_lane[2] = (uint16_t)(hats ? (lay_hats ? dpat(1) : 0u) | (lay_open || fill ? dpat(2) : 0u) : 0u);
    en_lane[3] = (uint16_t)(hats && lay_hats ? dpat(3) : 0u);
    en_lane[4] = (uint16_t)(hats && (sec == SEC_FULL || sec == SEC_GROOVE || par[P_DMODE]) ? dpat(4) : 0u);
    en_lane[5] = (uint16_t)(lay_bass && !(mutes & EN_M_BASS) && !(perf & EN_PF_KICKDROP) && par[P_BASS] > 0 ? en_groove(gpat()) : 0u);
}

/* a pattern's hit: its chance (Play: Pattern only, so Auto's music stays as it was), its accent */
static int dhit(int d, int st, float *vel)
{
    if (!par[P_DMODE])
        return 1;
    if (par[P_DCH0 + d] < 100 && frand() * 100.0f >= (float)par[P_DCH0 + d])
        return 0;
    if (!((uint32_t)(uint16_t)par[P_DACC0 + d] >> st & 1u))
        *vel *= 1.0f - 0.006f * (float)par[P_DACCENT];   /* (the unaccented, quieter as Accent turns up) */
    return 1;
}

/* the grid: one call a sixteenth (MIDI clock: 6 ticks) */
static void tick(void)
{
    int st = (int)(sixteenth & 15u), l, d;
    float r = frand();
    gate_ctr = 0;
    if (st == 0)
        bar_start();
    const int kick = lay_kick && !(mutes & EN_M_KICK) && !(perf & EN_PF_KICKDROP), hats = !(mutes & EN_M_HATS);
    lanes();
    {
        float v0 = st ? 0.88f : 1.0f, v1 = 0.35f + 0.3f * r + (st & 1 ? 0.0f : 0.15f), v2 = 0.7f + 0.2f * r, v3 = 0.75f + 0.15f * r;
        if (kick && dpat(0) >> st & 1u && !(fill && st >= 8) && dhit(0, st, &v0))
            drum_hit(0, v0, 0);
        if (hats && lay_hats && dpat(1) >> st & 1u && dhit(1, st, &v1))
            drum_hit(1, v1, st & 1);
        if (hats && (lay_open || fill) && dpat(2) >> st & 1u && dhit(2, st, &v2))
            drum_hit(2, v2, 0);
        if (hats && lay_hats && dpat(3) >> st & 1u && dhit(3, st, &v3))
            drum_hit(3, v3, st & 1);
        if (en_lane[4] >> st & 1u) {             /* the clap: rare (Auto: 3% + 5% x Evolve, ~5% at 40; Pattern: its Chance) */
            float v4 = 0.8f + 0.2f * r;
            if (par[P_DMODE] ? dhit(4, st, &v4) : frand() < 0.03f + 0.05f * evolve)
                drum_hit(4, v4, 0);
        }
    }
    if (lay_bass && !(mutes & EN_M_BASS) && !(perf & EN_PF_KICKDROP) && en_groove(gpat()) >> st & 1u) {
        /* Notes, and Energy: down thins it (to 0.4 of it), up fills a thin one; the first note of a bar always plays
         * (the random draw only while thinned: a full groove draws nothing, as before) */
        const float e = (float)mac[EN_MAC_ENERGY] * (1.0f / 50.0f), d0 = (float)par[P_BNOTES] * 0.01f;
        const float d = fm_clampf(e < 0.0f ? d0 * (1.0f + 0.6f * e) : d0 + (1.0f - d0) * e, 0.0f, 1.0f);
        if (d >= 0.999f || st == 0 || frand() < d)
            bass_hit(st == 0);
    }
    if (en_stab_pat >> st & 1u) {               /* (the random draws made muted or not: the music goes on the same) */
        uint32_t later = en_stab_pat >> (st + 1);
        int last = !later, thr = (fill && last) || frand() < fm_minf(0.075f * evolve * (sec == SEC_BREAK ? 2.5f : 1.0f) * throw_mul, 0.6f);
        float vel = st == (int)__builtin_ctz(en_stab_pat) ? 0.9f + 0.1f * frand() : 0.65f + 0.25f * frand();
        if (!(mutes & EN_M_CHORD))
            stab_fire(vel, thr || (perf & EN_PF_THROW) || (fxm >> EN_FX_THROW & 1u));
    }
    for (d = 0; d < EN_NDRUM; d++)
        en_drum_pos[d] = (uint8_t)st;
    if ((sixteenth & 1u) == 0u) {               /* the loops step in eighths */
        for (l = 0; l < EN_NLOOP; l++) {
            int s = loop_pos[l];
            if (loop_note[l][s] >= 0 && !loop_rest[l] && !(mutes & EN_M_PINGS))
                bell_ring(l, s);
            en_loop_pos[l] = (uint8_t)s;
            if (++loop_pos[l] >= EN_LOOP_LEN[l]) {
                loop_pos[l] = 0;
                loop_pass(l);
            }
        }
    }
    sixteenth++;
    en_bars = sixteenth;
}

/* ------------------------------------------------- reverb: Dattorro's plate --- */
/* J. Dattorro, "Effect Design, Part 1", JAES 1997 (as in FoMni): a predelay, four input diffusers,
 * then a figure-eight tank of two halves, each a modulated allpass, a delay, a damping lowpass, an
 * allpass and a delay, feeding the other half. Stereo out from seven taps a side. The paper's lengths
 * are for 29761 Hz; DS() scales them to 44.1 kHz. */
#define DS(n) (((n) * 14818 + 5000) / 10000)          /* 44100 / 29761 */
#define PRE_N 1323                                   /* 30 ms */
#define EXC DS(16)
enum { L_PRE, L_IN1, L_IN2, L_IN3, L_IN4, L_APL, L_D1L, L_AP2L, L_D2L, L_APR, L_D1R, L_AP2R, L_D2R, L_N };
static const int DL_LEN[L_N] = {PRE_N, DS(142), DS(107), DS(379), DS(277), DS(672) + EXC + 2, DS(4453), DS(1800),
                                DS(3720), DS(908) + EXC + 2, DS(4217), DS(2656), DS(3163)};
#define DL_TOTAL (PRE_N + DS(142) + DS(107) + DS(379) + DS(277) + DS(672) + DS(4453) + DS(1800) + DS(3720) + DS(908) + \
                  DS(4217) + DS(2656) + DS(3163) + 2 * (EXC + 2))
static float pl_buf[DL_TOTAL] EN_POOL;
typedef struct {
    float *b;
    int n, i;
} dl_t;
static dl_t dl[L_N];
static float pl_bw, pl_damp_l, pl_damp_r, pl_decay = 0.5f, pl_decay_t = 0.5f, pl_dd2 = 0.5f, lfo_s, lfo_c = 1.0f;

static inline float tap(const dl_t *d, int k)
{
    int j = d->i - k;
    return d->b[j < 0 ? j + d->n : j];
}
static inline void push(dl_t *d, float x)
{
    d->b[d->i] = x;
    if (++d->i >= d->n)
        d->i = 0;
}
static inline float allpass(dl_t *d, int len, float x, float g)
{
    float z = tap(d, len), v = x - g * z;
    push(d, fm_flush(v));
    return z + g * v;
}
static inline float allpass_mod(dl_t *d, float len, float x, float g)
{
    int k = (int)len;
    float f = len - (float)k, z = tap(d, k) + (tap(d, k + 1) - tap(d, k)) * f, v = x - g * z;
    push(d, fm_flush(v));
    return z + g * v;
}

static inline void reverb(float in, float *ol, float *or_)
{
    float x, a, b, fb_l = tap(&dl[L_D2R], DS(3163)), fb_r = tap(&dl[L_D2L], DS(3720));
    push(&dl[L_PRE], in);
    x = tap(&dl[L_PRE], PRE_N);
    pl_bw += 0.6f * (x - pl_bw);
    x = allpass(&dl[L_IN1], DS(142), pl_bw, 0.75f);
    x = allpass(&dl[L_IN2], DS(107), x, 0.75f);
    x = allpass(&dl[L_IN3], DS(379), x, 0.625f);
    x = allpass(&dl[L_IN4], DS(277), x, 0.625f);
    {
        float s = lfo_s + 0.0001f * lfo_c, c = lfo_c - 0.0001f * s;
        lfo_s = s;
        lfo_c = c;
    }
    a = allpass_mod(&dl[L_APL], (float)DS(672) + (float)EXC * lfo_s, x + pl_decay * fb_l, -0.7f);
    push(&dl[L_D1L], a);
    b = tap(&dl[L_D1L], DS(4453));
    pl_damp_l += 0.4f * (b - pl_damp_l);
    b = allpass(&dl[L_AP2L], DS(1800), pl_damp_l * pl_decay, pl_dd2);
    push(&dl[L_D2L], b);
    a = allpass_mod(&dl[L_APR], (float)DS(908) + (float)EXC * lfo_c, x + pl_decay * fb_r, -0.7f);
    push(&dl[L_D1R], a);
    b = tap(&dl[L_D1R], DS(4217));
    pl_damp_r += 0.4f * (b - pl_damp_r);
    b = allpass(&dl[L_AP2R], DS(2656), pl_damp_r * pl_decay, pl_dd2);
    push(&dl[L_D2R], b);
    *ol = 0.6f * (tap(&dl[L_D1R], DS(266)) + tap(&dl[L_D1R], DS(2974)) - tap(&dl[L_AP2R], DS(1913)) +
                  tap(&dl[L_D2R], DS(1996)) - tap(&dl[L_D1L], DS(1990)) - tap(&dl[L_AP2L], DS(187)) -
                  tap(&dl[L_D2L], DS(1066)));
    *or_ = 0.6f * (tap(&dl[L_D1L], DS(353)) + tap(&dl[L_D1L], DS(3627)) - tap(&dl[L_AP2L], DS(1228)) +
                   tap(&dl[L_D2L], DS(2673)) - tap(&dl[L_D1R], DS(2111)) - tap(&dl[L_AP2R], DS(335)) -
                   tap(&dl[L_D2R], DS(121)));
}

static void plate_init(void)
{
    float *p = pl_buf;
    int k, i;
    for (k = 0; k < L_N; k++) {
        dl[k].b = p;
        dl[k].n = DL_LEN[k];
        dl[k].i = 0;
        p += DL_LEN[k];
    }
    for (i = 0; i < DL_TOTAL; i++)
        pl_buf[i] = 0.0f;
    pl_bw = pl_damp_l = pl_damp_r = 0.0f;
    lfo_s = 0.0f;
    lfo_c = 1.0f;
}

/* ------------------------------------------------- echo: tape and Echoplex --- */
/* The dub echo: a tape loop (up to ~5.6 s, synced to the tempo) at a quarter rate (11 kHz, 16-bit,
 * mono), with two heads: the first plays the loop's length, the second a part of it (3/4 at first),
 * each panned to its side. In the feedback, a band-pass (each repeat thinner and darker) that the sweep
 * and a slow drift move, and a soft clip: the feedback may pass 100% and the echo swell on itself without
 * running away. With Motion the heads wander, the feedback leans now on one, now on the other, and now
 * and then the second slides to another part of the loop (the repeats bend in pitch on the way). A
 * thrown stab pushes the feedback up a little. Frozen (dub hold), the loop stops listening and plays
 * what it holds, sample for sample, for ever. A short Echoplex (~160 ms) smears what goes in. */
#define TAPE_N 62000                             /* 5.6 s at 11.025 kHz */
#define TAPE_SR 11025.0f
static int16_t tape[TAPE_N] EN_POOL;
static int tape_w, tape_ph;
static float tape_acc, tape_lp, tape_hp, tape_p1, tape_p2, tape_c1, tape_c2;
static const float HEAD2[5] = {0.75f, 0.6667f, 0.5f, 0.375f, 0.25f};    /* the second head's parts */
static float head2 = 0.75f, head2_t = 0.75f, head2_v;   /* its place (of the loop), target, speed */
static float fb_lean;                            /* 0..1: how much of the feedback is the second head's */
static float hg[4] = {1.0f, 0.0f, 0.0f, 1.0f};   /* the heads' pan gains: 1 left, 1 right, 2 left, 2 right */
static uint32_t head_left;                       /* samples until the second head may move */
#define PLEX_N 7100                              /* 160 ms at 44.1 kHz */
#define PLEX_TAP 2700                            /* the right's tap: ~100 ms */
static int16_t plex[PLEX_N];
static int stt_state, stt_len, stt_pos;         /* Stutter: 0 off, 1 recording a sixteenth, 2 looping it */
static float stt_mix;
static int plex_w;
static float plex_lp, plex_lp2;

static inline float tape_read(float d)           /* d quarter-rate samples ago */
{
    int k = (int)d, i0, i1;
    float f = d - (float)k, a, c;
    i0 = tape_w - k;
    i1 = i0 - 1;
    if (i0 < 0)
        i0 += TAPE_N;
    if (i1 < 0)
        i1 += TAPE_N;
    a = (float)tape[i0];
    c = (float)tape[i1];
    return (a + (c - a) * f) * (1.0f / 12000.0f);
}
static inline int16_t s16(float x)
{
    x *= 12000.0f;
    return (int16_t)(x > 32767.0f ? 32767.0f : x < -32767.0f ? -32767.0f : x);
}

static void echo_time(void)
{
    int i = par[P_ETIME];
    float beat, t;
    if (par[P_TEMPO] <= 0)
        return;                                  /* not yet (en_init applies in order) */
    beat = 60.0f / (float)par[P_TEMPO];
    t = ETIME_BEATS[i] * beat;
    if (t * TAPE_SR > (float)(TAPE_N - 8))
        t = (float)(TAPE_N - 8) / TAPE_SR;
    echo_len_t = t * TAPE_SR;
    if (echo_len <= 0.0f)
        echo_len = echo_len_t;
}

/* once a chunk: where the heads sit, how the feedback leans, the second head's slide, the band-pass */
static void heads_step(uint32_t n)
{
    float p, want, vmax, sw;
    int i;
    for (i = 0; i < 2; i++) {
        p = fm_clampf((i ? 0.8f : -0.8f) + 0.5f * motion * mod[6 + i].v, -1.0f, 1.0f);
        p = (p + 1.0f) * (FM_PI / 4.0f);
        hg[2 * i] = fm_cosf(p);
        hg[2 * i + 1] = fm_sinf(p);
    }
    fb_lean = 0.3f * motion * (1.0f + mod[8].v);
    if (head_left <= n) {
        head_left = (uint32_t)((16.0f + 32.0f * frand()) * EN_SR);
        if (en_playing && !en_frozen && frand() < motion * 0.6f)
            head2_t = HEAD2[irand(5)];
    } else {
        head_left -= n;
    }
    vmax = 0.008f / fm_maxf(echo_len, 100.0f);
    want = fm_clampf((head2_t - head2) * (1.0f / (2.0f * TAPE_SR)), -vmax, vmax);
    head2_v += (want - head2_v) * fm_minf((float)n * (1.0f / EN_SR), 1.0f);
    /* the band-pass: ~165..700 Hz below (1.3.0: from ~60..500, so the repeats thin), ~1.1..2.6 kHz above, moving
     * up together */
    dub_up = fm_maxf((float)mac[EN_MAC_DUB], 0.0f) * (1.0f / 50.0f);
    sw = fm_clampf(0.45f + 0.35f * arcm + 0.3f * perf_sw + 0.35f * motion * mod[5].v - 0.3f * dub_up, 0.0f, 1.0f);   /* (Dub up: darker) */
    tape_hpc = 0.09f + 0.24f * sw * sw;
    tape_lpc = 0.58f + 0.3f * sw;
}

/* one sample of both: the input (a mono send) to the tape's heads (el, er) and the Echoplex (xl, xr) */
static inline void echoes(float in, float *el, float *er, float *xl, float *xr)
{
    float y = (float)plex[plex_w] * (1.0f / 12000.0f);
    int t = plex_w - PLEX_TAP;
    plex_lp += 0.3f * (y - plex_lp);
    plex_lp2 += 0.3f * ((float)plex[t < 0 ? t + PLEX_N : t] * (1.0f / 12000.0f) - plex_lp2);
    if (stt_state) {                             /* (the grid's Stutter has the Echoplex's buffer) */
        *xl = *xr = 0.0f;
    } else {
        plex[plex_w] = s16(in + 0.45f * plex_lp);
        if (++plex_w >= PLEX_N)
            plex_w = 0;
        *xl = plex_lp;
        *xr = plex_lp2;
    }
    tape_acc += in + 0.2f * plex_lp;
    if (++tape_ph >= 4) {                        /* the tape, a quarter of the time */
        float d, h1, h2, fb, w;
        tape_ph = 0;
        echo_len += (echo_len_t * fx_em - echo_len) * fx_ek;   /* (the grid: the time doubled, the tape slowing) */
        if (echo_len > (float)(TAPE_N - 8))      /* (Tape stop slows a long echo past the tape: it stops at its end;
                                                  * the hold below reads echo_len itself. Reported by genkigenki) */
            echo_len = (float)(TAPE_N - 8);
        head2 += head2_v;
        d = echo_len * (1.0f + 0.002f * motion * mod[5].v);   /* wow */
        if (d > (float)(TAPE_N - 4))
            d = (float)(TAPE_N - 4);
        h1 = tape_read(d);
        h2 = tape_read(d * head2);
        if (en_frozen) {                         /* dub hold: the loop, as it is */
            int k = (int)echo_len, i0 = tape_w - k;
            int16_t s = tape[i0 < 0 ? i0 + TAPE_N : i0];
            if (++tape_w >= TAPE_N)
                tape_w = 0;
            tape[tape_w] = s;
        } else {
            float g = fm_minf(echo_fb + 0.15f * st_throw + 0.12f * perf_echo + 0.1f * dub_up, fb_cap);   /* (Dub: past the knob) */
            tape_lp += tape_lpc * (h1 + fb_lean * (h2 - h1) - tape_lp);   /* each time round: thinner, darker */
            tape_hp += tape_hpc * (tape_lp - tape_hp);
            fb = tape_lp - tape_hp;
            w = tape_acc * 0.25f + 0.7f * sat(fb * g * (1.0f / 0.7f));
            if (++tape_w >= TAPE_N)
                tape_w = 0;
            tape[tape_w] = s16(w);
        }
        tape_acc = 0.0f;
        tape_p1 = tape_c1;
        tape_p2 = tape_c2;
        tape_c1 = h1;
        tape_c2 = h2;
    }
    {   /* between the tape's samples: a line from the last to the next; then to the field */
        float f = (float)tape_ph * 0.25f;
        float h1 = tape_p1 + (tape_c1 - tape_p1) * f, h2 = tape_p2 + (tape_c2 - tape_p2) * f;
        *el = hg[0] * h1 + hg[2] * h2;
        *er = hg[1] * h1 + hg[3] * h2;
    }
}

/* ------------------------------------------------- cloud: a granular reverb --- */
/* After JIANT's CLOUD (github.com/juanjiant-bit/JIANT, GPL-3.0): what goes to the reverb, written at a
 * quarter rate (11 kHz, 16-bit) into a 0.74 s line, read by up to six grains born at random (denser as Cloud
 * turns up and as the filter opens), each 80..250 ms through a triangle window, from anywhere behind the
 * write, panned at random, a few cents off; now and then an octave down (rarely up). The grains go into the
 * plate, so they bloom into its tail, and a little of them back into the line: the wash builds on itself.
 * Dub hold stops the line too: the grains keep scattering over what it holds. */
#define CL_LEN 8192
#define CL_NG 6
static int16_t cl_buf[CL_LEN];                  /* (RAM, not the pool: the pool is nearly full) */
static int cl_w, cl_ph, cl_live;
static int32_t cl_next;
static float cl_acc, cl_fb, cl_pl, cl_pr, cl_cl, cl_cr, cl_dens, pl_back;
static struct {
    float pos, inc, gl, gr;
    int32_t age, len;
} cl_g[CL_NG];

static uint32_t cl_rng = 0x2545F491u;          /* its own: the Cloud knob leaves the music's choices be */
static float cl_frand(void)
{
    cl_rng = cl_rng * 1664525u + 1013904223u;
    return (float)(cl_rng >> 8) * (1.0f / 16777216.0f);
}
static int cl_irand(int n) { return (int)(cl_frand() * (float)n); }

static void cloud_birth(void)                   /* a grain, in the first voice free */
{
    int k, back;
    float r = cl_frand(), pk, p;
    for (k = 0; k < CL_NG && cl_g[k].len; k++)
        ;
    if (k >= CL_NG)
        return;
    pk = r < 0.2f ? 0.5f : r < 0.24f ? 2.0f : 1.0f;
    cl_g[k].len = 880 + cl_irand(1900);
    cl_g[k].inc = pk * (0.997f + 0.006f * cl_frand());
    back = 300 + (pk > 1.0f ? cl_g[k].len : 0) + cl_irand(CL_LEN - 3200 - (pk > 1.0f ? cl_g[k].len : 0));
    cl_g[k].pos = (float)((cl_w - back + CL_LEN) % CL_LEN);
    cl_g[k].age = 0;
    p = (cl_frand() * 1.6f - 0.8f + 1.0f) * (FM_PI / 4.0f);
    cl_g[k].gl = fm_cosf(p);
    cl_g[k].gr = fm_sinf(p);
    cl_live++;
}

/* one sample: the input (mono) in, the grains out (l, r) */
static inline void cloud(float in, float *ol, float *or_)
{
    cl_acc += in;
    if (++cl_ph >= 4) {                         /* the line and the grains, a quarter of the time */
        float l = 0.0f, r = 0.0f;
        int k;
        cl_ph = 0;
        if (!en_frozen && !(fxm >> EN_FX_FREEZE & 1u)) {   /* (the grid's Freeze: the cloud alone holds) */
            if (++cl_w >= CL_LEN)
                cl_w = 0;
            cl_buf[cl_w] = s16(cl_acc * 0.25f + (0.25f + 0.3f * cloud_amt) * cl_fb);
        }
        cl_acc = 0.0f;
        if (--cl_next <= 0) {
            int gap = 1500 - (int)(1100.0f * cl_dens);   /* 136 .. 36 ms apart, on average */
            cl_next = gap / 2 + cl_irand(gap);
            if (cl_dens > 0.0f)
                cloud_birth();
        }
        if (cl_live) {
            for (k = 0; k < CL_NG; k++) {
                int32_t a = cl_g[k].age, L = cl_g[k].len;
                int i0;
                float f, x, w;
                if (!L)
                    continue;
                i0 = (int)cl_g[k].pos;
                f = cl_g[k].pos - (float)i0;
                x = (float)cl_buf[i0] + ((float)cl_buf[i0 + 1 < CL_LEN ? i0 + 1 : 0] - (float)cl_buf[i0]) * f;
                w = (float)(a < L - a ? a : L - a) * (2.0f / (float)L);
                x *= w * (1.0f / 12000.0f);
                l += x * cl_g[k].gl;
                r += x * cl_g[k].gr;
                cl_g[k].pos += cl_g[k].inc;
                if (cl_g[k].pos >= (float)CL_LEN)
                    cl_g[k].pos -= (float)CL_LEN;
                if (++cl_g[k].age >= L) {
                    cl_g[k].len = 0;
                    cl_live--;
                }
            }
        }
        l *= 0.55f;
        r *= 0.55f;
        cl_fb = 0.5f * (l + r);
        cl_pl = cl_cl;
        cl_pr = cl_cr;
        cl_cl = l;
        cl_cr = r;
    }
    {   /* between the quarter-rate samples: a line from the last to the next */
        float f = (float)cl_ph * 0.25f;
        *ol = cl_pl + (cl_cl - cl_pl) * f;
        *or_ = cl_pr + (cl_cr - cl_pr) * f;
    }
}

int en_voice_fm(int layer)
{
    if (layer == 0)
        return cv_patch(cvoice) != 0;
    if (layer == 2)
        return BV_PATCH[(unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0] != 0;
    return layer == 1;
}
int en_fm_alg(int layer)
{
    if (layer == 0)
        return cv_patch(cvoice) ? fm_alg_of(P_CALG, cv_patch(cvoice)) : 0;
    if (layer == 2) {
        const fmpatch_t *p = BV_PATCH[(unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0];
        return p ? fm_alg_of(P_BALG, p) : 0;
    }
    return par[P_PALG] ? par[P_PALG] - 1 : par[P_PVOICE] == 2 ? 4 : FM_ALG2;
}

/* a layer's envelope, for a sound page (computed from the values, as the render would play them) */
void en_env_shape(int layer, float *lvl, float *aux, int n, float *secs)
{
    int i;
    float T = 1.0f, a = 0.035f, t60 = 0.3f, ft = 0.1f, fe = 0.0f, rel_s = 0.02f, hold = 0.0f;
    switch (layer) {
    case 0: {                                   /* the chord: the strike (or swell), the decay; the filter's envelope */
        float xa = rel(P_CATT);
        a = fm_clampf(CV[cvoice].att * fm_exp2f(-(xa > 0.0f ? 9.0f : 3.0f) * xa), 0.00004f, 0.3f);
        t60 = st_t60;
        ft = 0.25f * st_t60 + 0.02f;
        fe = fm_clampf(CV[cvoice].fenv * fm_maxf(1.0f + rel(P_CENV), 0.0f) / 3.4f, 0.0f, 1.0f);
        T = fm_clampf(t60 * 1.2f, 0.25f, 4.0f);
        break;
    }
    case 1: {
        int pv = (unsigned)par[P_PVOICE] < EN_NPV ? par[P_PVOICE] : 0;
        t60 = 0.8f * PV[pv].t60 * fm_exp2f(1.5f * rel(P_PLEN));
        a = 0.004f + 0.02f * fm_minf(btone * 2.0f, 1.0f);
        fe = fm_clampf(PV[pv].drop + 1.5f * rel(P_PBEND), 0.0f, 3.0f) / 3.0f;   /* the bend */
        ft = 0.035f;
        T = fm_clampf(t60 * 1.2f, 0.25f, 4.0f);
        break;
    }
    case 2: {                                   /* the bass: in, held for its gate, out */
        hold = fm_minf(GROOVE_GATE[gpat()] * fm_exp2f(rel(P_BLEN)), 15.8f) * step_len / EN_SR;
        a = 0.02f;
        T = fm_clampf(hold * 1.5f + 0.1f, 0.25f, 4.0f);
        break;
    }
    default: {                                  /* the kick: its decay; its pitch falling */
        t60 = KIT[kit].t60 * fm_exp2f(1.2f * rel(P_KLEN));
        a = 0.05f;
        ft = 1.0f / (KIT[kit].fall * EN_SR);
        fe = fm_clampf(KIT[kit].start * fm_exp2f(0.8f * rel(P_KPUNCH)) / 10.0f, 0.0f, 1.0f);
        T = fm_clampf(t60 * 1.2f, 0.25f, 2.0f);
        break;
    }
    }
    for (i = 0; i < n; i++) {
        float t = T * (float)i / (float)(n - 1), up = 1.0f - fm_expf(-a * EN_SR * t), v;
        if (layer == 2)
            v = t < hold ? up : up * fm_expf(-(t - hold) / rel_s);
        else
            v = up * fm_expf(-6.9f * t / t60);
        lvl[i] = fm_clampf(v, 0.0f, 1.0f);
        aux[i] = fe > 0.0f ? fe * fm_expf(-t / ft) : 0.0f;
    }
    *secs = T;
}

/* ------------------------------------------------------------ parameters --- */
static int pulse;
static uint32_t ext_age;

static void tempo_set(void) { step_len = EN_SR * 60.0f / ((float)par[P_TEMPO] * 4.0f); }

static void apply(int p, int v)
{
    const en_param_t *pi = &PARAMS[p];
    if (v < pi->lo)
        v = pi->lo;
    if (v > pi->hi)
        v = pi->hi;
    par[p] = (int16_t)v;
    v = pe(p);                                   /* (as the master actions move it) */
    switch (p) {
    case P_CHORD: chord_vol = (float)(v * v) * 0.0001f; lanes(); break;
    case P_PINGS: bell_vol = (float)(v * v) * 0.0001f; break;
    case P_BASS: bass_vol = (float)(v * v) * 0.0001f; lanes(); break;
    case P_DRUMS: pulse_vol = (float)(v * v) * 0.0001f; lanes(); break;
    case P_HITS: hats_home(); break;
    case P_DMODE: case P_DPAT0: case P_DPAT1: case P_DPAT2: case P_DPAT3: case P_DPAT4: lanes(); break;
    case P_STABS: stabs_home(); break;
    case P_SWING: swing = (float)v * 0.004f; break;      /* the off sixteenths up to 0.4 of a step late */
    case P_HATS: case P_HTONE: {                 /* the piece's colour, moved by the sound page's Hats */
        float h = fm_clampf((float)(pe(P_HATS) + par[P_HTONE]), 0.0f, 100.0f);
        hat_f = 0.25f + 0.0045f * h;                      /* the high-pass: ~1.8 kHz dark .. ~5 kHz bright */
        hat_open = 0.4f - 0.0025f * h;                    /* the open hat: long and soft .. short and crisp */
        break;
    }
    case P_ECHO: echo_send = (float)(v * v) * 0.0001f; break;
    case P_ETIME: echo_time(); break;
    case P_REVERB: rev_send = (float)(v * v) * 0.0001f; break;
    case P_FEEDBACK: echo_fb = 0.45f + 0.0067f * (float)v; break;   /* to 1.12: past 100%, held by the clip */
    case P_SIZE: pl_decay_t = 0.5f + 0.0045f * (float)v; break;      /* to 0.95 */
    case P_CUTOFF: cut_base = 110.0f * fm_exp2f((float)v * 0.055f); break;   /* 110 Hz .. 5.4 kHz */
    case P_DECAY: case P_CVOICE:                 /* 60 ms .. 1.5 s (times the voice's); the filter's envelope, quicker */
        cvoice = (unsigned)par[P_CVOICE] < EN_NCV ? par[P_CVOICE] : 0;
        st_t60 = 0.06f * fm_exp2f((float)pe(P_DECAY) * 0.0464f) * CV[cvoice].dec;
        st_dec = fm_expf(-6.9f / (st_t60 * EN_SR));
        st_fdec = fm_expf(-6.9f * 32.0f / ((0.25f * st_t60 + 0.02f) * EN_SR));   /* (1.3.0: a shorter strike) */
        break;
    case P_PTONE: case P_PTON: btone = fm_clampf((float)(par[P_PTONE] + par[P_PTON]), 0.0f, 100.0f) * 0.01f; break;
    case P_MOTION: motion = (float)v * 0.01f; break;
    case P_EVOLVE: evolve = (float)v * 0.01f; break;
    case P_CRACKLE:                              /* (1.3.0: straight, not squared: a bed at 15 too; 35 as before,
                                                  * 100 -9 dB; the palette widening as it turns up) */
        crackle_vol = (float)v * 0.0035f;
        cr_wd = fm_clampf(((float)v - 20.0f) * (1.0f / 40.0f), 0.0f, 1.0f);
        cr_ww = fm_clampf(((float)v - 35.0f) * (1.0f / 40.0f), 0.0f, 1.0f);
        cr_wr = fm_clampf(((float)v - 25.0f) * (1.0f / 50.0f), 0.0f, 1.0f);
        break;
    case P_DUCK: duck_depth = (float)v * 0.0075f; break;   /* to -12 dB */
    case P_CLOUD: cloud_amt = (float)v * 0.01f; break;
    case P_CR0: case P_CR1: case P_CR2: case P_CR3: case P_CR4: case P_CR5: case P_CR6: case P_CR7:
    case P_CQ0: case P_CQ1: case P_CQ2: case P_CQ3: case P_CQ4: case P_CQ5: case P_CQ6: case P_CQ7:
    case P_CX0: case P_CX1: case P_CX2: case P_CX3: case P_CX4: case P_CX5: case P_CX6: case P_CX7:
        if (nsteps && prog_pos >= 0 && (p - P_CR0) % 8 == prog_pos)
            slot_play(prog_pos);                 /* the slot playing, edited: heard at once */
        prog_publish();
        break;
    case P_PMODE:                                /* (the walk's character, turned by hand: C_REWALK) */
        if (v & EN_PLOOP) {                      /* to Loop: the eight as they are, on from the one playing */
            if (prog_pos < 0 && prog_len())
                prog_pos = 0;
        } else {                                 /* Endless (from Loop, the UI turns them round: C_PROT) */
            walk_fill();
            if (prog_pos < 0)
                prog_pos = 0, slot_play(0);
        }
        en_prog_pos = (int8_t)prog_pos;
        prog_publish();
        break;
    case P_KIT: kit = (unsigned)v < EN_NKIT ? v : 0; break;
    case P_NCOL: nctl.colour = (float)v * 0.02f; break;   /* the noise's controls, -1..+1 (render_crackle) */
    case P_NGRAIN: nctl.grain = (float)v * 0.02f; break;
    case P_NROOM: nctl.room = (float)v * 0.02f; break;
    case P_NDRIFT: nctl.drift = (float)v * 0.02f; break;
    case P_NCHAR: nctl.character = (uint8_t)((unsigned)v < EN_NNCHAR ? v : 0); break;
    case P_CTHROW:                               /* the stabs' throws: x0 .. x1 (50) .. x3 */
        throw_mul = v <= 50 ? (float)v * 0.02f : 1.0f + (float)(v - 50) * 0.04f;
        break;
    case P_PGHOST:                               /* the pings' dry: 0 dB (0), -6 dB (50), -20 dB (100) */
        ping_dry = 0.5f * fm_exp2f((v <= 50 ? 6.0f * (float)(50 - v) : -14.0f * (float)(v - 50)) * 0.02f * 0.16609640f);
        break;
    case P_HATSLVL: hats_lvl = fm_exp2f((float)(v - 50) * 0.04f); break;   /* x1 at 50, -12 .. +12 dB */
    case P_POCT:                                 /* let it be heard now: a ping from the first loop */
        if (en_playing && nsteps) {
            int l, k;
            for (l = 0; l < EN_NLOOP; l++)
                for (k = 0; k < EN_LOOP_LEN[l]; k++)
                    if (loop_note[l][k] >= 0) {
                        bell_ring(l, k);
                        return;
                    }
        }
        break;
    case P_TUNE: tunefac = fm_exp2f((float)v * (1.0f / 1200.0f)); break;
    case P_TEMPO:
        tempo_set();
        echo_time();
        break;
    case P_ROOT:
        if (nsteps)
            voice_chord();
        break;
    case P_MIDI:                                 /* off: every channel's notes off (the chord, pings, bass, drums) */
        if (!v) {
            int c;
            for (c = 0; c < 4; c++)
                en_midi_out(0xB0 | (uint32_t)c, 123, 0);
            en_midi_out(0xB9, 123, 0);
        }
        break;
    default: break;
    }
}

static int deg_dim(int d)                       /* the scale's chord on degree d has a diminished fifth */
{
    int t[3], n = stack(pool, 0, d, t, 3);
    return n >= 3 && (t[2] - t[0] + 12) % 12 == 6;
}
static void pool_set(uint32_t p)
{
    int k, prev = 0, moved = 0;
    pool = (p | 1u) & 0xFFFu;
    scale_set();
    /* the walk's slots follow the scale, as degrees; one the new scale has no room for, or that lands on a
     * diminished chord (which the walk never plays), or repeats the slot before, is walked again from that one */
    for (k = 0; k < EN_NPROG && nsteps; k++)
        if (slot_walk(k)) {
            int raw = par[P_CR0 + k] - 1, d = raw % nsteps;
            if (raw >= nsteps || deg_dim(d) || (k > 0 && slot_walk(k - 1) && d == prev)) {
                d = walk_from(prev);
                slot_walk_set(k, d);
                moved = 1;
            }
            prev = walk_last = d;
            if (k == prog_pos)
                deg = d;
        }
    if (moved)
        prog_publish();
    if (deg >= nsteps)
        deg = 0;
    voice_chord();
}

static void regrow(void)                        /* a new piece: fresh loops, home chord, patterns home */
{
    int l, k;
    for (l = 0; l < EN_NLOOP; l++) {
        loop_regrow(l);
        loop_rest[l] = 0;
    }
    chord_slot = -1;
    deg = 0;
    shape = cvoice == CV_KEYS || cvoice == CV_PAD || cvoice == CV_BELL;   /* these like the 9th; the rest the open 7th */
    walk_shape = shape;
    if (!PLAY_LOOP)                              /* Endless: a fresh piece's eight, from the walk */
        walk_fill();
    /* the slots: Endless from slot 1, Loop from the first that isn't a gap (none: home) */
    for (prog_pos = 0; prog_pos < EN_NPROG && slot_gap(prog_pos); prog_pos++)
        ;
    if (prog_pos >= EN_NPROG || !PLAY_LOOP)
        prog_pos = prog_pos >= EN_NPROG ? -1 : 0;
    en_prog_pos = (int8_t)prog_pos;
    for (walk_last = 0, k = 0; k < EN_NPROG; k++)   /* (the walk goes on from its last) */
        if (slot_walk(k))
            walk_last = (par[P_CR0 + k] - 1) % (nsteps ? nsteps : 1);
    if (prog_pos >= 0)
        slot_play(prog_pos);
    if (chord_slot < 0 && (prog_pos < 0 || !slot_walk(prog_pos)))
        voice_chord();
    prog_publish();
    stabs_home();
    hats_home();
    section_set(SEC_FULL);
    sec_next = SEC_FULL;
    chord_bars = 0;
    change_pending = 0;
}

static void notes_off(void)
{
    int i;
    for (i = 0; i < 4; i++)
        en_midi_out(0xB0 | (uint32_t)i, 123, 0);
    en_midi_out(0xB9, 123, 0);
}

static void quiet(void)
{
    int i;
    for (i = 0; i < NBELL; i++)
        bell[i].on = 0;
    for (i = 0; i < EN_NDRUM; i++)
        drum[i].on = 0, drum[i].wait = 0;
    st_env = st_amp = 0.0f;
    bass.env = 0.0f;
    bass.gate = 0;
    notes_off();
}

void en_init(void)
{
    int i;
    for (i = 0; i <= SN; i++)
        sint[i] = fm_sinf((float)i * (FM_TWO_PI / (float)SN));
    plate_init();
    for (i = 0; i < TAPE_N; i++)
        tape[i] = 0;
    for (i = 0; i < PLEX_N; i++)
        plex[i] = 0;
    for (i = 0; i < CL_LEN; i++)
        cl_buf[i] = 0;
    for (i = 0; i < CL_NG; i++)
        cl_g[i].len = 0;
    cl_live = 0;
    tape_w = plex_w = tape_ph = 0;
    head2 = head2_t = 0.75f;
    head2_v = 0.0f;
    echo_len = 0.0f;
    q_r = q_w = 0;
    for (i = 0; i < NMOD; i++) {
        mod[i].v = mod[i].tgt = 0.0f;
        mod[i].left = 0;
    }
    for (i = 0; i < EN_NPAD; i++) {
        float p = (STAB_PAN[i] + 1.0f) * (FM_PI / 4.0f);
        stab_gl[i] = fm_cosf(p);
        stab_gr[i] = fm_sinf(p);
        stab[i].ph[0] = 0.13f * (float)i;
        stab[i].ph[1] = 0.37f + 0.21f * (float)i;
    }
    duck_rel = fm_expf(-1.0f / (0.13f * EN_SR));
    duck_relb = fm_expf(-1.0f / (0.25f * EN_SR));
    throw_dec = fm_expf(-1.0f / (0.5f * EN_SR));
    bite_dec = fm_expf(-1.0f / (0.07f * EN_SR));
    gate_dec = fm_expf(-6.9f / (1.5f * EN_SR));
    arcm = arc = 0.0f;
    for (i = 0; i < P_NPARAMS; i++)
        apply(i, PARAMS[i].def);
    pool_set(EN_MODE_POOL[par[P_MODE]]);
    for (i = 0; i < EN_NLOOP; i++)
        loop_pos[i] = 0;
    regrow();
    bass.note = bass.target = (float)bass_note(chord_pcs[0]);
    pl_decay = pl_decay_t;
}

/* ------------------------------------------------------------ transport --- */
static int32_t tick_wait;                       /* Swing: samples until an off sixteenth plays (0: none waiting) */
static void clock_pulse(void)
{
    if (pulse == 0) {                           /* an off sixteenth: everything on it late together (the clock straight) */
        int32_t w = (sixteenth & 1u) ? (int32_t)(swing * step_len) : 0;
        if (w > 0)
            tick_wait = w;
        else
            tick();
    }
    if (!en_ext)
        midi(0xF8, 0, 0);
    if (++pulse >= 6)
        pulse = 0;
}

static void play_start(int from_top)
{
    int i;
    if (from_top) {
        pulse = 0;
        sixteenth = 0;
        en_bars = 0;
        bar = 0;
        for (i = 0; i < EN_NLOOP; i++)
            loop_pos[i] = 0;
        section_set(SEC_FULL);
        sec_next = SEC_FULL;
    }
    pulse_left = 0.0f;
    tick_wait = 0;
    en_playing = 1;
    if (!en_ext)
        midi(from_top ? 0xFA : 0xFB, 0, 0);
}

static void play_stop(void)
{
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        drum[d].wait = 0;                        /* a swung hit still waiting stays unplayed */
    tick_wait = 0;
    en_playing = 0;
    bass.gate = 0;
    if (!en_ext)
        midi(0xFC, 0, 0);
    notes_off();
}

/* a master action turned: the parameters it moves, applied again as heard (Filter: read by render_chunk) */
static void macro(int i, int v)
{
    /* each: the parameters it moves and how far at full turn (+50), in the parameter's own units */
    static const struct { uint8_t p; int8_t at50; } MOVE[] = {
        {P_ECHO, 90}, {P_FEEDBACK, 50},                  /* Dub (and, up, the music pushed into the echo: render_chunk) */
        {P_DENSITY, 85}, {P_HITS, 85}, {P_STABS, 85},    /* Energy: the notes and hits, */
        {P_PINGS, 40}, {P_HATS, 45}, {P_DECAY, 30},      /* the pings' level, the hats' colour, the stabs' length
                                                          * (and the hats' level, the phrases: render_drums, bar_start) */
        {P_REVERB, 85}, {P_CLOUD, 80}, {P_SIZE, 45},     /* Space */
    };
    static const uint8_t OF[] = {EN_MAC_DUB, EN_MAC_DUB, EN_MAC_ENERGY, EN_MAC_ENERGY, EN_MAC_ENERGY, EN_MAC_ENERGY,
                                 EN_MAC_ENERGY, EN_MAC_ENERGY, EN_MAC_SPACE,
                                 EN_MAC_SPACE, EN_MAC_SPACE};
    unsigned k;
    mac[i] = (int8_t)v;
    for (k = 0; k < sizeof OF; k++)
        if (OF[k] == i) {
            int o = MOVE[k].at50 * v / 50, before = pe(MOVE[k].p);
            pofs[MOVE[k].p] = (int8_t)o;
            if (pe(MOVE[k].p) != before)
                apply(MOVE[k].p, par[MOVE[k].p]);
        }
}

static void drain(void)
{
    while (q_r != q_w) {
        uint32_t c = q[q_r % QN];
        int a = (int)(c >> 16 & 0xFFu), b = (int)(int16_t)(c & 0xFFFFu);
        BARRIER();
        q_r++;
        switch (c >> 24) {
        case C_SET:
            if (a < P_NPARAMS)
                apply(a, b);
            break;
        case C_POOL: pool_set((uint32_t)b & 0xFFFu); break;
        case C_PLAY:
            if (a && !en_playing)
                play_start(1);
            else if (!a && en_playing)
                play_stop();
            break;
        case C_NEXT:
            if (en_playing)
                change_pending = 1;
            else
                chord_change();
            break;
        case C_FREEZE: en_frozen = (uint8_t)(a != 0); break;
        case C_SEED:
            rng ^= (uint32_t)(b & 0xFFFF) * 2654435761u;
            if (!rng)
                rng = 1u;
            break;
        case C_CLOCK:
            en_ext = 1;
            ext_age = 0;
            if (a == EN_CLK_TICK) {
                if (en_playing)
                    clock_pulse();
            } else if (a == EN_CLK_START) {
                play_start(1);
            } else if (a == EN_CLK_CONTINUE) {
                if (!en_playing)
                    play_start(0);
            } else if (en_playing) {
                play_stop();
            }
            break;
        case C_PANIC: quiet(); break;
        case C_REGROW:
            regrow();
            if (pings_have)                      /* (a piece's own loops, kept) */
                pings_take();
            break;
        case C_PINGS:
            pings_have = (uint8_t)a;
            if (a)
                pings_take();
            break;
        case C_MUTE: mutes = (uint32_t)a; lanes(); break;
        case C_PERF: perf = (uint32_t)a; lanes(); break;
        case C_FX: fxm = c & 0xFFFFu; break;
        case C_REWALK: {                         /* the walk's slots walked again, all but the one playing, each from
                                                  * the slot before (yours and the gaps kept) */
            int k, prev = deg;
            for (k = 0; k < EN_NPROG && nsteps; k++) {
                if (k != prog_pos && slot_walk(k)) {
                    slot_walk_set(k, walk_from(prev));
                    prev = par[P_CR0 + k] - 1;
                } else if (slot_walk(k)) {
                    prev = (par[P_CR0 + k] - 1) % nsteps;
                } else if (par[P_CQ0 + k]) {
                    prev = en_deg_of(pool, (par[P_CR0 + k] - par[P_ROOT] + 12) % 12);
                }
            }
            prog_publish();
            break;
        }
        case C_PNUDGE: {                         /* a nudge on the pings: one note of one loop, another */
            int l = irand(EN_NLOOP), k = loop_pick(l);
            if (k >= 0)
                loop_note[l][k] = (int8_t)rand_offset();
            break;
        }
        case C_PROT:                             /* Loop to Endless: the one playing up to slot 1, all turned round */
            if (prog_pos > 0) {
                int16_t t[3][EN_NPROG];
                int j, k;
                for (k = 0; k < EN_NPROG; k++)
                    for (j = 0; j < 3; j++)
                        t[j][k] = par[P_CR0 + 8 * j + (k + prog_pos) % EN_NPROG];
                for (k = 0; k < EN_NPROG; k++)
                    for (j = 0; j < 3; j++)
                        par[P_CR0 + 8 * j + k] = t[j][k];
                prog_pos = 0;
                en_prog_pos = 0;
                prog_publish();
            }
            break;
        case C_MACRO:
            if (a < EN_NMAC && (unsigned)b <= 100u)
                macro(a, b - 50);
            break;
        }
    }
}

/* --------------------------------------------------------------- render --- */
#define BLK 256
static float mixl[BLK], mixr[BLK], rsend[BLK], esend[BLK], kickb[BLK], bassb[BLK], crl[BLK], crr[BLK];
/* (bassb: the bass, mono, on its own so the duck takes it deeper than the rest) */

static void render_stabs(uint32_t n)
{
    uint32_t j;
    int i;
    const int cv = cvoice;
    /* the sound page: Attack (a swell up, a snap down), Colour (as each voice has it: the detune, the FM index, the
     * drawbars, the noise against the saw), Resonance, Env (how far each stab opens the filter) */
    const float xa = rel(P_CATT), xc = rel(P_CCOL), up = fm_clampf(1.0f + xc, 0.0f, 2.0f), fx = fm_exp2f(1.5f * xc);
    const float g = chord_vol * 0.5f * CV[cv].gain,
                k = fm_clampf(1.1f - 0.6f * fm_clampf(0.5f + 0.5f * arcm, 0.0f, 1.0f) - 0.55f * rel(P_CRES) -
                                 0.22f * fm_maxf(perf_sw, 0.0f), 0.12f, 2.0f);   /* (Open: it sings) */
    const float att = fm_clampf(CV[cv].att * fm_exp2f(-(xa > 0.0f ? 9.0f : 3.0f) * xa), 0.00004f, 0.3f),
                fe = CV[cv].fenv * fm_maxf(1.0f + rel(P_CENV), 0.0f);
    float pc = 0.0f, ck = click;
    const float dk = st_dec + (gate_dec - st_dec) * fx_gate;   /* (the grid's Gate: the chord rings on, to be chopped) */
    const fmpatch_t *pt = cv_patch(cv);
    const int alg = pt ? fm_alg_of(P_CALG, pt) : 0;
    const float cfb = pt ? fm_fb(P_CFB, pt) : 0.0f;
    float s0 = st_s[0], s1 = st_s[1], s2 = st_s[2], s3 = st_s[3], env = st_env, amp = st_amp, thr = st_throw;
    float hl = st_h[0], hr = st_h[1];
    if (st_midi_left > 0 && (st_midi_left -= (int32_t)n) <= 0)
        for (i = 0; i < EN_NPAD; i++)
            midi(0x80, stab[i].midi_note, 0);
    if (env < 1e-5f && amp < 1e-5f) {
        st_env = st_amp = 0.0f;
        st_throw = thr * fm_expf(-(float)n / (0.5f * EN_SR));
        en_pad_level = 0.0f;
        for (i = 0; i < EN_NPAD; i++)
            en_pad_amp[i] = 0.0f;
        return;
    }
    for (j = 0; j < n; j++) {
        float l = 0.0f, r = 0.0f, v1, v2, v3, yl, yr, e;
        if (!(j & 31u)) {                        /* the filter: its base, its envelope, the sweep */
            float fc = cut_base * fm_exp2f(fe * st_fenv + 1.0f * arcm + perf_sw + 0.3f * motion * mod[0].v);
            svf_coefs(st_c, fm_clampf(fc, 90.0f, 12000.0f), k);
            st_fenv *= st_fdec;
            pc = st_fenv * st_fenv;              /* (Tonewheel: the percussion, quicker still) */
            if (pt)
                for (i = 0; i < EN_NPAD; i++)
                    fm_ctl(&cfm[i], pt, alg, fx);   /* (Colour: the modulators' levels) */
        }
        env *= dk;
        amp += (env - amp) * att;               /* the strike, without a click (or a swell, for the Pad) */
        if (cv == CV_NOISE) {                   /* one noise for all four: the filter gives it its pitch */
            noise = noise * 1664525u + 1013904223u;
            l = r = (float)(int32_t)noise * (1.0f / 2147483648.0f) * 1.6f * (1.0f - 0.5f * xc);
        }
        for (i = 0; i < EN_NPAD; i++) {
            stab_t *v = &stab[i];
            float x;
            switch (cv) {
            case CV_KEYS: case CV_BELL:          /* the FM core */
                x = fm_sample(&cfm[i], alg, cfb, 1.0f) * 0.9f;
                break;
            case CV_ORGAN: {
                float p = v->ph[0];
                x = (sine(p) + up * (0.5f * sine(2.0f * p) + 0.33f * sine(3.0f * p) + 0.2f * sine(4.0f * p))) * 0.8f;
                break;
            }
            case CV_WHEEL: {                     /* 8', 4', 2 2/3' and 2'; the percussion (the third harmonic) dies fast */
                float p = v->ph[0];
                x = (sine(p) + up * (0.7f * sine(2.0f * p) + 0.3f * sine(4.0f * p)) + (0.35f * up + 0.9f * pc) * sine(3.0f * p)) * 0.7f;
                break;
            }
            case CV_NOISE:
                x = 0.35f * up * blep_saw(v->ph[0], v->inc[0]);
                break;
            default:                             /* Saws, Pad */
                x = blep_saw(v->ph[0], v->inc[0]) + blep_saw(v->ph[1], v->inc[1]);
                break;
            }
            v->ph[0] += v->inc[0];
            if (v->ph[0] >= 1.0f)
                v->ph[0] -= 1.0f;
            v->ph[1] += v->inc[1];
            if (v->ph[1] >= 1.0f)
                v->ph[1] -= 1.0f;
            l += x * stab_gl[i];
            r += x * stab_gr[i];
        }
        if (cv == CV_WHEEL) {                   /* the click, and the rotor: left and right in turn */
            float ro;
            noise = noise * 1664525u + 1013904223u;
            ck *= 0.996f;
            l += ck * (float)(int32_t)noise * (1.0f / 2147483648.0f);
            r += ck * (float)(int32_t)noise * (1.0f / 2147483648.0f);
            rotor += 5.6f / EN_SR;              /* the fast speed, ~5.6 turns a second */
            if (rotor >= 1.0f)
                rotor -= 1.0f;
            ro = 0.3f * sine(rotor);
            l *= 1.0f + ro;
            r *= 1.0f - ro;
        }
        v3 = l - s1;                             /* the low-pass, left */
        v1 = st_c[0] * s0 + st_c[1] * v3;
        v2 = s1 + st_c[1] * s0 + st_c[2] * v3;
        s0 = 2.0f * v1 - s0;
        s1 = 2.0f * v2 - s1;
        yl = v2;
        v3 = r - s3;                             /* and right */
        v1 = st_c[0] * s2 + st_c[1] * v3;
        v2 = s3 + st_c[1] * s2 + st_c[2] * v3;
        s2 = 2.0f * v1 - s2;
        s3 = 2.0f * v2 - s3;
        yr = v2;
        e = amp * g;
        yl = sat(yl * e * 3.0f) * 0.4f;
        yr = sat(yr * e * 3.0f) * 0.4f;
        hl += 0.0225f * (yl - hl);               /* a gentle high-pass, ~160 Hz: the stabs out of the low-mids */
        hr += 0.0225f * (yr - hr);
        yl -= hl;
        yr -= hr;
        if (fx_gate > 0.001f) {                  /* the grid's Gate: the chord in sixteenths */
            float gt = (float)(gate_ctr + j) < 0.5f * step_len ? 1.0f : 0.0f, gg;
            gate_g += (gt - gate_g) * 0.05f;
            gg = 1.0f - fx_gate * (1.0f - gate_g);
            yl *= gg;
            yr *= gg;
        }
        mixl[j] += yl;
        mixr[j] += yr;
        esend[j] += (yl + yr) * (0.8f + 2.5f * thr);
        rsend[j] += (yl + yr) * 0.35f;
        thr *= throw_dec;
    }
    gate_ctr += (int32_t)n;
    st_s[0] = fm_flush(s0);
    st_s[1] = fm_flush(s1);
    st_s[2] = fm_flush(s2);
    st_s[3] = fm_flush(s3);
    st_h[0] = fm_flush(hl);
    st_h[1] = fm_flush(hr);
    st_env = env;
    st_amp = amp;
    st_throw = thr;
    click = fm_flush(ck);
    en_pad_level = fm_minf(amp * 1.2f, 1.0f);
    for (i = 0; i < EN_NPAD; i++) {
        en_pad_note[i] = stab[i].note;
        en_pad_amp[i] = amp;
    }
}

static void render_bass(uint32_t n)
{
    uint32_t j;
    const int bv = (unsigned)par[P_BVOICE] < EN_NBV ? par[P_BVOICE] : 0;
    const float g = bass_vol * 0.5f * BV_GAIN[bv] * GROOVE_LVL[gpat()],
                xb = rel(P_BBITE), xs = rel(P_BSHAPE),
                drv = 1.2f * fm_exp2f(1.2f * xb), bi = fm_maxf(1.0f + xb, 0.0f);   /* Bite: the drive, the FM bite */
    const int balg = BV_PATCH[bv] ? fm_alg_of(P_BALG, BV_PATCH[bv]) : 0;
    const float bfb = BV_PATCH[bv] ? fm_fb(P_BFB, BV_PATCH[bv]) : 0.0f;
    float ph = bass.ph, env = bass.env, inc, bite = bass_bite;
    int32_t gate = bass.gate;
    if (env < 1e-5f && gate <= 0) {
        bass.env = 0.0f;
        en_bass_level = 0.0f;
        return;
    }
    bass.note += (bass.target - bass.note) * fm_minf((float)n / (0.012f * fm_exp2f(4.4f * rel(P_BSLIDE)) * EN_SR), 1.0f);
                                                 /* a short slide (Slide: ~1 ms .. ~250 ms) */
    inc = mtof(bass.note) * (1.0f / EN_SR);
    if (BV_PATCH[bv])                            /* the FM voices follow the slide */
        fm_tune(&bfm, BV_PATCH[bv], mtof(bass.note), 1.0f, fm_q(P_BFRAT));
    for (j = 0; j < n; j++) {
        float x, tgt = gate > 0 ? 1.0f : 0.0f;
        if (gate > 0 && --gate == 0)
            midi(0x82, bass.midi_note, 0);
        if (perf & EN_PF_KICKDROP && gate > 0) {   /* PERFORM's Drop: the bass let go at once (~20 ms) */
            gate = 0;
            midi(0x82, bass.midi_note, 0);
            tgt = 0.0f;
        }
        env += (tgt - env) * (tgt > env ? 0.02f : 0.0012f);   /* ~1 ms in, ~20 ms out */
        if (BV_PATCH[bv]) {                      /* (Bite: the modulators' levels) */
            if (!(j & 31u))
                fm_ctl(&bfm, BV_PATCH[bv], balg, bi);
            x = fm_sample(&bfm, balg, bfb, 1.0f);
        } else {
            x = sine(ph) + 0.2f * sine(2.0f * ph);
        }
        if (xs > 0.0f)                           /* Shape: towards a square, or back to a sine */
            x += xs * (sat(x * 4.0f) - x);
        else if (xs < 0.0f)
            x -= xs * (sine(ph) - x);
        bite *= bite_dec;
        ph += inc;
        if (ph >= 1.0f)
            ph -= 1.0f;
        x = sat(x * drv) * env * g * (1.2f / drv + 0.0f) * (0.6f + 0.4f * fm_minf(drv / 1.2f, 1.6f));
        bassb[j] += x;
        rsend[j] += x * 0.04f;
    }
    bass.ph = ph;
    bass_bite = fm_flush(bite);
    bass.env = fm_flush(env);
    bass.gate = gate;
    en_bass_level = env;
}

static void render_bells(uint32_t n)
{
    int i;
    const float g = bell_vol * 1.4f, arate = 0.004f + 0.02f * fm_minf(btone * 2.0f, 1.0f);
    for (i = 0; i < NBELL; i++) {
        bell_t *v = &bell[i];
        float amp = v->amp, att = v->att;
        const int alg = fm_alg_of(P_PALG, &v->pt);
        const float fb = fm_fb(P_PFB, &v->pt);
        uint32_t j = 0;
        if (!v->on)
            continue;
        if (v->wait > 0) {
            if ((uint32_t)v->wait >= n) {
                v->wait -= (int32_t)n;
                continue;
            }
            j = (uint32_t)v->wait;
            v->wait = 0;
        }
        for (; j < n; j++) {
            float x;
            if (!(j & 31u))
                fm_ctl(&v->fm, &v->pt, alg, 1.0f);
            x = fm_sample(&v->fm, alg, fb, 1.0f + v->drop) * amp * att * g;
            mixl[j] += x * v->gl * ping_dry;   /* (Ghost: the dry against the sends, which stay; 50: -6 dB) */
            mixr[j] += x * v->gr * ping_dry;
            rsend[j] += x * 0.4f;
            esend[j] += x * 1.1f;
            v->drop *= 0.9993f;                  /* (Drop: ~35 ms to fall) */
            amp *= v->adec;
            att += (1.0f - att) * arate;
        }
        v->amp = amp;
        v->att = att;
        if (amp < 0.0008f) {
            v->on = 0;
            midi(0x81, v->note, 0);
        }
    }
}

/* a hat: the metal and noise, high-passed (Chamberlin SVF, ~1.8 to 5 kHz by Hats) and low-passed (~8 kHz):
 * in the references' 2-8 kHz, not the air above (1.3.0); no pitch; into the echo (metal: the square's share,
 * at: the attack's smoothing) */
static void render_hat(drum_t *v, uint32_t n, float g, float pl, float pr, float es, float metal, float at)
{
    uint32_t j;
    int k;
    float x1, x2, amp, att, lp, lp2, hf = fm_minf(hat_f * KIT[kit].hat_f, 1.2f);
    if (!v->on)
        return;
    x1 = v->x1, x2 = v->x2, amp = v->amp, att = v->att, lp = v->lp[0], lp2 = v->lp[1];
    for (j = 0; j < n; j++) {
        float in = 0.0f, hp, x;
        for (k = 0; k < 4; k++) {
            v->mph[k] += HAT_INC[k];
            if (v->mph[k] >= 1.0f)
                v->mph[k] -= 1.0f;
            in += v->mph[k] < 0.5f ? 0.25f : -0.25f;
        }
        noise = noise * 1664525u + 1013904223u;
        in = metal * (1.1f - KIT[kit].noise) * in + KIT[kit].noise * (float)(int32_t)noise * (1.0f / 2147483648.0f);
        hp = in - x2 - 0.8f * x1;
        x1 += hf * hp;
        x2 += hf * x1;
        lp += 0.65f * (hp - lp);
        lp2 += 0.65f * (lp - lp2);
        x = lp2 * amp * att * g;
        mixl[j] += x * pl;
        mixr[j] += x * pr;
        rsend[j] += x * 0.15f;
        esend[j] += x * es;
        amp *= v->adec;
        att += (1.0f - att) * at;
    }
    v->x1 = fm_flush(x1);
    v->x2 = fm_flush(x2);
    v->lp[0] = fm_flush(lp);
    v->lp[1] = fm_flush(lp2);
    v->amp = amp;
    v->att = att;
    v->on = amp > 0.0005f;
}

static void render_drums(uint32_t n)
{
    uint32_t j;
    const float g = pulse_vol * KIT[kit].gain, fall = KIT[kit].fall, drive = KIT[kit].drive;
    drum_t *v = &drum[0];
    if (v->on) {                                 /* the kick: its own bus, ducked by nothing */
        float ph = v->ph, inc = v->inc, amp = v->amp, att = v->att, clk = v->clk, cl = v->x1;
        for (j = 0; j < n; j++) {
            float x;
            noise = noise * 1664525u + 1013904223u;
            cl += 0.5f * ((float)(int32_t)noise * (1.0f / 2147483648.0f) - cl);   /* the click: noise, softened */
            x = (sine(ph) * att + 0.5f * cl * clk) * amp;
            kickb[j] += sat(x * drive) * g * (0.63f / drive);   /* (-5 dB in 1.3.0: the drums as spice) */
            ph += inc;
            if (ph >= 1.0f)
                ph -= 1.0f;
            inc += (v->inc_end - inc) * fall;
            amp *= v->adec;
            att += (1.0f - att) * 0.05f;          /* (in over ~0.5 ms: softer) */
            clk *= 0.985f;
        }
        v->ph = ph;
        v->inc = inc;
        v->amp = amp;
        v->att = att;
        v->clk = clk;
        v->x1 = fm_flush(cl);
        v->on = amp > 0.0005f;
    }
    {   /* (Energy: the hats -6 dB down to +6 dB up) */
        const float hl = fm_exp2f((float)mac[EN_MAC_ENERGY] * (1.0f / 50.0f)) * hats_lvl;   /* (and Hats: +-12 dB) */
        render_hat(&drum[1], n, g * 0.16f * hl, 0.6f, 1.0f, 0.2f, 0.6f, 0.02f);   /* shaker: a little right, soft, into the echo */
        render_hat(&drum[2], n, g * 0.17f * hl, 1.0f, 0.7f, 0.2f, 1.0f, 0.3f);   /* open: a little left, some echo */
    }
    v = &drum[3];                                /* the rim: two tones (~520 Hz, ~1.7 kHz) and a click of noise */
    if (v->on) {
        float amp = v->amp, att = v->att, clk = v->clk;
        for (j = 0; j < n; j++) {
            float x;
            v->ph += 520.0f / EN_SR;
            if (v->ph >= 1.0f)
                v->ph -= 1.0f;
            v->mph[0] += 1710.0f / EN_SR;
            if (v->mph[0] >= 1.0f)
                v->mph[0] -= 1.0f;
            noise = noise * 1664525u + 1013904223u;
            x = (sine(v->ph) * 0.55f + sine(v->mph[0]) * 0.45f + (float)(int32_t)noise * (1.0f / 2147483648.0f) * clk) *
                amp * att * g * 0.3f;
            mixl[j] += x * 0.85f;
            mixr[j] += x;
            esend[j] += x * 1.2f;                /* (mostly into the echo: a dub rim) */
            rsend[j] += x * 0.5f;
            amp *= v->adec;
            att += (1.0f - att) * 0.4f;
            clk *= 0.9f;
        }
        v->amp = amp;
        v->att = att;
        v->clk = clk;
        v->on = amp > 0.0005f;
    }
    v = &drum[4];                                /* the clap: noise through a band-pass (~1.1 kHz), its envelope */
    if (v->on) {                                 /* three bursts 9 ms apart, then the tail */
        float amp = v->amp, t = v->ph, x1 = v->x1, x2 = v->x2;
        for (j = 0; j < n; j++) {
            float w, hp, bp, e, x, tb = t < 1200.0f ? (t - 400.0f * (float)(int)(t * (1.0f / 400.0f))) : 1e9f;
            noise = noise * 1664525u + 1013904223u;
            w = (float)(int32_t)noise * (1.0f / 2147483648.0f);
            hp = w - x2 - 0.7f * x1;            /* (a Chamberlin SVF: the band) */
            x1 += 0.16f * hp;
            x2 += 0.16f * x1;
            bp = x1;
            /* a burst: ~3 ms, in over ~0.3 ms (from nothing at once, the ringing band stepped: a click each burst,
             * which the plate and the echo repeated) */
            e = tb < 1e8f ? (1.0f - fm_expf(-tb * (1.0f / 6.0f))) * fm_expf(-tb * (1.0f / 130.0f)) : 0.0f;
            if (t >= 1200.0f)
                e = amp * 0.9f;
            x = bp * e * g * 1.1f;
            mixl[j] += x;
            mixr[j] += x * 0.8f;
            rsend[j] += x * 1.4f;                /* (into the plate, most of it) */
            esend[j] += x * 0.5f;
            if (t >= 1200.0f)
                amp *= v->adec;
            t += 1.0f;
        }
        v->amp = amp;
        v->ph = t;
        v->x1 = fm_flush(x1);
        v->x2 = fm_flush(x2);
        v->on = amp > 0.0005f;
    }
}

/* Crackle: a small palette of noises, each mono, each drifting on its own over minutes (its level, and the
 * hiss its colour), so the bed never settles into one static noise (1.3.0; it was a garnish of hiss and clicks):
 *   the hiss: the tape's floor, flat over 2-8 kHz and dark above, its band moving slowly (an auto-filter on a
 *     random LFO);
 *   the vinyl: clicks at random, mostly small, now and then a pop, each its own colour (dull to sharp) and its
 *     own side; how many drifts, so the transients slow down and space out now and then;
 *   the dust: soft grains of band-passed noise (1.5-6 kHz, 10-50 ms), a few a beat at random, partly into the
 *     echo: a texture between the hits;
 *   the rumble: a room's, a deck's, under 60 Hz and mono, very quiet;
 *   the wind: a breath of noise through a narrow band (~250 Hz-2 kHz), swept slowly by the drift and Motion's arc.
 * The Crackle knob is the level of all of it; turned up, more of the palette comes in (the hiss and the vinyl
 * from the start, the dust from ~20, the rumble from ~25, the wind from ~35). In a Break or Thin, the hiss and
 * the dust come forward (+2.5 dB), to carry the groove. It plays while the music plays, fading in and out over
 * a second; a little of it into the plate.
 * Four controls shape it, each -1..+1 around the piece's own (0), and a character (a preset of the mix): ready
 * on ARP's Noise page (P_NCOL .. P_NCHAR, into nctl) */
const char *const EN_NCHAR_NAME[EN_NNCHAR] = {"Vinyl", "Tape", "Dust", "Room", "Machine"};
static const struct {                           /* each source's level (x the default's), and the controls' offsets */
    float hiss, vinyl, dust, wind, rumble, colour, grain, room, drift;
} NCHAR[EN_NNCHAR] = {
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},       /* Vinyl: the default, as tuned */
    {1.4f, 0.35f, 0.5f, 1.0f, 0.7f, -0.3f, -0.6f, 0.0f, -0.3f},   /* Tape: the hiss, dark and steady */
    {0.7f, 0.8f, 1.8f, 0.6f, 0.5f, 0.3f, 0.5f, -0.2f, 0.2f},      /* Dust: the grains forward, brighter */
    {0.8f, 0.5f, 0.8f, 1.8f, 2.5f, -0.2f, 0.0f, 0.6f, 0.0f},      /* Room: the wind and the rumble, a space */
    {0.9f, 1.2f, 1.4f, 0.8f, 2.0f, -0.4f, 0.6f, 0.5f, 0.4f},      /* Machine: darker, coarser, restless (its clank
                                                                   * and hum want the resonator: later) */
};
#define CR_ND 6                                 /* the drifts: hiss level, hiss colour, vinyl density, dust, wind, rumble */
static float cr_lp, cr_lp2, cr_hp, cr_cl, cr_cr, cr_fl, cr_fr, cr_cc = 0.45f, cr_gate, cr_sg = 1.0f;
static float cr_dv[CR_ND] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, cr_dt[CR_ND] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float CR_DRANGE[CR_ND] = {0.66f, 0.5f, 2.0f, 1.33f, 1.33f, 1.33f};   /* (octaves of each drift's span) */
static float du_env, du_att, du_dec, du_f, du_s1, du_s2, du_gl, du_gr, wi_f, wi_s1, wi_s2, ru1, ru2, ru3;
static int32_t cr_dleft, du_left;
static float du_fk = 1.0f, du_lk = 1.0f, ch_h, ch_v, ch_d, ch_w, ch_r;        /* the dust's band and length, as Colour and Grain have them */
static float cr_u(void)                          /* the noises' own randomness: the music's choices left be */
{
    cnoise = cnoise * 1664525u + 1013904223u;
    return (float)(cnoise >> 8) * (1.0f / 16777216.0f);
}
static void dust_birth(void)                    /* a dust grain: its band, size, length, side, and the wait to the next */
{
    float fc = fm_minf(1500.0f * fm_exp2f(2.0f * cr_u()) * du_fk, 9000.0f);
    du_f = 2.0f * fm_sinf(FM_PI * fc / EN_SR);
    du_env = 0.3f + 0.7f * cr_u();
    du_dec = fm_expf(-6.9f / ((0.01f + 0.04f * cr_u()) * du_lk * EN_SR));
    du_gl = cr_u();
    du_gr = 1.0f - du_gl;
    du_left = (int32_t)((0.5f + 2.0f * cr_u()) * step_len);
}
static void render_crackle(uint32_t n)
{
    uint32_t j, thr;
    int i;
    float g, gh, lpc, k, sg, gd, gw, gr, vg, nc, ng, nr, nd;
    cr_gate += ((en_playing ? 1.0f : 0.0f) - cr_gate) * fm_minf((float)n / (0.5f * EN_SR), 1.0f);
    if (cr_gate < 1e-4f)
        cr_gate = 0.0f;
    g = mutes & EN_M_NOISE ? 0.0f : crackle_vol * cr_gate;   /* (muted: its level kept) */
    if (g <= 0.0f) {
        for (j = 0; j < n; j++)
            crl[j] = crr[j] = 0.0f;
        return;
    }
    {
        const unsigned c = nctl.character < EN_NNCHAR ? nctl.character : 0u;
        nc = fm_clampf(nctl.colour + NCHAR[c].colour, -1.0f, 1.0f);
        ng = fm_clampf(nctl.grain + NCHAR[c].grain, -1.0f, 1.0f);
        nr = fm_clampf(nctl.room + NCHAR[c].room, -1.0f, 1.0f);
        nd = fm_clampf(nctl.drift + NCHAR[c].drift, -1.0f, 1.0f);
        ch_h = NCHAR[c].hiss, ch_v = NCHAR[c].vinyl, ch_d = NCHAR[c].dust, ch_w = NCHAR[c].wind, ch_r = NCHAR[c].rumble;
    }
    if ((cr_dleft -= (int32_t)n) <= 0) {        /* the drifts: new targets every 2-6 s, eased over ~3 s (Drift: wider
                                                  * and quicker, or narrower and slower) */
        float sp = fm_exp2f(nd);
        for (i = 0; i < CR_ND; i++)
            cr_dt[i] = fm_exp2f((cr_u() - 0.5f) * CR_DRANGE[i] * sp);
        cr_dleft = (int32_t)((2.0f + 4.0f * cr_u()) * EN_SR / sp);
    }
    k = fm_minf((float)n * fm_exp2f(nd) / (3.0f * EN_SR), 1.0f);
    for (i = 0; i < CR_ND; i++)
        cr_dv[i] += (cr_dt[i] - cr_dv[i]) * k;
    cr_sg += ((sec == SEC_BREAK || sec == SEC_THIN ? 1.33f : 1.0f) - cr_sg) * fm_minf((float)n / (2.0f * EN_SR), 1.0f);
    sg = cr_sg;
    /* Colour: the hiss's band, the dust's, the wind's; Grain: the hiss against the vinyl and the dust (more,
     * bigger, longer); Room: the rumble and the wind */
    gh = 0.06f * cr_dv[0] * sg * ch_h * fm_exp2f(-0.5f * ng);
    lpc = fm_clampf(0.62f * cr_dv[1] * fm_exp2f(0.5f * nc), 0.25f, 0.9f);   /* the hiss's band: ~3.3-6 kHz at its top */
    vg = 2.2f * ch_v * fm_exp2f(0.7f * ng);
    gd = 0.7f * cr_wd * cr_dv[3] * sg * ch_d * fm_exp2f(1.2f * ng);
    gw = 0.04f * cr_ww * cr_dv[4] * ch_w * fm_exp2f(nr);
    gr = 0.4f * cr_wr * cr_dv[5] * ch_r * fm_exp2f(2.0f * nr);
    du_fk = fm_exp2f(nc);
    du_lk = fm_exp2f(0.7f * ng);
    wi_f = fm_minf(2.0f * FM_PI * 600.0f / EN_SR * fm_exp2f(1.6f * (cr_dv[1] - 1.0f) + 1.2f * arcm + 0.6f * (cr_dv[4] - 1.0f) +
                                                          0.8f * nc), 0.3f);
    thr = (uint32_t)((8.0f + 30.0f * g) * cr_dv[2] * fm_exp2f(ng) * (4294967296.0f / EN_SR));   /* clicks a second */
    gh *= g, vg *= g, gd *= g, gw *= g, gr *= g;  /* (the knob's level folded in) */
    {   /* (the state in locals: the compiler keeps it in registers, past the sends' stores) */
        uint32_t cn = cnoise;
        float lp1 = cr_lp, lp2 = cr_lp2, hp1 = cr_hp, cl = cr_cl, cr = cr_cr, fl = cr_fl, fr = cr_fr, cc = cr_cc;
        float de = du_env, da = du_att, ds1 = du_s1, ds2 = du_s2, ws1 = wi_s1, ws2 = wi_s2, r1 = ru1, r2 = ru2, r3 = ru3;
        float df = du_f, dd = du_dec, dgl = du_gl, dgr = du_gr;
        const float wf = wi_f;
        const int wind = gw > 0.0f, rum = gr > 0.0f;
        int32_t dl = du_left;
#define CRU() (cn = cn * 1664525u + 1013904223u, (float)(cn >> 8) * (1.0f / 16777216.0f))
        for (j = 0; j < n; j++) {
            float w, w2, h, d = 0.0f, x, l, r;
            cn = cn * 1664525u + 1013904223u;
            w = (float)(int32_t)cn * (1.0f / 2147483648.0f);
            lp1 += lpc * (w - lp1);              /* the hiss */
            lp2 += lpc * (lp1 - lp2);
            hp1 += 0.15f * (lp2 - hp1);
            h = (lp2 - hp1) * gh;
            cn = cn * 1664525u + 1013904223u;
            w2 = (float)(int32_t)cn * (1.0f / 2147483648.0f);
            if (cn < thr) {                      /* the vinyl: a click (its size, side, colour), now and then a pop */
                float a = CRU(), sg1 = CRU() < 0.5f ? -1.0f : 1.0f, pn = CRU();
                cc = 0.12f + 0.75f * CRU();
                a = (0.03f + 0.5f * a * a * a) * (1.5f - 0.6f * cc);
                if (CRU() < 0.03f)
                    a = 0.5f + 0.4f * CRU(), cc = 0.1f + 0.1f * CRU();
                cl += a * sg1 * (1.0f - pn);
                cr += a * sg1 * pn;
            }
            fl += cc * (cl - fl);
            fr += cc * (cr - fr);
            cl *= 0.55f;
            cr *= 0.55f;
            l = h + vg * fl;
            r = 0.9f * h + vg * fr;
            if (--dl <= 0) {                     /* the dust: a grain, every 1/2 to 2 1/2 sixteenths */
                cnoise = cn;
                dust_birth();
                cn = cnoise;
                de = du_env, da = 0.0f, df = du_f, dd = du_dec, dgl = du_gl, dgr = du_gr;
                dl = du_left;
                ds1 = ds2 = 0.0f;
            }
            if (de > 0.001f) {
                float hp = w2 - ds2 - 0.5f * ds1;   /* (a Chamberlin band-pass, fairly wide) */
                ds1 += df * hp;
                ds2 += df * ds1;
                da += (1.0f - da) * 0.01f;
                d = ds1 * de * da * gd;
                de *= dd;
                l += d * dgl;
                r += d * dgr;
                esend[j] += d * 0.6f;
            }
            if (wind) {                          /* the wind: a narrow band, slowly swept */
                float b;
                x = w2 - ws2 - 0.3f * ws1;
                ws1 += wf * x;
                ws2 += wf * ws1;
                b = ws1 * gw;
                l += 0.8f * b;
                r += b;
            }
            rsend[j] += (l + r) * 0.4f;
            if (rum) {                           /* the rumble: under ~60 Hz, its own sub-sonic cut */
                r1 += 0.008f * (w - r1);
                r2 += 0.008f * (r1 - r2);
                r3 += 0.0036f * (r2 - r3);
                x = (r2 - r3) * gr;
                l += x;
                r += x;
            }
            crl[j] = l;
            crr[j] = r;
        }
#undef CRU
        cnoise = cn;
        cr_lp = fm_flush(lp1), cr_lp2 = fm_flush(lp2), cr_hp = fm_flush(hp1);
        cr_cl = fm_flush(cl), cr_cr = fm_flush(cr), cr_fl = fm_flush(fl), cr_fr = fm_flush(fr), cr_cc = cc;
        du_env = fm_flush(de), du_att = da, du_s1 = fm_flush(ds1), du_s2 = fm_flush(ds2), du_left = dl;
        wi_s1 = fm_flush(ws1), wi_s2 = fm_flush(ws2), ru1 = fm_flush(r1), ru2 = fm_flush(r2), ru3 = fm_flush(r3);
    }
}

/* -------------------------------------------------------- the effects grid --- */
/* REC held, the keys: each effect eased in while its key is held and out when let go, costing nothing until
 * then. The echo ones (Throw, Time, Tape stop, Swell) are in the echo; Gate in the chord; Freeze in the cloud;
 * the rest here: a master filter on everything (Low-pass and High-pass sweeping, Wah, Phone), the plate thrown
 * (Verb, Crash), Wash (everything into the echo and the plate, the dry down), Stutter (a sixteenth caught and
 * looped), a dub Siren and a noise Riser */
static float mf_lp = 14.2f, mf_hp = 4.3f, mf_wah, mf_ph, mf_lfo;   /* the master filter: log2 of its cutoffs */
static float mf_c[3][4], mf_s[12];
static float fx_vt, fx_wash, fx_cf, crash, sir_amp, sir_ph, sir_lfo, ris_amp, ris_oct, ris_s[2], ris_c[4];
static uint32_t fx_noise = 9871u;

static void fx_ctl(uint32_t n)                   /* once a chunk: the effects' easing, the filters' coefficients */
{
    const float t = (float)n / EN_SR;
    uint32_t on = fxm, rise = fxm & ~fxm_was;
    float lpt, hpt, ec;
    fxm_was = fxm;
    /* the echo: Time doubles it (halves it, if the tape is too short), Tape stop slows the tape to a quarter */
    fx_ts += ((on >> EN_FX_TAPESTOP & 1u ? 4.0f : 1.0f) - fx_ts) * fm_minf(t / (on >> EN_FX_TAPESTOP & 1u ? 0.7f : 0.25f), 1.0f);
    ec = on >> EN_FX_TIME & 1u ? (echo_len_t * 2.0f <= (float)(TAPE_N - 8) ? 2.0f : 0.5f) : 1.0f;
    fx_em = ec * fx_ts;
    fx_ek = fx_ts > 1.01f ? 0.004f : 0.001f;
    /* the master filter's targets: Low-pass down to 350 Hz, High-pass up to 1.3 kHz, Phone a band 450..3 kHz */
    lpt = on >> EN_FX_LOWPASS & 1u ? 8.45f : on >> EN_FX_PHONE & 1u ? 11.55f : 14.2f;
    hpt = on >> EN_FX_HIGHPASS & 1u ? 10.35f : on >> EN_FX_PHONE & 1u ? 8.8f : 4.3f;
    mf_lp += (lpt - mf_lp) * fm_minf(t / (lpt < mf_lp ? 0.5f : 0.3f), 1.0f);
    mf_hp += (hpt - mf_hp) * fm_minf(t / (hpt > mf_hp ? 0.5f : 0.3f), 1.0f);
    mf_wah += ((on >> EN_FX_WAH & 1u ? 1.0f : 0.0f) - mf_wah) * fm_minf(t / 0.08f, 1.0f);
    mf_ph += ((on >> EN_FX_PHONE & 1u ? 1.0f : 0.0f) - mf_ph) * fm_minf(t / 0.05f, 1.0f);
    if (mf_lp < 14.15f || mf_hp > 4.35f || mf_wah > 0.001f) {
        float wf;
        mf_lfo += t * 1.3f;                      /* (the wah: 1.3 sweeps a second) */
        if (mf_lfo >= 1.0f)
            mf_lfo -= 1.0f;
        wf = 350.0f * fm_exp2f(2.2f * (0.5f + 0.5f * sine(mf_lfo)));
        svf_coefs(mf_c[0], fm_exp2f(mf_lp), 1.0f);
        svf_coefs(mf_c[1], fm_exp2f(mf_hp), 1.2f);
        svf_coefs(mf_c[2], wf, 0.35f);
    }
    fx_gate += ((on >> EN_FX_GATE & 1u ? 1.0f : 0.0f) - fx_gate) * fm_minf(t / 0.03f, 1.0f);
    fx_vt += ((on >> EN_FX_VERB & 1u ? 1.0f : 0.0f) - fx_vt) * fm_minf(t / 0.03f, 1.0f);
    fx_cf += ((on >> EN_FX_FREEZE & 1u ? 1.0f : 0.0f) - fx_cf) * fm_minf(t / 0.1f, 1.0f);
    fx_wash += ((on >> EN_FX_WASH & 1u ? 1.0f : 0.0f) - fx_wash) * fm_minf(t / (on >> EN_FX_WASH & 1u ? 0.4f : 0.8f), 1.0f);
    if (rise >> EN_FX_CRASH & 1u)
        crash = 1.0f;
    if (rise >> EN_FX_STUTTER & 1u) {            /* a sixteenth, caught from now */
        stt_state = 1;
        stt_pos = 0;
        stt_len = (int)fm_clampf(step_len, 1000.0f, (float)PLEX_N);
    } else if (!(on >> EN_FX_STUTTER & 1u) && stt_mix < 0.001f) {
        stt_state = 0;
    }
    sir_amp += ((on >> EN_FX_SIREN & 1u ? 1.0f : 0.0f) - sir_amp) * fm_minf(t / (on >> EN_FX_SIREN & 1u ? 0.02f : 0.15f), 1.0f);
    if (on >> EN_FX_RISER & 1u) {                /* the riser: up an octave and a half a second, louder as it goes */
        ris_oct = fm_minf(ris_oct + t * 1.5f, 5.2f);
        ris_amp += (fm_minf(0.25f + ris_oct * 0.15f, 1.0f) - ris_amp) * fm_minf(t / 0.05f, 1.0f);
    } else {
        ris_amp *= fm_expf(-t / 0.15f);
        if (ris_amp < 0.001f)
            ris_oct = 0.0f;
    }
    if (ris_amp > 0.0005f)
        svf_coefs(ris_c, 250.0f * fm_exp2f(ris_oct), 0.5f);
}

static void render_fx(uint32_t n)                /* the Siren and the Riser: two more voices, into the mix */
{
    uint32_t j;
    if (sir_amp > 0.0005f)
        for (j = 0; j < n; j++) {               /* a square-ish tone, its pitch swung by a triangle, ~0.8 a second */
            float tri, x;
            sir_lfo += 0.8f / EN_SR;
            if (sir_lfo >= 1.0f)
                sir_lfo -= 1.0f;
            tri = sir_lfo < 0.5f ? 4.0f * sir_lfo - 1.0f : 3.0f - 4.0f * sir_lfo;
            sir_ph += 480.0f * fm_exp2f(1.3f * tri) * (1.0f / EN_SR);
            if (sir_ph >= 1.0f)
                sir_ph -= 1.0f;
            x = sat(sine(sir_ph) * 3.0f) * 0.12f * sir_amp;
            mixl[j] += x;
            mixr[j] += x;
            esend[j] += x * 2.0f;
        }
    if (ris_amp > 0.0005f)
        for (j = 0; j < n; j++) {               /* noise through a rising band-pass */
            float w, v1, v2, v3, x;
            fx_noise = fx_noise * 1664525u + 1013904223u;
            w = (float)(int32_t)fx_noise * (1.0f / 2147483648.0f);
            v3 = w - ris_s[1];
            v1 = ris_c[0] * ris_s[0] + ris_c[1] * v3;
            v2 = ris_s[1] + ris_c[1] * ris_s[0] + ris_c[2] * v3;
            ris_s[0] = 2.0f * v1 - ris_s[0];
            ris_s[1] = 2.0f * v2 - ris_s[1];
            x = v1 * 0.35f * ris_amp;
            mixl[j] += x;
            mixr[j] += x;
            rsend[j] += x * 1.2f;
            esend[j] += x * 0.5f;
        }
    ris_s[0] = fm_flush(ris_s[0]);
    ris_s[1] = fm_flush(ris_s[1]);
}

static inline float svf_lp(const float *c, float *s, float x, float *bp)   /* one TPT SVF step: low-pass (bp: its band) */
{
    float v3 = x - s[1], v1 = c[0] * s[0] + c[1] * v3, v2 = s[1] + c[1] * s[0] + c[2] * v3;
    s[0] = 2.0f * v1 - s[0];
    s[1] = 2.0f * v2 - s[1];
    *bp = v1;
    return v2;
}

/* the master filter on one side (s: its six states): the low-pass, then the high-pass, the wah's band, the phone */
static inline float mfx(float x, float *s)
{
    float bp, lp;
    x = svf_lp(mf_c[0], s, x, &bp);
    lp = svf_lp(mf_c[1], s + 2, x, &bp);
    x = x - mf_c[1][3] * bp - lp;
    if (mf_wah > 0.001f) {
        (void)svf_lp(mf_c[2], s + 4, x, &bp);
        x += mf_wah * (bp * 1.8f - x);
    }
    if (mf_ph > 0.001f)
        x += mf_ph * (sat(x * 2.2f) * 0.6f - x);
    return x;
}

static float ms_lp1, ms_lp2;                     /* the side's lows (mono below ~150 Hz) */
static void render_chunk(int32_t *out, uint32_t n, float gain)
{
    uint32_t j;
    const float rg = rev_send * rev_in_gain * (1.0f - 0.2f * arcm);
    /* (the cloud listens as if Reverb were at 63 at least, so Cloud works with little reverb too) */
    const float cl_in = rev_send > 0.0001f ? fm_maxf(rev_send, 0.4f) / rev_send : 0.0f;
    const float push = fm_maxf(perf_echo, 0.5f * fm_maxf((float)mac[EN_MAC_DUB], 0.0f) * (1.0f / 50.0f));   /* (Dub up pushes too) */
    float eg = (echo_send + (1.0f - echo_send) * push) * ein_gain, ca = cloud_amt * (sec == SEC_BREAK ? 1.4f : 1.0f);   /* more in a Break */
    /* PERFORM's Open and Shut on the music, the kick left out on top: Shut a low-pass down to ~230 Hz, Open a
     * resonant high-pass up to ~400 Hz (thin and bright), both swept with perf_sw */
    const float shut = fm_maxf(-perf_sw, 0.0f), open = fm_maxf(perf_sw, 0.0f);
    if (shut > 0.01f)
        svf_coefs(mb_c, 14000.0f * fm_exp2f(-1.9f * shut), 0.55f);
    else if (open > 0.01f)
        svf_coefs(mb_c, 25.0f * fm_exp2f(1.33f * open), 0.5f);
    else
        mb_s[0] = mb_s[1] = mb_s[2] = mb_s[3] = 0.0f;
    for (j = 0; j < n; j++)
        mixl[j] = mixr[j] = rsend[j] = esend[j] = kickb[j] = bassb[j] = 0.0f;
    render_stabs(n);
    render_bass(n);
    render_bells(n);
    render_drums(n);
    render_crackle(n);
    if (fxm || sir_amp > 0.0005f || ris_amp > 0.0005f)
        render_fx(n);
    eg += (1.0f - eg) * fx_wash;                 /* Wash: everything into the echo */
    for (j = 0; j < n; j++) {
        float wl, wr, l, r, el, er, xl, xr, dg, dgb, rin, cl, cr, sd, dry = 1.0f - 0.75f * fx_wash;
        duck_d += (duck_x - duck_d) * 0.08f;
        duck_x *= duck_rel;
        dg = 1.0f - 0.3f * duck_depth * duck_d;  /* the duck split: the wash ducked lightly, the bass deep (it pumps) */
        duck_db += (duck_xb - duck_db) * 0.08f;
        duck_xb *= duck_relb;
        dgb = 1.0f - fm_minf(2.0f * duck_depth, 0.9f) * duck_db;
        echoes(esend[j] * eg, &el, &er, &xl, &xr);
        rin = (rsend[j] * rg + (el + er) * 0.25f * rev_send) * 0.8f;
        if (fx_vt + fx_wash > 0.001f || crash > 0.001f) {   /* Verb, Wash: the dry into the plate; Crash: a burst */
            rin += (mixl[j] + mixr[j] + 2.0f * bassb[j]) * (0.35f * fx_vt + 0.3f * fx_wash);
            if (crash > 0.001f) {
                fx_noise = fx_noise * 1664525u + 1013904223u;
                rin += (float)(int32_t)fx_noise * (1.0f / 2147483648.0f) * crash * 2.0f;
                crash *= 0.99971f;               /* (~80 ms) */
            }
        }
        cloud(rin * ca * 2.5f * cl_in + pl_back * cloud_amt * 0.22f, &cl, &cr);   /* the grains, into the plate: they bloom;
                                                 * the plate's tail back into the line: the wash grows on itself */
        reverb(rin * (1.0f - 0.5f * cloud_amt) + (cl + cr) * 0.8f, &wl, &wr);
        pl_back = 0.5f * (wl + wr);
        cl *= 1.0f + 9.0f * fx_cf;               /* (Freeze: the held cloud brought forward) */
        cr *= 1.0f + 9.0f * fx_cf;
        l = mixl[j] * dry + 1.2f * el + 0.25f * xl + 1.1f * wl + 1.4f * cl;
        r = mixr[j] * dry + 1.2f * er + 0.25f * xr + 1.1f * wr + 1.4f * cr;
        sd = 0.5f * (l - r);                     /* mono below ~150 Hz: the side's lows taken out (two poles), so the
                                                  * stabs' and pings' low partials, the echo's and the plate's, sit in
                                                  * the middle with the bass */
        ms_lp1 += 0.0211f * (sd - ms_lp1);
        ms_lp2 += 0.0211f * (sd - ms_lp1 - ms_lp2);
        sd = ms_lp1 + ms_lp2;
        l = (l - sd) * dg + bassb[j] * dry * dgb;
        r = (r + sd) * dg + bassb[j] * dry * dgb;
        if (shut > 0.01f) {
            float bp;
            l = svf_lp(mb_c, mb_s, l, &bp);
            r = svf_lp(mb_c, mb_s + 2, r, &bp);
        } else if (open > 0.01f) {               /* (the high-pass: what the low-pass and its band leave) */
            float bp, lp = svf_lp(mb_c, mb_s, l, &bp);
            l -= lp + mb_c[3] * bp;
            lp = svf_lp(mb_c, mb_s + 2, r, &bp);
            r -= lp + mb_c[3] * bp;
        }
        l = (l + kickb[j] * (1.0f - 0.5f * fx_wash) + crl[j]) * 0.595f * gain;   /* (1.3.0: +1.5 dB, what the
                                                                                    * re-balance took, from the headroom) */
        r = (r + kickb[j] * (1.0f - 0.5f * fx_wash) + crr[j]) * 0.595f * gain;
        if (mf_lp < 14.15f || mf_hp > 4.35f || mf_wah > 0.001f) {   /* the master filter */
            l = mfx(l, mf_s);
            r = mfx(r, mf_s + 6);
        }
        if (stt_state) {                         /* Stutter: catch a sixteenth (mono), then loop it */
            float m = 0.5f * (l + r), v = 0.0f, tg = 0.0f;
            if (stt_state == 1) {
                plex[stt_pos] = s16(m * 0.5f);
                if (++stt_pos >= stt_len)
                    stt_state = 2, stt_pos = 0;
            } else {
                int e = stt_pos < stt_len - stt_pos ? stt_pos : stt_len - stt_pos;
                v = (float)plex[stt_pos] * (2.0f / 12000.0f) * fm_minf((float)e * (1.0f / 64.0f), 1.0f);
                if (++stt_pos >= stt_len)
                    stt_pos = 0;
                tg = fxm >> EN_FX_STUTTER & 1u ? 1.0f : 0.0f;
            }
            stt_mix += (tg - stt_mix) * 0.005f;
            l += stt_mix * (v - l);
            r += stt_mix * (v - r);
        }

        l = fm_tanhf(l);
        r = fm_tanhf(r);
        out[2 * j] = (int32_t)(l * 8300000.0f);
        out[2 * j + 1] = (int32_t)(r * 8300000.0f);
        scope_acc += l + r;                      /* (the screen's scope and spectrum) */
        spec_acc += l + r;
        if (scope_ph & 1u) {
            en_spec[en_spec_w & (EN_SPEC_N - 1u)] = (int16_t)(fm_clampf(spec_acc, -4.0f, 4.0f) * 8000.0f);
            en_spec_w++;
            spec_acc = 0.0f;
        }
        if (++scope_ph >= 8u) {                  /* (an eighth of the rate, each the mean of its eight) */
            scope_ph = 0;
            en_scope[en_scope_w & (EN_SCOPE_N - 1u)] = (int16_t)(fm_clampf(scope_acc, -16.0f, 16.0f) * 2000.0f);
            scope_acc = 0.0f;
            en_scope_w++;
        }
    }
    duck_x = fm_flush(duck_x);
    duck_d = fm_flush(duck_d);
    duck_xb = fm_flush(duck_xb);
    duck_db = fm_flush(duck_db);
    ms_lp1 = fm_flush(ms_lp1);
    ms_lp2 = fm_flush(ms_lp2);
}

static void control(uint32_t n)                 /* once a chunk: the slow things */
{
    int i;
    mods_step(n);
    fx_ctl(n);
    heads_step(n);
    if (en_playing)
        arc += (arc_tgt - arc) * fm_minf((float)n / arc_tau, 1.0f);
    arcm = arc * motion;
    en_arc = arcm;
    pl_decay += ((en_frozen ? 0.9995f : pl_decay_t) - pl_decay) * 0.002f;
    pl_dd2 = fm_clampf(pl_decay + 0.15f, 0.25f, 0.5f);
    rev_in_gain += ((en_frozen ? 0.0f : 1.0f) - rev_in_gain) * 0.01f;
    ein_gain += ((en_frozen ? 0.0f : 1.0f) - ein_gain) * 0.05f;
    fb_cap += ((en_playing ? 2.0f : 0.9f) - fb_cap) * fm_minf((float)n / (2.0f * EN_SR), 1.0f);
    {   /* the performance's pushes, eased in and out (the filter swept over ~400 ms, three octaves; the echo ~40 ms) */
        /* (the Filter knob: the same sweep, quicker to follow a hand; the keys on top of it, to the same ends) */
        /* (the knob reaches further than the keys: Shut down to ~120 Hz, Open up to ~1.2 kHz) */
        float t = fm_clampf((float)mac[EN_MAC_FILTER] * (mac[EN_MAC_FILTER] < 0 ? 0.072f : 0.084f) +
                            (perf & EN_PF_OPEN ? 3.0f : 0.0f) - (perf & EN_PF_CLOSE ? 3.0f : 0.0f), -3.6f, 4.2f);
        perf_sw += (t - perf_sw) * fm_minf((float)n / ((perf & (EN_PF_OPEN | EN_PF_CLOSE) ? 0.4f : 0.12f) * EN_SR), 1.0f);
        perf_echo += ((perf & EN_PF_ECHO || fxm >> EN_FX_SWELL & 1u ? 1.0f : 0.0f) - perf_echo) * fm_minf((float)n / (0.04f * EN_SR), 1.0f);
    }
    if (echo_len > 0.0f)                        /* for the screen's ripples */
        en_echo_ms = echo_len * (1000.0f / TAPE_SR);
    en_echo_g = en_frozen ? 1.0f : fm_minf(echo_fb + 0.12f * perf_echo, fb_cap) * 0.82f;
    en_echo_in = echo_send + (1.0f - echo_send) * fm_maxf(perf_echo, 0.5f * fm_maxf((float)mac[EN_MAC_DUB], 0.0f) * (1.0f / 50.0f));
    en_duck = duck_d * (duck_depth > 0.0f ? 1.0f : 0.0f);
    cl_dens = cloud_amt > 0.0f ? fm_clampf(0.25f + 0.5f * cloud_amt + 0.3f * arcm, 0.05f, 1.0f) : 0.0f;
    if (fx_cf > 0.01f)                          /* (Freeze: the grains thick, even at Cloud 0) */
        cl_dens = fm_maxf(cl_dens, fx_cf);
    for (i = 0; i < EN_NLOOP; i++)
        en_loop_glow[i] *= 0.985f;
    for (i = 0; i < EN_NDRUM; i++)
        en_drum_glow[i] *= 0.93f;
}

void en_render(int32_t *out, uint32_t n, uint32_t gain_q12)
{
    const float gain = (float)gain_q12 * (1.0f / 4096.0f) * 2.0f;
    drain();
    if (en_ext && ++ext_age > 100u)
        en_ext = 0;
    while (n) {
        uint32_t k = n > BLK ? BLK : n;
        int timed = en_playing && !en_ext;
        if (timed) {
            if (pulse_left <= 0.0f) {
                clock_pulse();
                pulse_left += step_len * (1.0f / 6.0f);
            }
            if ((float)k > pulse_left)
                k = (uint32_t)pulse_left + 1u;
        }
        if (k > n)
            k = n;
        {
            int32_t w = drums_wait();            /* a swung hit lands on its sample */
            if (w > 0 && (uint32_t)w < k)
                k = (uint32_t)w;
            if (tick_wait > 0 && (uint32_t)tick_wait < k)   /* (and a swung sixteenth) */
                k = (uint32_t)tick_wait;
        }
        if (timed)
            pulse_left -= (float)k;
        control(k);
        render_chunk(out, k, gain);
        drums_wait_step((int32_t)k);
        if (tick_wait > 0 && (tick_wait -= (int32_t)k) <= 0) {
            tick_wait = 0;
            if (en_playing)
                tick();
        }
        out += 2 * k;
        n -= k;
    }
}
