const fs = require('fs');
let code = fs.readFileSync('src/bluearchive.css', 'utf8');

code = code.replace(/\[data-theme="bluearchive"\] button \{\n[\s\S]*?\n\}\n\n/, '');

fs.writeFileSync('src/bluearchive.css', code);
