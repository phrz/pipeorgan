//
//  SoundGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-03-15.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef SoundGenerator_h
#define SoundGenerator_h

#include <vector>

class SoundFilter {
public:
	virtual amplitude_t next(amplitude_t input) { return input; }; // null filter
};

class SoundGenerator {
protected:
	// "active" is equivalent to "piano key being pressed down"
	// aka sound can persist beyond deactivation for various reasons.
	bool _isActive {false};
	double _targetVolume {1.0}; // the volume level requested by the user.
	virtual amplitude_t _nextWithoutFilters() = 0;
public:
	std::vector<SoundFilter> filters {};
	// wraps the custom _nextWithoutFilters and applies all filters
	amplitude_t next() {
		amplitude_t a = _nextWithoutFilters();
		for(auto filter: filters) {
			a = filter.next(a);
		}
		return a;
	}
	
	bool isActive() { return this->_isActive; }
	void activate(bool a) { this->_isActive = a; }
	
	double volume() { return this->_targetVolume; }
	void volume(double v) { this->_targetVolume = v; }
};

class VariableFrequencySoundGenerator: public SoundGenerator {
protected:
	frequency_t _targetFrequency {CONCERT_A}; // the target frequency
public:
	frequency_t frequency() { return this->_targetFrequency; }
	void frequency(frequency_t f) { this->_targetFrequency = abs(f); }
};

#endif /* SoundGenerator_h */
