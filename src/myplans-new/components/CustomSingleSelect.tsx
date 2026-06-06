'use client';

import { useState, useRef, useEffect } from 'react';
import styles from '../assets/styles/singleselect.module.scss';

interface QueueOption {
    label: string;
}

interface Props {
    label?: string;
    options: QueueOption[];
    onChange?: (selected: QueueOption | null) => void;
}

export default function CustomSingleSelectDropdown({
    label = 'Queue',
    options,
    onChange,
}: Props) {
    const [selectedOption, setSelectedOption] = useState<QueueOption | null>(null);
    const [isOpen, setIsOpen] = useState(false);
    const dropdownRef = useRef<HTMLDivElement>(null);

    const handleSelect = (option: QueueOption) => {
        setSelectedOption(option);
        onChange?.(option);
        setIsOpen(false);
    };

    const handleClickOutside = (event: MouseEvent) => {
        if (dropdownRef.current && !dropdownRef.current.contains(event.target as Node)) {
            setIsOpen(false);
        }
    };

    useEffect(() => {
        document.addEventListener('mousedown', handleClickOutside);
        return () => document.removeEventListener('mousedown', handleClickOutside);
    }, []);

    return (
        <div className={styles.dropdownWrapper} ref={dropdownRef}>
            <div className={styles.dropdownButton} onClick={() => setIsOpen(!isOpen)}>
                <p>
                    {selectedOption &&
                        <>
                            {selectedOption.label}
                        </>
                    }
                </p>
                <svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
                    <path
                        d="M6.25717 5.90231C5.65467 5.90231 5.08917 5.66791 4.66387 5.24121L0.810366 1.37691C0.517866 1.08391 0.518866 0.609307 0.811866 0.316407C1.10577 0.024407 1.58087 0.0244068 1.87287 0.318407L5.72587 4.18271C6.00957 4.46591 6.50517 4.46591 6.78787 4.18271L10.6414 0.318407C10.9334 0.0245068 11.4085 0.024507 11.7024 0.316407C11.9954 0.609407 11.9963 1.08401 11.7039 1.37691L7.85037 5.24121C7.42507 5.66801 6.85927 5.90231 6.25717 5.90231Z"
                        fill="#757676"
                    />
                </svg>
            </div>

            {isOpen && (
                <div className={styles.dropdownMenu}>
                    <div className={styles.dropdownMenuinner}>
                        {options.map((option) => (
                            <div
                                key={option.label}
                                className={styles.option}
                                onClick={() => handleSelect(option)}
                            >
                                {option.label}
                            </div>
                        ))}
                    </div>
                </div>
            )}
        </div>
    );
}
