const fs = require('fs');
let code = fs.readFileSync('src/workers/vgm.worker.js', 'utf8');

code = code.replace(/const \{ type, data, sampleRate, bufferSize \} = e.data;/, "const { type, data, sampleRate, bufferSize, multichannel } = e.data;");
code = code.replace(/const MAX_VOICES = Math\.min\(numVoices, 16\);/, "const MAX_VOICES = multichannel ? Math.min(numVoices, 16) : 1;");
code = code.replace(/const visualizerBuffer = new Float32Array\(totalFrames \* MAX_VOICES \* bufferSize\);/, "const visualizerBuffer = new Int8Array(totalFrames * MAX_VOICES * bufferSize);");
code = code.replace(/let bitmask = 0n;\s+for \(let i = 0; i < 64; i\+\+\) \{\s+if \(i !== v\) bitmask \+= 1n << BigInt\(i\);\s+\}\s+core\._lvgm_set_voice_mask\(vgmCtx, bitmask\);/g, 
`let bitmask = 0n;
      if (multichannel) {
        for (let i = 0; i < 64; i++) {
          if (i !== v) bitmask += 1n << BigInt(i);
        }
      }
      core._lvgm_set_voice_mask(vgmCtx, bitmask);`
);
code = code.replace(/visualizerBuffer\[baseOffset \+ i\] = \(vL \+ vR\) \/ 2\.0;/, `visualizerBuffer[baseOffset + i] = Math.round(((vL + vR) / 2.0) * 127);`);

fs.writeFileSync('src/workers/vgm.worker.js', code);
