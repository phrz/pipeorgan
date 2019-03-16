//
//  SoundFilter.h
//  Music
//
//  Created by Paul Herz on 2019-03-16.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef SoundFilter_h
#define SoundFilter_h

class SoundFilter {
public:
	// before the generator generates the raw amplitude (which is processed
	// through each filter with processAmplitude) the generator passes its
	// target (user-provided) frequency through all the filters.
	virtual amplitude_t modulateFrequency(frequency_t input) { return input; } // null filter
	
	// each filter is provided the amplitude output of the previous filter,
	// or of the raw generator if it is the first filter.
	virtual amplitude_t modulateAmplitude(amplitude_t input) { return input; }; // null filter
};

#endif /* SoundFilter_h */
