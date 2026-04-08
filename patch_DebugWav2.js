const fs = require('fs');
let code = fs.readFileSync('src/components/DebugWav.js', 'utf8');

code = code.replace(/if \(frames > 0\) \{\s+const links = \[\];[^{]+for \(let v = 0; v < voices; v\+\+\) \{/, `let links = [];\n    if (frames > 0) {\n      for (let v = 0; v < voices; v++) {`);

fs.writeFileSync('src/components/DebugWav.js', code);
