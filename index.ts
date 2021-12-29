export interface GrassAudio {
  new (): GrassAudio;

  setFile(path: string): void;

  setFileFromMemory(buffer: Buffer): void;

  play(): void;

  pause(): void;

  stop(): void;

  setPosition(position: number): void;

  getPosition(): number;

  setVolume(volume: number): void;

  on(event: string, callback: () => void, ...args: any): number;

  once(event: string, callback: () => void, ...args: any): number;
}

module.exports = require("./bin/win/x64/grass_audio.node")
  .GrassAudio as GrassAudio;
