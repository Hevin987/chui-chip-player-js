const fs = require('fs');
let indexCss = fs.readFileSync('src/index.css', 'utf8');

// Find all hardcoded color lines
let lines = indexCss.split('\n');
let newLines = [];

for (let i = 0; i < lines.length; i++) {
  let line = lines[i];
  
  if (i > 800 && line.match(/(background|color|border)(-[a-z]+)?\s*:.*(#[0-9a-fA-F]+|rgba|white)/i)) {
      if (!line.includes('var(')) {
          console.log("Removing:", i, line);
          continue; 
      }
  }
  
  newLines.push(line);
}

fs.writeFileSync('src/index.css', newLines.join('\n'));
