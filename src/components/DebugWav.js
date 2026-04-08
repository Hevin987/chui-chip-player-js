import React, { useRef, useEffect, useState } from 'react';

function writeString(view, offset, string) {
  for (let i = 0; i < string.length; i++) {
    view.setUint8(offset + i, string.charCodeAt(i));
  }
}

function generateWavBlob(map, frames, voices, bufferSize, v, sampleRate) {
  const dataLen = frames * bufferSize;
  const channelData = new Int8Array(dataLen);
  
  // Our visualMap is organized by [frame][voice][bufferSize]
  for (let f = 0; f < frames; f++) {
    const srcOffset = f * voices * bufferSize + v * bufferSize;
    channelData.set(map.subarray(srcOffset, srcOffset + bufferSize), f * bufferSize);
  }

  const buffer = new ArrayBuffer(44 + dataLen);
  const view = new DataView(buffer);
  
  // RIFF chunk
  writeString(view, 0, 'RIFF');
  view.setUint32(4, 36 + dataLen, true);
  writeString(view, 8, 'WAVE');
  
  // FMT chunk
  writeString(view, 12, 'fmt ');
  view.setUint32(16, 16, true);
  view.setUint16(20, 1, true); // PCM
  view.setUint16(22, 1, true); // Mono
  view.setUint32(24, sampleRate, true);
  view.setUint32(28, sampleRate * 1, true); // ByteRate
  view.setUint16(32, 1, true); // BlockAlign
  view.setUint16(34, 8, true); // 8-bit
  
  // Data chunk
  writeString(view, 36, 'data');
  view.setUint32(40, dataLen, true);
  
  // Write and convert from signed 8-bit (-128..127) to unsigned 8-bit (0..255) for standard WAV
  const dataView = new Uint8Array(buffer, 44);
  for (let i = 0; i < dataLen; i++) {
    dataView[i] = channelData[i] + 128;
  }
  
  return new Blob([buffer], { type: 'audio/wav' });
}

export default function DebugWav({ player, onClose }) {
  const canvasRef = useRef(null);
  const [wavLinks, setWavLinks] = useState([]);

  useEffect(() => {
    if (!player || !player.visualMap) return;

    // Generate WAV Blobs
    const map = player.visualMap;
    const frames = player.totalVisualFrames;
    const bufferSize = player.bufferSize;
    const voices = player.maxVoices || 1;
    const sampleRate = player.sampleRate || 44100;
    
    let links = [];
    if (frames > 0) {
      for (let v = 0; v < voices; v++) {
        const blob = generateWavBlob(map, Math.min(frames, map.length / (voices * bufferSize)), voices, bufferSize, v, sampleRate);
        const url = URL.createObjectURL(blob);
        links.push({ voice: v, url });
      }
      setWavLinks(links);
    }

    return () => {
      links.forEach(lk => URL.revokeObjectURL(lk.url));
    };

    // Draw canvas
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    
    const width = canvas.width;
    const height = canvas.height;
    ctx.fillStyle = '#000';
    ctx.fillRect(0, 0, width, height);

    const totalSamples = frames * bufferSize;
    if (totalSamples === 0) return;

    const rowHeight = height / voices;
    ctx.lineWidth = 1;
    
    for (let v = 0; v < voices; v++) {
      const startY = v * rowHeight + rowHeight / 2;
      
      ctx.strokeStyle = '#333';
      ctx.beginPath();
      ctx.moveTo(0, startY);
      ctx.lineTo(width, startY);
      ctx.stroke();

      ctx.strokeStyle = `hsl(${(v / voices) * 360}, 100%, 50%)`;
      ctx.beginPath();

      const samplesPerPixel = Math.max(1, Math.floor(totalSamples / width));

      for (let x = 0; x < width; x++) {
        const sampleIdx = x * samplesPerPixel;
        if (sampleIdx >= totalSamples) break;

        const frame = Math.floor(sampleIdx / bufferSize);
        const subIndex = sampleIdx % bufferSize;
        const offset = frame * voices * bufferSize + v * bufferSize + subIndex;

        let min = 127, max = -128;
        for (let i = 0; i < samplesPerPixel; i++) {
            const f = Math.floor((sampleIdx + i) / bufferSize);
            const si = (sampleIdx + i) % bufferSize;
            if (f >= frames) break;
            const idx = f * voices * bufferSize + v * bufferSize + si;
            if (idx >= map.length) break;
            const val = map[idx];
            if (val < min) min = val;
            if (val > max) max = val;
        }

        const upperY = startY - ((max / 128.0) * (rowHeight / 2));
        const lowerY = startY - ((min / 128.0) * (rowHeight / 2));

        if (x === 0) {
          ctx.moveTo(x, upperY);
        } else {
          ctx.lineTo(x, upperY);
          ctx.lineTo(x, lowerY);
        }
      }
      ctx.stroke();
    }
  }, [player]);

  return (
    <div style={{ position: 'fixed', top: '5%', left: '5%', width: '90%', height: '90%', backgroundColor: '#222', zIndex: 9999, border: '2px solid #555', display: 'flex', flexDirection: 'column', color: '#fff', fontSize: '14px' }}>
      <div style={{ padding: '8px', background: '#333', display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <span>Pre-rendered WAV Debug ({player?.maxVoices} channels)</span>
        <button onClick={onClose} style={{ cursor: 'pointer', padding: '4px 12px', background: '#e74c3c', color: 'white', border: 'none', borderRadius: '4px' }}>Close</button>
      </div>
      <div style={{ padding: '8px', background: '#444', display: 'flex', gap: '8px', flexWrap: 'wrap' }}>
        <strong style={{ alignSelf: 'center' }}>Listen to Channels:</strong>
        {wavLinks.map(lk => (
          <a key={lk.voice} href={lk.url} target="_blank" rel="noopener noreferrer" style={{ padding: '4px 8px', background: '#3498db', color: 'white', textDecoration: 'none', borderRadius: '4px' }}>
            Ch {lk.voice}
          </a>
        ))}
      </div>
      <div style={{ flex: 1, padding: '8px', overflow: 'hidden' }}>
        <canvas ref={canvasRef} width={1000} height={800} style={{ width: '100%', height: '100%', background: '#000' }} />
      </div>
    </div>
  );
}
