const bufferSize = 2048;
const MAX_VOICES = 8;
let currentFrame = 1;
let v = 1;
const baseOffset = currentFrame * MAX_VOICES * bufferSize + (v * bufferSize);
console.log(baseOffset);
console.log(baseOffset + bufferSize);
