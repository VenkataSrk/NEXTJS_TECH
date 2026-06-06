import React, { useState } from "react";
import styles from "./scss/variant.module.scss";
// import styles from "./scss/qrcode.module.scss";

const VariantDropdown = ({variantEmail} :any) => {
  const [open, setOpen] = useState(false);
  const [selected, setSelected] = useState("Variant A");

  const options = ["Variant A", "Variant B"];

  const handleSelect = (opt :any) => {
    console.log("opt",opt)
    if(opt =="Variant A"){
      variantEmail(1);
    }
    if(opt == "Variant B"){
      variantEmail(2);
    }
    setSelected(opt);
    setOpen(false);
  };

  return (
    <div className={styles.dropdownWrapper}>
      <button
        className={styles.trigger}
        onClick={() => setOpen(!open)}
      >
        {selected}
        <span className={styles.arrow}><svg xmlns="http://www.w3.org/2000/svg" width="8" height="4" viewBox="0 0 8 4" fill="none">
  <path d="M3.77708 3.86977C3.37541 3.86977 2.99841 3.7135 2.71488 3.42903L0.145876 0.852833C-0.0491244 0.6575 -0.0484576 0.3411 0.146876 0.145834C0.342809 -0.0488329 0.659542 -0.0488329 0.854209 0.147167L3.42288 2.72337C3.61201 2.91217 3.94241 2.91217 4.13088 2.72337L6.69988 0.147167C6.89454 -0.0487663 7.21128 -0.0487662 7.40721 0.145834C7.60254 0.341167 7.60314 0.657567 7.40821 0.852833L4.83921 3.42903C4.55568 3.71357 4.17848 3.86977 3.77708 3.86977Z" fill="#757676"/>
</svg></span>
      </button>

      {open && (
        <div className={styles.menu}>
          {options.map((opt) => (
            <div
              key={opt}
              className={styles.itemmm}
              onClick={() => handleSelect(opt)}
            >
              {opt}
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export default VariantDropdown;


