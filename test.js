const GrassAudio = require("./build/Debug/grass_audio.node").GrassAudio;
const audio = new GrassAudio(["gapless2.wav", "gapless3.wav"]);

(async () => {
  audio.play();
  audio.setPosition(320);
  // let x = 0
  // const listener = audio.once('positionReached', () => {
  //   console.log("js")
  //   x=x+1
  //   console.log(x)
  // }, 3)
  //
  audio.addListener("end", () => console.log("js"), false);

  setTimeout(() => {
    // audio.next()
    // audio.setPosition(0)
  }, 4000);
  // setTimeout(() => {
  //   audio.play()
  //   // audio.setPosition(0)
  // }, 10000)

  //

  setInterval(() => {}, 1 << 30);
})();
