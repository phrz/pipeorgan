//
//  PipeOrganGenerator.h
//  Music
//
//  Created by Paul Herz on 2019-03-16.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef PipeOrganGenerator_h
#define PipeOrganGenerator_h

#include <array>
#include <memory>

#include "SoundGenerator.h"
#include "SimpleSineWaveGenerator.h"
#include "EnvelopeGenerator.h"
#include "VibratoFilter.h"
#include "util.h"

class PipeOrganGenerator: public VariableFrequencySoundGenerator {
protected:
	constexpr static std::array<double, N_DRAWBARS> const _drawbarHarmonics {
		0.5, 1.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0
	};
	using SineEnvelope = EnvelopeGenerator<SimpleSineWaveGenerator>;
	std::array<SineEnvelope, N_DRAWBARS> _drawbars {SineEnvelope()};
	std::array<std::shared_ptr<VibratoFilter>, N_DRAWBARS> _drawbarVibratos {};
public:
	PipeOrganGenerator(std::array<double, N_DRAWBARS> const& dbs) {
		for(size_t i = 0; i < N_DRAWBARS; ++i) {
			_drawbarVibratos[i] = std::make_shared<VibratoFilter>();
			_drawbarVibratos[i]->intensity = 5.0;
			_drawbars[i].filters.push_back(_drawbarVibratos[i]);
			
			_drawbars[i].releaseDuration(0.25);
			_drawbars[i].attackDuration(0.1);
			
			_drawbars[i].volume(clamp(dbs[i], 0.0, 8.0) / 8.0);
		}
	}
	
	amplitude_t _nextWithoutFilters() override {
		frequency_t f_fund {this->frequency()};
		amplitude_t sum_a {0.0};
		
		for(size_t i = 0; i < N_DRAWBARS; ++i) {
			_drawbars[i].innerGenerator()->frequency(f_fund * _drawbarHarmonics[i]);
			_drawbarVibratos[i]->rate = f_fund * _drawbarHarmonics[i] / 50.0;
			_drawbars[i].activate(this->isActive());
			sum_a += _drawbars[i].next();
		}
		
		return applyVolume(sum_a, this->volume());
	}
};

#endif /* PipeOrganGenerator_h */
