import React, { useEffect, useState } from "react";
import styles from "./scss/blurtoast.module.scss";

type BlurToastDemoProps = {
  open: boolean;
  message?: any;
  duration?: number;
};

const BlurToastDemo: React.FC<BlurToastDemoProps> = ({
  open,
  message = "✅ Saved successfully",
  duration = 5000,
}) => {
  const [showToast, setShowToast] = useState(false);
  const [animateExit, setAnimateExit] = useState(false);

  useEffect(() => {
    let exitTimer: ReturnType<typeof setTimeout>;
    let removeTimer: ReturnType<typeof setTimeout>;

    if (open) {
      setShowToast(true);
      setAnimateExit(false);

      exitTimer = setTimeout(() => setAnimateExit(true), duration);
      removeTimer = setTimeout(() => setShowToast(false), duration + 600);
    }

    return () => {
      clearTimeout(exitTimer);
      clearTimeout(removeTimer);
    };
  }, [open, duration]);

  if (!showToast) return null;

  return (
    <div
      className={`${styles.toast} ${
        animateExit ? styles.toastExit : styles.toastEnter
      }`}
      role="status"
      aria-live="polite"
    >
      {message}
    </div>
  );
};

export default BlurToastDemo;
