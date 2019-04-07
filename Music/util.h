//
//  util.h
//  Music
//
//  Created by Paul Herz on 2019-03-16.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef util_h
#define util_h

#include <cmath>
#include <algorithm>

// ensures equivalency between radian measurements
// by making them the modulus of tau, such that
// equivalent angles are equal numbers.
template<typename T>
T radians(T r) {
	return fmod(r, τ);
}

// clamp the value to within the Lower Limit (ll)
// and the Upper Limit (ll) inclusive.
template<typename T>
T clamp(T x, T ll, T ul) {
	return std::max(ll, std::min(ul, x));
}

template<typename T>
T saturate(T x) {
	return clamp(x, 0.0, 1.0);
}

// linearly interpolate between the values `a` and `b`.
// The point on the linear interpolation is determined
// by `t`, a value between 0.0 and 1.0 where t=0.0 is
// 'output = a' and t=1.0 is 'output = b'.
template<typename T>
T lerp(T a, T b, T _t) {
	// clamp t for safety (ensuring output is always in range a..b)
	T t = clamp(_t, 0.0, 1.0);
	return a + t * (b - a);
}

amplitude_t applyVolume(amplitude_t a, double v) {
	double _v = saturate(v);
	return a * _v * _v;
}

// [CITE] http://subsynth.sourceforge.net/midinote2freq.html
frequency_t midiNumberToFrequency(midi_t midiNumber) {
	return (CONCERT_A / 32.) * pow(2., (midiNumber - 9) / 12.);
}

#endif /* util_h */
