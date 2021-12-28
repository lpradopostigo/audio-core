const GrassAudio = require('./build/Debug/grass_audio.node').GrassAudio
const { readFile } = require('fs/promises')
const audio = new GrassAudio()

;(async () => {
  const file = await readFile('1.wav')
  audio.setFileFromMemory(file)
  audio.play()
  let x = 0
  const listener = audio.on('positionReached', () => {
    console.log("js")
    x=x+1
    console.log(x)
  }, 5)

  setTimeout(() => {
    audio.setPosition(0)
  }, 8000)


  setInterval(() => {}, 1 << 30)

})()

