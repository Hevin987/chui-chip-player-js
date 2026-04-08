const fs = require('fs');
let code = fs.readFileSync('src/components/App.js', 'utf8');
code = code.replace(/<div className="App">/, `<div className="App">\n        {this.state.showDebugWav && this.sequencer?.getPlayer()?.visualMap && (\n          <DebugWav player={this.sequencer.getPlayer()} onClose={() => this.setState({showDebugWav: false})} />\n        )}`);
fs.writeFileSync('src/components/App.js', code);
