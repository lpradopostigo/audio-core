# Grass Audio
A library that allows you to play audio files without gaps and pain.
**NOTE: this library is on development, some bugs may arise 😱**

## Get started
**NOTE: You will need at least a c++20 compiler. For now ,available targets are win_x64**

Install by running

```bash
npm install grass-audio
```

## Usage
```js
import GrassAudio from "grass-audio";
const audioPlayer = new GrassAudio();
audioPlayer.setFiles([path1, path2, ...])
audioPlayer.play()
```

## Api reference
please check the index.d.ts file
