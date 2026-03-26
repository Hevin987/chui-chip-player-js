import TestWorker from 'worker-loader!./workers/test.worker.js';
const worker = new TestWorker();
worker.onmessage = (e) => console.log('Main thread received:', e.data);
worker.postMessage('Test');
