declare module 'grass-audio'{
  export class GrassAudio {
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
}



