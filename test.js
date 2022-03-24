const GrassAudio = require("./index");
const filePaths = ["gapless2.wav", "gapless3.wav"];
const audio = new GrassAudio();

(async () => {
  audio.setFiles(filePaths);
  audio.play();
  audio.seek(320);
  console.log(audio.getState());
  // let x = 0
  // const listener = audio.once('positionReached', () => {
  //   console.log("js")
  //   x=x+1
  //   console.log(x)
  // }, 3)
  //
  audio.addListener("end", () => console.log("js"));

  setTimeout(() => {
    // audio.next()
    // audio.setPosition(0)
    audio.setFiles(filePaths);
    audio.play();
  }, 8000);
  // setTimeout(() => {
  //   audio.play()
  //   // audio.setPosition(0)
  // }, 10000)

  //

  setInterval(() => {}, 1 << 30);
})();
