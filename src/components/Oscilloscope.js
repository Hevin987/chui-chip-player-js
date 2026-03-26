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
    } else {
      // Find true max channels by filtering out uninitialized or completely empty buffers (optional style choice)
    }

    const cols = 2;
    // Dynamically calculate rows needed for up to 16+ channels (like Genesis 10 ch)
    const rows = Math.max(1, Math.ceil(numChannels / cols)); 
    
    // Draw dynamic grid lines
    ctx.shadowBlur = 0;
    ctx.strokeStyle = '#440000'; // Dark red like Image 1
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(width / 2, 0); ctx.lineTo(width / 2, height);
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

    for (let c = 0; c < numChannels && c < (cols * rows); c++) {
      ctx.strokeStyle = colors[c % colors.length];
      ctx.shadowColor = colors[c % colors.length];

      const buffer = window.voiceBuffers[c];
      if (!buffer) continue;
      
      const bufferLength = buffer.length;

      const col = c % cols;
      const row = Math.floor(c / cols);

      // Draw Channel Label
      ctx.fillStyle = '#ffffff';
      ctx.font = "bold 14px monospace";
      const channelName = (typeof window !== "undefined" && window.voiceNames && window.voiceNames[c]) ? window.voiceNames[c] : `Channel ${c}`;
      ctx.fillText(channelName, col * channelWidth + 10, row * channelHeight + 24);

      // Find min, max, and DC average to handle unsigned waves (like SN76489 pulses)
      let minVal = 1.0;
      let maxVal = -1.0;
      let sum = 0.0;
      for (let i = 0; i < bufferLength; i++) {
        if (buffer[i] < minVal) minVal = buffer[i];
        if (buffer[i] > maxVal) maxVal = buffer[i];
        sum += buffer[i];
      }
      
      const average = sum / bufferLength;
      let triggerLevel = average;

      // Trigger on crossing the average for stable wave
      let startIdx = 0;
      if (maxVal - minVal > 0.001) { // Only try to sync if there's actual amplitude
        for (let i = 0; i < bufferLength - 1; i++) {
            if (buffer[i] <= triggerLevel && buffer[i+1] > triggerLevel) {
                startIdx = i;
                break;
            }
        }
      }

      // If drawing window would exceed buffer given the trigger, pull it back so we don't truncate
      const DRAW_SAMPLES = 512;
      if (startIdx + DRAW_SAMPLES > bufferLength) {
          startIdx = Math.max(0, bufferLength - DRAW_SAMPLES);
      }

      const actualSamplesToDraw = Math.min(DRAW_SAMPLES, bufferLength - startIdx);
      const sliceWidth = channelWidth / actualSamplesToDraw;

      let x = col * channelWidth;
      const centerY = (row * channelHeight) + (channelHeight / 2);

      ctx.beginPath();

      // Auto-scale to ensure the wave completely fills the channel height
      const amplitudeRange = maxVal - minVal;
      
      for (let i = 0; i < actualSamplesToDraw; i++) {
        const raw_v = buffer[startIdx + i]; 
        
        let normalizedPos = 0.5; // Default to center line for complete silence
        if (amplitudeRange > 0.001) { // Prevents dividing by zero or magnifying extreme noise
            normalizedPos = (raw_v - minVal) / amplitudeRange;
        }
        
        // Map 0.0 - 1.0 (bottom to top) to actual pixels inside the channel box.
        // Leave 15% padding on top and bottom so it doesn't touch the borders.
        const padding = channelHeight * 0.15;
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
