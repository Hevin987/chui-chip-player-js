const fs = require('fs');
let code = fs.readFileSync('src/components/LocalFiles.js', 'utf8');

const regex = /{!\(!ejected && title\) &&[\s\S]*?<\/>\s*\n\s*}/;

let replacement = `{(!ejected && title) &&
              <div className="metadata-container">
                <div><span className="font-bold">Track Name:</span> {title}</div>
                <div><span className="font-bold">Artist:</span> {subtitle || 'Unknown'}</div>
                {metadata?.system && <div><span className="font-bold">System:</span> {metadata.system}</div>}
              </div>
            }`;

code = code.replace(/\{\(!ejected && title\) &&\s*<>[\s\S]*?<\/>\s*\n\s*\}/, replacement);
fs.writeFileSync('src/components/LocalFiles.js', code);
