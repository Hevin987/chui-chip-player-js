import React, { memo, useCallback, useContext } from 'react';
import PlayerParams from './PlayerParams';
import { UserContext } from "./UserProvider";

const themes = [
  {
    value: 'msdos',
    label: 'MS-DOS',
  },
  {
    value: 'winamp',
    label: 'Winamp',
  },
  {
    value: 'pixel',
    label: 'Pixel Theme',
  },
  {
    value: 'bluearchive',
    label: 'Blue Archive',
  },
];

function Settings(props) {
  const {
    ejected,
    tempo,
    numVoices,
    voiceMask,
    voiceNames,
    voiceGroups,
    onVoiceMaskChange,
    onTempoChange,
    paramDefs,
    paramValues,
    onParamChange,
    onPinParam,
    persistedSettings,
    sequencer,
  } = props;

  const { settings, updateSettings } = useContext(UserContext);
  const theme = settings?.theme;

  const handleThemeChange = useCallback((e) => {
    updateSettings({ theme: e.target.value });
  }, [updateSettings]);

  const handleToggleVisualizer = useCallback((e) => {
    updateSettings({ visualizerEnabled: e.target.value === 'true' });
  }, [updateSettings]);

  const handleToggleMultichannel = useCallback((e) => {
    updateSettings({ vgmMultichannel: e.target.value === 'true' });
  }, [updateSettings]);

  return (
    <div className='Settings'>
      <h3>{sequencer?.getPlayer()?.name || 'Player'} Settings</h3>
      {sequencer?.getPlayer() ?
        <PlayerParams
          ejected={ejected}
          tempo={tempo}
          numVoices={numVoices}
          voiceMask={voiceMask}
          voiceNames={voiceNames}
          voiceGroups={voiceGroups}
          onTempoChange={onTempoChange}
          onVoiceMaskChange={onVoiceMaskChange}
          paramDefs={paramDefs}
          paramValues={paramValues}
          onParamChange={onParamChange}
          onPinParam={onPinParam}
          persistedSettings={persistedSettings}
          playerKey={sequencer?.getPlayer()?.playerKey}
        />
        :
        <div>(No active player)</div>}
      <h3>Global Settings</h3>
      <span className='PlayerParams-param'>
        <label className='PlayerParams-label'>
          Visualizer:{' '}
        </label>
        <input onClick={handleToggleVisualizer}
               id='vis-on'
               type='radio'
               value="true"
               defaultChecked={settings?.visualizerEnabled !== false}
               name='visualizer-enabled'/>
        <label htmlFor='vis-on' className='inline'>On</label>
        <input onClick={handleToggleVisualizer}
               id='vis-off'
               type='radio'
               value="false"
               defaultChecked={settings?.visualizerEnabled === false}
               name='visualizer-enabled'/>
        <label htmlFor='vis-off' className='inline'>Off</label>
      </span>
      <span className='PlayerParams-param'>
        <label className='PlayerParams-label'>
          VGM/SPC Multichannel Mode:{' '}
        </label>
        <input onClick={handleToggleMultichannel}
               id='multi-on'
               type='radio'
               value="true"
               defaultChecked={settings?.vgmMultichannel === true}
               name='multichannel-enabled'/>
        <label htmlFor='multi-on' className='inline'>On (CPU Heavy)</label>
        <input onClick={handleToggleMultichannel}
               id='multi-off'
               type='radio'
               value="false"
               defaultChecked={settings?.vgmMultichannel !== true}
               name='multichannel-enabled'/>
        <label htmlFor='multi-off' className='inline'>Off</label>
      </span>
      <span className='PlayerParams-param'>
        <label htmlFor='theme' className="PlayerParams-label">
          Theme:{' '}
        </label>
        <select
          id='theme'
          onChange={handleThemeChange}
          value={theme}>
          {themes.map(option =>
            <option key={option.value} value={option.value}>{option.label}</option>
          )}
        </select>
      </span>
    </div>
  );
}

export default memo(Settings);