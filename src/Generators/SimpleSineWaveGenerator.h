//
//  SimpleSineWaveGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-04-07.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef SimpleSineWaveGenerator_h
#define SimpleSineWaveGenerator_h

#include "SoundGenerator.h"

// A simpler alternative to SineWaveGenerator that still carries over
// phase for frequency changes but does not keep track of amplitude in order
// to persist a signal until it approaches zero, which is not useful in
// some situations (e.g. inside an EnvelopeGenerator).
class SimpleSineWaveGenerator: public VariableFrequencySoundGenerator {
private:
	bool _wasActiveLastSample {false};
	
	double _θ {0.0}; // signal phase represented as angle (radians)
	double _Δ_θ {0.0}; // phase delta (radians), only recalc. when frequency changes.
	
	double _calculatePhaseDelta(timecode_t const Δ_sample, frequency_t const f) {
		frequency_t const f_sample = static_cast<double>(SAMPLE_RATE);
		frequency_t const _f = clamp(f, 0.0, MAX_FREQUENCY);
		// 2*pi * f Hz
		// ----------- = delta theta (phase shift for d_s sec time shift at f Hz)
		//   d_s sec
		double const Δ_θ = τ * _f * static_cast<double>(Δ_sample) / f_sample;
		return Δ_θ;
	}
	
	amplitude_t _nextWithoutFilters() override {
		double const v = this->_targetVolume;
		
		// speculatively calculate this sample's phase delta,
		// resulting phase, and amplitude. Whether we use it
		// depends.
		double const θ_speculative = radians(_θ + _Δ_θ);
		double const a = applyVolume(sin(_θ), v);
		
		// 1. the key is active, so play the signal normally
		if(_isActive) {
			_wasActiveLastSample = true;
			if(!_wasActiveLastSample) {
				// beginning of new signal, reset phase.
				this->_θ = 0.0;
				return 0.0; // sin(0) is always 0.
			} else {
				// continuation of active signal, calculate phase delta.
				this->_θ = θ_speculative;
				return a;
			}
		} else {
			_wasActiveLastSample = false;
		}
		
		return 0.0;
	}
public:
	void frequency(frequency_t f) override {
		timecode_t const Δ_sample = 1U; // assume next sample
		
		this->_targetFrequency = f;
		this->_Δ_θ = _calculatePhaseDelta(Δ_sample, f);
	}
};

#endif /* SimpleSineWaveGenerator_h */
