const fs = require('fs');
let code = fs.readFileSync('src/players/VGMPlayer.js', 'utf8');

// make it async
code = code.replace(/loadData\(data, filepath, persistedSettings\) \{/, "async loadData(data, filepath, persistedSettings) {");

// find "if (this.useMultichannel) {" block and replace it
const targetRegex = /if \(this\.useMultichannel\) \{(.|\n)*?this\.resolveParamValues\(persistedSettings\);/m;

const replacement = `if (persistedSettings.visualizerType === 'oscilloscope') {
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

    this.resolveParamValues(persistedSettings);`;

if (!targetRegex.test(code)) {
    console.log('Regex did not match VGMPlayer');
} else {
    code = code.replace(targetRegex, replacement);
    // Finally, replace Float32Array read with Int8Array read in processAudioInner
    code = code.replace(/window\.voiceBuffers\[v\]\.set\(this\.visualMap\.subarray\(baseOffset, baseOffset \+ this\.bufferSize\)\);/, 
            `const chunk = this.visualMap.subarray(baseOffset, baseOffset + this.bufferSize);
            const target = window.voiceBuffers[v];
            for (let i = 0; i < this.bufferSize; i++) target[i] = chunk[i] / 128.0;`);
    fs.writeFileSync('src/players/VGMPlayer.js', code);
    console.log('patched VGMPlayer');
}
