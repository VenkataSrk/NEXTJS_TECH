import React from 'react';
import './scss/drawer.scss';


interface RightDrawerProps {
  open: boolean;
  onClose: () => void;
  children: React.ReactNode;
  showClose?: boolean; 
}

export default function RightDrawer({
  open,
  onClose,
  children,
  showClose = true, 
}: RightDrawerProps) {
  return (
    <div className={`drawer-overlay ${open ? 'open' : ''}`} onClick={onClose}>
      <div className="drawer" onClick={(e) => e.stopPropagation()}>
        {showClose && (
          <button className="drawer-close" onClick={onClose}>
            ✕
          </button>
        )}
        <div className="drawer-body">{children}</div>
      </div>
    </div>
  );
}