const fs = require('fs');
let code = fs.readFileSync('src/index.css', 'utf8');

code = code.replace(
  /\.track-info-panel \{\n\s*max-width: 60rem;\n\s*\}/,
  `.track-info-panel {
      max-width: 60rem;
    }
    
    @media (min-width: 976px) {
      .track-info-panel {
        height: 0;
        min-height: 100%;
        overflow: hidden;
      }
    }`
);

fs.writeFileSync('src/index.css', code);
