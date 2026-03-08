import React from 'react';

const AppHeader = ({ onToggleSettings, showPlayerSettings }) => {
  return (
    <header className="AppHeader">
      <span className="AppHeader-title">Chip Player JS</span>
      <div style={{ marginLeft: 'auto', display: 'flex', gap: '1em', alignItems: 'center' }}>
        <button
          className={`tab tab-settings ${showPlayerSettings ? 'tab-selected' : ''}`}
          onClick={onToggleSettings}
          style={{ margin: 0 }}
        >
          Settings
        </button>
      </div>
    </header>
  );
}

export default AppHeader;
