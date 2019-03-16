//
//  SineWaveGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-03-15.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef SineWaveGenerator_h
#define SineWaveGenerator_h

#include "SoundGenerator.h"

// FEATURE TO DO LIST
// [x] 1. generates sine waves
// [x] 2. allows dynamic frequency modulation while maintaining phase
//    (no inter-note pops)
// [x] 3. allows turning signal "on" safely: phase resets at 0
//    (no start pops) - signal defaults to off.
// [x] 4. allows turning signal "off" safely: signal continues until
//    amplitude approaches zero (no ending pops)
// [ ] 5. Ramps between changing frequencies with a ____ function.
class SineWaveGenerator: public VariableFrequencySoundGenerator {
private:
	bool _wasActiveLastSample {false};
	bool _persistSignalUntilZero {false};
	
	double _θ {0.0}; // signal phase represented as angle (radians)
	double _targetVolume {1.0}; // the volume level requested by the user.
	
	double _calculatePhaseDelta(timecode_t const Δ_sample, frequency_t const f) {
		double const τ = 2.0 * M_PI;
		
		frequency_t const f_sample = static_cast<double>(SAMPLE_RATE);
		
		// 2*pi * f Hz
		// ----------- = delta theta (phase shift for d_s sec time shift at f Hz)
		//   d_s sec
		double const Δ_θ = τ * f * static_cast<double>(Δ_sample) / f_sample;
		return Δ_θ;
	}
public:
	double volume() { return this->_targetVolume; }
	void volume(double v) { this->_targetVolume = v; }
	
	amplitude_t next() {
		// calculate the next position of θ
		double const τ = M_PI * 2.0;
		timecode_t const Δ_sample = 1U; // assume next sample
		
		double const v = this->_targetVolume;
		
		// speculatively calculate this sample's phase delta,
		// resulting phase, and amplitude. Whether we use it
		// depends.
		double const Δ_θ = _calculatePhaseDelta(Δ_sample, this->_targetFrequency);
		double const θ_speculative = fmod(_θ + Δ_θ, τ);
		double const a = sin(_θ) * v * v;
		
		// 1. the key is active, so play the signal normally
		if(_isActive) {
			_persistSignalUntilZero = false;
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
		}
		// 2. this is the first sample after the key was lifted, we need to
		//    determine if we need to continue the signal until it approaches zero
		else if(_wasActiveLastSample) {
			_wasActiveLastSample = false;
			// test what the amplitude will be for this sample to see if
			// we need to continue
			if(abs(a) >= AMPLITUDE_EPSILON) {
				_persistSignalUntilZero = true;
			}
		}
		// 3. we have determined a need to continue playing this signal until
		//    it approaches zero (anti-end of signal popping)
		if(_persistSignalUntilZero) {
			if(abs(a) < AMPLITUDE_EPSILON) {
				_persistSignalUntilZero = false;
			}
			this->_θ = θ_speculative;
			return a;
		}
		// 4. the key isn't pressed, the key wasn't pressed last sample, and
		//    we did not decide earlier to persist the signal past key-up.
		return 0.0;
	}
};

#endif /* SineWaveGenerator_h */
