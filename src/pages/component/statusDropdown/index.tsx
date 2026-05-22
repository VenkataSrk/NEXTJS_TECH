import { useEffect, useState } from "react";
import "./statusDropdown.scss";

const StatusDropdown = ({
  value,
  options = [],
  defaultStatus,
  onChange,
}: any) => {
  const [open, setOpen] = useState(false);
  const [selected, setSelected] = useState<any>(null);

  useEffect(() => {
    if (value && options.length) {
      if (typeof value === "string") {
        const found = options.find(
          (o: any) => o.value === value
        );
        setSelected(found || null);
      } else {
        setSelected(value);
      }
    } else if (defaultStatus && options.length) {
      const found = options.find(
        (o: any) => o.value === defaultStatus
      );
      setSelected(found || options[0]);
    }
  }, [value, defaultStatus, options]);

  const handleSelect = (option: any) => {
    if (option.disabled) return;
    setSelected(option);
    setOpen(false);
    onChange?.(option);
  };

  if (!selected) return null;
  const selectedLabel =
    selected?.label ??
    selected?.statusName ??
    (typeof selected?.value === "string" ? selected.value : "");

  return (
    <div className="status-dropdown">
      <button
        className="status-btn"
        onClick={() => setOpen(!open)}
      >
        <span
          className="dot"
          style={{ backgroundColor: selected.colorCode || 'red'}}
        />
        <span className="text">{selectedLabel || "Not Ready"}</span>
        <span className="arrow"><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
  <path d="M5.66561 5.80465C5.06311 5.80465 4.49761 5.57025 4.07231 5.14355L0.218813 1.27925C-0.0736866 0.98625 -0.0726864 0.511651 0.220314 0.218751C0.514214 -0.0732493 0.989313 -0.0732494 1.28131 0.220751L5.13431 4.08505C5.41801 4.36825 5.91361 4.36825 6.19631 4.08505L10.0498 0.220751C10.3418 -0.0731494 10.8169 -0.0731493 11.1108 0.218751C11.4038 0.511751 11.4047 0.98635 11.1123 1.27925L7.25881 5.14355C6.83351 5.57035 6.26771 5.80465 5.66561 5.80465Z" fill="#757676"/>
</svg></span>
      </button>

      {open && (
        <div className="menu">
          {options.map((option: any) => (
            <div
              key={option.value}
              className={`menu-item ${
                option.disabled ? "disabled" : ""
              }`}
              onClick={() => handleSelect(option)}
            >
              <span
                className="dot"
                style={{ backgroundColor: option.colorCode }}
              />
              <span className="label">{option.label}</span>
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export default StatusDropdown;
