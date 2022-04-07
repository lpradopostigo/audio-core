declare module "grass-audio" {
    class GrassAudio {
        constructor(sampleRate?: 44100 | 48000);

        play(): void;

        pause(): void;

        stop(): void;

        skipToIndex(index: number): void;

        previous(): void;

        next(): void;

        seek(position: number): void;

        setFiles(files: string[]);

        setVolume(volume: number): void;

        getState(): {
            playbackState: "playing" | "stopped" | "paused";
            fileDuration: number;
            filePosition: number;
            fileIndex: number;
        };
    }

    export = GrassAudio;
}
