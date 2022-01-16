# Grass Audio

A gapless audio player class for node, it uses the bass library as engine.
**NOTE: this library is on early development**

## Get started

**NOTE: You will need a c++20 compiler. For now ,available targets are win_x64**

Install by running

```bash
npm install grass-audio
```

Import the module

```js
import GrassAudio from "grass-audio";
```

Now you can instantiate the class with, specifying a file path array

```js
const audioPlayer = new GrassAudio(["path1", "path2", "..."]);
```

## Api reference

please check the index.d.ts file