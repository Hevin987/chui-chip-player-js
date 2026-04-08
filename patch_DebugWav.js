const fs = require('fs');
let code = fs.readFileSync('src/components/DebugWav.js', 'utf8');

if (!code.includes('return () => {')) {
  code = code.replace(/setWavLinks\(links\);\n    \}/, `setWavLinks(links);\n    }\n\n    return () => {\n      links.forEach(lk => URL.revokeObjectURL(lk.url));\n    };`);
  fs.writeFileSync('src/components/DebugWav.js', code);
  console.log("Patched cleanup");
}
