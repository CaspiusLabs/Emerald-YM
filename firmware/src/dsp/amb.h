/* SPDX-License-Identifier: GPL-3.0-only */
/* The instrument: a generative dub techno machine. Four layers draw on one pool of notes (a scale: the
 * root plus up to eleven more pitch classes) and one chord that moves now and then through it:
 *
 *   CHORD   the stab: four voices (two detuned saws each, or an FM keys) struck together on a Euclidean
 *           pattern off the beat, through a resonant low-pass that opens with each stab and sweeps with the
 *           arrangement; most of it goes to the echo. One voicing (an open minor 7, or a 9 without its fifth)
 *           shifted whole to each new root, as a chord memory. The chord moves every DRIFT bars, or holds.
 *   PINGS   short FM blips played by four loops of unequal length (13, 17, 23, 29 eighths), each holding a
 *           few notes of the chord that shift against the others; heavily echoed.
 *   BASS    a sub on the chord's root (now and then its fifth), on one of five grooves.
 *   DRUMS   a four-on-the-floor kick (it ducks the rest: DUCK), a shaker on the sixteenths (HITS sets how
 *           many) and an open hat off the beat.
 *
 * Over it all, the ARRANGEMENT: eight-bar phrases, each Full, a Break (the kick and the bass out, the filter
 * building), a Groove (fewer stabs) or Thin (no bass, no shaker); MOTION sets how often it leaves Full. Before
 * the kick comes back, a fill. EVOLVE turns the patterns and throws a stab into the echo now and then.
 * Then a tape echo (to ~5.6 s, two heads, a band-pass in its feedback that the arrangement sweeps, a
 * saturating feedback that may pass 100%), a short Echoplex, a granular CLOUD (after JIANT's) blooming into
 * Dattorro's plate, and a bed of vinyl crackle.
 * FREEZE (dub hold): the echo and the cloud stop listening and play what they hold for ever; the chord holds.
 *
 * Threads: en_render() runs in the audio interrupt. Everything else posts commands, which the render
 * drains at the start of a block; the state below marked (UI) is written by the render and only read
 * elsewhere. Float only, no libm: fastmath.h. */
#pragma once
#include <stdint.h>

#define EN_SR 44100.0f
#define EN_NLOOP 4                     /* ping loops */
#define EN_LOOP_MAX 29
#define EN_NDRUM 3                     /* kick, shaker, open hat */
#define EN_NPAD 4                      /* chord voices */

/* modes: pools relative to the root (bit i: root + i semitones) */
enum { MODE_IONIAN, MODE_DORIAN, MODE_PHRYGIAN, MODE_LYDIAN, MODE_MIXOLYDIAN, MODE_AEOLIAN, MODE_PENT_MAJ,
       MODE_PENT_MIN, EN_NMODES };
extern const char *const EN_MODE_NAME[EN_NMODES];
extern const uint16_t EN_MODE_POOL[EN_NMODES];
extern const char *const EN_NOTE_NAME[12];
extern const uint8_t EN_LOOP_LEN[EN_NLOOP];
extern const uint8_t EN_DRUM_N[EN_NDRUM];

/* parameters: value ranges in en_param_info() */
enum {
    P_CHORD, P_PINGS, P_BASS, P_DRUMS,                /* layer levels */
    P_DENSITY, P_EVOLVE, P_DRIFT, P_HITS,             /* motion: the pings' notes, change, the chord's bars, the shaker */
    P_ECHO, P_ETIME, P_REVERB, P_FEEDBACK,            /* space */
    P_CUTOFF, P_DECAY, P_PTONE, P_MOTION,             /* the stab's filter and length, the pings' tone, the arrangement */
    P_TUNE, P_MIDI, P_TEMPO, P_ROOT, P_MODE, P_POCT,  /* setup, panel encoders, the pings' octave */
    P_CTONE,                                          /* the chord's colour: Saws, Keys */
    P_BASSOCT, P_GROOVE, P_FIFTH,                     /* bass: its octave, its pattern, how often the fifth */
    P_SWING, P_HATS,                                  /* drums: swing, the hats' colour */
    P_STABS, P_CRACKLE, P_DUCK, P_SIZE,               /* stabs a bar, the vinyl, the kick's ducking, the plate */
    P_CLOUD,                                          /* the granular reverb */
    P_NPARAMS
};
typedef struct {
    const char *name;
    int16_t lo, hi, def;
} en_param_t;
const en_param_t *en_param_info(int i);
void en_param_text(int i, int v, char *buf);   /* the value as the screen shows it (10 bytes) */

/* commands (main loop -> render) */
void en_init(void);
void en_set(int param, int value);
void en_pool(uint32_t pool);                   /* the scale, relative to the root (bit 0 always set) */
void en_play(int on);                          /* it plays / rests (the tails ring on) */
void en_next(void);                            /* the next chord: on the next bar (now while resting) */
void en_freeze(int on);
void en_seed(uint32_t s);                      /* stir some entropy into the generators */
void en_panic(void);                           /* everything quiet, now */
void en_regrow(void);                          /* a new piece: fresh loops and patterns, the home chord */
/* MIDI clock in (as FoMni): ticks drive the 1/16 grid (6 a step) while they keep coming */
enum { EN_CLK_TICK, EN_CLK_START, EN_CLK_CONTINUE, EN_CLK_STOP };
void en_clock(int msg);

/* the render (audio ISR): n stereo frames, 24-bit in int32; gain Q12 (the MASTER pot) */
void en_render(int32_t *out_lr, uint32_t n, uint32_t gain_q12);

/* MIDI out, from the render: the app sends it (USB); status, data 1, data 2 */
void en_midi_out(uint32_t st, uint32_t d1, uint32_t d2);

/* the arrangement's sections */
enum { SEC_FULL, SEC_BREAK, SEC_GROOVE, SEC_THIN, EN_NSEC };
extern const char *const EN_SEC_NAME[EN_NSEC];

/* state for the screen (UI) */
extern volatile uint8_t en_playing, en_frozen, en_ext;
extern volatile uint8_t en_chord[EN_NPAD];     /* the chord's notes (MIDI), as the voices play them */
extern volatile uint8_t en_chord_root;         /* the chord's root, MIDI pitch class 0..11 */
extern volatile uint8_t en_chord_n;            /* its distinct tones: 3 or 4 */
extern volatile float en_drift;                /* 0..1: how far to the next chord (0 while it holds) */
extern volatile float en_pad_level, en_bass_level;   /* the stab's and the bass's envelopes, 0..1 */
extern volatile uint8_t en_loop_pos[EN_NLOOP];
extern volatile int8_t en_loop_note[EN_NLOOP][EN_LOOP_MAX];   /* -1 rest, else the slot's offset */
extern volatile float en_loop_glow[EN_NLOOP];  /* 0..1: a ping just sounded from this loop */
extern volatile uint8_t en_drum_pos[EN_NDRUM];
extern volatile uint32_t en_drum_pat[EN_NDRUM];/* bit i: a hit on step i (all 16 steps) */
extern volatile float en_drum_glow[EN_NDRUM];
extern volatile uint32_t en_bars;              /* sixteenths played since start (for tests) */
extern volatile float en_arc;                  /* the filter's sweep, -1 dark .. +1 open (times Motion) */
extern volatile uint8_t en_pulse_rest;         /* the kick is out (a Break) */
extern volatile uint8_t en_pad_note[EN_NPAD];  /* the note each voice plays (MIDI) */
extern volatile float en_pad_amp[EN_NPAD];     /* and how loud */
extern volatile uint8_t en_section;            /* SEC_*: the phrase playing */
extern volatile uint8_t en_phrase_bar;         /* 0..7: its bar */
extern volatile uint32_t en_stab_pat;          /* the stabs' pattern this bar (bit i: step i) */
extern volatile uint32_t en_throws;            /* stabs thrown into the echo so far (for tests) */

/* the pure parts, for the tests and the screen */
uint32_t en_euclid(int k, int n, int rot);     /* Bjorklund's pattern, as a bitmask (bit 0 = step 0) */
int en_scale(uint32_t pool, int *steps);       /* the pool's pitch classes (relative), ascending; count */
/* the chord on scale degree deg: its tones (MIDI pitch classes, absolute), 3 or 4 of them; count */
int en_chord_tones(uint32_t pool, int root, int deg, int *pcs);
/* the stab's voicing on degree deg: four intervals above the chord's root, ascending (shape 0: an open
 * 7th chord, root 5th 7th 10th; shape 1: root 7th 10th 14th, a 9th without its fifth) */
void en_voicing(uint32_t pool, int deg, int shape, int *iv);
const char *en_chord_name(const int *pcs, int n, char *buf);   /* "Em7", "Dsus4", "C" ... */
uint32_t en_groove(int g);                     /* the bass's pattern g: bit i, a note on step i */
int en_drift_bars(int v);                      /* Drift's value in bars; 0: the chord holds */

/* the pieces: recipes for every parameter (Tune and MIDI out included, but they are the player's) */
#define EN_NPIECES 8
const char *en_piece_name(int i);
void en_piece(int i, int16_t *par, uint16_t *pool);
