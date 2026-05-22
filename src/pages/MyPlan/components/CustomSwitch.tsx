'use client';
import { useEffect, useState } from 'react';
import styles from '../assets/styles/customswitch.module.scss';

interface CustomSwitchProps {
  checked?: boolean;
  onChange?: (val: boolean) => void;
  label?: string;
  disabled?: boolean;
}

export default function CustomSwitch({ checked = false, onChange, label, disabled = false}: CustomSwitchProps) {
  const [isChecked, setIsChecked] = useState(checked);

  useEffect(() => {
    setIsChecked(checked);
  }, [checked]);

  const toggleSwitch = () => {
    const newValue = !isChecked;
    if (onChange) onChange(newValue);
  };

  return (
    <label className={styles.switchWrapper}>
      <input
        type="checkbox"
        checked={isChecked}
        onChange={toggleSwitch}
        disabled={disabled}
        className={styles.hiddenCheckbox}
      />
      <span className={styles.slider}></span>
    </label>
  );
}
