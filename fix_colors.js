const fs = require('fs');
let indexCss = fs.readFileSync('src/index.css', 'utf8');

// Find all hardcoded color lines
let lines = indexCss.split('\n');
let newLines = [];
let extracted = [];

let inNewSection = false;

// We know the new section starts around line 800. Let's find "Reset / Base rules"
let currentRule = '';

for (let i = 0; i < lines.length; i++) {
  let line = lines[i];
  if (line.includes('/* Reset / Base rules */')) {
    inNewSection = true;
  }
  
  if (inNewSection && line.match(/(background|color|border).*(#[0-9a-fA-F]+|rgba|white)/i)) {
      // It's a color rule! BUT we must make sure it doesn't have var(--...) instead.
      if (!line.includes('var(')) {
          console.log("Removing:", line);
          // Keep it to extract later? No, we can just replace it.
          // Wait, we need to know the selector! Best is just to comment it out or remove.
          continue; 
      }
  }
  
  newLines.push(line);
}

fs.writeFileSync('src/index.css', newLines.join('\n'));

