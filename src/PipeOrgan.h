//
//  PipeOrgan.h
//  Music
//
//  Created by Paul Herz on 2019-04-07.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef PipeOrgan_h
#define PipeOrgan_h

#include <memory>
#include <map>
#include "SoundGenerator.h"
#include "EnvelopeGenerator.h"
#include "SimpleSineWaveGenerator.h"
#include "util.h"

// the following drawbar harmonics:
// [0.5, 1.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0]
// correspond to the following MIDI number deltas:
// [-12, 7, 0, 12, 19, 24, 28, 31, 36]

class PipeOrgan {
private:
	using _SineEnvelope = EnvelopeGenerator<SimpleSineWaveGenerator>;
	using _Pipe = std::shared_ptr<_SineEnvelope>;
	
	double const _organAttackDuration;
	double const _organDecayDuration;
	double const _organSustainVolume;
	double const _organReleaseDuration;
	
	_Pipe makePipe(frequency_t f) {
		auto pipe = std::make_shared<_SineEnvelope>();
		// this pipe will only ever have one frequency.
		pipe->innerGenerator()->frequency(f);
		
		// set the pipe's ADSR envelope based on the organ's global envelope.
		pipe->attackDuration(_organReleaseDuration);
		pipe->decayDuration(_organDecayDuration);
		pipe->sustainVolume(_organSustainVolume);
		pipe->releaseDuration(_organAttackDuration);
		
		return pipe;
	}
	
	constexpr static std::array<midi_t, N_DRAWBARS> const
		_drawbarOffsets {-12, 7, 0, 12, 19, 24, 28, 31, 36};
	
	std::array<double, N_DRAWBARS> _drawbarVolumes {0.0};
	
	std::map<midi_t, _Pipe> _pipes {};
	
	// for each midi key, collect volume amounts.
	// if a key is played, add 1.0 to its volume.
	// if it's a harmonic for a different key, add the appropriate fraction.
	// when unpressing a key or removing a harmonic, just subtract.
	std::map<midi_t, double> _pipeSumVolumes {};
	
	// we need to track if a key is directly active (being played)
	// to distinguish it being active as part of being a harmonic
	std::map<midi_t, bool> _keysActive {};
	
	// a hidden volume parameter to normalize total organ volume based on
	// the theoretical loudest the organ can be given its drawbar settings
	double _drawbarCompensationVolume {1.0};
public:
	PipeOrgan(
		std::array<double, N_DRAWBARS> const dvs, // drawbar volumes 0.0-8.0
		double a,
		double d,
		double s,
		double r
	):
		_organAttackDuration(a),
		_organDecayDuration(d),
		_organSustainVolume(s),
		_organReleaseDuration(r)
	{
		double drawbarVolumesSum {0.0};
		for(size_t i = 0; i < N_DRAWBARS; i++) {
			_drawbarVolumes[i] = saturate(dvs[i] / 8.0);
			drawbarVolumesSum += _drawbarVolumes[i];
		}
		_drawbarCompensationVolume = saturate(1.0 / drawbarVolumesSum);
		
		// these midi codes are not compliant (not between 21-108 inclusive)
		// but are used to represent subharmonics and harmonics outside the
		// midi defined range.
		for(midi_t m = MIN_ORGAN_MIDI_CODE; m <= MAX_ORGAN_MIDI_CODE; m++) {
			_pipes[m] = makePipe(midiNumberToFrequency(m));
			_pipeSumVolumes[m] = 0.0;
			_keysActive[m] = false;
		}
	}
	
	amplitude_t next() {
		amplitude_t a {0.0};
		for(midi_t m = MIN_ORGAN_MIDI_CODE; m <= MAX_ORGAN_MIDI_CODE; m++) {
			a += _pipes[m]->next();
		}
		return applyVolume(a, _drawbarCompensationVolume);
	}
	
	void setKey(midi_t m, bool active) {
		double volumeFactor {0.0};
		// only care if key is changing state, on->on off->off unimportant.
		if(_keysActive[m] && !active) {
			// switching key off
			volumeFactor = -1.0;
		} else if(!_keysActive[m] && active) {
			// switching key on
			volumeFactor = 1.0;
		} else {
			return;
		}
		_keysActive[m] = active;
		
		for(size_t i = 0; i < N_DRAWBARS; i++) {
			midi_t m_pipe = m + _drawbarOffsets[i];
			if(_pipes.count(m_pipe) == 0) continue;
			// volumeFactor decides whether we're adding or subtracting from
			// the pipe's volume to represent either a full key press or a
			// fractional volume increase due to a harmonic (drawbar).
			_pipeSumVolumes[m_pipe] += _drawbarVolumes[i] * volumeFactor;
			
			double pipeVolume = saturate(_pipeSumVolumes[m_pipe]);
			
			// rather than set volume to zero,
			// activate and deactivate to allow for
			// anti-pop measures like zero-approach stuff in SineWaveGenerator
			// and release in EnvelopeGenerator (depending on what type
			// _Pipe is and what filters are applied).
			if(pipeVolume < 0.125) {
				_pipes[m_pipe]->activate(false);
			} else {
				_pipes[m_pipe]->activate(true);
				_pipes[m_pipe]->volume(pipeVolume);
			}
		}
	}
};

#endif /* PipeOrgan_h */
