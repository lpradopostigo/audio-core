const testAddon = require('./build/Release/audio_player.node');
const audio = new testAddon.AudioPlayerWrapper()
audio.setFile("1.wav")

audio.play()
setTimeout(function () {
    audio.pause()
    console.log('pause')
}, 2000)

setTimeout(function () {
    audio.play()
    console.log('resume')
    audio.setPosition(100)
    console.log(audio.getPosition())

}, 4000)


setInterval(() => {
}, 1 << 30);
