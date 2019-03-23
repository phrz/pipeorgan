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
	size_t _samplesInActiveState {0U};
	size_t _samplesInReleaseState {0U};
	
	amplitude_t _nextWithoutFilters() override {
		// activate whether this note is active or we're in release
		_innerGenerator->activate(this->isActive());
		
		// where the magic happens
		_innerGenerator->volume(this->volume());
		
		amplitude_t a = _innerGenerator->next();
		
		if(_isActive) _samplesInActiveState++;
		// always increment to allow release and attack to blend
		_samplesInReleaseState++;
		
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
			// leave release active to allow attack+release blending
			// _isReleaseActive = false;
			_samplesInActiveState = 0U;
		}
		// switching from on to off
		else if(this->_isActive && !a) {
			 _isReleaseActive = true;
			_samplesInReleaseState = 0U;
		}
		
		this->_isActive = a;
	}
	
	double volume() override {
		// return the calculated volume rather than the provided target volume.
		frequency_t const f_sample = static_cast<double>(SAMPLE_RATE);
		
		double const timeInRelease = static_cast<double>(_samplesInReleaseState) / f_sample;
		
		double attackFactor {0.0}, releaseFactor {0.0};
		
		// ATTACK / SUSTAIN
		if(_isActive) {
			if(_attackDuration < ε_adsr) {
				attackFactor = 1.0;
			} else {
				double const timeInActive = static_cast<double>(_samplesInActiveState) / f_sample;
				// avoid branching timeInActive > attackDuration with saturate
				attackFactor = saturate(timeInActive / _attackDuration);
			}
		}
		// RELEASE
		if(_releaseDuration > ε_adsr && timeInRelease < _releaseDuration) {
			// we don't worry about turning off isReleaseActive so that release
			// can blend with attack when _isActive changes to true during
			// release phase. We DO worry about isReleaseActive for other reasons.
			releaseFactor = 1.0 - timeInRelease / _releaseDuration;
			// no saturate: releaseDuration guaranteed greater than timeInRelease
		} else {
			// we ONLY set this to keep track of keeping inner generator on/off.
			_isReleaseActive = false;
		}
		
		// finding the max prevents a newly played note from beginning at
		// zero volume (attack) during the release of a prior note, which
		// causes clicking due to drop out and does not sound natural.
		return clamp(_targetVolume * fmax(attackFactor, releaseFactor), 0.0, _targetVolume);
	}
	
	void volume(double v) override {
		this->_targetVolume = v;
	}
	
	std::shared_ptr<Generator> innerGenerator() {
		return this->_innerGenerator;
	}
};

#endif /* EnvelopeGenerator_h */
