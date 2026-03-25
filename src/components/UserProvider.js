import React, { createContext, useCallback, useEffect, useMemo, useState } from 'react';
import { CATALOG_PREFIX } from '../config';
import { pathJoin } from '../util';

const UserContext = createContext({
  user: null,
  loadingUser: true,
  faves: [],
  favesContext: [],
  showPlayerSettings: false,
  handleLogin: () => {},
  handleLogout: () => {},
  handleToggleFavorite: () => {},
  settings: {},
  updateSettings: () => {},
  replaceSettings: () => {},
});

const DEFAULT_SETTINGS = {
  showPlayerSettings: false,
  theme: 'msdos',
  visualizerEnabled: true,
};

// User management and server sync disabled in this build.

const UserProvider = ({ children }) => {
  // Use authState hook for user state
  // const [authUser, userLoading] = useAuthState(firebase.auth());
  const [user, setUser] = useState(null); // Local state for user data
  const [faves, setFaves] = useState(() => {
    // Restore favorites from localStorage.
    try {
      const faves = window.localStorage.getItem('faves');
      return faves ? JSON.parse(faves) : [];
    } catch (e) {
      console.error('Could not load faves from localStorage', e);
      return [];
    }
  });
  const [loadingUser, setLoadingUser] = useState(true); // Manage loading state
  const [settings, setSettings] = useState(() => {
    // Restore settings from localStorage.
    try {
      const settings = window.localStorage.getItem('settings');
      return settings ? JSON.parse(settings) : DEFAULT_SETTINGS;
    } catch (e) {
      console.error('Could not load settings from localStorage', e);
      return DEFAULT_SETTINGS;
    }
  });

  useEffect(() => {
    // User management disabled: always run as anonymous (no server auth).
    setLoadingUser(false);
  }, []);

  // Copy favorites to localStorage whenever they change, so they persist across page reloads.
  useEffect(() => {
    try {
      window.localStorage.setItem('faves', JSON.stringify(faves));
    } catch (e) {
      console.error('Could not save faves to localStorage', e);
    }
  }, [faves]);

  // Copy settings to localStorage whenever they change, so they persist across page reloads.
  useEffect(() => {
    try {
      window.localStorage.setItem('settings', JSON.stringify(settings));
    } catch (e) {
      console.error('Could not save settings to localStorage', e);
    }
  }, [settings]);

  // No server-side user data fetching when user management is disabled.


  const handleLogin = async () => {
    console.warn('User login is disabled in this build.');
  };

  const handleLogout = async () => {
    console.warn('User logout is disabled in this build. Clearing local state.');
    setUser(null);
    setFaves([]);
  };

  const handleToggleFavorite = async (path, songId) => {
    // User management disabled: operate on local favorites only.
    const isFavorite = faves.find(fave => fave.path === path);

    const fave = {
      path,
      songId,
      href: pathJoin(CATALOG_PREFIX, encodeURIComponent(path)),
      mtime: Math.floor(Date.now() / 1000),
    };

    const newFaves = isFavorite
      ? faves.filter(fave => fave.path !== path)
      : [...faves, fave];
    setFaves(newFaves);
  };

  const updateSettings = useCallback((partialSettings) => {
    const newSettings = { ...settings, ...partialSettings };
    setSettings(newSettings);
  }, [settings]);

  const replaceSettings = useCallback((newSettings) => {
    setSettings(newSettings);
  }, []);

  // We need to derive a list of paths to use as the play context.
  const favesContext = useMemo(() => {
    return faves.map(fave => fave.path);
  }, [faves]);

  return (
    <UserContext.Provider
      value={{
        user,
        loadingUser,
        faves,
        favesContext,
        handleLogin,
        handleLogout,
        handleToggleFavorite,
        settings,
        updateSettings,
        replaceSettings,
      }}>
      {children}
    </UserContext.Provider>
  );
};

export { UserContext, UserProvider };
