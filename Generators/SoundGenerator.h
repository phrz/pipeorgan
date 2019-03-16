//
//  SoundGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-03-15.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef SoundGenerator_h
#define SoundGenerator_h

class SoundGenerator {
protected:
	// "active" is equivalent to "piano key being pressed down"
	// aka sound can persist beyond deactivation for various reasons.
	bool _isActive {false};
public:
	virtual amplitude_t next() = 0;
	
	bool isActive() { return this->_isActive; }
	void activate(bool a) { this->_isActive = a; }
};

class VariableFrequencySoundGenerator: public SoundGenerator {
protected:
	frequency_t _targetFrequency {CONCERT_A}; // the target frequency
public:
	frequency_t frequency() { return this->_targetFrequency; }
	void frequency(frequency_t f) { this->_targetFrequency = abs(f); }
};

#endif /* SoundGenerator_h */
