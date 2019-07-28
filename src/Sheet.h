//
//  Sheet.h
//  Music
//
//  Created by Paul Herz on 2019-03-14.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef Sheet_h
#define Sheet_h

#include "config.h"
#include <vector>
#include <utility>
#include <stdint.h>
#include <string>
#include <iostream>

using namespace std::literals::string_literals;

struct Note;
using Action = std::pair<Note, double>; // duration

struct Sheet {
	std::vector<Action> instructions;
};

struct Note {
	enum Tone { A = 0, B, C, D, E, F, G, _Rest };
	Tone tone;
	uint8_t octave;
	
	Note(Tone t, uint8_t o): tone(t), octave(o) {
		if((o == 8 && t != C) || o > 8) {
			std::cerr << "Warning: note above C8 specified (" << this->str()
			<< "). Clamping to C8." << std::endl;
			t = C;
			o = 8;
		}
	}
	
	static Note const rest;
	
	bool isSilent() {
		return this->tone == _Rest;
	}
	
	std::string str() {
		if(isSilent()) {
			return "rest"s;
		}
		std::string s {};
		s += "ABCDEFG"[this->tone];
		s = s.append(std::to_string(this->octave));
		return s;
	}
};

Note const Note::rest { _Rest, 0 };

#endif /* Sheet_h */
