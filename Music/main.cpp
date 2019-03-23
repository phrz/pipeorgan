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
#include "TrigGenerator.hpp"
#include "DancingMad.h"
#include "SimpleTestSong.h"
#include "SineWaveGenerator.h"
#include "PipeOrganGenerator.h"
#include "VibratoFilter.h"

using namespace std;

// [CITE] http://subsynth.sourceforge.net/midinote2freq.html
frequency_t midiNumberToFrequency(midi_t midiNumber) {
	return (CONCERT_A / 32.) * pow(2., (midiNumber - 9) / 12.);
}

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
	size_t const N_VOICES = N_MIDI_CODES;
	
	set<midi_t> currentNotes {};
	
	vector<midi_t> commands {};
	vector<PipeOrganGenerator> voices {};
	
	double const baselineVolume = 0.25; // arbitrary, avoids overflow
	
	std::array<double, N_DRAWBARS> drawbarSettings {
		0,7, 8,1,2,0, 0,0,0 // Bassoon 8'
	};
	for(size_t i = 0; i < N_VOICES; ++i) {
		voices.emplace_back(drawbarSettings);
		voices[i].volume(baselineVolume);
		voices[i].frequency(midiNumberToFrequency(MIN_MIDI_CODE + i));
	}
	
	// for testing a simple tone
//	voices[0].activate(true);
//	voices[0].frequency(440.0);
//	for(size_t i = 0; i < 44010; ++i) {
//		printSample(amplitudeToSample(voices[0].next()));
//	}
//	return 0;
	
	for(auto [delta, commands]: simpleTestEvents) {
		// first: we play the current notes for the duration of
		// delta * samples per tick.
		for(timecode_t i = 0; i < delta * SAMPLES_PER_TICK; ++i) {
			amplitude_t sum_a {0.0};
			
			for(midi_t note_i = 0; note_i < N_VOICES; note_i++) {
				midi_t m = MIN_MIDI_CODE + note_i;
				// active notes
				if(currentNotes.count(m) == 1) {
					voices[note_i].activate(true);
					amplitude_t a = voices[note_i].next();
					sum_a += a;
					note_i++;
				}
				// dead notes
				else {
					voices[note_i].activate(false);
					amplitude_t a = voices[note_i].next();
					sum_a += a;
				}
			}
			
			sample_t s = amplitudeToSample(sum_a);
			printSample(s);
		}
		// next: we account for new notes
		for(auto command : commands) {
			if(command > 0) {
				currentNotes.insert(command);
			} else {
				currentNotes.erase(-1 * command);
			}
		}
	}
}
