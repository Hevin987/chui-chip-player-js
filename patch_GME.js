const fs = require('fs');
if (!fs.existsSync('src/players/GMEPlayer.js')) process.exit(0);
let code = fs.readFileSync('src/players/GMEPlayer.js', 'utf8');

// make it async
code = code.replace(/loadData\(data, filepath, persistedSettings, subtune\) \{/, "async loadData(data, filepath, persistedSettings, subtune) {");

// replace block
const targetRegex = /if \(this\.useMultichannel\) \{(.|\n)*?this\.resolveParamValues\(persistedSettings\);/m;
const replacement = `if (persistedSettings.visualizerType === 'oscilloscope') {
      if (visualizerWorker) {
        visualizerWorker.terminate();
      }
      visualizerWorker = new GMEWorker();
      
      await new Promise((resolve) => {
        visualizerWorker.onerror = (e) => {
          console.error("GME Worker error:", e);
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
            console.error("GME Worker explicitly reported error:", e.data.error);
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
          multichannel: this.useMultichannel,
          subtune
        });
      });
    }

    this.resolveParamValues(persistedSettings);`;

if (!targetRegex.test(code)) {
    console.log('Regex did not match GMEPlayer');
} else {
    code = code.replace(targetRegex, replacement);
    // processAudioInner patch:
    code = code.replace(/if \(this\.useMultichannel && this\.visualMap\) \{/g, `if (this.visualMap) {`);
    code = code.replace(/window\.voiceBuffers\[v\]\.set\(this\.visualMap\.subarray\(baseOffset, baseOffset \+ this\.bufferSize\)\);/, 
            `const chunk = this.visualMap.subarray(baseOffset, baseOffset + this.bufferSize);
            const target = window.voiceBuffers[v];
            for (let i = 0; i < this.bufferSize; i++) target[i] = chunk[i] / 128.0;`);
    fs.writeFileSync('src/players/GMEPlayer.js', code);
    console.log('patched GMEPlayer');
}
