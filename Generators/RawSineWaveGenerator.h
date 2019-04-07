//
//  RawSineWaveGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-04-07.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef RawSineWaveGenerator_h
#define RawSineWaveGenerator_h

#include "SoundGenerator.h"

// Unlike SineWaveGenerator, does not persist signal until zero (unsafe)
// Unlike SimpleSineWaveGenerator, does not persist phase (bad for multi freq.)
class RawSineWaveGenerator: public VariableFrequencySoundGenerator {
private:
	frequency_t _sinFactor {0.0};
	size_t _sample_i {0U};
	
	amplitude_t _nextWithoutFilters() override {
		double a = 0.0;
		
		double const sample_f = static_cast<double>(SAMPLE_RATE);
		double const _sinFactor = this->frequency() / sample_f;
		
		if(_isActive) {
			double x = static_cast<double>(_sample_i);
			return applyVolume(sin(x * _sinFactor), _targetVolume);
		}
		
		_sample_i++;
		return a;
	}
};

#endif /* RawSineWaveGenerator_h */
