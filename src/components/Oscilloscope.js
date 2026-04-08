import React, { Component } from 'react';

export default class Oscilloscope extends Component {
  constructor(props) {
    super(props);
    this.canvasRef = React.createRef();
    this.wrapperRef = React.createRef();
    this.animFrameId = null;
  }

  componentDidMount() {
    this.draw();
  }

  componentWillUnmount() {
    if (this.animFrameId) {
      cancelAnimationFrame(this.animFrameId);
    }
  }

  toggleFullscreen = () => {
    if (!document.fullscreenElement) {
      if (this.wrapperRef.current) {
        this.wrapperRef.current.requestFullscreen().catch(err => {
          console.log(`Error attempting to enable full-screen mode: ${err.message}`);
        });
      }
    } else {
      document.exitFullscreen();
    }
  };

  draw = () => {
    this.animFrameId = requestAnimationFrame(this.draw);

    if (!this.props.enabled || this.props.paused) return;

    const canvas = this.canvasRef.current;
    if (!canvas) return;

    // Dynamically adjust internal resolution to keep crisp rendering when fullscreen
    if (canvas.width !== canvas.clientWidth && canvas.clientWidth > 0) {
      canvas.width = canvas.clientWidth;
    }
    if (canvas.height !== canvas.clientHeight && canvas.clientHeight > 0) {
      canvas.height = canvas.clientHeight;
    }

    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;

    ctx.fillStyle = 'rgba(0, 0, 0, 0.9)'; // Darker background
    ctx.fillRect(0, 0, width, height);

    ctx.lineWidth = 2;
    // Cool retro CRT glow effect
    ctx.shadowBlur = 4;
    ctx.shadowColor = '#00ffcc';
    ctx.strokeStyle = '#00ffcc';

    if (typeof window === "undefined" || !window.voiceBuffers || window.voiceBuffers.length === 0) return;

    let numChannels = window.voiceBuffers.length;
    if (numChannels === 1 && window.voiceBuffers[0].every(val => val === 0)) {
      numChannels = 1;
    } 

    let activeChannels = [];
    for (let i = 0; i < numChannels; i++) activeChannels.push(i);

    let displayChannels = activeChannels.length;
    let cols = 1;
    if (displayChannels > 1) {
      if (document.fullscreenElement) {
        // Aim for very wide channels in fullscreen. e.g. 1920 -> 3 cols
        cols = Math.max(1, Math.floor(width / 500));
        if (cols > 3) cols = 3;
      } else {
        cols = 2; // Always default to 2 columns for normal view if > 1 ch
      }
      if (cols > displayChannels) cols = displayChannels;
    }
    
    // Dynamically calculate rows needed for up to 16+ channels (like Genesis 10 ch)
    const rows = Math.max(1, Math.ceil(displayChannels / cols)); 
    
    // Draw dynamic grid lines
    ctx.shadowBlur = 0;
    ctx.strokeStyle = '#440000'; // Dark red like Image 1
    ctx.lineWidth = 1;
    ctx.beginPath();
    
    // Draw vertical center lines based on cols
    if (cols > 1) {
      for (let c = 1; c < cols; c++) {
        ctx.moveTo((c * width) / cols, 0); 
        ctx.lineTo((c * width) / cols, height);
      }
    }
    
    for(let r = 1; r < rows; r++) {
       ctx.moveTo(0, (r * height) / rows); ctx.lineTo(width, (r * height) / rows);
    }
    ctx.stroke();

    ctx.lineWidth = 2;
    ctx.shadowBlur = 4;

    const channelWidth = width / cols;
    const channelHeight = height / rows;

    // Corrscope style distinct neon tracker colors
    const colors = [
      '#ff3366', // Pink/Red
      '#33ccff', // Light Blue
      '#ffcc00', // Yellow
      '#33ff66', // Lime Green
      '#cc33ff', // Purple
      '#ff9933', // Orange
      '#3366ff', // Deep Blue
      '#00ffcc', // Cyan/Teal
      '#ff66cc', // Hot Pink
      '#66ffcc', // Sea Green
      '#ccff00', // Electric Lime
      '#66ccff', // Sky Blue
      '#ff0033', // Crimson
      '#00ccff', // Azure
      '#cc00ff', // Violet
      '#ffcc33'  // Gold
    ];

    // Track the absolute global peak to properly scale all waves relative to each other.
    // We do NOT decay this quickly, otherwise quiet channels get auto-scaled up to 100% height when loud channels stop!
    if (!window.globalVoiceMax) window.globalVoiceMax = 0.01;
    let currentFrameMax = 0.0;
    for (let idx = 0; idx < displayChannels && idx < (cols * rows); idx++) {
      const c = activeChannels[idx];
      const buffer = window.oscilloscopeHistory ? window.oscilloscopeHistory[c] : window.voiceBuffers[c];
      if (buffer) {
        let minVal = 1.0;
        let maxVal = -1.0;
        // Sample for max peak-to-peak amplitude in this frame
        for (let i = 0; i < buffer.length; i+=16) {
          if (buffer[i] < minVal) minVal = buffer[i];
          if (buffer[i] > maxVal) maxVal = buffer[i];
        }
        const amplitude = (maxVal - minVal) / 2.0;
        if (amplitude > currentFrameMax) currentFrameMax = amplitude;
      }
    }
    // Only lock to the highest peak ever seen so relative volumes between channels remain authentic!
    if (currentFrameMax > window.globalVoiceMax) {
       window.globalVoiceMax = currentFrameMax;
    } else {
       // Extremely slow decay (over ~3 minutes) to adjust if next track is much quieter
       window.globalVoiceMax = window.globalVoiceMax * 0.9998;
    }
    
    // Add a bit of headroom (0.9 multiplier in fullscreen). 
    // In windowed mode (not fullscreen), boost the scale by 1.5x so the waves look taller!
    const baseScale = document.fullscreenElement ? 0.9 : 1.35;
    const targetScale = baseScale / Math.max(0.01, window.globalVoiceMax);

    for (let idx = 0; idx < displayChannels && idx < (cols * rows); idx++) {
      const c = activeChannels[idx];
      ctx.strokeStyle = colors[idx % colors.length];
      ctx.shadowColor = colors[idx % colors.length];

      const buffer = window.oscilloscopeHistory ? window.oscilloscopeHistory[c] : window.voiceBuffers[c];
      if (!buffer) continue;
      
      const bufferLength = buffer.length;

      const col = idx % cols;
      const row = Math.floor(idx / cols);

      // Draw Channel Label without blur
      ctx.shadowBlur = 0;
      ctx.fillStyle = '#ffffff';
      ctx.font = "bold 14px monospace";
      const channelName = (typeof window !== "undefined" && window.voiceNames && window.voiceNames[c]) ? window.voiceNames[c] : `Channel ${c}`;
      const isLikelyNoiseChannel = /noise|snare|hat|cymbal|rim|tom|drum/i.test(channelName);
      ctx.fillText(channelName, col * channelWidth + 10, row * channelHeight + 24);

      // Restore blur for the neon wave styling
      ctx.shadowBlur = 4;

      // Scale drawn samples by width so it shows more wave in fullscreen (density)
      const targetSamples = Math.min(bufferLength, Math.max(512, Math.floor(channelWidth * 2.0)));
      const triggerSamples = Math.min(targetSamples, Math.max(192, Math.floor(targetSamples * 0.28)));

      // Find min, max for DC offset calculation
      let minVal = 1.0;
      let maxVal = -1.0;
      for (let i = 0; i < bufferLength; i++) {
        if (buffer[i] < minVal) minVal = buffer[i];
        if (buffer[i] > maxVal) maxVal = buffer[i];
      }
      
      const average = (maxVal + minVal) / 2.0;

      // Deterministic trigger: lock to a rising edge nearest a fixed anchor
      // close to the right side of the trailing history window.
      const latestStart = Math.max(0, bufferLength - targetSamples);

      let startIdx = latestStart;

      if (maxVal - minVal > 0.005) {
        if (isLikelyNoiseChannel) {
          startIdx = latestStart;
        } else {
          const amplitude = maxVal - minVal;
          const bestOffset = this.findTriggerOffset(buffer, average, amplitude, latestStart);
          startIdx = bestOffset >= 0 ? bestOffset : latestStart;
        }
      } else {
        // Silence/noise floor
        startIdx = latestStart;
      }

      // Safeguard boundaries
      startIdx = Math.max(0, Math.min(startIdx, bufferLength - targetSamples));
      const actualSamplesToDraw = Math.min(targetSamples, bufferLength - startIdx);
      
      // Calculate sliceWidth dynamically based on actualSamplesToDraw so the wave PERFECTLY spans 
      // the full width of the channel box without abruptly ending when startIdx consumes some buffer!
      const sliceWidth = channelWidth / Math.max(1, actualSamplesToDraw);

      let x = col * channelWidth;
      const centerY = (row * channelHeight) + (channelHeight / 2);

      ctx.beginPath();

      // Scale with the dynamically tracked targetScale. 
      // This correctly shows true loudness envelopes bouncing naturally,
      // avoiding making every wave auto-scale to identically 100% height!
      for (let i = 0; i < actualSamplesToDraw; i++) {
        const raw_v = buffer[startIdx + i]; 
        
        // 0.5 puts silence on the center line. 
        // We multiply the raw AC swing by targetScale, then *0.5 so it spans 0.0 to 1.0 
        let normalizedPos = 0.5 + ((raw_v - average) * targetScale * 0.5); 
        
        // Map 0.0 - 1.0 (bottom to top) to actual pixels inside the channel box.
        // Leave 15% padding on top and bottom so it doesn't touch the borders when fullscreen.
        // When windowed, reduce padding so the wave can stretch taller!
        const padding = document.fullscreenElement ? (channelHeight * 0.15) : (channelHeight * 0.05);
        const drawableHeight = channelHeight - (padding * 2);
        
        // Y goes down. normalizedPos = 1.0 (peak) -> top of drawable height (padding).
        // normalizedPos = 0.0 (trough) -> bottom of drawable height (padding + drawable).
        let y = (row * channelHeight) + padding + ((1.0 - normalizedPos) * drawableHeight);
        
        // Clamp to box
        y = Math.max((row * channelHeight), Math.min(y, (row + 1) * channelHeight));

        if (i === 0) {
          ctx.moveTo(x, y);
        } else {
          ctx.lineTo(x, y);
        }

        x += sliceWidth;
      }

      ctx.stroke();
    }
  };

  // Trigger lock by selecting a Schmitt-style rising edge anchored near the right edge.
  findTriggerOffset = (buffer, level, amplitude, latestStart) => {
    const minIdx = Math.max(1, latestStart - 2048);
    const maxIdx = latestStart;
    const low = level - (amplitude * 0.08);
    const high = level + (amplitude * 0.08);

    // Deterministic choice: pick the latest valid rising edge before the right boundary.
    for (let i = maxIdx - 1; i >= minIdx; i--) {
      const prev = buffer[i - 1];
      const curr = buffer[i];
      if (curr >= high && prev <= low) {
        return i;
      }
    }

    return latestStart;
  };

  // Refine correlation anchor to nearest rising-edge crossing with strongest slope.
  refineTriggerEdge = (buffer, seedIdx, level, latestStart) => {
    const minIdx = Math.max(1, seedIdx - 32);
    const maxIdx = Math.min(latestStart, buffer.length - 2, seedIdx + 32);
    let bestIdx = seedIdx;
    let bestScore = -Infinity;

    for (let i = minIdx; i <= maxIdx; i++) {
      const prev = buffer[i - 1];
      const curr = buffer[i];
      if (curr >= level && prev < level) {
        const next = buffer[i + 1];
        const slope = next - prev;
        const distancePenalty = Math.abs(i - seedIdx) * 0.001;
        const score = slope - distancePenalty;
        if (score > bestScore) {
          bestScore = score;
          bestIdx = i;
        }
      }
    }

    return bestIdx;
  };

  render() {
    return (
      <div className="Oscilloscope" 
           ref={this.wrapperRef}
           onDoubleClick={this.toggleFullscreen}
           style={{ display: this.props.enabled ? 'block' : 'none', position: 'absolute', top: 0, left: 0, width: '100%', height: '100%' }}>
        <button
          className="box-button"
          onClick={this.toggleFullscreen}
          title="Toggle Fullscreen"
          style={{ position: 'absolute', bottom: 8, right: 8, zIndex: 10, padding: '2px 8px', fontSize: '10px' }}
        >
          [ ]
        </button>
        <canvas
          ref={this.canvasRef}
          width={800}
          height={600}
          style={{ width: '100%', height: '100%', display: 'block', cursor: 'pointer' }}
        />
      </div>
    );
  }
}
