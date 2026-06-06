import React, { useState, useRef, useEffect } from "react";
import styles from "./datepicker.module.scss";

const week = ["S", "M", "T", "W", "T", "F", "S"];

const getDays = (year: number, month: number) => {
  const first = new Date(year, month, 1);
  const start = first.getDay();
  const days = [];

  for (let i = start - 1; i >= 0; i--) {
    days.push({ date: new Date(year, month, -i), current: false });
  }

  const d = new Date(year, month, 1);
  while (d.getMonth() === month) {
    days.push({ date: new Date(d), current: true });
    d.setDate(d.getDate() + 1);
  }

  while (days.length < 42) {
    days.push({ date: new Date(d), current: false });
    d.setDate(d.getDate() + 1);
  }

  return days;
};

const sameDay = (a: Date, b: { getDate: () => any; getMonth: () => any; getFullYear: () => any; } | null) =>
  a && b &&
  a.getDate() === b.getDate() &&
  a.getMonth() === b.getMonth() &&
  a.getFullYear() === b.getFullYear();

  interface DateRangeInputProps {
  handleDatePicker: (startDate: any | undefined, endDate: any | undefined) => void;
  startDate: any;
  endDate: any;
  reset ?:any;
}

export default function DateRangeInput({handleDatePicker,startDate,endDate,reset=0}: DateRangeInputProps) {
  const [open, setOpen] = useState(false);
  // const [month, setMonth] = useState(new Date(2025, 8));
  const [month, setMonth] = useState(new Date(new Date().getFullYear(), new Date().getMonth()));

  const [start, setStart] = useState(null);
  const [end, setEnd] = useState(null);
  const ref  = useRef(null);

  const days = getDays(month.getFullYear(), month.getMonth());

  const select = (date) => {
    if (!start || end) {
      setStart(date);
      setEnd(null);
    } else if (date < start) {
      setEnd(start);
      setStart(date);
    } else {
      setEnd(date);
    }
  };

  const inRange = (d) => start && end && d > start && d < end;

  useEffect(() => {
    const close = (e) => ref.current && !ref.current.contains(e.target) && setOpen(false);
    document.addEventListener("mousedown", close);
    return () => document.removeEventListener("mousedown", close);
  }, []);

  useEffect(()=>{
    setStart(null);
    setEnd(null);
    ref.current=null;
    setOpen(false);
  },[reset])
  const format = (d) =>
    d ? d.toLocaleDateString("en-GB") : "";

  return (
    <div className={styles.wrapper} ref={ref}>
      {/* INPUT */}
      <div className={styles.inputBox} onClick={() => setOpen(!open)}>
        <span className={styles.icon}><svg xmlns="http://www.w3.org/2000/svg" width="18" height="20" viewBox="0 0 18 20" fill="none">
  <path d="M17.2388 8.3706C17.2135 6.636 17.2135 4.4456 15.8967 3.1034C14.9312 2.1185 13.6195 1.7758 12.2818 1.651V0.7501C12.2818 0.336 11.9459 0.000100136 11.5318 0.000100136C11.1177 0.000100136 10.7818 0.336 10.7818 0.7501V1.581C10.0676 1.5659 9.35168 1.5555 8.63527 1.5555C7.91237 1.5555 7.18987 1.5658 6.46927 1.5812V0.75C6.46927 0.3359 6.13337 0 5.71927 0C5.30517 0 4.96927 0.3359 4.96927 0.75V1.6575C3.63837 1.7858 2.33468 2.1232 1.37378 3.1033C0.121275 4.3678 0.0604749 6.3918 0.0257749 8.0728L0.025475 8.0743L0.0256748 8.0755C0.0235748 8.178 0.021175 8.2837 0.018975 8.3833C-0.006325 9.6875 -0.006325 10.9916 0.018975 12.2958C0.044275 14.0305 0.0442749 16.2208 1.36107 17.563C2.62727 18.8545 4.48847 19.0444 6.22318 19.0824C7.07648 19.1028 7.92997 19.1158 8.78317 19.1158C9.52527 19.1158 10.2671 19.1059 11.0093 19.0824C12.7439 19.0319 14.6052 18.8545 15.8714 17.563C17.1882 16.2208 17.1755 14.0305 17.2135 12.2958C17.2135 11.7007 17.2388 11.1182 17.2388 10.5232H17.2515C17.2515 9.8014 17.2515 9.0924 17.2388 8.3706ZM4.96927 3.1668V3.6564C4.96927 4.0705 5.30517 4.4064 5.71927 4.4064C6.13337 4.4064 6.46927 4.0705 6.46927 3.6564V3.0814C7.18977 3.0657 7.91237 3.0556 8.63527 3.0556C9.35087 3.0557 10.0665 3.0657 10.7819 3.0812V3.6564C10.7819 4.0705 11.1178 4.4064 11.5319 4.4064C11.946 4.4064 12.2819 4.0705 12.2819 3.6564V3.1594C13.3657 3.2745 14.2259 3.542 14.826 4.1542C15.5093 4.8507 15.6672 6.114 15.7142 7.3244H1.54877C1.59877 6.09 1.75177 4.8534 2.44487 4.1536C3.03517 3.5516 3.87187 3.2865 4.96927 3.1668ZM15.7388 10.5233C15.7388 10.81 15.7326 11.0937 15.7265 11.3774C15.7199 11.6825 15.7135 11.9874 15.7139 12.263L15.7091 12.4897C15.6788 13.9722 15.6445 15.6525 14.8002 16.513C13.9948 17.3345 12.7358 17.5316 10.9617 17.5832C10.2688 17.6052 9.55617 17.6159 8.78327 17.6159C7.94187 17.6159 7.10037 17.603 6.25607 17.5828C4.55398 17.5456 3.25227 17.3494 2.43187 16.5125C1.56647 15.6304 1.54098 13.8376 1.52058 12.3969L1.51867 12.2668C1.49647 11.1236 1.50117 9.968 1.51867 8.8244H15.7417C15.7474 9.3256 15.7514 9.8245 15.7515 10.3271C15.7431 10.3913 15.7388 10.4568 15.7388 10.5233Z" fill="#404040"/>
</svg></span>
        <span className={styles.value}>
          {start && end
            ? `${format(start)} - ${format(end)}`
            : "Date Range"}
        </span>
        <span className={styles.arrow}><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
  <path d="M5.66561 5.80465C5.06311 5.80465 4.49761 5.57025 4.07231 5.14355L0.218813 1.27925C-0.0736866 0.98625 -0.0726864 0.511651 0.220314 0.218751C0.514214 -0.0732493 0.989313 -0.0732494 1.28131 0.220751L5.13431 4.08505C5.41801 4.36825 5.91361 4.36825 6.19631 4.08505L10.0498 0.220751C10.3418 -0.0731494 10.8169 -0.0731493 11.1108 0.218751C11.4038 0.511751 11.4047 0.98635 11.1123 1.27925L7.25881 5.14355C6.83351 5.57035 6.26771 5.80465 5.66561 5.80465Z" fill="#404040"/>
</svg></span>
      </div>

      {/* DROPDOWN */}
      {open && (
        <div className={styles.dropdown}>
          <div className={styles.header}>
            <button onClick={() => setMonth(new Date(month.getFullYear(), month.getMonth() - 1))}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M5.0556 11.331C4.8642 11.331 4.6723 11.2579 4.5258 11.1123L0.6611 7.25875C0.2353 6.83395 0.000500797 6.26855 7.96571e-07 5.66595C-0.000499203 5.06335 0.2344 4.49795 0.6611 4.07125L4.5263 0.21875C4.8193 -0.0732495 5.2944 -0.0732492 5.5868 0.220751C5.8793 0.513751 5.8783 0.988351 5.5848 1.28125L1.7201 5.13375C1.578 5.27535 1.4999 5.46485 1.4999 5.66595C1.4999 5.86615 1.578 6.05465 1.7201 6.19625L5.5853 10.0498C5.8783 10.3428 5.8792 10.8174 5.5868 11.1103C5.4403 11.2578 5.2479 11.331 5.0556 11.331Z" fill="#757676"/>
</svg></button>
            <span>
              {month.toLocaleDateString("en-US", { month: "long", year: "numeric" })}
            </span>
            <button onClick={() => setMonth(new Date(month.getFullYear(), month.getMonth() + 1))}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#757676"/>
</svg></button>
          </div>

          <div className={styles.week}>
            {week.map(w => <span key={w}>{w}</span>)}
          </div>

          <div className={styles.grid}>
            {days.map(({ date, current }, i) => (
              <div
                key={i}
                onClick={() => select(date)}
                className={`${styles.day}
                  ${!current ? styles.fade : ""}
                  ${sameDay(date, start) ? styles.start : ""}
                  ${sameDay(date, end) ? styles.end : ""}
                  ${inRange(date) ? styles.range : ""}`}
              >
                {date.getDate()}
              </div>
            ))}
          </div>

          <div className={styles.inputs}>
            <div>
              <label>Start date</label>
              <input value={format(start)} placeholder="DD/MM/YYYY" readOnly />
            </div>
            <div>
              <label>End date</label>
              <input value={format(end)} placeholder="DD/MM/YYYY" readOnly />
            </div>
          </div>

          <div className={styles.footer}>
            <button className={styles.cancel} onClick={() => setOpen(false)}>Cancel</button>
            <button className={styles.apply} onClick={()=>{handleDatePicker(start,end); setOpen(false);}}>Apply</button>
          </div>
        </div>
      )}
    </div>
  );
}
