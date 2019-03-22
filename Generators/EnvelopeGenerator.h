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
	
	bool _isReleaseActive {false};
	// keeps track of point in release period
	// OR point in note activated period
	size_t _samplesInNoteState {0U};
	
	amplitude_t _nextWithoutFilters() override {
		// activate whether this note is active or we're in release
		_innerGenerator->activate(this->isActive());
		
		// where the magic happens
		_innerGenerator->volume(this->volume());
		
		amplitude_t a = _innerGenerator->next();
		
		if(_isReleaseActive || _isActive) _samplesInNoteState++;
		
		return a;
	}
public:
	EnvelopeGenerator(Args&&... args): _innerGenerator(std::make_shared<Generator>(args...)) {
		static_assert(
			std::is_base_of<SoundGenerator, Generator>::value,
			"Generator not derived from SoundGenerator");
	}
	
	void attackDuration(double a) {
		this->_attackDuration = clamp(a, 0.0, 60.0);
	}
	
	void releaseDuration(double r) {
		this->_releaseDuration = clamp(r, 0.0, 60.0);
	}
	
	bool isActive() override {
		// return whether we are active OR in the release phase of the
		// envelope, determined by the time since deactivation being less
		// than the provided release duration.
		return this->_isActive || this->_isReleaseActive;
	}
	
	virtual void activate(bool a) override {
		// on activation, make note to terminate release.
		// on deactivation, make note to begin release, to begin downramping
		// volume on the inner generator, and report continued activation until
		// zero is reached.
		
		// switching from off to on
		if(!this->_isActive && a) {
			_isReleaseActive = false;
			_samplesInNoteState = 0U;
		}
		// switching from on to off
		else if(this->_isActive && !a) {
			_isReleaseActive = true;
			_samplesInNoteState = 0U;
		}
		
		this->_isActive = a;
	}
	
	double volume() override {
		// return the calculated volume rather than the provided target volume.
		frequency_t const f_sample = static_cast<double>(SAMPLE_RATE);
		
		// time so far into either note active state, or release state.
		double const timeInState = static_cast<double>(_samplesInNoteState) / f_sample;
		
		if(_isActive) {
			double v = _targetVolume;
			
			// ATTACK
			double attackFactor = 1.0;
			// floating point safety
			if(timeInState < _attackDuration && _attackDuration > ε_adsr) {
				attackFactor = saturate(timeInState / _attackDuration);
			}
			v *= attackFactor;
			
			// DECAY: TODO
			
			// SUSTAIN (default)
			return v;
		}
		// RELEASE
		else if(_isReleaseActive && _releaseDuration > ε_adsr) {
			// seconds into release
			
			if(timeInState > _releaseDuration) {
				_isReleaseActive = false;
				return 0.0;
			}
			// reduces from 1.0 to 0.0 as end of release phase approaches
			double const releaseFactor = 1.0 - saturate(timeInState / _releaseDuration);
			return clamp(_targetVolume * releaseFactor, 0.0, _targetVolume);
		}
		
		return 0.0;
	}
	
	void volume(double v) override {
		this->_targetVolume = v;
	}
	
	std::shared_ptr<Generator> innerGenerator() {
		return this->_innerGenerator;
	}
};

#endif /* EnvelopeGenerator_h */
