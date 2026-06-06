'use client';
import { useState } from 'react';
import styles from '../assets/styles/customswitch.module.scss';

export default function CustomSwitch({ checked = false, onChange, label }) {
  const [isChecked, setIsChecked] = useState(checked);

  const toggleSwitch = () => {
    const newValue = !isChecked;
    setIsChecked(newValue);
    if (onChange) onChange(newValue);
  };

  return (
    <label className={styles.switchWrapper}>
      <input
        type="checkbox"
        checked={isChecked}
        onChange={toggleSwitch}
        className={styles.hiddenCheckbox}
      />
      <span className={styles.slider}></span>
      {/* {label && <span className={styles.labelText}>{label}</span>} */}
    </label>
  );
}
