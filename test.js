const GrassAudio = require("./build/Release/grass_audio.node").GrassAudio;
const {readFile} = require("fs/promises")
const audio = new GrassAudio();

(async () => {
  const file = await readFile("1.wav")
  audio.setFileFromMemory(file);
  audio.play();
  setTimeout(function () {
    audio.pause();
    console.log("pause");
  }, 2000);

  setTimeout(function () {
    audio.play();
    console.log("resume");
    audio.setPosition(100);
    console.log(audio.getPosition());
  }, 4000);

})()


setInterval(() => {
}, 1 << 30);
