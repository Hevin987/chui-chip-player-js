import React, { memo, useCallback, useRef } from 'react';
import { FORMATS } from '../config';
import bytes from 'bytes';

const formatList = FORMATS.filter(f => f !== 'miniusf').map(f => `.${f}`);

export default memo(LocalFiles);

function LocalFiles(props) {
  const {
    listing,
    currContext,
    playContext,
    currIdx,
    onSongClick,
    onDelete,
    onAddFiles,
    loading,
    title,
    subtitle,
    ejected,
    metadata,
  } = props;
  const fileInputRef = useRef(null);

  const handleDelete = useCallback((event) => {
    const href = event.currentTarget.dataset.href;
    if (event.shiftKey) {
      onDelete(href);
    } else {
      if (window.confirm('Are you sure you want to remove this file from browser storage?\n(Shift-click to remove without confirmation.)'))
        onDelete(href);
    }
  }, [onDelete]);

  const handleAddClick = () => {
    fileInputRef.current.click();
  };

  const handleFileChange = (event) => {
    const files = event.target.files;
    if (files.length > 0) {
      onAddFiles([...files]);
    }
  };

  const handleClear = () => {
    if (window.confirm('Are you sure you want to remove ALL local files?\nThis cannot be undone.')) {
      onDelete(listing.map(item => item.path));
    }
  }

  return (
    <div>
      {listing.length > 0 &&
        <h3 className="Browse-topRow">
          Local Files ({listing.length})
          <div className='button-container'>
            <button className='box-button' onClick={handleAddClick} title='Add files'>
              Add Files...
            </button>
            {' '}
            <button className='box-button' onClick={handleClear} title='Remove all local files'>
              Clear
            </button>
          </div>
          <input
            ref={fileInputRef}
            type="file"
            multiple
            accept={formatList.join(',')}
            style={{ display: 'none' }}
            onChange={handleFileChange}
          />
        </h3>
      }
      {loading ?
        "Loading local files..."
        :
        listing.length === 0 ?
          <div style={{ padding: '1.5em 1em' }}>
            {(!ejected && title) &&
              <>
                <h2 style={{ margin: '0 0 0.25em' }}>{title}</h2>
                {subtitle && <p style={{ margin: '0 0 1.5em', opacity: 0.65, fontSize: '0.9em' }}>{subtitle}</p>}
                {[
                  ['Title',     metadata?.title],
                  ['Artist',    metadata?.artist],
                  ['Game',      metadata?.game],
                  ['System',    metadata?.system],
                  ['Copyright', metadata?.copyright],
                ].filter(([, v]) => v).map(([label, value]) => (
                  <div key={label} style={{ display: 'flex', gap: '0.5em', marginBottom: '0.4em', fontSize: '0.85em' }}>
                    <span style={{ minWidth: '5.5em', opacity: 0.55, textAlign: 'right' }}>{label}:</span>
                    <span>{value}</span>
                  </div>
                ))}
              </>
            }
          </div>
          :
          <div>
            {
              listing.map((item, i) => {
                const href = item.path;
                const title = decodeURIComponent(href.split('/').pop());
                const isPlaying = currContext === playContext && currIdx === i;
                return (
                  <div key={title} className={isPlaying ? 'Song-now-playing BrowseList-row' : 'BrowseList-row'}>
                    <button className='Trash-button' title='Delete' onClick={handleDelete} data-href={href}>
                      <span className='inline-icon icon-trash'/>
                    </button>
                    <div className="BrowseList-colName">
                      <a
                        onClick={onSongClick(href, playContext, i)}
                        href={href}
                      >{title}</a>
                    </div>
                    <div className="BrowseList-colMtime">
                      {item.mtime}
                    </div>
                    <div className="BrowseList-colSize">
                      {bytes(item.size, { unitSeparator: ' ' })}
                    </div>
                  </div>
                );
              })
            }
          </div>
      }
    </div>
  );
}
