const fs = require('fs');

let css = fs.readFileSync('src/index.css', 'utf8');
const lines = css.split('\n');

const msdosIndex = lines.findIndex(l => l.includes('.retro-window {'));
if (msdosIndex > 0) {
  // Strip hardcoded colors/borders from the layout definition
  let rest = lines.slice(msdosIndex).join('\n');
  
  // Make a backup
  fs.writeFileSync('src/index.css.bak', css);
  
  // Wrap colors into [data-theme="bluearchive"]
  const colorRules = `
[data-theme="bluearchive"] .retro-window {
  background: #cddce3;
  border: 3px solid #6d8591;
  box-shadow: 4px 4px 0px rgba(0,0,0,0.2);
  border-radius: 10px;
}
[data-theme="bluearchive"] .retro-header {
  background: rgba(67, 84, 102, 1);
}
[data-theme="bluearchive"] .retro-header-center {
  border-radius: 5px 5px 0 0;
}
[data-theme="bluearchive"] .retro-header button:not(.close-btn) {
  color: #cddce3;
}
[data-theme="bluearchive"] .player-content {
  background-color: rgb(208, 225, 232);
  border-radius: 10px;
}
[data-theme="bluearchive"] .visualization-area {
  background-color: rgb(170, 195, 197);
  border-radius: 10px;
  border-bottom: 2px solid #6d8591;
  border-radius: 5px;
}
[data-theme="bluearchive"] .album {
  border-radius: 10px;
}
[data-theme="bluearchive"] .album-sticker {
  background-color: rgb(149, 170, 175);
  border-radius: 5px;
}
[data-theme="bluearchive"] .waveform-bg {
  background-color: rgb(205, 220, 227);
  border-radius: 10px;
}
[data-theme="bluearchive"] .control-container {
  background-color: rgb(170, 195, 197);
  border-radius: 10px;
  border-top: 2px solid #6d8591;
}

[data-theme="msdos"] .retro-header button:not(.close-btn) {
  color: var(--neutral0);
}
[data-theme="msdos"] .album-sticker .album-label {
  display: none;
}
`;

  // Define layout only replacements:
  let layoutStr = rest
    .replace(/background:\s*#cddce3;/g, '')
    .replace(/border:\s*3px solid #6d8591;/g, '')
    .replace(/box-shadow:.*?;/g, '')
    .replace(/border-radius:.*?;/g, '')
    .replace(/background-color:\s*rgb.*?;/g, '')
    .replace(/border-bottom:\s*2px solid #6d8591;/g, '')
    .replace(/border-top:\s*2px solid #6d8591;/g, '')
    .replace(/background:\s*rgba\(67,\s*84,\s*102,\s*1\);/g, '')
    .replace(/color:\s*#cddce3;/g, '');

  fs.writeFileSync('src/index.css', lines.slice(0, msdosIndex).join('\n') + '\n' + layoutStr);
  fs.appendFileSync('src/bluearchive.css', '\n' + colorRules);
}
