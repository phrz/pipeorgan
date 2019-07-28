default: build

build:
	g++ -O3 -std=c++1z src/main.cpp -I src/ -I src/Filters -I src/Generators -o bin/organ

rawaudio: build
	bin/organ > output/organ.pcm

audio: rawaudio
	ffmpeg -y -f s16le -ar 22050 -ac 1 -i output/organ.pcm output/organ.wav

audacity: audio
	open -a Audacity output/organ.wav
