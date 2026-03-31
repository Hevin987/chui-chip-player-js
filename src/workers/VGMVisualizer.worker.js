import autoBind from 'auto-bind';
import ChipCore from '../chip-core';

const BASE_URL = process.env.PUBLIC_URL || self.location.origin;

self.onmessage = async (e) => {
  console.log("Worker building VGM", e.data);
  const core = await new ChipCore({
    noInitialRun: true,
    locateFile: (path) => {
      // In a worker, this might need an absolute path or relative to something. We will see.
      if (path.endsWith('.wasm') || path.endsWith('.wast'))
        return `${BASE_URL}/chipPlayer/${path}`;
      return '/' + path;
    }
  });
  self.postMessage({ status: 'ready' });
};
