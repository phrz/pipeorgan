//
//  main.cpp
//  Music
//
//  Created by Paul Herz on 2019-03-10.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

// ./music | mplayer -cache 2048 -really-quiet -rawaudio samplesize=2:channels=1:rate=22050 -demuxer rawaudio -


#include<vector>
#include<iostream>
#include<map>
#include<set>
#include <cmath>
#include <array>

#include "config.h"
#include "DancingMad.h"
#include "PipeOrgan.h"

using namespace std;

void printSample(sample_t t) {
	for(size_t i = 0; i < SAMPLE_T_BYTES; ++i) {
		putchar((t >> 8*i) & 0xFF);
	}
}

sample_t amplitudeToSample(amplitude_t a) {
//	sample_t s2 = SAMPLE_T_MAX / 2;
//	return a * s2 + s2;
	return a * SAMPLE_T_MAX;
}

int main() {	
	PipeOrgan organ {
		//{0,7, 8,1,2,0, 0,0,0} // Bassoon 8' (used .4/.1 attack/release)
		{0,0, 8,8,8,0, 0,0,0}, // "classic" Jimmy Smith (harpsicord .2/.04 a/r)
		// A D S R envelope
		.1,.05,.1,.02
	};
	
	double const baselineVolume = 0.25; // arbitrary, avoids overflow
	
	timecode_t lastTick = 0U;
	for(auto [tick, commands]: dancingMadEvents) {
		// first: we play the current notes for the duration of
		// delta * samples per tick.
		timecode_t delta = tick - lastTick;
		lastTick = tick;
		for(timecode_t i = 0; i < delta * SAMPLES_PER_TICK; ++i) {
			printSample(
				amplitudeToSample(
					applyVolume(
						organ.next(), baselineVolume)));
		}
		// next: we account for new notes
		for(auto command : commands) {
			if(command > 0) {
				organ.setKey(command, true);
			} else {
				organ.setKey(-1 * command, false);
			}
		}
	}
}
