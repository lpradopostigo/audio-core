const path = require("path");
const { GrassAudio } = require("./build/Release/grass_audio.node");

GrassAudio.setupEnvironment(path.join(__dirname, "build/Release"));

module.exports = GrassAudio;
