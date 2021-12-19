const testAddon = require('./build/Release/audio_player.node');
const audio = new testAddon.AudioPlayerWrapper()
audio.setFile("1.wav")

audio.play()
setTimeout(function () {
    audio.pause()
    console.log('pause')
}, 5000)

setTimeout(function () {
    audio.play()
    console.log('resume')
}, 10000)


setInterval(() => {}, 1 << 30);
