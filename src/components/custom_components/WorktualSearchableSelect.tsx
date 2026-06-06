import React, { memo, useEffect, useMemo, useRef, useState } from "react";
import { assets } from "../../styles/assets/assets";
import { useClickOutside } from "./useClickOutside";
import "./scss/worktualselect.scss";

interface Option {
  label: string;
  value: string | number;
}

interface WorktualSearchableSelectProps {
  options: Option[];
  placeholder?: string;
  showSearchIcon?: boolean;
  onChange?: (value: string | number) => void;
  defaultValue?: Option;
  noOptionsText?: string;
}



const WorktualSearchableSelect: React.FC<WorktualSearchableSelectProps> = memo(
  ({
    options,
    placeholder = "Search and select",
    showSearchIcon = true,
    onChange,
    defaultValue,
    noOptionsText = "No options found",
  }) => {
    const [inputValue, setInputValue] = useState(defaultValue?.label || "");
    const [isOpen, setIsOpen] = useState(false);
    const [selectedIndex, setSelectedIndex] = useState<number | null>(null);

    const mainRef = useRef<HTMLDivElement>(null);
    const dropdownRef = useRef<HTMLDivElement>(null);
    const optionsRef = useRef<(HTMLLIElement | null)[]>([]);

    useClickOutside([mainRef, dropdownRef], () => {
      setIsOpen(false);
      setSelectedIndex(null);
    });

    useEffect(() => {
      setInputValue(defaultValue?.label || "");
    }, [defaultValue?.label]);

    const filteredOptions = useMemo(
      () =>
        options.filter((opt) =>
          opt.label.toLowerCase().includes(inputValue.toLowerCase())
        ),
      [inputValue, options]
    );

    useEffect(() => {
      if (!isOpen) {
        setSelectedIndex(null);
      } else {
        optionsRef.current = [];
      }
    }, [isOpen, filteredOptions.length]);

    const selectOption = (option: Option) => {
      setInputValue(option.label);
      setIsOpen(false);
      setSelectedIndex(null);
      onChange?.(option.value);
    };

    const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
      if (e.key === "ArrowDown") {
        e.preventDefault();
        setIsOpen(true);
        setSelectedIndex((prev) => {
          const next = prev === null ? 0 : Math.min(prev + 1, filteredOptions.length - 1);
          optionsRef.current[next]?.scrollIntoView({ block: "nearest", behavior: "smooth" });
          return next;
        });
      } else if (e.key === "ArrowUp") {
        e.preventDefault();
        setSelectedIndex((prev) => {
          const next = prev === null ? Math.max(filteredOptions.length - 1, 0) : Math.max(prev - 1, 0);
          optionsRef.current[next]?.scrollIntoView({ block: "nearest", behavior: "smooth" });
          return next;
        });
      } else if (e.key === "Enter") {
        e.preventDefault();
        if (selectedIndex !== null && filteredOptions[selectedIndex]) {
          selectOption(filteredOptions[selectedIndex]);
        }
      } else if (e.key === "Escape") {
        setIsOpen(false);
      }
    };

    return (
      <div className="selectsearchcontainer" ref={mainRef}>
        <div className="selectshow">
          {showSearchIcon && (
            <span className="selectprefixicon">
              <img src={assets.search_icon} alt="search icon" />
            </span>
          )}

          <input
            type="text"
            placeholder={placeholder}
            value={inputValue}
            onFocus={() => setIsOpen(true)}
            onClick={(e) => {
              e.stopPropagation();
              setIsOpen(true);
            }}
            onChange={(e) => {
              setInputValue(e.target.value);
              setIsOpen(true);
            }}
            onKeyDown={handleKeyDown}
          />

          <span
            onClick={() => setIsOpen((prev) => !prev)}
            className={`selectarrow ${isOpen ? "activeselectarrow" : ""}`}
          >
            <img src={assets.chevrondown_icon} alt="chevron" />
          </span>
        </div>

        {isOpen && (
          <div ref={dropdownRef} className="optioncontainer select-container-open">
            <ul>
              {filteredOptions.length > 0 ? (
                filteredOptions.map((option, index) => (
                  <li
                    key={`${option.value}-${index}`}
                    ref={(el) => {
                      optionsRef.current[index] = el;
                    }}
                    className={selectedIndex === index ? "hightlightopt" : ""}
                    onClick={() => selectOption(option)}
                  >
                    {option.label}
                  </li>
                ))
              ) : (
                <li className="nooptionsinselect">{noOptionsText}</li>
              )}
            </ul>
          </div>
        )}
      </div>
    );
  }
);

export default WorktualSearchableSelect;

