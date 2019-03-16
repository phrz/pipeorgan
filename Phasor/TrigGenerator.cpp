//
//  TrigGenerator.cpp
//  Music
//
//  Created by Paul Herz on 2019-03-10.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#include "TrigGenerator.hpp"

TrigGenerator::TrigGenerator(frequency_t sampleRate): sampleRate(sampleRate) {}
TrigGenerator::TrigGenerator(): sampleRate((double)SAMPLE_RATE) {}

amplitude_t TrigGenerator::sine(timecode_t t2, frequency_t f) {
	// determine how many samples we have to advance
	// typically this is 1: >1 implies time dilation
	timecode_t delta_t = t2 - this->t;
	
	// compute the angular frequency of the oscilator
	// in radians
	frequency_t omega = τ * f / this->sampleRate;
	
	// compute the complex angular coefficient
	// cmplx.Exp(complex(0, ω))
	auto bigOmega = std::exp(std::complex<double>(0, omega));
	
	// advance the phasor Δt units
	for(timecode_t i = delta_t; i > 0; --i) {
		this->z *= bigOmega;
	}
	
	// stabilize the phasor's amplitude every once in a while
	// the amplitude can drift due to rounding errors
	// since z is a unity phasor, adjust its amplitude back towards unity
	if(this->s > STABILIZATION_PERIOD) {
		double a, b, c;
		a = this->z.real();
		b = this->z.imag();
		c = (3 - std::pow(a, 2) - std::pow(b, 2)) / 2;
		this->z *= std::complex<double>(c, 0);
		this->s = 0;
	}
	
	// advance time
	this->t += delta_t;
	this->s += delta_t;
	// return the 'sine' component of the phasor
	return this->z.imag();
}

void TrigGenerator::resetPhase(timecode_t sample_i) {
	this->z = std::complex<double>(1.0, 0.0);
	this->t = sample_i;
	this->s = sample_i;
}
