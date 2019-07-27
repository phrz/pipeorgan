#  Music

```bash
xcodebuild && /<BUILD_PATH>/Build/Products/Debug/Music > ~/Desktop/org.pcm && ffmpeg -y -f s16le -ar 22050 -ac 1 -i ~/Desktop/org.pcm ~/Desktop/org.wav && open -a Audacity ~/Desktop/org.wav
```
