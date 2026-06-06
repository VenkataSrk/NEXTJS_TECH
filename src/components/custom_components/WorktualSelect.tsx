// import React, { useState, useRef } from "react";
import { useClickOutside } from "./useClickOutside";
import './scss/worktualselect.scss'
import { assets } from "../../styles/assets/assets";
import React, { useState, useRef, useEffect, memo } from "react";
import ReactDOM from 'react-dom';

interface Option {
  label: string;
  value: string | number;
}
interface CustomSelectProps {
    options: Option[];
    placeholder?: string;
    showSearchIcon?:boolean;
    onChange?: (value: string | number) => void;
    defaultValue?:Option;
    isSearchable?:boolean;
}

const WorktualSelect: React.FC<CustomSelectProps> = memo(({ 
    options, 
    placeholder = "Select",
    onChange,
    showSearchIcon=false,
    defaultValue, 
    isSearchable=false 
  }) => {
    const [selectval,setSelectVal] = useState(defaultValue?.label || '');
    const [isOpen, setIsOpen] = useState(false);
    const [searchQuery, setSearchQuery] = useState("");
    const [position, setPosition] = useState({ top: 0, left: 0,elemWidth:0 });
    const [selectedIndex, setSelectedIndex] = useState<number | null>(null);
    const inputRef = useRef<HTMLInputElement>(null);
    const optionsRef = useRef<(HTMLLIElement | null)[]>([]);
    const mainRef = useRef<HTMLDivElement>(null);
    const dropdownRef = useRef<HTMLDivElement>(null);
    
    // useClickOutside([mainRef, dropdownRef], () => setIsOpen(false));

    const filteredOptions = options.filter((opt) =>
        opt.label.toLowerCase().includes(searchQuery.toLowerCase())
    );

    useEffect(()=>{
        setSelectVal(defaultValue?.label || '')
    },[defaultValue?.label])

    const toggleFloating = () => {
        // if (!isOpen && inputRef.current) {
        //   const rect = inputRef.current.getBoundingClientRect();
        //   setPosition({
        //     top: rect.bottom + window.scrollY + 8, 
        //     left: rect.left + window.scrollX,
        //     elemWidth:rect.width
        //   });
        // }
        setIsOpen(!isOpen);
    };

    // // fixed in position while resize the screen
    // useEffect(()=>{
    //     console.log("resizeee");
    //     const handleResize = () =>{
    //         if (isOpen && inputRef.current) {
    //             const rect = inputRef.current.getBoundingClientRect();
    //             setPosition({
    //               top: rect.bottom + window.scrollY + 8,
    //               left: rect.left + window.scrollX,
    //               elemWidth:rect.width
    //             });
    //           }
    //     }
    //     window.addEventListener('resize', handleResize);

    // return () => {
    //   window.removeEventListener('resize', handleResize);
    // };
    // })

    useEffect(() => {
        if (!isOpen) {
            setSelectedIndex(null);
        }
    }, [isOpen]);

    const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>) => {
        if (e.key === "ArrowDown") {
            e.preventDefault();
            setIsOpen(true);
            setSelectedIndex((prev) => {
                const next = prev === null ? 0 : Math.min(prev + 1, filteredOptions.length - 1);
                optionsRef.current[next]?.scrollIntoView({ block: "nearest", behavior: "smooth" });
                return next;
            });
        } 
        else if (e.key === "ArrowUp") {
            e.preventDefault();
            setSelectedIndex((prev) => {
                const prevIndex = prev === null ? filteredOptions.length - 1 : Math.max(prev - 1, 0);
                optionsRef.current[prevIndex]?.scrollIntoView({ block: "nearest", behavior: "smooth" });
                return prevIndex;
            });
        } 
        else if (e.key === "Enter" && selectedIndex !== null) {
            const value = filteredOptions[selectedIndex];
            setSearchQuery('');
            setSelectVal(value.label);
            setIsOpen(false);
            onChange?.(value.value);
        } 
        else if (e.key === "Escape") {
            setIsOpen(false);
        }
    };

    return (
        <div className="selectsearchcontainer" ref={mainRef}>
            <div className="selectshow">
                {showSearchIcon && <span className="selectprefixicon"><img src={assets.search_icon} alt="searchicon" /></span>}
                <input
                    type="text"
                    placeholder={placeholder}
                    value={selectval}
                    onClick={(e) => {
                        e.stopPropagation()
                        toggleFloating()
                    }}
                    onChange={(e) => {
                        setSelectVal(e.target.value);
                        setSearchQuery(e.target.value);
                    }}
                    onKeyDown={handleKeyDown}
                    ref={inputRef}
                    readOnly={!isSearchable}
                />
                <span onClick={()=>{toggleFloating()}} className={`selectarrow ${isOpen ? 'activeselectarrow' : ''}`}><img src={assets.chevrondown_icon} alt="chevron" /> </span>
            </div>

            {isOpen && (
                <div ref={dropdownRef} className="optioncontainer select-container-open" >
                    <ul>
                        {filteredOptions.length > 0 ? (
                            filteredOptions.map((option, index) => (
                                <li
                                    key={index}
                                    // ref={(el) => (optionsRef.current[index] = el)}
                                    className={`${selectedIndex === index ? "hightlightopt" : ""}`}
                                    onClick={() => {
                                        setSelectVal(option.label);
                                        setSearchQuery('');
                                        setIsOpen(false);
                                        onChange?.(option.value);
                                    }}
                                >
                                    {option.label}
                                </li>
                            ))
                        ) : (
                            <li className="nooptionsinselect">No options found</li>
                        )}
                    </ul>
                </div>
            )}
        </div>
    );
});

export default WorktualSelect;

