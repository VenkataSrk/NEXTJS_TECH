import { useState } from "react";
import styles from "./scss/statusdropdown.module.scss";

type StatusDropdownProps = {
  initialStatus: "Active" | "Inactive";
  onChange?: (value: "Active" | "Inactive") => void;
};

export default function StatusDropdown({ initialStatus, onChange }: StatusDropdownProps) {
  const [open, setOpen] = useState(false);
  const [status, setStatus] = useState(initialStatus);

  const handleSelect = (value: "Active" | "Inactive") => {
    setStatus(value);
    setOpen(false);
    onChange?.(value);
  };

  return (
    <div className={styles.dropdown}>
      <button onClick={() => setOpen(!open)} className={styles.trigger}>
        {status}
        <span style={{ margin: "0 0px 0 12px" }} className={styles.arrow}>
          <svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
            <path d="M6.00009 5.90231C5.39759 5.90231 4.83209 5.66791 4.40679 5.24121L0.553286 1.37691C0.260786 1.08391 0.261786 0.609307 0.554786 0.316407C0.848686 0.024407 1.32379 0.0244068 1.61579 0.318407L5.46879 4.18271C5.75249 4.46591 6.24809 4.46591 6.53079 4.18271L10.3843 0.318407C10.6763 0.0245068 11.1514 0.024507 11.4453 0.316407C11.7383 0.609407 11.7392 1.08401 11.4468 1.37691L7.59329 5.24121C7.16799 5.66801 6.60219 5.90231 6.00009 5.90231Z" fill="#757676"/>
          </svg>
        </span>
      </button>

      {open && (
        <div className={styles.menu}>
          <div onClick={() => handleSelect("Active")}>Active</div>
          <div onClick={() => handleSelect("Inactive")}>Inactive</div>
        </div>
      )}
    </div>
  );
}
