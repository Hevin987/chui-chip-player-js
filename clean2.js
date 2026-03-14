const fs = require('fs');
let code = fs.readFileSync('src/bluearchive.css', 'utf8');

const regex = /\[data-theme="bluearchive"\] <!doctype html>[\s\S]*?<style data-purpose="global-typography">/m;
code = code.replace(regex, '');

code = code.replace(/\[data-theme="bluearchive"\] \/\* 4\. Seek Bar \*\/\n\s*\.seek-bar-container/m, '[data-theme="bluearchive"] .seek-bar-container');

fs.writeFileSync('src/bluearchive.css', code);
