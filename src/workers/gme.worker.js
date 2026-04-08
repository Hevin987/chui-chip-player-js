/* eslint-disable no-restricted-globals */
import ChipCore from '../chip-core';

let corePromise = null;
let core = null;

const BASE_URL = process.env.PUBLIC_URL || self.location.origin;

self.onmessage = async (e) => {
  const { type, data, sampleRate, bufferSize, track } = e.data;

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
    const gmeCtx = core._gme_new_emu(6 /* gme_spc_type */, sampleRate);
    if (!gmeCtx) return self.postMessage({ type: 'error', error: 'Failed to init GME' });
    
    // 2. Load File
    const dataPtr = core._malloc(data.byteLength);
    core.HEAPU8.set(new Uint8Array(data), dataPtr);
    const err = core._gme_load_data(gmeCtx, dataPtr, data.byteLength);
    core._free(dataPtr);
    if (err !== 0) {
      return self.postMessage({ type: 'error', error: 'Failed to load GME' });
    }
    
    // 3. Prepare
    core._gme_start_track(gmeCtx, track || 0);
    const numVoices = core._gme_voice_count(gmeCtx);
    const MAX_VOICES = Math.min(numVoices, 16) || 8; 

    // SPC files loop forever by default, let's limit visual caching to 3 minutes
    const maxSeconds = 180;
    const totalFrames = Math.ceil((maxSeconds * sampleRate) / bufferSize);
    
    const visualizerBuffer = new Int8Array(totalFrames * MAX_VOICES * bufferSize);
    
    // Allocate buffer for frames (2 channels, 16-bit)  (gme_play outputs 16-bit signed shorts)
    const renderBuffer = core._malloc(bufferSize * 2 * 2); 
    
    try {
      for (let v = 0; v < MAX_VOICES; v++) {
        self.postMessage({ type: 'progress', percent: (v / MAX_VOICES) * 100 });
        
        // Fully reset the track to the beginning to avoid state pollution
        core._gme_start_track(gmeCtx, track || 0);
        
        // Mute all except v
        const voiceMask = new Array(64).fill(false);
        if (numVoices > 0) {
           voiceMask[v] = true;
           let mask = 0;
           voiceMask.forEach((isEnabled, i) => {
             if (!isEnabled) mask += 1 << i;
           });
           core._gme_mute_voices(gmeCtx, mask);
        }
        
        let currentFrame = 0;
        
        while (currentFrame < totalFrames) {
          const trackErr = core._gme_play(gmeCtx, bufferSize * 2, renderBuffer);
          if (core._gme_track_ended(gmeCtx) === 1) break;
          
          const baseOffset = currentFrame * MAX_VOICES * bufferSize + (v * bufferSize);
          
          for (let i = 0; i < bufferSize; i++) {
              const vL = core.getValue(renderBuffer + i * 4, 'i16') / 32768.0;
              const vR = core.getValue(renderBuffer + i * 4 + 2, 'i16') / 32768.0;
              visualizerBuffer[baseOffset + i] = Math.round(((vL + vR) / 2.0) * 127);
          }
          
          currentFrame++;
        }
      }
    } catch(err) {
      return self.postMessage({ type: 'error', error: "Render err: " + err.toString() });
    } finally {
      core._free(renderBuffer);
      core._gme_delete(gmeCtx);
    }
    
    self.postMessage({ 
      type: 'done', 
      visualBuffer: visualizerBuffer.buffer,
      totalFrames,
      bufferSize,
      MAX_VOICES
    }, [visualizerBuffer.buffer]);
  }
};
