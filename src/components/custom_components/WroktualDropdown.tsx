import React, { useRef, useState, useEffect } from "react";
import { useClickOutside } from "./useClickOutside"; // assumes you have this hook
import "./scss/worktualdropdown.scss";
import Dropdown from '../../styles/assets/images/chevrondown.svg'

interface Option {
  label: string | React.ReactNode;
  value: string | number;
}

interface SelectProps {
  options: Option[];
  onChange?(value: Option): void;
  defaultValue?: Option;
  dropdownImage?: boolean;
  children?: React.ReactNode; // optional trigger
  placement?: "top" | "bottom" | "left" | "right"; // new prop,
  reset?:number;
}

const WorktualDropdown: React.FC<SelectProps> = ({
  options,
  onChange,
  defaultValue,
  children,
  dropdownImage,
  placement = "bottom", // default is bottom
  reset=0
}) => {
  const [isOpen, setIsOpen] = useState(false);
  const [selected, setSelected] = useState<Option | null>(defaultValue || null);
  const [highlightedIndex, setHighlightedIndex] = useState<number>(-1);

  const triggerRef = useRef<HTMLDivElement>(null);
  const optionRef = useRef<HTMLDivElement>(null);

  // Close when clicking outside
  useClickOutside([triggerRef, optionRef], () => setIsOpen(false));

  const toggleFloating = () => {
    setIsOpen((prev) => !prev);
  };

  const handleSelect = (option: Option) => {
    setSelected(option);
    onChange(option);
    setIsOpen(false);
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLDivElement>) => {
    if (e.key === "ArrowDown") {
      setHighlightedIndex((prev) => Math.min(prev + 1, options.length - 1));
    } else if (e.key === "ArrowUp") {
      setHighlightedIndex((prev) => Math.max(prev - 1, 0));
    } else if (e.key === "Enter" && highlightedIndex !== -1) {
      handleSelect(options[highlightedIndex]);
    } else if (e.key === "Escape") {
      setIsOpen(false);
    }
  };

  // Reset highlight when dropdown opens
  useEffect(() => {
    if (isOpen) setHighlightedIndex(-1);
  }, [isOpen]);

  useEffect(()=>{
    setSelected(defaultValue||null);
  },[reset])



  return (
    <div
      ref={triggerRef}
      className="dropdowncontainer"
      tabIndex={0}
      onKeyDown={handleKeyDown}
    >
      <div className="dropshow" onClick={toggleFloating}>
        {children || selected?.label || "Select..."}
        {dropdownImage && (
        <img
          src={Dropdown}
          alt="dropdown icon"
          className={isOpen ? 'dropactive' : ''}
        />
      )}

      </div>

      {isOpen && (
        <div
          ref={optionRef}
          className={`dropoptcontainer dropopt-${placement}`} // dynamic placement class
        >
          <ul>
            {options.length > 0 ? (
              options.map((option, index) => (
                <li
                  key={option.value}
                  onClick={() => handleSelect(option)}
                  className={`${
                    highlightedIndex === index ? "drophightlightopt" : ""
                  }`}
                >
                  {option.label}
                </li>
              ))
            ) : (
              <li className="dropnooptionsinselect">No options found</li>
            )}
          </ul>
        </div>
      )}
    </div>
  );
};

export default WorktualDropdown;
