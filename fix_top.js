const fs = require('fs');
let text = fs.readFileSync('src/bluearchive.css', 'utf8');

text = text.replace(/\/\* Extracted layout colors \*\/[^\[]+\[data-theme="bluearchive"\] \.tab_selected/, '/* Extracted layout colors */\n\n[data-theme="bluearchive"] .custom-scrollbar::-webkit-scrollbar-track {\n  background: #cddce3;\n}\n\n[data-theme="bluearchive"] .custom-scrollbar::-webkit-scrollbar-thumb {\n  background: #8fa7b2;\n  border: 2px solid #cddce3;\n}\n\n[data-theme="bluearchive"] .tab_selected');

fs.writeFileSync('src/bluearchive.css', text);
