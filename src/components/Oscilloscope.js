import React, { Component } from 'react';

export default class Oscilloscope extends Component {
  constructor(props) {
    super(props);
    this.canvasRef = React.createRef();
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

  draw = () => {
    this.animFrameId = requestAnimationFrame(this.draw);

    if (!this.props.enabled || this.props.paused) return;

    const canvas = this.canvasRef.current;
    if (!canvas) return;

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

    ctx.shadowBlur = 0;
    ctx.strokeStyle = '#113322'; 
    ctx.lineWidth = 1;
    ctx.beginPath();
    // Draw 2x4 grid lines
    ctx.moveTo(width / 2, 0); ctx.lineTo(width / 2, height);
    for(let r = 1; r < 4; r++) {
       ctx.moveTo(0, (r * height) / 4); ctx.lineTo(width, (r * height) / 4);
    }
    ctx.stroke();

    ctx.lineWidth = 2;
    ctx.shadowBlur = 4;

    if (typeof window === "undefined" || !window.voiceBuffers) return;

    const numChannels = window.voiceBuffers.length;
    const cols = 2;
    const rows = 4; // 2x4 grid
    
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
      '#00ffcc'  // Cyan/Teal
    ];

    for (let c = 0; c < numChannels && c < (cols * rows); c++) {
      ctx.strokeStyle = colors[c % colors.length];
      ctx.shadowColor = colors[c % colors.length];

      const buffer = window.voiceBuffers[c];
      if (!buffer) continue;
      
      const bufferLength = buffer.length;

      ctx.beginPath();
      // Fit wave in half the screen width
      const sliceWidth = channelWidth * 1.0 / bufferLength;
      
      const col = c % cols;
      const row = Math.floor(c / cols);
      
      let x = col * channelWidth;
      const centerY = (row * channelHeight) + (channelHeight / 2);

      // Trigger on zero-crossing for stable wave
      let startIdx = 0;
      for (let i = 0; i < bufferLength - 1; i++) {
          if (buffer[i] < 0 && buffer[i+1] >= 0) {
              startIdx = i;
              break;
          }
      }

      for (let i = startIdx; i < bufferLength; i++) {
        // v is typically between -1.0 and 1.0
        const v = buffer[i]; 
        const y = centerY - (v * (channelHeight / 2) * 0.9);

        if (i === startIdx) {
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
      <div className="Oscilloscope" style={{ display: this.props.enabled ? 'block' : 'none', position: 'absolute', top: 0, left: 0, width: '100%', height: '100%' }}>
        <canvas
          ref={this.canvasRef}
          width={800}
          height={600}
          style={{ width: '100%', height: '100%', display: 'block' }}
        />
      </div>
    );
  }
}
