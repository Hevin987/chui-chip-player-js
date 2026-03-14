const fs = require('fs');
let text = fs.readFileSync('src/bluearchive.css', 'utf8');

text = text.replace(/\[data-theme="bluearchive"\] <!doctype html>\n<html lang="en">\n\n<head>[\s\S]*?body \{/m, '[data-theme="bluearchive"] body {');

text = text.replace(/\[data-theme="bluearchive"\] \/\* 4\. Seek Bar \*\/\n    \.seek-bar-container \{/m, '[data-theme="bluearchive"] .seek-bar-container {');

fs.writeFileSync('src/bluearchive.css', text);
