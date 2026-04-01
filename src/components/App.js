import React, { useContext } from 'react';
import autoBindReact from 'auto-bind/react';
import isMobile from 'ismobilejs';
import clamp from 'lodash/clamp';
import shuffle from 'lodash/shuffle';
import path from 'path';
import queryString from 'querystring';
import { withRouter } from 'react-router-dom';
import Dropzone from 'react-dropzone';

import ChipCore from '../chip-core';
import {
  MAX_VOICES,
  REPLACE_STATE_ON_SEEK,
  SOUNDFONT_MOUNTPOINT,
  MAX_SAMPLE_RATE,
  FORMATS,
} from '../config';
import {
  ensureEmscFileWithData,
  titlesFromMetadata,
  unlockAudioContext
} from '../util';
import LocalFilesManager from '../LocalFilesManager';
import Sequencer, { NUM_REPEAT_MODES, NUM_SHUFFLE_MODES, REPEAT_OFF, SHUFFLE_OFF } from '../Sequencer';

import GMEPlayer from '../players/GMEPlayer';
import MIDIPlayer from '../players/MIDIPlayer';
import V2MPlayer from '../players/V2MPlayer';
import XMPPlayer from '../players/XMPPlayer';
import N64Player from '../players/N64Player';
import MDXPlayer from '../players/MDXPlayer';
import VGMPlayer from '../players/VGMPlayer';

import AppFooter from './AppFooter';
import DropMessage from './DropMessage';
import Visualizer from './Visualizer';
import Oscilloscope from './Oscilloscope';
import Toast, { ToastLevels } from './Toast';
import MessageBox from './MessageBox';
import Settings from './Settings';
import LocalFiles from './LocalFiles';
import { UserContext } from './UserProvider';
import { ToastContext } from './ToastProvider';

const BASE_URL = process.env.PUBLIC_URL || document.location.origin;

class App extends React.Component {
  constructor(props) {
    super(props);
    autoBindReact(this);

    this.attachMediaKeyHandlers();
    this.contentAreaRef = React.createRef();
    this.listRef = React.createRef(); // react-virtualized List component ref
    this.playContexts = {};
    this.midiPlayer = null; // Need a reference to MIDIPlayer to handle SoundFont loading.
    window.ChipPlayer = this;


    // Initialize audio graph
    // ┌────────────┐      ┌────────────┐      ┌─────────────┐
    // │ playerNode ├─────>│  gainNode  ├─────>│ destination │
    // └────────────┘      └────────────┘      └─────────────┘

    // Smaller buffer for mobile devices. 'interactive' yields 128 samples on iOS/Android.
    const latencyHint = isMobile.any ? 'interactive' : 'playback';
    let audioCtx = this.audioCtx = window.audioCtx = new (window.AudioContext || window.webkitAudioContext)({
      latencyHint,
    });

    // Limit the sample rate if needed
    if (audioCtx.sampleRate > MAX_SAMPLE_RATE) {
      console.warn("AudioContext default sample rate was too high (%s). Limiting to %s.", audioCtx.sampleRate, MAX_SAMPLE_RATE);
      let targetRate = audioCtx.sampleRate;
      while (targetRate > MAX_SAMPLE_RATE) {
        targetRate /= 2;
      }
      audioCtx = this.audioCtx = window.audioCtx = new (window.AudioContext || window.webkitAudioContext)({
        latencyHint,
        sampleRate: targetRate,
      });
    }

    const bufferSize = Math.max( // Make sure script node bufferSize is at least baseLatency
      Math.pow(2, Math.ceil(Math.log2((audioCtx.baseLatency || 0.001) * audioCtx.sampleRate))), 512);
    const gainNode = this.gainNode = audioCtx.createGain();
    gainNode.gain.value = 1;
    gainNode.connect(audioCtx.destination);
    const playerNode = this.playerNode = audioCtx.createScriptProcessor(bufferSize, 0, 2);
    playerNode.connect(gainNode);

    unlockAudioContext(audioCtx);
    console.log('Sample rate: %d hz. Base latency: %d. Buffer size: %d.',
      audioCtx.sampleRate, audioCtx.baseLatency * audioCtx.sampleRate, bufferSize);

    this.state = {
      loading: true,
      loadingLocalFiles: true,
      paused: true,
      ejected: true,
      currentSongMetadata: {},
      currentSongNumVoices: 0,
      currentSongNumSubtunes: 0,
      currentSongSubtune: 0,
      currentSongDurationMs: 1,
      currentSongPositionMs: 0,
      tempo: 1,
      voiceMask: Array(MAX_VOICES).fill(true),
      voiceNames: Array(MAX_VOICES).fill(''),
      voiceGroups: [],
      imageUrl: null,
      infoTexts: [],
      showInfo: false,
      songPath: null,
      volume: 100,
      repeat: REPEAT_OFF,
      shuffle: SHUFFLE_OFF,
      hasPlayer: false,
      paramDefs: [],
      paramValues: {},
      // Special playable contexts
      localFiles: [],
    };

    this.initChipCore(audioCtx, playerNode, bufferSize);
  }

  async initChipCore(audioCtx, playerNode, bufferSize) {
    // Load the chip-core Emscripten runtime
    try {
      this.chipCore = await new ChipCore({
        // Look for .wasm file in web root, not the same location as the app bundle (static/js).
        locateFile: (path, prefix) => {
          if (path.endsWith('.wasm') || path.endsWith('.wast'))
            return `${BASE_URL}/chipPlayer/${path}`;
          return prefix + path;
        },
        print: (msg) => console.debug('[stdout] ' + msg),
        printErr: (msg) => console.debug('[stderr] ' + msg),
      });
    } catch (e) {
      console.error("Failed to load chip-core WebAssembly:", e);
      // Browser doesn't support WASM (Safari in iOS Simulator)
      this.setState({ loading: false });
      this.props.toastContext.enqueueToast('Error loading player engine. Old browser?', ToastLevels.ERROR);
      return;
    }

    // Get debug from location.search
    const debug = queryString.parse(window.location.search.substring(1)).debug;
    // Create all the players. Players will set up IDBFS mount points.
    const players = [
      MIDIPlayer,
      GMEPlayer,
      XMPPlayer,
      V2MPlayer,
      N64Player,
      MDXPlayer,
      VGMPlayer,
    ].map(P => new P(this.chipCore, audioCtx.sampleRate, bufferSize, debug));
    players.forEach(p => {
      p.audioNode = this.playerNode;
    });
    this.midiPlayer = players[0];

    // Set up the central audio processing callback. This is where the magic happens.
    playerNode.onaudioprocess = (e) => {
      const channels = [];
      for (let i = 0; i < e.outputBuffer.numberOfChannels; i++) {
        channels.push(e.outputBuffer.getChannelData(i));
      }
      for (let player of players) {
        if (player.stopped) continue;
        player.processAudio(channels);
      }
      
      // Update global oscilloscope history so we can draw 60fps *AND* have 4096 samples to find perfect triggers!
      if (typeof window !== 'undefined' && window.voiceBuffers) {
        if (!window.oscilloscopeHistory) window.oscilloscopeHistory = [];
        for (let c = 0; c < window.voiceBuffers.length; c++) {
          if (window.voiceBuffers[c]) {
            if (!window.oscilloscopeHistory[c]) window.oscilloscopeHistory[c] = new Float32Array(4096);
            const hist = window.oscilloscopeHistory[c];
            const buf = window.voiceBuffers[c];
            const len = buf.length;
            hist.copyWithin(0, len);
            hist.set(buf, hist.length - len);
          }
        }
      }
    }

    // Create localFilesManager before performing the syncfs.
    this.localFilesManager = new LocalFilesManager(this.chipCore.FS, 'local');

    // Populate all mounted IDBFS file systems from IndexedDB.
    this.chipCore.FS.syncfs(true, (err) => {
      if (err) {
        console.log('Error populating FS from indexeddb.', err);
      }
      players.forEach(player => player.handleFileSystemReady());
      this.updateLocalFiles();
      this.setState({ loadingLocalFiles: false });
    });

    this.sequencer = new Sequencer(players, this.localFilesManager, () => this.props.userContext.settings);
    this.sequencer.on('sequencerStateUpdate', this.handleSequencerStateUpdate);
    this.sequencer.on('playerError', (message) => this.props.toastContext.enqueueToast(message, ToastLevels.ERROR));

    // Handle files opened via OS file association (PWA File Handling API).
    if ('launchQueue' in window) {
      window.launchQueue.setConsumer(async (launchParams) => {
        if (launchParams.files && launchParams.files.length > 0) {
          const fileHandles = launchParams.files;
          const files = await Promise.all(fileHandles.map(h => h.getFile()));
          this.handleFiles(files);
        }
      });
    }

    this.setState({ loading: false }, () => {
      // Auto-play a file passed via ?play=<path> from an external file browser.
      // Accepts:
      //   ?play=/catalog/song.vgm        (server-relative path, stripped to filename)
      //   ?play=Some%20Song.vgm          (filename, resolved under CATALOG_PREFIX)
      //   ?play=http://host/path/song.vgm (full URL, used as-is)
      const params = queryString.parse(window.location.search.substring(1));
      if (params.play) {
        let playPath = params.play;
        let queuePaths = [playPath];

        try {
          const storedQueue = sessionStorage.getItem('chipPlayerQueue');
          if (storedQueue) {
            queuePaths = JSON.parse(storedQueue);
            sessionStorage.removeItem('chipPlayerQueue');
          }
        } catch (e) {
          console.error("Failed to read chipPlayerQueue", e);
        }

        const resolvePath = (p) => {
          if (!p.startsWith('http')) {
            return window.location.origin + (p.startsWith('/') ? '' : '/') + p;
          }
          return p;
        };

        const resolvedPaths = queuePaths.map(resolvePath);
        const resolvedPlayPath = resolvePath(playPath);
        let playIdx = resolvedPaths.findIndex(p => p === resolvedPlayPath);
        if (playIdx < 0) playIdx = 0;

        // Resume AudioContext — Chrome transfers user activation from the
        // page that triggered the navigation, allowing immediate playback.
        this.audioCtx.resume();
        this.sequencer.playContext(resolvedPaths, playIdx);
      }
    });
  }

  static mapSequencerStateToAppState(sequencerState) {
    const map = {
      ejected: 'isEjected',
      paused: 'isPaused',
      currentSongSubtune: 'subtune',
      currentSongMetadata: 'metadata',
      currentSongNumVoices: 'numVoices',
      currentSongPositionMs: 'positionMs',
      currentSongDurationMs: 'durationMs',
      currentSongNumSubtunes: 'numSubtunes',
      tempo: 'tempo',
      voiceNames: 'voiceNames',
      voiceMask: 'voiceMask',
      voiceGroups: 'voiceGroups',
      songPath: 'songPath',
      hasPlayer: 'hasPlayer',
      // TODO: Move to a separate paramStateUpdate?
      paramDefs: 'paramDefs',
      paramValues: 'paramValues',
      infoTexts: 'infoTexts',
    };
    const appState = {};
    for (let prop in map) {
      const seqProp = map[prop];
      if (seqProp in sequencerState) {
        appState[prop] = sequencerState[seqProp];
      }
    }
    return appState;
  }

  attachMediaKeyHandlers() {
    if ('mediaSession' in navigator) {
      console.log('Attaching Media Key event handlers.');

      // Limitations of MediaSession: there must always be an active audio element.
      // See https://bugs.chromium.org/p/chromium/issues/detail?id=944538
      //     https://github.com/GoogleChrome/samples/issues/637
      this.mediaSessionAudio = document.createElement('audio');
      this.mediaSessionAudio.src = BASE_URL + '/chipPlayer/5-seconds-of-silence.mp3';
      this.mediaSessionAudio.loop = true;
      this.mediaSessionAudio.volume = 0;

      navigator.mediaSession.setActionHandler('play', () => this.togglePause());
      navigator.mediaSession.setActionHandler('pause', () => this.togglePause());
      navigator.mediaSession.setActionHandler('previoustrack', () => this.prevSong());
      navigator.mediaSession.setActionHandler('nexttrack', () => this.nextSong());
      navigator.mediaSession.setActionHandler('seekbackward', () => this.seekRelative(-5000));
      navigator.mediaSession.setActionHandler('seekforward', () => this.seekRelative(5000));
    }

    document.addEventListener('keydown', (e) => {
      // Keyboard shortcuts: tricky to get it just right and keep the browser behavior intact.
      // The order of switch-cases matters. More privileged keys appear at the top.
      // More restricted keys appear at the bottom, after various input focus states are filtered out.
      if (e.ctrlKey || e.metaKey) return; // avoid browser keyboard shortcuts

      switch (e.key) {
        case 'Escape':
          this.setState({ showInfo: false });
          e.target.blur();
          break;
        default:
      }

      if (e.target.tagName === 'INPUT' && e.target.type === 'text') return; // text input has focus

      switch (e.key) {
        case ' ':
          this.togglePause();
          e.preventDefault();
          break;
        case '-':
          this.setSpeedRelative(-0.1);
          break;
        case '_':
          this.setSpeedRelative(-0.01);
          break;
        case '=':
          this.setSpeedRelative(0.1);
          break;
        case '+':
          this.setSpeedRelative(0.01);
          break;
        default:
      }

      if (e.target.tagName === 'INPUT' && e.target.type === 'range') return; // a range slider has focus

      switch (e.key) {
        case 'ArrowLeft':
          this.seekRelative(-5000);
          e.preventDefault();
          break;
        case 'ArrowRight':
          this.seekRelative(5000);
          e.preventDefault();
          break;
        default:
      }
    });
  }

  playContext(context, index = 0, subtune = 0) {
    this.sequencer.playContext(context, index, subtune);
  }

  prevSong() {
    this.sequencer.prevSong();
  }

  nextSong() {
    this.sequencer.nextSong();
  }

  prevSubtune() {
    this.sequencer.prevSubtune();
  }

  nextSubtune() {
    this.sequencer.nextSubtune();
  }

  handleSequencerStateUpdate(sequencerState) {
    const { isEjected } = sequencerState;
    console.debug('App.handleSequencerStateUpdate(isEjected=%s)', isEjected);

    if (isEjected) {
      this.setState({
        ejected: true,
        currentSongSubtune: 0,
        currentSongMetadata: {},
        currentSongNumVoices: 0,
        currentSongPositionMs: 0,
        currentSongDurationMs: 1,
        currentSongNumSubtunes: 0,
        imageUrl: null,
        songPath: null,
      });
      // TODO: Disabled to support scroll restoration.
      // updateQueryString({ play: undefined });

      if ('mediaSession' in navigator) {
        this.mediaSessionAudio.pause();

        navigator.mediaSession.playbackState = 'none';
        if ('MediaMetadata' in window) {
          navigator.mediaSession.metadata = new window.MediaMetadata({});
        }
      }
    } else {
      const player = this.sequencer.getPlayer();
      const songPath = this.sequencer.getCurrSongPath();
      // Local files do not have remote artwork; clear image URL on song change.
      if (!songPath || songPath !== this.state.songPath) {
        this.setState({ imageUrl: null, infoTexts: [] });
      }

      const metadata = player.getMetadata();

      if ('mediaSession' in navigator) {
        this.mediaSessionAudio.play();

        if ('MediaMetadata' in window) {
          navigator.mediaSession.metadata = new window.MediaMetadata({
            title: metadata.title || metadata.formatted?.title,
            artist: metadata.artist || metadata.formatted?.subtitle,
            album: metadata.game,
            artwork: []
          });
        }
      }

      this.setState({
        ...App.mapSequencerStateToAppState(sequencerState),
      });
    }
  }

  togglePause() {
    if (this.state.ejected || !this.sequencer.getPlayer()) return;

    const paused = this.sequencer.getPlayer().togglePause();
    if ('mediaSession' in navigator) {
      if (paused) {
        this.mediaSessionAudio.pause();
      } else {
        this.mediaSessionAudio.play();
      }
    }
    this.setState({ paused: paused });
  }

  handleTimeSliderChange(event) {
    if (!this.sequencer.getPlayer()) return;

    const pos = event.target ? event.target.value : event;
    const seekMs = Math.floor(pos * this.state.currentSongDurationMs);

    this.seekRelativeInner(seekMs);

    if (REPLACE_STATE_ON_SEEK) {
      const urlParams = {
        ...queryString.parse(window.location.search.substr(1)),
        t: seekMs,
      };
      const stateUrl = '?' + queryString.stringify(urlParams)
        .replace(/%20/g, '+')
        .replace(/%2F/g, '/');
      window.history.replaceState(null, '', stateUrl);
    }
  }

  seekRelative(ms) {
    if (!this.sequencer.getPlayer()) return;

    const durationMs = this.state.currentSongDurationMs;
    const seekMs = clamp(this.sequencer.getPlayer().getPositionMs() + ms, 0, durationMs);

    this.seekRelativeInner(seekMs);
  }

  seekRelativeInner(seekMs) {
    this.sequencer.getPlayer().seekMs(seekMs);
    this.setState({
      currentSongPositionMs: seekMs, // Smooth
    });
    setTimeout(() => {
      if (this.sequencer.getPlayer().isPlaying()) {
        this.setState({
          currentSongPositionMs: this.sequencer.getPlayer().getPositionMs(), // Accurate
        });
      }
    }, 100);
  }

  handleSetVoiceMask(voiceMask) {
    if (!this.sequencer.getPlayer()) return;

    this.sequencer.getPlayer().setVoiceMask(voiceMask);
    this.setState({ voiceMask: [...voiceMask] });
  }

  handleTempoChange(event) {
    if (!this.sequencer.getPlayer()) return;

    const value = parseFloat((event.target ? event.target.value : event)) || 1.0;
    this.sequencer.getPlayer().setTempo(value);
    this.setState({
      tempo: value
    });

    const { settings, updateSettings } = this.props.userContext;
    const persistedKey = 'tempo';
    if (settings[persistedKey] != null) {
      updateSettings({ [persistedKey]: value });
    }
  }

  handleParamChange(id, value) {
    if (!this.sequencer.getPlayer()) return;
    const player = this.sequencer.getPlayer();
    player.setParameter(id, value);
    this.setState(prevState => ({
      paramValues: { ...prevState.paramValues, [id]: value },
    }));

    const { settings, updateSettings } = this.props.userContext;
    const persistedKey = `${player.playerKey}.${id}`;
    if (settings[persistedKey] != null) {
      updateSettings({ [persistedKey]: value });
    }
  }

  handlePinParam(persistedKey, currentValue) {
    const { settings, replaceSettings } = this.props.userContext;
    const newSettings = { ...settings };

    if (newSettings[persistedKey] != null) {
      delete newSettings[persistedKey];
    } else {
      newSettings[persistedKey] = currentValue;
    }

    replaceSettings(newSettings);
  }

  setSpeedRelative(delta) {
    if (!this.sequencer.getPlayer()) return;

    const tempo = clamp(this.state.tempo + delta, 0.1, 2);
    this.sequencer.getPlayer().setTempo(tempo);
    this.setState({
      tempo: tempo
    });
  }

  handleShufflePlay(path) {
    if (path === 'local') {
      this.sequencer.playContext(shuffle(this.playContexts['local']));
    }
  }

  handleCycleShuffle() {
    const shuffle = (this.state.shuffle + 1) % NUM_SHUFFLE_MODES;
    this.setState({ shuffle });
    this.sequencer.setShuffle(shuffle);
  }

  handleSongClick(url, context, index) {
    return (e) => {
      e.preventDefault();
      if (context) {
        this.playContext(context, index);
      } else {
        this.sequencer.playSonglist([url]);
      }
    }
  }

  handleVolumeChange(volume) {
    this.setState({ volume });
    this.gainNode.gain.value = Math.max(0, Math.min(2, volume * 0.01));
  }

  handleCycleRepeat() {
    // TODO: Handle dropped file repeat
    const repeat = (this.state.repeat + 1) % NUM_REPEAT_MODES;
    this.setState({ repeat });
    this.sequencer.setRepeat(repeat);
  }

  toggleInfo() {
    this.setState({
      showInfo: !this.state.showInfo,
    });
  }

  updateLocalFiles() {
    const localFiles = this.localFilesManager.readAll();
    // Convert timestamp 1704067200 to ISO date 2024-01-01
    localFiles.forEach(item => item.mtime = new Date(item.mtime * 1000).toISOString().split('T')[0]);
    this.playContexts['local'] = localFiles.map(item => item.path);
    this.setState({ localFiles });
  }

  handleFiles = (files) => {
    const promises = files.map(file => {
      return new Promise((resolve, reject) => {
        // TODO: refactor, avoid creating new reader/handlers for every dropped file.
        const reader = new FileReader();
        reader.onerror = (event) => reject(event.target.error);
        reader.onload = async () => {
          const ext = path.extname(file.name).toLowerCase().substring(1);
          if (ext === 'sf2') {
            // Handle dropped Soundfont
            if (!this.midiPlayer) {
              reject('MIDIPlayer has not been created - unable to load SoundFont.');
            } else if (files.length !== 1) {
              reject('Soundfonts must be added one at a time, separate from other files.');
            } else {
              const sf2Path = `user/${file.name}`;
              await ensureEmscFileWithData(this.chipCore, `${SOUNDFONT_MOUNTPOINT}/${sf2Path}`, new Uint8Array(reader.result), /*forceWrite=*/true);
              this.midiPlayer.updateSoundfontParamDefs();
              this.midiPlayer.setParameter('soundfont', sf2Path, /*isTransient=*/false);
              // TODO: emit "paramDefsChanged" from player.
              // See https://reactjs.org/docs/integrating-with-other-libraries.html#integrating-with-model-layers
              this.forceUpdate();
              resolve(0);
            }
          } else if (FORMATS.includes(ext)) {
            // Handle dropped song file
            const songData = reader.result;
            this.localFilesManager.write(path.join('local', file.name), songData);
            resolve(1);
          } else {
            reject(`The file format ".${ext}" was not recognized.`);
          }
        };
        reader.readAsArrayBuffer(file);
      });
    });

    Promise.allSettled(promises).then(results => {
      const numSongsAdded = results
        .filter(result => result.status === 'fulfilled')
        .reduce((acc, result) => acc + result.value, 0);
      if (numSongsAdded > 0) {
        const currContextIsLocalFiles = this.sequencer?.getCurrContext() === this.playContexts['local'];
        this.updateLocalFiles();
        if (currContextIsLocalFiles) this.sequencer.context = this.playContexts['local'];
        this.forceUpdate();
        // Auto-play the first newly added file.
        setTimeout(() => {
          const newContext = this.playContexts['local'];
          if (newContext && newContext.length > 0) {
            const startIdx = Math.max(0, newContext.length - numSongsAdded);
            this.sequencer.playContext(newContext, startIdx);
          }
        }, 100);
      }
      // Display all rejection reasons with duplicate reasons removed.
      results.filter(result => result.status === 'rejected')
        .reduce((acc, result) => acc.includes(result.reason) ? acc : [ ...acc, result.reason ], [])
        .forEach((reason, i) => setTimeout(() => this.props.toastContext.enqueueToast(reason, ToastLevels.ERROR), i * 1500));
    });
  }

  onDrop = (droppedFiles) => {
    this.handleFiles(droppedFiles);
  };

  handleLocalFileDelete = (filePaths) => {
    if (!Array.isArray(filePaths)) filePaths = [filePaths];
    const currContextIsLocalFiles = this.sequencer?.getCurrContext() === this.playContexts['local'];
    let currIndexWasDeleted = false;
    filePaths.forEach(filePath => {
      const deleted = this.localFilesManager.delete(filePath);

      if (deleted && currContextIsLocalFiles) {
        const index = this.playContexts['local'].indexOf(filePath);
        if (index === this.sequencer.currIdx) currIndexWasDeleted = true;
        if (index <= this.sequencer.currIdx) {
          this.sequencer.currIdx--;
        }
      }
    });

    this.updateLocalFiles();
    if (currContextIsLocalFiles) this.sequencer.context = this.playContexts['local'];
    if (currIndexWasDeleted)     this.sequencer.nextSong();
  }

  handleCopyLink = (url) => {
    navigator.clipboard.writeText(url);
    this.props.toastContext.enqueueToast('Copied song link to clipboard.', ToastLevels.INFO);
  }

  handleToggleSettings = (e) => {
    const { settings, updateSettings } = this.props.userContext;
    const showPlayerSettings = settings?.showPlayerSettings;
    updateSettings({ showPlayerSettings: !showPlayerSettings });
  }

  render() {
    const { title, subtitle } = titlesFromMetadata(this.state.currentSongMetadata);
    const currContext = this.sequencer?.getCurrContext();
    const currIdx = this.sequencer?.getCurrIdx();
    const { settings, updateSettings } = this.props.userContext;
    const showPlayerSettings = settings?.showPlayerSettings;

    return (
      <Dropzone
        disableClick
        style={{}} // Required to clear Dropzone styles
        onDrop={this.onDrop}>{dropzoneProps => (
        <div className="App app-body">
          <main className="player-interface">
          <DropMessage dropzoneProps={dropzoneProps}/>
          <MessageBox showInfo={this.state.showInfo}
                      infoTexts={this.state.infoTexts}
                      toggleInfo={this.toggleInfo}/>
          <Toast/>

          {/* BEGIN: Main Player Window */}
          <section className="retro-window main-player-window" style={{ flexGrow: 1, display: 'flex', flexDirection: 'column', minWidth: 0 }}>
            <div className="retro-header retro-header-container">
              <span className="pixel-title text-header mTitle" style={{textTransform: 'uppercase'}}>{title || 'CHIP PLAYER JS'}</span>
              <button className="close-btn">
                <svg className="close-icon" viewBox="0 0 20 20" style={{ width: '1.25rem', height: '1.25rem', fill: 'currentColor' }}>
                  <path clipRule="evenodd" d="M4.293 4.293a1 1 0 011.414 0L10 8.586l4.293-4.293a1 1 0 111.414 1.414L11.414 10l4.293 4.293a1 1 0 01-1.414 1.414L10 11.414l-4.293 4.293a1 1 0 01-1.414-1.414L8.586 10 4.293 5.707a1 1 0 010-1.414z" fillRule="evenodd"></path>
                </svg>
              </button>
            </div>
            <div className="player-content" style={{ flexGrow: 1, display: 'flex', flexDirection: 'column' }}>
              <div className="visualization-area" style={{ flexGrow: 1, minHeight: 0 }}>
                <div className="album">
                  <div className="album-sticker-wrapper">
                    <div className="album-sticker">
                      {this.state.imageUrl ? 
                        <img alt="Album Art" className="album-sticker-img" src={this.state.imageUrl} />
                        :
                        <div className="album-sticker-img" style={{display: 'flex', alignItems: 'center', justifyContent: 'center'}}>No Art</div>
                      }
                      <div className="album-label">{subtitle || 'Unknown'}</div>
                    </div>
                  </div>
                </div>
                <div className="waveform-bg">
                  <div className="waveform-wrapper" style={{ width: '100%', height: '100%', position: 'relative' }}>
                    {!isMobile.phone && !this.state.loading && settings?.visualizerType !== 'oscilloscope' &&
                      <Visualizer audioCtx={this.audioCtx}
                                  sourceNode={this.playerNode}
                                  chipCore={this.chipCore}
                                  enabled={settings?.visualizerEnabled !== false}
                                  paused={this.state.ejected || this.state.paused}/>}
                    {!isMobile.phone && !this.state.loading && settings?.visualizerType === 'oscilloscope' &&
                      <Oscilloscope 
                                  enabled={settings?.visualizerEnabled !== false}
                                  paused={this.state.ejected || this.state.paused}/>}
                    {!isMobile.phone && !this.state.loading && settings?.visualizerEnabled !== false && (
                      <button
                        className="box-button"
                        style={{ position: 'absolute', top: 8, right: 8, zIndex: 10, padding: '2px 8px', fontSize: '10px' }}
                        onClick={() => updateSettings({ visualizerType: settings?.visualizerType === 'oscilloscope' ? 'spectrogram' : 'oscilloscope' })}
                        title="Toggle Visualizer Type"
                      >
                        Swap Visualizer
                      </button>
                    )}
                  </div>
                </div>
              </div>
              <div className="control-container">
                <AppFooter
                  currentSongDurationMs={this.state.currentSongDurationMs}
                  currentSongNumSubtunes={this.state.currentSongNumSubtunes}
                  currentSongNumVoices={this.state.currentSongNumVoices}
                  currentSongSubtune={this.state.currentSongSubtune}
                  ejected={this.state.ejected}
                  getCurrentSongLink={() => null}
                  handleCopyLink={this.handleCopyLink}
                  handleCycleRepeat={this.handleCycleRepeat}
                  handleCycleShuffle={this.handleCycleShuffle}
                  handleSetVoiceMask={this.handleSetVoiceMask}
                  handleTempoChange={this.handleTempoChange}
                  handleTimeSliderChange={this.handleTimeSliderChange}
                  handleVolumeChange={this.handleVolumeChange}
                  imageUrl={this.state.imageUrl}
                  infoTexts={this.state.infoTexts}
                  md5={this.state.md5}
                  nextSong={this.nextSong}
                  nextSubtune={this.nextSubtune}
                  paused={this.state.paused}
                  prevSong={this.prevSong}
                  prevSubtune={this.prevSubtune}
                  repeat={this.state.repeat}
                  shuffle={this.state.shuffle}
                  sequencer={this.sequencer}
                  songId={null}
                  songPath={this.state.songPath}
                  subtitle={subtitle}
                  tempo={this.state.tempo}
                  title={title}
                  toggleInfo={this.toggleInfo}
                  togglePause={this.togglePause}
                  voiceNames={this.state.voiceNames}
                  voiceMask={this.state.voiceMask}
                  volume={this.state.volume}
                />
              </div>
            </div>
          </section>

          {/* BEGIN: Selected Track Panel */}
          <section className="retro-window track-info-panel" style={{ display: 'flex', flexDirection: 'column', flexShrink: 0, width: '22rem' }}>
            <div className="retro-header retro-header-center">
              <button className={`tab ${!showPlayerSettings ? 'tab-selected' : ''}`} onClick={() => this.props.userContext.updateSettings({ showPlayerSettings: false })}>
                <span className="pixel-title text-header">Track</span>
              </button>
              <button className={`tab ${showPlayerSettings ? 'tab-selected' : ''}`} onClick={() => this.props.userContext.updateSettings({ showPlayerSettings: true })}>
                <span className="pixel-title text-header">Settings</span>
              </button>
            </div>

            <div className="track-panel-scroll custom-scrollbar track" style={{ display: !showPlayerSettings ? 'flex' : 'none', flexDirection: 'column', flex: 1 }}>
              <div style={{ flex: 1, marginTop: '1rem', minHeight: 0, overflowY: 'auto' }}>
                {this.state.loading ?
                  <p style={{ padding: '1em' }}>Loading player engine...</p>
                  :
                  <LocalFiles
                    loading={this.state.loadingLocalFiles}
                    onAddFiles={this.handleFiles}
                    handleShufflePlay={this.handleShufflePlay}
                    onSongClick={this.handleSongClick}
                    onDelete={this.handleLocalFileDelete}
                    playContext={this.playContexts['local']}
                    currContext={currContext}
                    currIdx={currIdx}
                    listing={this.state.localFiles}
                    title={title}
                    subtitle={subtitle}
                    ejected={this.state.ejected}
                    metadata={this.state.currentSongMetadata}/>
                }
              </div>
            </div>

            <div className="track-panel-scroll custom-scrollbar setting" style={{ display: showPlayerSettings ? 'flex' : 'none', flexDirection: 'column', flex: 1 }}>
              <Settings
                ejected={this.state.ejected}
                tempo={this.state.tempo}
                numVoices={this.state.currentSongNumVoices}
                voiceMask={this.state.voiceMask}
                voiceNames={this.state.voiceNames}
                voiceGroups={this.state.voiceGroups}
                onVoiceMaskChange={this.handleSetVoiceMask}
                onTempoChange={this.handleTempoChange}
                paramDefs={this.state.paramDefs}
                paramValues={this.state.paramValues}
                onParamChange={this.handleParamChange}
                onPinParam={this.handlePinParam}
                persistedSettings={settings}
                sequencer={this.sequencer}
              />
            </div>
          </section>
          </main>
        </div>
      )}</Dropzone>
    );
  }
}

// TODO: convert App to a function component and remove this.
// Inject contexts as props since class components only support a single context.
const AppWithContext = (props) => {
  const userContext = useContext(UserContext);
  const toastContext = useContext(ToastContext);
  return (<App {...props} userContext={userContext} toastContext={toastContext} />);
}

export default withRouter(AppWithContext);
