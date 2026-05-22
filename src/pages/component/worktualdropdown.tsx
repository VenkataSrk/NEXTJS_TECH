import { useState, useRef, useEffect } from 'react';
import styles from './scss/worktualdropdown.module.scss';
import workspacedotsicon from '../../assets/images/projectinsights/verticalkebab.svg'

interface QueueOption {
    key: any;
    value: string;
    item?: any;
}

interface Props {
    label?: string | React.ReactNode;
    options: any;
    value?: string | QueueOption | null;
    onChange?: (selected: QueueOption | null) => void;
    disabled?: boolean;
    right?:boolean;
    triggerIcon?: string | React.ReactNode;
}

export default function Worktualdropdown({
    label = 'Select',
    options,
    value,
    onChange,
    disabled = false,
    triggerIcon = workspacedotsicon, 
    right = false
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
                 {typeof triggerIcon === "string" ? (
          <img src={triggerIcon} alt="menu" />
        ) : (
          triggerIcon
        )}
            </div>

            {isOpen && (
                <div
                    ref={menuRef}
                    className={`${styles.dropdownMenu} drmenuinner ${dropdownPosition === 'top' ? styles.dropdownTop : styles.dropdownBottom} ${right ? styles.dropdownrightend : ''}`}
                    style={{
                        position: 'absolute',
                        zIndex: 1000,
                    }}
                >
                    <div className={styles.dropdownMenuinner}>
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
