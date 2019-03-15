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

using namespace std;

// [CITE] http://subsynth.sourceforge.net/midinote2freq.html
frequency_t midiNumberToFrequency(midi_t midiNumber) {
	return (CONCERT_A / 32.) * pow(2., (midiNumber - 9) / 12.);
}

void printSample(sample_t t) {
//	putchar(t & 0xFF);
//	putchar(t >> 8);
	for(size_t i = 0; i < SAMPLE_T_BYTES; ++i) {
		putchar((t >> 8*i) & 0xFF);
	}
}

sample_t set_volume(sample_t s, double volume) {
	if(volume == 1.0) return s;
	s = volume * volume * ((double)s - SAMPLE_T_ZERO_POINT);
	// bring the center of the waveform back to the middle
	return s - (0.5 * volume * volume * (SAMPLE_T_MAX - SAMPLE_T_ZERO_POINT));
}

sample_t normalize(amplitude_t a) {
	return (a + 1.0) * (SAMPLE_T_MAX >> 1);
}

int main() {
	size_t const N_VOICES = 20;
	
	set<midi_t> currentNotes {};
	vector<frequency_t> currentFrequencies {};
	
	vector<midi_t> commands {};
	timecode_t sample_i {0U};
	vector<TrigGenerator> voices {};
	array<frequency_t, N_VOICES> voicesLastFrequencies {-1.0}; // -1 == off
	array<bool, N_VOICES> voicePlayedLastSample {false};
	array<bool, N_VOICES> persistVoiceUntilZero {false};
	
	for(size_t i = 0; i < N_VOICES; ++i) {
		voices.emplace_back(SAMPLE_RATE);
	}
	
	for(timecode_t tick = 0; tick < MAX_TICK; tick++) {
		try {
			commands = dancingMadEvents.at(tick);
		} catch(out_of_range e) {
			commands.clear();
		}
		// `commands` now contains the commands for this tick,
		// though it may be empty. A positive number is a new note,
		// and a negative number denotes that the positive equivalent
		// should be removed.
		for(midi_t command : commands) {
			if(command > 0) {
				currentNotes.insert(command);
			} else {
				currentNotes.erase(-1 * command);
			}
		}
		
		// now that currentNotes reflects the notes being played
		// this tick, calculate `s`, the integer representing the sample value
		// at this second.
		double const desired_note_volume = 0.25; // arbitrary, avoids overflow
		
		currentFrequencies.clear();
		for(midi_t midiNote : currentNotes) {
			frequency_t fundamental = midiNumberToFrequency(midiNote);
			currentFrequencies.push_back(fundamental);
		}
		
		for(timecode_t i = 0; i < SAMPLES_PER_TICK; ++i) {
			sample_t s {0U};
			size_t note_i {0U};
			// deal with active voices (corresponding to active notes)
			for(frequency_t hz : currentFrequencies) {
				voicesLastFrequencies[note_i] = hz;
				voicePlayedLastSample[note_i] = true;
				amplitude_t a = voices[note_i].sine(sample_i, hz);
				s += set_volume(normalize(a), desired_note_volume);
				note_i++;
			}
			// deal with dead voices (the remainder)
			for(size_t n = note_i; n < N_VOICES; ++n) {
				// the note in the note set corresponding to this #
				// voice is not playing, but this voice should continue
				// playing until its wave reaches near zero to avoid pops.
				frequency_t hz = voicesLastFrequencies[n];
				amplitude_t a = voices[n].sine(sample_i, hz);
				
				if(voicePlayedLastSample[n] && fabs(a) > AMPLITUDE_EPSILON) {
					persistVoiceUntilZero[n] = true;
				}
				
				if(persistVoiceUntilZero[n]) {
					if(fabs(a) <= AMPLITUDE_EPSILON) persistVoiceUntilZero[n] = false;
					else s += set_volume(normalize(a), desired_note_volume);
				} else {
					voices[n].resetPhase(sample_i + 1);
				}
				
				voicePlayedLastSample[n] = false;
			}
			printSample(s);
			sample_i++;
		}
	}
}
