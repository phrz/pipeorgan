//
//  TrigGenerator.hpp
//  Music
//
//  Created by Paul Herz on 2019-03-10.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef TrigGenerator_hpp
#define TrigGenerator_hpp

#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>

#include "config.h"

// [CITE] https://github.com/rmichela/Acoustico/blob/194945595e78d1e0d1a46d9ffde03fc8a7212d38/phasor.go

class TrigGenerator {
	timecode_t static const STABILIZATION_PERIOD = 500;
	
	// The last known generator timecode
	timecode_t t {0};
	
	// Time of last stabilization
	timecode_t s {0};
	
	// A phasor holding the current state of the trig generator
	std::complex<double> z {1.0, 0.0};
	
	// The sample rate for generation
	frequency_t sampleRate;
	
public:
	TrigGenerator(frequency_t sampleRate);
	TrigGenerator();
	amplitude_t sine(timecode_t t2, frequency_t f);
	void resetPhase(timecode_t sample_i);
};

#endif /* TrigGenerator_hpp */
