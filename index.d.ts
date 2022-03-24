declare module "grass-audio" {
  class GrassAudio {
    constructor();

    play(): void;

    pause(): void;

    stop(): void;

    skipToIndex(index: number): void;

    previous(): void;

    next(): void;

    seek(position: number): void;

    setFiles(files: string[]);

    setVolume(volume: number): void;

    addListener(
      event: "end" | "positionReached",
      callback: () => void,
      removeOnTrigger?: boolean,
      ...args: any
    ): number;

    removeListener(listener: number);

    getState(): {
      playbackState: "playing" | "stopped" | "paused";
      fileDuration: number;
      filePosition: number;
      fileIndex: number;
    };
  }

  export = GrassAudio;
}
