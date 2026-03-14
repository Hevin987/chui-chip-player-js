const fs = require('fs');
let code = fs.readFileSync('src/components/Visualizer.js', 'utf8');

code = code.replace(/enabled: false,/, '');
code = code.replace(/handleToggleVisualizer = \(e\) => \{[\s\S]*?\};\n/, '');
code = code.replace(/this\.state\.enabled \? 'block' : 'none'/g, "this.props.enabled ? 'block' : 'none'");
code = code.replace(/<h3 className='Visualizer-toggle'>[\s\S]*?<\/h3>/, '');
code = code.replace(/this\.state\.enabled && this\.state\.vizMode/g, "this.props.enabled && this.state.vizMode");

fs.writeFileSync('src/components/Visualizer.js', code);
