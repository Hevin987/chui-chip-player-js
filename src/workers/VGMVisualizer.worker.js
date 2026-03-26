import autoBind from 'auto-bind';
import ChipCore from '../chip-core';

self.onmessage = async (e) => {
  console.log("Worker building VGM", e.data);
  const core = await new ChipCore({
    noInitialRun: true,
    locateFile: (path) => {
      // In a worker, this might need an absolute path or relative to something. We will see.
      return '/' + path;
    }
  });
  self.postMessage({ status: 'ready' });
};
