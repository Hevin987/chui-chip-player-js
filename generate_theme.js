const fs = require('fs');

const cssContent = fs.readFileSync('chipjsTheme/MusicPlayer.htm', 'utf8');

// very quick n dirty CSS parser
const rules = cssContent.match(/([^{}]+)\{([^{}]+)\}/g);
let output = [];

rules.forEach(rule => {
    const match = rule.match(/([^{}]+)\{([^{}]+)\}/);
    if (!match) return;
    
    const selector = match[1].trim();
    if (selector.startsWith('@') || selector === 'body' || selector === 'html') return;
    
    const declarations = match[2].split(';').map(d => d.trim()).filter(d => d);
    let colorDecs = declarations.filter(d => {
        return d.match(/(background|color|border|box-shadow).*(#[0-9a-fA-F]+|rgba|white)/i) &&
               !d.match(/var\(/);
    });
    
    if (colorDecs.length > 0) {
        // Strip duplicate comma-separated selectors to add [data-theme="bluearchive"] to each
        let sels = selector.split(',').map(s => {
            s = s.trim();
            if (s === '') return '';
            return '[data-theme="bluearchive"] ' + s;
        }).filter(s => s).join(',\n');
        
        if (sels) {
            output.push(`${sels} {\n  ${colorDecs.join(';\n  ')};\n}`);
        }
    }
});

fs.appendFileSync('src/bluearchive.css', '\n\n/* Extracted layout colors */\n' + output.join('\n\n') + '\n');
console.log("Appended to bluearchive.css");
