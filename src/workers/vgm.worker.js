/* eslint-disable no-restricted-globals */
import ChipCore from '../chip-core';

let corePromise = null;
let core = null;

const INT32_MAX = 0x8000000;

const BASE_URL = process.env.PUBLIC_URL || self.location.origin;

self.onmessage = async (e) => {
  const { type, data, sampleRate, bufferSize, multichannel } = e.data;

  if (type === 'init') {
    if (!corePromise) {
      corePromise = new ChipCore({
        noInitialRun: true,
        locateFile: (path) => {
          if (path.endsWith('.wasm') || path.endsWith('.wast'))
            return `${BASE_URL}/chipPlayer/${path}`;
          return '/' + path;
        }
      });
      core = await corePromise;
    }
    self.postMessage({ type: 'ready' });
  } 
  
  if (type === 'analyze') {
    if (!corePromise) {
      corePromise = new ChipCore({
        noInitialRun: true,
        locateFile: (path) => {
          if (path.endsWith('.wasm') || path.endsWith('.wast'))
            return `${BASE_URL}/chipPlayer/${path}`;
          return '/' + path;
        }
      });
    }
    core = await corePromise;
    
    // 1. Initialize Context
    const vgmCtx = core._lvgm_init(sampleRate);
    
    // 2. Load File
    const dataPtr = core._malloc(data.byteLength);
    core.HEAPU8.set(new Uint8Array(data), dataPtr);
    const err = core._lvgm_load_data(vgmCtx, dataPtr, data.byteLength);
    core._free(dataPtr);
    if (err !== 0) {
      self.postMessage({ type: 'error', error: 'Failed to load vgm' });
      return;
    }
    
    // 3. Prepare the massive Float32Array for sharing
    const durationMs = core._lvgm_get_duration_ms(vgmCtx) || 180000;
    const maxSeconds = Math.min(durationMs / 1000, 300); // hard cap at 5 minutes
    const totalFrames = Math.ceil((maxSeconds * sampleRate) / bufferSize);
    
    // Dynamically take up to 16 voices for advanced chips (like Genesis which has ~10)
    let numVoices = core._lvgm_get_voice_count(vgmCtx);
    if (!numVoices || numVoices === 0) numVoices = 8;
    const MAX_VOICES = multichannel ? Math.min(numVoices, 16) : 1;
    
    const visualizerBuffer = new Int8Array(totalFrames * MAX_VOICES * bufferSize);
    
    // Buffer for engine rendering
    const renderBuffer = core._malloc(bufferSize * 4 * 2); // 2 channels, 32-bit (4 bytes)
    
    for (let v = 0; v < MAX_VOICES; v++) {
      // Send progress back to main thread
      self.postMessage({ type: 'progress', percent: (v / MAX_VOICES) * 100 });
      
      core._lvgm_reset(vgmCtx);
      core._lvgm_start(vgmCtx);
      
      // Mute all except v
      let bitmask = 0n;
      if (multichannel) {
        for (let i = 0; i < 64; i++) {
          if (i !== v) bitmask += 1n << BigInt(i);
        }
      }
      core._lvgm_set_voice_mask(vgmCtx, bitmask);
      
      try {
        let currentFrame = 0;
        while (currentFrame < totalFrames) {
          const samplesWritten = core._lvgm_render(vgmCtx, renderBuffer, bufferSize);
          if (samplesWritten === 0) break; // Finished early
          
          const baseOffset = currentFrame * MAX_VOICES * bufferSize + (v * bufferSize);
          
          // We read only Left channel (channel 0) for visualizer (or master mono)
          for (let i = 0; i < bufferSize; i++) {
              const vL = core.getValue(renderBuffer + i * 8, 'i32') / INT32_MAX;
              const vR = core.getValue(renderBuffer + i * 8 + 4, 'i32') / INT32_MAX;
              visualizerBuffer[baseOffset + i] = Math.round(((vL + vR) / 2.0) * 127);
          }
          
          currentFrame++;
        }
      } catch (err) {
        self.postMessage({ type: 'error', error: "Render err: " + err.toString() });
      }
    }
    
    core._free(renderBuffer);
    
    self.postMessage({ 
      type: 'done', 
      visualBuffer: visualizerBuffer.buffer,
      totalFrames,
      bufferSize,
      MAX_VOICES
    }, [visualizerBuffer.buffer]);
  }
};
