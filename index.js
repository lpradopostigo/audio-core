const addonPath = "./build/Release";
const path = require("path");
const { GrassAudio } = require(path.resolve(addonPath, "grass_audio.node"));

GrassAudio.setupEnvironment(path.resolve(addonPath));

module.exports = GrassAudio;
