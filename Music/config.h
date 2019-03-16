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

using sample_t = int16_t;
using midi_t = int16_t;

#define N_DRAWBARS 9

#define SAMPLE_T_BYTES 2
#define SAMPLE_T_ZERO_POINT 0x0000
#define SAMPLE_T_MAX 0x7FFF

// amplitude at which we allow a voice to "die" (drop to zero)
// to avoid pops due to infinite-velocity amplitude delta between samples.
// if there's still pops at the ENDS OF NOTES, make this smaller.
// if it's at the beginnings of notes, the phasers are not properly reset.
// if it's between notes that carry on a voice, the phaser is broken and not
// properly persisting wave phase state.
#define AMPLITUDE_EPSILON 0.01

using timecode_t = uint64_t;
using amplitude_t = double;
using frequency_t = double;

timecode_t const SAMPLE_RATE {22050};
timecode_t const MAX_TICK {1008U};
timecode_t const SAMPLES_PER_TICK {2000};

frequency_t const CONCERT_A = 440.;

#endif /* config_h */
