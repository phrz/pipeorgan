//
//  PipeOrganGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-03-16.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef PipeOrganGenerator_h
#define PipeOrganGenerator_h

#include<array>

#include "SoundGenerator.h"
#include "SineWaveGenerator.h"
#include "VibratoFilter.h"
#include "util.h"

class PipeOrganGenerator: public VariableFrequencySoundGenerator {
protected:
	constexpr static std::array<double, N_DRAWBARS> const _drawbarHarmonics {
		0.5, 1.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0
	};
	std::array<SineWaveGenerator, N_DRAWBARS> _drawbars {{}};
public:
	PipeOrganGenerator(std::array<double, N_DRAWBARS> const& dbs) {
		for(size_t i = 0; i < N_DRAWBARS; ++i) {
			_drawbars[i].volume(clamp(dbs[i], 0.0, 8.0) / 8.0);
		}
	}
	
	amplitude_t _nextWithoutFilters() override {
		frequency_t f_fund {this->frequency()};
		amplitude_t sum_a {0.0};
		
		for(size_t i = 0; i < N_DRAWBARS; ++i) {
			_drawbars[i].frequency(f_fund * _drawbarHarmonics[i]);
			_drawbars[i].activate(this->isActive());
			sum_a += _drawbars[i].next();
		}
		
		return applyVolume(sum_a, this->volume());
	}
};

#endif /* PipeOrganGenerator_h */
