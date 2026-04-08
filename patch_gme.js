const fs = require('fs');
if (!fs.existsSync('src/workers/gme.worker.js')) {
    console.log('no gme helper');
    process.exit(0);
}
let code = fs.readFileSync('src/workers/gme.worker.js', 'utf8');

code = code.replace(/const \{ type, data, sampleRate, bufferSize, subtune \} = e.data;/, "const { type, data, sampleRate, bufferSize, subtune, multichannel } = e.data;");
code = code.replace(/const MAX_VOICES = Math\.min\(numVoices, 16\);/, "const MAX_VOICES = multichannel ? Math.min(numVoices, 16) : 1;");
code = code.replace(/const visualizerBuffer = new Float32Array\(totalFrames \* MAX_VOICES \* bufferSize\);/, "const visualizerBuffer = new Int8Array(totalFrames * MAX_VOICES * bufferSize);");
code = code.replace(/let bitmask = 0;\s+for \(let i = 0; i < 32; i\+\+\) \{\s+if \(i !== v\) bitmask \|= \(1 << i\);\s+\}\s+core\._gme_mute_voices\(emu, bitmask\);/g, 
`let bitmask = 0;
      if (multichannel) {
        for (let i = 0; i < 32; i++) {
          if (i !== v) bitmask |= (1 << i);
        }
      }
      core._gme_mute_voices(emu, bitmask);`
);
code = code.replace(/visualizerBuffer\[baseOffset \+ i\] = \(vL \+ vR\) \/ 2\.0;/, `visualizerBuffer[baseOffset + i] = Math.round(((vL + vR) / 2.0) * 127);`);

fs.writeFileSync('src/workers/gme.worker.js', code);
