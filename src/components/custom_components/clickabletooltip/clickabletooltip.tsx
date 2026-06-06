import React, { useState, useRef, useEffect } from "react";
import styles from "./scss/clickabletooltip.module.scss";

export default function ClickTooltip({
  trigger,
  children,
}: {
  trigger: React.ReactNode;
  children: React.ReactNode;
}) {
  const [open, setOpen] = useState(false);
  const ref = useRef<HTMLDivElement | null>(null);

  // Close when clicking outside
  useEffect(() => {
    const handleClickOutside = (e: MouseEvent) => {
      if (ref.current && !ref.current.contains(e.target as Node)) {
        setOpen(false);
      }
    };
    document.addEventListener("mousedown", handleClickOutside);
    return () => {
      document.removeEventListener("mousedown", handleClickOutside);
    };
  }, []);

  return (
    <div className={styles.wrapper} ref={ref}>
      <div onClick={() => setOpen(!open)} className={styles.trigger}>
        {trigger}
      </div>
      {open && (
        <div className={styles.tooltip}>
          <div className={styles.arrow} />
          {children}
        </div>
      )}
    </div>
  );
}
