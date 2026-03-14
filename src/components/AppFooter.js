import React, { memo, useCallback, useContext } from 'react';
import TimeSlider from './TimeSlider';
import VolumeSlider from './VolumeSlider';
import FavoriteButton from './FavoriteButton';
import { REPEAT_LABELS, SHUFFLE_LABELS } from '../Sequencer';
import { UserContext } from './UserProvider';
import DirectoryLink from './DirectoryLink';
import { getUrlFromFilepath, pathJoin } from '../util';

function directoryLinkFromFilepath(filepath) {
  if (!filepath) return null;
  const sep = '/';

  filepath = filepath
    .split(sep).slice(0, -1).join(sep);
  return <DirectoryLink dim to={pathJoin('/browse', encodeURI(filepath))}>{filepath}</DirectoryLink>;
}

export default memo(AppFooter);
function AppFooter(props) {
  const {
    // this.state.
    currentSongDurationMs,
    currentSongNumSubtunes,
    currentSongSubtune,
    ejected,
    imageUrl,
    infoTexts,
    md5,
    paused,
    repeat,
    shuffle,
    songId,
    songPath,
    subtitle,
    title,
    volume,

    // this.
    getCurrentSongLink,
    handleCopyLink,
    handleCycleRepeat,
    handleCycleShuffle,
    handleTimeSliderChange,
    handleVolumeChange,
    nextSong,
    nextSubtune,
    prevSong,
    prevSubtune,
    sequencer,
    toggleInfo,
    togglePause,
  } = props;

  const {
    faves,
  } = useContext(UserContext);

  const directoryLink = directoryLinkFromFilepath(songPath);
  const songUrl = getUrlFromFilepath(songPath);
  const subtuneText = `Tune ${currentSongSubtune + 1} of ${currentSongNumSubtunes}`;

  const handleToggleInfo = useCallback((e) => {
    e.preventDefault();
    toggleInfo();
  }, [toggleInfo]);

  const handleCopySongLink = useCallback((e) => {
    e.preventDefault();
    handleCopyLink(getCurrentSongLink());
  }, [getCurrentSongLink, handleCopyLink]);

  const handleCopySubtuneLink = useCallback((e) => {
    e.preventDefault();
    handleCopyLink(getCurrentSongLink(/*withSubtune=*/true));
  }, [getCurrentSongLink, handleCopyLink]);

  const playPauseTitle = paused ? 'Play' : 'Pause';
  const playPauseClass = paused ? 'icon-play' : 'icon-pause';

  return (
    <div className="control-container-inner" style={{ display: 'flex', flexDirection: 'column', width: '100%', gap: '0.5rem' }}>
      <div className="transport-controls" style={{ display: 'flex', alignItems: 'center', justifyContent: 'space-between', width: '100%' }}>
        <div className="control-group">
          {/* Play/Pause */}
          <button className="control-btn btn-large box-button" onClick={togglePause} title={playPauseTitle} disabled={ejected}>
            <img src={`${process.env.PUBLIC_URL || ''}/${paused ? 'play.png' : 'pause.png'}`} className="btn-icon" alt={playPauseTitle} style={{ maxHeight: '100%', maxWidth: '100%' }} onError={(e) => { e.target.style.display='none'; e.target.nextSibling.style.display='block'; }}/>
            <span style={{ display: 'none', fontSize: '2rem' }} className={`inline-icon ${playPauseClass}`} />
          </button>
        </div>

        <div className="control-group">
          <div className="control-group-small" style={{ display: 'flex', flexDirection: 'column', gap: '4px' }}>
            {/* Download and Share and info buttons */}
            <div style={{ display: 'flex', gap: '4px', justifyContent: 'center' }}>
              {songPath &&
                <a href={songUrl} title="Download song" className="box-button" style={{ padding: '2px 4px' }}>
                  <span className="inline-icon icon-download" style={{ fontSize: '12px' }}/>
                </a>
              }
              {songPath &&
                <a href={getCurrentSongLink()} title="Copy song link" onClick={handleCopySongLink} className="box-button" style={{ padding: '2px 4px' }}>
                  <span className="inline-icon icon-copy" style={{ fontSize: '12px' }}/>
                </a>
              }
            </div>
            {currentSongNumSubtunes > 1 && (
              <div style={{ display: 'flex', gap: '4px', fontSize: '10px' }}>
                <button className="control-btn box-button" disabled={ejected} onClick={prevSubtune} style={{ padding: '0px 4px' }}>
                  <span className="inline-icon icon-back"/>
                </button>
                <div style={{ padding: '2px 4px' }}>{currentSongSubtune + 1}/{currentSongNumSubtunes}</div>
                <button className="control-btn box-button" disabled={ejected} onClick={nextSubtune} style={{ padding: '0px 4px' }}>
                  <span className="inline-icon icon-forward"/>
                </button>
              </div>
            )}
          </div>

          {/* Back */}
          <button className="control-btn btn-medium box-button" onClick={prevSong} title="Previous" disabled={ejected}>
            <img src={`${process.env.PUBLIC_URL || ''}/previous.png`} alt="Previous" style={{ maxHeight: '100%', maxWidth: '100%' }} onError={(e) => { e.target.style.display='none'; e.target.nextSibling.style.display='block'; }}/>
            <span style={{ display: 'none', fontSize: '1.5rem' }} className="inline-icon icon-prev" />
          </button>
          {/* Next */}
          <button className="control-btn btn-medium box-button" onClick={nextSong} title="Next" disabled={ejected}>
            <img src={`${process.env.PUBLIC_URL || ''}/next.png`} alt="Next" style={{ maxHeight: '100%', maxWidth: '100%' }} onError={(e) => { e.target.style.display='none'; e.target.nextSibling.style.display='block'; }}/>
            <span style={{ display: 'none', fontSize: '1.5rem' }} className="inline-icon icon-next" />
          </button>
        </div>

        <div className="control-group" style={{ display: 'flex', gap: '0.5rem' }}>
          {/* Loop */}
          <button className={`control-btn btn-medium box-button ${repeat !== 0 ? 'active' : ''}`} onClick={handleCycleRepeat} title={`Repeat: ${REPEAT_LABELS[repeat]}`}>
            <img src={`${process.env.PUBLIC_URL || ''}/loop.png`} alt="Loop" style={{ maxHeight: '100%', maxWidth: '100%' }} onError={(e) => { e.target.style.display='none'; e.target.nextSibling.style.display='block'; }}/>
            <span style={{ display: 'none', fontSize: '1.5rem' }} className="inline-icon icon-repeat" />
          </button>
          {/* Shuffle */}
          <button className={`control-btn btn-medium box-button ${shuffle !== 0 ? 'active' : ''}`} onClick={handleCycleShuffle} title={`Shuffle: ${SHUFFLE_LABELS[shuffle]}`}>
            <img src={`${process.env.PUBLIC_URL || ''}/shuffle.png`} alt="Shuffle" style={{ maxHeight: '100%', maxWidth: '100%' }} onError={(e) => { e.target.style.display='none'; e.target.nextSibling.style.display='block'; }}/>
            <span style={{ display: 'none', fontSize: '1.5rem' }} className="inline-icon icon-shuffle" />
          </button>
        </div>
      </div>

      <div style={{ display: 'flex', width: '100%', gap: '1rem', alignItems: 'center' }}>
        <div className="seek-bar-container" style={{ flexGrow: 1, margin: 0, display: 'flex' }}>
          <TimeSlider
            paused={paused}
            currentSongDurationMs={currentSongDurationMs}
            getCurrentPositionMs={() => {
              if (sequencer && sequencer.getPlayer()) {
                return sequencer.getPlayer().getPositionMs();
              }
              return 0;
            }}
            onChange={handleTimeSliderChange}/>
        </div>
        <div className="volume-area" style={{ display: 'flex', alignItems: 'center', gap: '0.5rem' }}>
          <VolumeSlider
            onChange={(e) => handleVolumeChange(e.target.value)}
            handleReset={(e) => {
              handleVolumeChange(100);
              e.preventDefault();
              e.stopPropagation();
            }}
            title="Double-click or right-click to reset to 100%."
            value={volume}/>
        </div>
      </div>
    </div>
  );
}
