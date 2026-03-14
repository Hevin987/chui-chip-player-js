const fs = require('fs');
let code = fs.readFileSync('src/components/App.js', 'utf8');

code = code.replace(
  '<div className="App app-body player-interface">',
  '<div className="App app-body">\n          <main className="player-interface">'
);

code = code.replace(
  /          <\/section>\n        <\/div>\n      \)}<\/Dropzone>/,
  '          </section>\n          </main>\n        </div>\n      )}</Dropzone>'
);

fs.writeFileSync('src/components/App.js', code);
