import { useState, useRef, useEffect } from 'react';
import styles from '../../../src/pages/component/scss/singleselect.module.scss';

interface QueueOption {
  key: any;
  value: string;
  item?: any;
}

interface Props {
  label?: string | React.ReactNode;
  options: QueueOption[];
  value?: string | QueueOption | null;
  onChange?: (selected: QueueOption | null) => void;
  disabled?: boolean;
  iconColor?: string;
}

export default function CustomSingleSelectDropdown({
  label = 'Select',
  options,
  value,
  onChange,
  iconColor = '#000000',
  disabled = false,
}: Props) {
  const [selectedOption, setSelectedOption] = useState<QueueOption | null>(null);
  const [isOpen, setIsOpen] = useState(false);
  const [dropdownPosition, setDropdownPosition] = useState<'bottom' | 'top'>('bottom');
  const dropdownRef = useRef<HTMLDivElement>(null);
  const menuRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (typeof value === 'string') {
      const found = options.find((opt) => opt.value === value);
      setSelectedOption(
        found || { key: value, value: value });
    } else {
      setSelectedOption(value || null);
    }
  }, [value, options]);

  // Calculate dropdown position
  const calculatePosition = () => {
    if (!dropdownRef.current || !menuRef.current) return;

    const rect = dropdownRef.current.getBoundingClientRect();
    const menuHeight = menuRef.current.scrollHeight;
    const spaceBelow = window.innerHeight - rect.bottom;
    
    if (spaceBelow < menuHeight + 8) { // 8px buffer
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

  const handleClickOutside = (event: MouseEvent) => {
    if (dropdownRef.current && !dropdownRef.current.contains(event.target as Node)) {
      setIsOpen(false);
    }
  };

  const toggleDropdown = () => {
    if (disabled) return;
    setIsOpen(!isOpen);
    if (!isOpen) {
      // Calculate position on open
      setTimeout(calculatePosition, 0);
    }
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
    <div className={`${styles.dropdownWrapper} wrapdrop`} ref={dropdownRef}>
      <div
        className={`${styles.dropdownButton} selectborder ${disabled ? styles.disabled : ''}`}
        onClick={toggleDropdown}
      >
        <div className='lablevalue'>{selectedOption ? selectedOption.value : label}</div>
        <svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
            <path style={{ fill: iconColor }} d="M5.66561 5.80465C5.06311 5.80465 4.49761 5.57025 4.07231 5.14355L0.218813 1.27925C-0.0736866 0.98625 -0.0726864 0.511651 0.220314 0.218751C0.514214 -0.0732493 0.989313 -0.0732494 1.28131 0.220751L5.13431 4.08505C5.41801 4.36825 5.91361 4.36825 6.19631 4.08505L10.0498 0.220751C10.3418 -0.0731494 10.8169 -0.0731493 11.1108 0.218751C11.4038 0.511751 11.4047 0.98635 11.1123 1.27925L7.25881 5.14355C6.83351 5.57035 6.26771 5.80465 5.66561 5.80465Z" fill="black"/>
        </svg> 
      </div>

      {isOpen && (
        <div 
          ref={menuRef}
          className={`${styles.dropdownMenu} drmenuinner ${dropdownPosition === 'top' ? styles.dropdownTop : styles.dropdownBottom}`}
          style={{
            position: 'absolute',
            zIndex: 1000,
          }}
        >
          <div className={`${styles.dropdownMenuinner} drscroler`}>
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
