import { useState, useRef, useEffect } from 'react';
import styles from '../assets/styles/singleselect.module.scss';
import { assets } from '../../../styles/images/assets';

interface QueueOption {
  key: string;
  value: React.ReactNode;
  item?: any;
}

interface Props {
  label?: string | React.ReactNode;
  options: QueueOption[];
  value?: string | QueueOption | null;
  onChange?: (selected: QueueOption | null) => void;
  disabled?: boolean;
  triggerIcon?: string | React.ReactNode;
}

export default function CustomSingleSelectDropdownupdate({
  label = 'Select',
  options,
  value,
  onChange,
  disabled = false,
  triggerIcon = assets.Kebabvertical, 
}: Props) {
  const [selectedOption, setSelectedOption] = useState<QueueOption | null>(null);
  const [isOpen, setIsOpen] = useState(false);
  const [dropdownPosition, setDropdownPosition] = useState<'bottom' | 'top'>('bottom');
  const dropdownRef = useRef<HTMLDivElement>(null);
  const menuRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (typeof value === 'string') {
      const found = options.find((opt) => opt.value === value);
      setSelectedOption(found || null);
    } else {
      setSelectedOption(value || null);
    }
  }, [value, options]);

  const calculatePosition = () => {
    if (!dropdownRef.current || !menuRef.current) return;

    const rect = dropdownRef.current.getBoundingClientRect();
    const menuHeight = menuRef.current.scrollHeight;
    const spaceBelow = window.innerHeight - rect.bottom;

    if (spaceBelow < menuHeight + 8) {
      setDropdownPosition('top');
    } else {
      setDropdownPosition('bottom');
    }
  };

  const handleSelect = (option: QueueOption) => {
    if (disabled) return;
    setSelectedOption(option);
    onChange?.(option);
    setIsOpen(false);
  };

  const handleClickOutside = (e: MouseEvent) => {
    if (dropdownRef.current && !dropdownRef.current.contains(e.target as Node)) {
      setIsOpen(false);
    }
  };

  const toggleDropdown = () => {
    if (disabled) return;
    setIsOpen(!isOpen);
    if (!isOpen) setTimeout(calculatePosition, 0);
  };

  useEffect(() => {
    if (isOpen) {
      calculatePosition();
      document.addEventListener('mousedown', handleClickOutside);
      document.addEventListener('scroll', calculatePosition);
      window.addEventListener('resize', calculatePosition);
    }
    return () => {
      document.removeEventListener('mousedown', handleClickOutside);
      document.removeEventListener('scroll', calculatePosition);
      window.removeEventListener('resize', calculatePosition);
    };
  }, [isOpen]);

  return (
    <div className={`${styles.dropdownWrapper}`} ref={dropdownRef}>

      {/* ▼ Kebab Button Trigger (Dropdown) */}
      <div
        className={`${styles.dropdownButton} selectborder ${isOpen ? styles.active : ''} ${disabled ? styles.disabled : ''}`}
        onClick={toggleDropdown}
      >
        {typeof triggerIcon === "string" ? (
          <img src={triggerIcon} alt="menu" />
        ) : (
          triggerIcon
        )}
      </div>

      {/* ▼ Dropdown Menu */}
      {isOpen && (
        <div
          ref={menuRef}
          className={`${styles.dropdownMenu} ${styles.dropdownBottom} drmenuinner ${
            dropdownPosition === 'top' ? styles.dropdownTop : `${styles.dropdownBottom} ${styles.dropdownpositionset}`
          }`}
          style={{ position: 'absolute', zIndex: 1000 }}
        >
          <div className={styles.dropdownMenuinner} >
            {options.map((option) => (
              <div
                key={option.key}
                className={`${styles.option} optionbar`}
                onClick={() => handleSelect(option)}
              >
                {option.value}
              </div>
            ))}
          </div>
        </div>
      )}
    </div>
  );
}