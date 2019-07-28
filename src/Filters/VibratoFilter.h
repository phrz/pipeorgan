//
//  VibratoFilter.h
//  Music
//
//  Created by Paul Herz on 2019-03-16.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef VibratoFilter_h
#define VibratoFilter_h

#include "SoundFilter.h"
#include "SineWaveGenerator.h"

class VibratoFilter: public SoundFilter {
protected:
	SineWaveGenerator _lfo {};
public:
	amplitude_t intensity {0.0}; // amplitude of the LFO, aka ±hz of freq modulation
	frequency_t rate {0.0}; // frequency of the LFO, aka
	amplitude_t modulateFrequency(frequency_t input) override {
		_lfo.frequency(rate);
		_lfo.volume(1.0);
		_lfo.activate(true);
		
		amplitude_t modFactor = _lfo.next();
		frequency_t output = input + modFactor * intensity;
		return output;
	}
};

#endif /* VibratoFilter_h */
