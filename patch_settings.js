function handleVisualizerChange(val, updateSettings) {
  if (val === 'none') {
    updateSettings({ visualizerEnabled: false });
  } else if (val === 'piano') {
    updateSettings({ visualizerEnabled: true, visualizerType: 'spectrogram' });
  } else if (val === 'oscilloscope') {
    updateSettings({ visualizerEnabled: true, visualizerType: 'oscilloscope' });
  }
}
