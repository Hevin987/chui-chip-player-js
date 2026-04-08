const fs = require('fs');
let code = fs.readFileSync('src/components/App.js', 'utf8');

// import DebugWav
if (!code.includes('import DebugWav')) {
  code = code.replace(/import Oscilloscope from '\.\/Oscilloscope';/, "import Oscilloscope from './Oscilloscope';\nimport DebugWav from './DebugWav';");
}

code = code.replace(/this\.state = \{/, "this.state = { showDebugWav: false,");

// add button to toggle DebugWav next to visualizer logic
const rx = /<Oscilloscope([\s\S]+?)\/>\}/m;

// we can also add it underneath Oscilloscope
const match = code.match(rx);
if (match) {
    const orig = match[0];
    const button = `
                    {!isMobile.phone && !this.state.loading && settings?.visualizerType === 'oscilloscope' && (
                      <button
                        className="box-button"
                        style={{ position: 'absolute', top: 8, right: 8, zIndex: 10, padding: '2px 8px', fontSize: '10px' }}
                        onClick={() => this.setState({showDebugWav: true})}
                        title="Show Pre-rendered Data"
                      >
                        Debug Full WAV
                      </button>
                    )}`;
    
    code = code.replace(rx, orig + button);
    console.log("patched Oscilloscope area");
} else {
    console.log("could not match rx");
}

// Add the component rendering near the top or bottom of render()
const renderRe = /<div className="App">/;
if (code.match(renderRe)) {
    code = code.replace(renderRe, `<div className="App">
        {this.state.showDebugWav && this.sequencer?.getPlayer()?.visualMap && (
          <DebugWav player={this.sequencer.getPlayer()} onClose={() => this.setState({showDebugWav: false})} />
        )}`);
}

fs.writeFileSync('src/components/App.js', code);
