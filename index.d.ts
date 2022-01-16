declare module "grass-audio" {
  class GrassAudio {
    constructor(files: string[]);
    play(): void;
    pause(): void;
    previous(): void;
    next(): void;
    stop(): void;
    skipToFile(index: number): void;
    setPosition(position: number): void;
    getPosition(): number;
    getCurrentFileIndex(): number;
    setVolume(volume: number): void;
    addListener(
      event: "end" | "positionReached",
      callback: () => void,
      removeOnTrigger?: boolean,
      ...args: any
    ): number;
    removeListener(listener: number);
  }
  export = GrassAudio;
}
