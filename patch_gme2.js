const fs = require('fs');
let code = fs.readFileSync('src/players/GMEPlayer.js', 'utf8');

// replace loadData
if (!code.includes('async loadData')) {
    code = code.replace(/loadData\(data, filepath, persistedSettings, subtune\) \{/, "async loadData(data, filepath, persistedSettings, subtune) {");
}

const targetReplacement = `this.resolveParamValues(persistedSettings);

    if (persistedSettings.visualizerType === 'oscilloscope') {
      if (visualizerWorker) {
        visualizerWorker.terminate();
      }
      visualizerWorker = new GMEWorker();
      
      await new Promise((resolve) => {
        visualizerWorker.onerror = (e) => {
          console.error("GME Worker error:", e);
          resolve(); 
        };

        visualizerWorker.onmessage = (e) => {
          if (e.data.type === 'done') {
            this.visualMap = new Int8Array(e.data.visualBuffer);
            this.totalVisualFrames = e.data.totalFrames;
            this.maxVoices = e.data.MAX_VOICES;
            console.log("Multichannel GME data ready. " + this.totalVisualFrames + " frames.");
            resolve();
          } else if (e.data.type === 'error') {
            console.error("GME Worker explicitly reported error:", e.data.error);
            resolve();
          }
        };

        visualizerWorker.postMessage({
          type: 'analyze',
          data: data.slice(0),
          sampleRate: this.sampleRate,
          bufferSize: this.bufferSize,
          multichannel: persistedSettings.vgmMultichannel !== false,
          subtune
        });
      });
    }`;

code = code.replace(/this\.resolveParamValues\(persistedSettings\);/, targetReplacement);

// processAudioInner patch:
const ifWindowVoiceBuffersRegex = /if \(typeof window !== "undefined" && window\.voiceBuffers\) \{([\s\S]*?)for \(let v = 1; v < window\.voiceBuffers\.length; v\+\+\) \{\s+window\.voiceBuffers\[v\]\.fill\(0\);\s+\}\s+\}/;

const innerReplacement = `if (typeof window !== "undefined" && window.voiceBuffers) {
      if (this.visualMap) {
        const rawMs = this.getPositionMs();
        const currentFrame = Math.floor((rawMs / 1000) * this.sampleRate / this.bufferSize);
        
        if (currentFrame < this.totalVisualFrames) {
          const maxVoices = this.maxVoices || 8;
          while (window.voiceBuffers.length < maxVoices) {
            window.voiceBuffers.push(new Float32Array(this.bufferSize));
          }
          if (window.voiceBuffers.length > maxVoices) {
            window.voiceBuffers.length = maxVoices;
          }

          for (let v = 0; v < maxVoices; v++) {
            const baseOffset = currentFrame * maxVoices * this.bufferSize + (v * this.bufferSize);
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
    }`;

code = code.replace(ifWindowVoiceBuffersRegex, innerReplacement);

fs.writeFileSync('src/players/GMEPlayer.js', code);
