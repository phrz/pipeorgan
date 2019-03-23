//
//  SimpleTestSong.h
//  Music
//
//  Created by Paul Herz on 2019-03-23.
//  Copyright © 2019 Paul Herz. All rights reserved.
//

#ifndef SimpleTestSong_h
#define SimpleTestSong_h

static std::vector<std::pair<timecode_t, std::vector<midi_t>>> const simpleTestEvents {
	{0,{60}},
	{5,{-60}},
	{5,{60}},
	{5,{-60}},
	{5,{60}},
	{5,{-60}},
	{5,{60}},
	{5,{-60}}
};

#endif /* SimpleTestSong_h */
