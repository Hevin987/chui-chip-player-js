const fs = require('fs');

const musicPlayerHtm = fs.readFileSync('chipjsTheme/MusicPlayer.htm', 'utf8');

// very quick n dirty CSS parser
const rules = musicPlayerHtm.match(/([^{}]+)\{([^{}]+)\}/g);
let output = [];

rules.forEach(rule => {
    const match = rule.match(/([^{}]+)\{([^{}]+)\}/);
    if (!match) return;
    
    const selector = match[1].trim();
    const declarations = match[2].split(';').map(d => d.trim()).filter(d => d);
    if (selector.includes('@')) return;
    if (selector.includes('font-face')) return;
    
    let colorDecs = declarations.filter(d => {
        return d.match(/(background|color|border|box-shadow|opacity|fill|stroke|font-family|font-weight|text-transform).*(#[0-9a-fA-F]+|rgba|rgb|white|black|transparent|vt323|roboto|uppercase|bold|none)/i) &&
               !d.match(/var\(/);
    });
    
    if (colorDecs.length > 0) {
        let sels = selector.split(',').map(s => {
            s = s.trim();
            if (s === '') return '';
            if (s === 'body' || s.startsWith('body ')) {
              return '[data-theme="bluearchive"] ' + s.replace('body', '');
            }
            return '[data-theme="bluearchive"] ' + s;
        }).filter(s => s).join(',\n');
        
        if (sels) {
            output.push(`${sels} {\n  ${colorDecs.join(';\n  ')};\n}`);
        }
    }
});

let vars = `[data-theme="bluearchive"] {
    --buttonHeight: var(--charH);
    --rowHeight:    var(--charH);

    --clickable:  #2c3e50;
    --active:     #4b5563;
    --button:     rgb(149, 170, 175);
    --selected:   #8fa7b2;
    --focus:      #6d8591;
    --background: rgb(208, 225, 232);
    --shadow:     #8fa7b2;
    --neutral0:   #000000;
    --neutral1:   #4b5563;
    --neutral2:   #6b7280;
    --neutral3:   #9ca3af;
    --neutral4:   #ffffff;

    --font: 'roboto', monospace;

    color: #333;
}

[data-theme="bluearchive"] button {
    font-family: 'roboto', monospace;
    background: transparent;
    border: none;
    color: inherit;
    cursor: pointer;
}

[data-theme="bluearchive"] .BrowseList-row-selected {
    background-color: var(--selected);
    color: white;
}
`;

fs.writeFileSync('src/bluearchive.css', vars + '\n' + output.join('\n\n') + '\n');
