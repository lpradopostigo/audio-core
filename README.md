# Grass Audio
An audio player class for node, it uses the bass library as engine.
**NOTE: this library is on early development**

## Get started
Install by running
```bash
npm install grass-audio
```
Import the module
```js
import { GrassAudio } from 'grass-audio'
```
Now you can instantiate the class with
```js
const audioPlayer = new GrassAudio(); 
```
With that you can now use the different methods

## Reference

| Method                    | Description                                                                              |
|---------------------------|------------------------------------------------------------------------------------------|
| setFile(path)             | load the file from the given path, can block the main thread                             |
| setFileFromMemory(buffer) | load the file from a node Buffer, this is the recommended way to load files              |
| play()                    | play the loaded file                                                                     |
| pause()                   | pause the playback                                                                       |
| stop()                    | stop the playback, the file will remain set                                              |
| setPosition(position)     | set the playback position, the position is given in seconds                              |
| getPosition()             | get the current playback position                                                        |
| setVolume()               | set the volume, valid values are from 0 to 1, values greater than one will be set to one |
| on()                      | set an event listener, valid events are 'end', 'positionReached' and 'positionSet'       |
| once()                    | same as on() but it will remove the listener after the callback is called                |

