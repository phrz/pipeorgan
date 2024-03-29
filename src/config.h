//
//  config.h
//  Music
//
//  Created by Paul Herz on 2019-03-10.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef config_h
#define config_h

#include <cstdint>

static double const τ = M_PI * 2.0;

using sample_t = int16_t;
using midi_t = int16_t;

using timecode_t = uint64_t;
using amplitude_t = double;
using frequency_t = double;

static size_t const N_DRAWBARS = 9;

static size_t const SAMPLE_T_BYTES = 2;
static sample_t const SAMPLE_T_ZERO_POINT = 0x0000;
static sample_t const SAMPLE_T_MAX = 0x7FFF;

static frequency_t const MAX_FREQUENCY = 22000.0;

// amplitude at which we allow a voice to "die" (drop to zero)
// to avoid pops due to infinite-velocity amplitude delta between samples.
// if there's still pops at the ENDS OF NOTES, make this smaller.
// if it's at the beginnings of notes, the phasers are not properly reset.
// if it's between notes that carry on a voice, the phaser is broken and not
// properly persisting wave phase state.
static double const AMPLITUDE_EPSILON = 0.01;

static timecode_t const SAMPLE_RATE {22050};
static timecode_t const SAMPLES_PER_TICK {2000};

static frequency_t const CONCERT_A = 440.;

static size_t const N_MIDI_CODES = 88;
static midi_t const MIN_MIDI_CODE = 21;

// must be <=21 to play MIDI music. Smaller == lower subharmonics.
// represented as pseudo-midi codes (because it can be < smallest allowable
// midi code)
static midi_t const MIN_ORGAN_MIDI_CODE = 0;

// must be >=108 to play MIDI music. Bigger == higher harmonics.
// represented as pseudo-midi codes (because it can be > largest allowable
// midi code)
static midi_t const MAX_ORGAN_MIDI_CODE = 150;
#endif /* config_h */
