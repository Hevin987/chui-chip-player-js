const fs = require('fs');
let code = fs.readFileSync('src/components/App.js', 'utf8');

// The button was hidden behind other z-indexes or clipped due to overflow hidden.
// Let's place it outside the track panel or give it a high z-index.
// We'll replace the existing toggle button code:
const re = /<button\s+className="box-button"\s+style=\{\{ position: 'absolute', top: 8, right: 8, zIndex: 10, padding: '2px 8px', fontSize: '10px' \}\}\s+onClick=\{\(\) => this\.setState\(\{showDebugWav: true\}\)\}\s+title="Show Pre-rendered Data"\s+>/m;

if (re.test(code)) {
    code = code.replace(/<button\s+className="box-button"\s+style=\{\{ position: 'absolute', top: 8, right: 8, zIndex: 10, padding: '2px 8px', fontSize: '10px' \}\}\s+onClick=\{\(\) => this\.setState\(\{showDebugWav: true\}\)\}\s+title="Show Pre-rendered Data"\s+>/m, 
    `<button
                        className="box-button"
                        style={{ position: 'absolute', top: 8, right: 8, zIndex: 9999, padding: '4px 12px', fontSize: '12px', background: '#e74c3c', color: 'white', border: '1px solid black', borderRadius: '4px' }}
                        onClick={() => this.setState({showDebugWav: true})}
                        title="Show Pre-rendered Data"
                      >`);
    fs.writeFileSync('src/components/App.js', code);
    console.log("patched App.js zindex");
} else {
    console.log('regex mismatch');
}
