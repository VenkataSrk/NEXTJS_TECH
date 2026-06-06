import React from "react";
import styles from "./scss/progressbar.module.scss";

const ProgressBar = ({ progress }:any) => {
  return (
    <div className={styles.progressBar}>
      <div
        className={styles.progressFill}
        style={{ width: `${progress}%` }}
      ></div>
    </div>
  );
};

export default ProgressBar;