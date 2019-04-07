//
//  EnvelopeGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-03-16.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef EnvelopeGenerator_h
#define EnvelopeGenerator_h

#include <memory>

template <typename Generator, class... Args>
class EnvelopeGenerator: public SoundGenerator {
protected:
	// the value at which a ramp factor or a duration
	// is effectively zero
	constexpr static double const ε_adsr = 1e-6;
	
	std::shared_ptr<Generator> _innerGenerator;
	double _attackDuration {0.0}; // begins at start of note
	double _decayDuration {0.0};
	double _sustainVolume {1.0}; // scale of _targetVolume
	double _releaseDuration {0.0}; // begins at end of note
	
	double _envelopeVolume {0.0};
	double _envelopeVelocity {0.0}; // rate at which to change envelopeVolume
	
	amplitude_t _nextWithoutFilters() override {
		_smoothedTargetVolume =
			_smoothedTargetVolume * (1.0 - _smoothingRate) +
			_targetVolume * _smoothingRate;
		
		_envelopeVolume = saturate(_envelopeVolume + _envelopeVelocity);
		// check if we need to reset velocity to stop decay.
		if(_resetVelocityCountdown == 0) _envelopeVelocity = 0;
		if(_resetVelocityCountdown != -1) _resetVelocityCountdown--;
		
		// activate whether this note is active or we're in release
		_innerGenerator->activate(this->isActive());
		
		// where the magic happens
		_innerGenerator->volume(this->volume());
		
		amplitude_t a = _innerGenerator->next();
		
		return a;
	}
	
	// if ==1, no smoothing. If 0.1, it takes 10 samples to smooth.
	double const _smoothingRate {0.001};
	double _smoothedTargetVolume {0.0};
	
	// if ==0, reset velocity (used to stop decay at sustain volume)
	int _resetVelocityCountdown {-1};
public:
	EnvelopeGenerator(Args&&... args): _innerGenerator(std::make_shared<Generator>(args...)) {
		static_assert(
			std::is_base_of<SoundGenerator, Generator>::value,
			"Generator not derived from SoundGenerator");
	}
	
	void attackDuration(double a) {
		this->_attackDuration = clamp(a, 0.0, 60.0);
	}
	
	void decayDuration(double d) {
		this->_decayDuration = clamp(d, 0.0, 60.0);
	}
	
	void sustainVolume(double s) {
		this->_sustainVolume = saturate(s);
	}
	
	void releaseDuration(double r) {
		this->_releaseDuration = clamp(r, 0.0, 60.0);
	}
	
	bool isActive() override {
		// return whether we are active OR in the release phase of the
		// envelope, determined by the time since deactivation being less
		// than the provided release duration.
		return this->volume() >= ε_adsr;
	}
	
	virtual void activate(bool a) override {
		frequency_t const f_sample = static_cast<double>(SAMPLE_RATE);
		
		// switching from off to on
		// (beginning attack)
		if(!this->_isActive && a) {
			// if attackDuration is near 0, instantaneous.
			if(_attackDuration < ε_adsr) {
				_envelopeVelocity = 0.0;
				_envelopeVolume = 1.0;
			} else {
				// convert volume per second to volume per sample
				_envelopeVelocity = 1.0 / (_attackDuration * f_sample);
			}
		}
		// switching from on to off
		// (beginning release)
		else if(this->_isActive && !a) {
			_resetVelocityCountdown = -1;
			if(_releaseDuration < ε_adsr) {
				_envelopeVelocity = 0.0;
				_envelopeVolume = 0.0;
			} else {
				_envelopeVelocity = -1.0 / (_releaseDuration * f_sample);
			}
		}
		// note is already playing, and attack is done
		// only decay to sustain volume if we have a reasonable decay dur.
		// check resetVelocityCountdown so as to only begin decay once.
		else if(this->_isActive && a && abs(_envelopeVolume - 1.0) < ε_adsr && _decayDuration > ε_adsr && _resetVelocityCountdown == -1) {
			double decayForSamples = _decayDuration * f_sample;
			_envelopeVelocity = (_sustainVolume - 1.0) / decayForSamples;
			_resetVelocityCountdown = round(decayForSamples);
		}
		
		this->_isActive = a;
	}
	
	double volume() override {
		// return the calculated volume rather than the provided target volume.
		// scale envelope volume by user-provided volume as our ceiling.
		return _envelopeVolume * _smoothedTargetVolume;
	}
	
	void volume(double v) override {
		this->_targetVolume = v;
	}
	
	std::shared_ptr<Generator> innerGenerator() {
		return this->_innerGenerator;
	}
};

#endif /* EnvelopeGenerator_h */
