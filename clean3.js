const fs = require('fs');
let code = fs.readFileSync('src/bluearchive.css', 'utf8');

const regex = /\[data-theme="bluearchive"\] \{\n\s*background: transparent;[\s\S]*?color: #333;\n\}/m;
code = code.replace(regex, '');

fs.writeFileSync('src/bluearchive.css', code);
