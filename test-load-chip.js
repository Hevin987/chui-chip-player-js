const ChipCore = require('./src/chip-core.js');
console.log(typeof ChipCore);
async function run() {
  try {
    console.log("Instantiating...");
    const core = await ChipCore({
        print: console.log,
        printErr: console.error
    });
    console.log("SUCCESS!", !!core);
  } catch (e) {
    console.error("FAILED TO LOAD:", e);
  }
}
run().then(() => console.log('Done'));
