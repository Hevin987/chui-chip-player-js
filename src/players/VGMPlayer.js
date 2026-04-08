import Player from "./Player.js";
import autoBind from 'auto-bind';
import { allOrNone } from '../util';
// eslint-disable-next-line import/no-webpack-loader-syntax
import VGMWorker from 'worker-loader?name=chipPlayer/[contenthash].worker.js!../workers/vgm.worker.js';

let visualizerWorker = null;

const fileExtensions = [
  'vgm',
  'vgz',
  'gym',
  's98',
  'dro',
];

const INT32_MAX = 0x8000000; // 2147483648

export default class VGMPlayer extends Player {
  paramDefs = [
    {
      id: 'debugChannel',
      label: 'Debug Channel Mode',
      type: 'enum',
      options: [
        {
          label: 'Channel',
          items: [{ value: -1, label: 'All Channels (Normal)' }]
        }
      ],
      defaultValue: -1,
    }
  ];

  constructor(...args) {
    super(...args);
    autoBind(this);

    this.playerKey = 'vgm';
    this.name = 'LibVGM Player';
    this.speed = 1;
    this.fileExtensions = fileExtensions;
    this.buffer = this.core._malloc(this.bufferSize * 4 * 2);
    this.vgmCtx = this.core._lvgm_init(this.sampleRate);
    this.visualMap = null;
    this.useMultichannel = false;
    this.currentRenderFrame = 0;
  }

  async loadData(data, filepath, persistedSettings) {
    const dataPtr = this.copyToHeap(data);
    const err = this.core._lvgm_load_data(this.vgmCtx, dataPtr, data.byteLength);
    this.core._free(dataPtr);
    const filepathMeta = Player.metadataFromFilepath(filepath);

    if (err !== 0) {
      console.error("lvgm_load_data failed. error code: %d", err);
      throw Error('Unable to load this file!');
    }

    this.core._lvgm_start(this.vgmCtx);

    const metaPtr = this.core._lvgm_get_metadata(this.vgmCtx);
    const meta = {
      title:   this.core.UTF8ToString(this.core.getValue(metaPtr + 0, 'i32')) || filepathMeta.title,
      artist:  this.core.UTF8ToString(this.core.getValue(metaPtr + 4, 'i32')),
      game:    this.core.UTF8ToString(this.core.getValue(metaPtr + 8, 'i32')),
      system:  this.core.UTF8ToString(this.core.getValue(metaPtr + 12, 'i32')),
      date:    this.core.UTF8ToString(this.core.getValue(metaPtr + 16, 'i32')),
      comment: this.core.UTF8ToString(this.core.getValue(metaPtr + 20, 'i32')),
    };

    // Custom title/subtitle formatting, same as GMEPlayer:
    meta.formatted = {
      title: meta.game === meta.title ?
        meta.title :
        allOrNone(meta.game, ' - ') + meta.title,
      subtitle: [meta.artist, meta.system].filter(x => x).join(' - ') +
        allOrNone(' (', meta.date, ')'),
    };
    this.metadata = meta;
    
    this.currentRenderFrame = 0;
    this.visualMap = null;
    this.useMultichannel = persistedSettings.vgmMultichannel ?? false;

    // Populate Debug Channel Solo Dropdown dynamically based on this specific file's voices
    const numVoices = this.core._lvgm_get_voice_count(this.vgmCtx) || 8;
    const debugItems = [{ value: -1, label: 'All Channels (Normal)' }];
    for (let v = 0; v < numVoices; v++) {
      const voiceName = this.core.UTF8ToString(this.core._lvgm_get_voice_name(this.vgmCtx, v)) || `Channel ${v}`;
      debugItems.push({ value: v, label: `Channel ${v}: ${voiceName}` });
    }
    this.paramDefs[0].options[0].items = debugItems;

    if (persistedSettings.visualizerType === 'oscilloscope') {
      if (visualizerWorker) {
        visualizerWorker.terminate();
      }
      visualizerWorker = new VGMWorker();
      
      await new Promise((resolve, reject) => {
        visualizerWorker.onerror = (e) => {
          console.error("VGM Worker error:", e);
          resolve(); // Resolve anyway so music still plays if it fails
        };

        visualizerWorker.onmessage = (e) => {
          if (e.data.type === 'done') {
            this.visualMap = new Int8Array(e.data.visualBuffer);
            this.totalVisualFrames = e.data.totalFrames;
            this.maxVoices = e.data.MAX_VOICES;
            console.log("Multichannel visualizer data ready. " + this.totalVisualFrames + " frames.");
            resolve();
          } else if (e.data.type === 'error') {
            console.error("VGM Worker explicitly reported error:", e.data.error);
            resolve();
          } else if (e.data.type === 'progress') {
            console.log("Visualizer Worker Progress: " + e.data.percent + "%");
          }
        };

        visualizerWorker.postMessage({
          type: 'analyze',
          data: data.slice(0),
          sampleRate: this.sampleRate,
          bufferSize: this.bufferSize,
          multichannel: this.useMultichannel
        });
      });
    }

    this.resolveParamValues(persistedSettings);
    this.setTempo(persistedSettings.tempo || 1);
    this.resume();
    this.emit('playerStateUpdate', {
      ...this.getBasePlayerState(),
      isStopped: false
    });
  }

  processAudioInner(channels) {
    let i, ch;

    if (this.paused) {
      for (ch = 0; ch < channels.length; ch++) {
        channels[ch].fill(0);
      }
      return;
    }

    const samplesWritten = this.core._lvgm_render(this.vgmCtx, this.buffer, this.bufferSize);
    if (samplesWritten === 0) {
      this.stop();
    }

    // Initialize UI Oscilloscope buffers
    if (typeof window !== "undefined") {
      if (!window.voiceBuffers) window.voiceBuffers = [];
      if (!window.voiceNames) window.voiceNames = [];
      const numVoices = this.core._lvgm_get_voice_count(this.vgmCtx) || 8;
      window.voiceBuffers.length = numVoices;
      window.voiceNames.length = numVoices;
      for (let v = 0; v < numVoices; v++) {
        if (!window.voiceBuffers[v]) window.voiceBuffers[v] = new Float32Array(this.bufferSize);
        window.voiceNames[v] = this.core.UTF8ToString(this.core._lvgm_get_voice_name(this.vgmCtx, v)) || `Channel ${v}`;
      }
    }

    for (ch = 0; ch < channels.length; ch++) {
      for (i = 0; i < this.bufferSize; i++) {
        channels[ch][i] = this.core.getValue(
          this.buffer +           // Interleaved channel format
          i * 4 * 2 +             // frame offset   * bytes per sample * num channels +
          ch * 4,                 // channel offset * bytes per sample
          'i32'                   // the sample values are 32-bit integer
        ) / INT32_MAX;
      }
    }

    if (typeof window !== "undefined" && window.voiceBuffers) {
      if (this.visualMap) {
        // Read pre-rendered multi-channel buffers!
        const rawMs = this.getPositionMs();
        // The worker saved raw frames linearly based on logical track bounds at 1.0x speed
        const currentFrame = Math.floor((rawMs / 1000) * this.sampleRate / this.bufferSize);
        
        if (currentFrame < this.totalVisualFrames) {
          const maxVoices = this.maxVoices || 8;
          // Ensure window.voiceBuffers has enough arrays
          while (window.voiceBuffers.length < maxVoices) {
            window.voiceBuffers.push(new Float32Array(this.bufferSize));
          }
          // Trim if too many
          if (window.voiceBuffers.length > maxVoices) {
            window.voiceBuffers.length = maxVoices;
          }

          for (let v = 0; v < maxVoices; v++) {
            const baseOffset = currentFrame * maxVoices * this.bufferSize + (v * this.bufferSize);
            // Copy the Float32 subarray directly out of the shared visualMap
            const chunk = this.visualMap.subarray(baseOffset, baseOffset + this.bufferSize);
            const target = window.voiceBuffers[v];
            for (let i = 0; i < this.bufferSize; i++) target[i] = chunk[i] / 128.0;
          }
        }
      } else {
        // Render the master stereo mix for the visualizer
        for (i = 0; i < this.bufferSize; i++) {
          window.voiceBuffers[0][i] = (channels[0][i] + channels[1][i]) / 2.0;
        }
        for (let v = 1; v < window.voiceBuffers.length; v++) {
          window.voiceBuffers[v].fill(0);
        }
      }
    }
  }

  getTempo() {
    if (this.vgmCtx)
      return this.core._lvgm_get_playback_speed(this.vgmCtx);
  }

  setTempo(val) {
    if (this.vgmCtx)
      return this.core._lvgm_set_playback_speed(this.vgmCtx, val);
  }

  getPositionMs() {
    if (this.vgmCtx)
      return this.core._lvgm_get_position_ms(this.vgmCtx);
  }

  getDurationMs() {
    if (this.vgmCtx)
      return this.core._lvgm_get_duration_ms(this.vgmCtx);
  }

  getNumVoices() {
    if (this.vgmCtx)
      return this.core._lvgm_get_voice_count(this.vgmCtx);
  }

  getVoiceGroups() {
    const voiceGroups = [];
    const numVoices = this.core._lvgm_get_voice_count(this.vgmCtx);
    let currChipName;
    let currGroup;
    for (let i = 0; i < numVoices; i++) {
      const voiceName = this.core.UTF8ToString(this.core._lvgm_get_voice_name(this.vgmCtx, i));
      const chipName = this.core.UTF8ToString(this.core._lvgm_get_voice_chip_name(this.vgmCtx, i));
      if (chipName !== currChipName) {
        currGroup = {
          name: chipName,
          icon: true, // currently hardcoded CSS class 'image-chip' (images/icon-chip.png)
          voices: [],
        };
        currChipName = chipName;
        voiceGroups.push(currGroup);
      }
      currGroup.voices.push({
        idx: i,
        name: voiceName,
      });
    }
    return voiceGroups;
  }

  getMetadata() {
    return this.metadata;
  }

  isPlaying() {
    return !this.isPaused();
  }

  seekMs(seekMs) {
    if (this.vgmCtx) {
      this.core._lvgm_seek_ms(this.vgmCtx, seekMs);
      this.currentRenderFrame = Math.floor((seekMs / 1000) * this.sampleRate / this.bufferSize);
    }
  }

  getVoiceName(index) {
    if (this.vgmCtx) return this.core.UTF8ToString(this.core._lvgm_get_voice_name(this.vgmCtx, index));
  }

  getVoiceMask() {
    if (this.vgmCtx) {
      const bitmask = this.core._lvgm_get_voice_mask(this.vgmCtx);
      const voiceMask = [];
      for (let i = 0n; i < 64n; i++) {
        voiceMask.push((bitmask & (1n << i)) === 0n);
      }
      return voiceMask;
    }
    return [];
  }

  setVoiceMask(voiceMask) {
    if (this.vgmCtx) {
      let bitmask = 0n;
      voiceMask.forEach((isEnabled, i) => {
        if (!isEnabled) {
          bitmask += 1n << BigInt(i);
        }
      });
      this.core._lvgm_set_voice_mask(this.vgmCtx, bitmask);
    }
  }

  stop() {
    this.suspend();
    if (this.vgmCtx) this.core._lvgm_stop(this.vgmCtx);
    console.debug('VGMPlayer.stop()');
    this.emit('playerStateUpdate', { isStopped: true });
  }

  setParameter(id, value) {
    if (id === 'debugChannel') {
      this.params[id] = parseInt(value, 10);
      if (this.vgmCtx) {
        const debugVal = this.params[id];
        if (debugVal === -1) {
          this.core._lvgm_set_voice_mask(this.vgmCtx, 0n);
        } else {
          const numVoices = this.core._lvgm_get_voice_count(this.vgmCtx);
          let bitmask = 0n;
          for (let i = 0; i < numVoices; i++) {
            if (i !== debugVal) {
              bitmask += 1n << BigInt(i);
            }
          }
          this.core._lvgm_set_voice_mask(this.vgmCtx, bitmask);
        }
      }
    } else {
      super.setParameter(id, value);
    }
  }
}
