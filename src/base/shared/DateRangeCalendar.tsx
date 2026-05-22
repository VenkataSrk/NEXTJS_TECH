import React, { useState } from "react";
import styles from "./DateRangeCalendar.module.scss";

const WEEK_DAYS = ["S", "M", "T", "W", "T", "F", "S"];

interface CalendarDay {
  date: Date;
  currentMonth: boolean;
}

const getDays = (year: number, month: number): CalendarDay[] => {
  const days: CalendarDay[] = [];
  const firstDay = new Date(year, month, 1).getDay();

  for (let i = firstDay - 1; i >= 0; i--) {
    days.push({ date: new Date(year, month, -i), currentMonth: false });
  }

  const d = new Date(year, month, 1);
  while (d.getMonth() === month) {
    days.push({ date: new Date(d), currentMonth: true });
    d.setDate(d.getDate() + 1);
  }

  while (days.length < 42) {
    days.push({ date: new Date(d), currentMonth: false });
    d.setDate(d.getDate() + 1);
  }

  return days;
};

const isSameDay = (a: Date | null, b: Date | null): boolean =>
  !!a &&
  !!b &&
  a.getDate() === b.getDate() &&
  a.getMonth() === b.getMonth() &&
  a.getFullYear() === b.getFullYear();

interface DateRangeCalendarProps {
  onChange?: (start: Date | null, end: Date | null) => void;
  initialStart?: Date | null;
  initialEnd?: Date | null;
}

export default function DateRangeCalendar({
  onChange,
  initialStart = null,
  initialEnd = null,
}: DateRangeCalendarProps) {
  const today = new Date();
  const [viewMonth, setViewMonth] = useState(
    new Date(today.getFullYear(), today.getMonth())
  );
  const [start, setStart] = useState<Date | null>(initialStart);
  const [end, setEnd] = useState<Date | null>(initialEnd);

  const days = getDays(viewMonth.getFullYear(), viewMonth.getMonth());

  const handleSelect = (date: Date) => {
    if (!start || end) {
      setStart(date);
      setEnd(null);
      onChange?.(date, null);
    } else if (date < start) {
      setStart(date);
      setEnd(start);
      onChange?.(date, start);
    } else {
      setEnd(date);
      onChange?.(start, date);
    }
  };

  const prevMonth = () =>
    setViewMonth(new Date(viewMonth.getFullYear(), viewMonth.getMonth() - 1));

  const nextMonth = () =>
    setViewMonth(new Date(viewMonth.getFullYear(), viewMonth.getMonth() + 1));

  const monthLabel = viewMonth.toLocaleDateString("en-US", {
    month: "long",
    year: "numeric",
  });

  return (
    <div className={styles.calendar}>
      {/* Header */}
      <div className={styles.header}>
        <button className={styles.navBtn} onClick={prevMonth} aria-label="Previous month">
          <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
            <path d="M5.0556 11.331C4.8642 11.331 4.6723 11.2579 4.5258 11.1123L0.6611 7.25875C0.2353 6.83395 0.000500797 6.26855 7.96571e-07 5.66595C-0.000499203 5.06335 0.2344 4.49795 0.6611 4.07125L4.5263 0.21875C4.8193 -0.0732495 5.2944 -0.0732492 5.5868 0.220751C5.8793 0.513751 5.8783 0.988351 5.5848 1.28125L1.7201 5.13375C1.578 5.27535 1.4999 5.46485 1.4999 5.66595C1.4999 5.86615 1.578 6.05465 1.7201 6.19625L5.5853 10.0498C5.8783 10.3428 5.8792 10.8174 5.5868 11.1103C5.4403 11.2578 5.2479 11.331 5.0556 11.331Z" fill="currentColor"/>
          </svg>
        </button>
        <span className={styles.monthLabel}>{monthLabel}</span>
        <button className={styles.navBtn} onClick={nextMonth} aria-label="Next month">
          <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="currentColor"/>
          </svg>
        </button>
      </div>

      {/* Week day labels */}
      <div className={styles.weekRow}>
        {WEEK_DAYS.map((d, i) => (
          <span key={i} className={styles.weekDay}>{d}</span>
        ))}
      </div>

      {/* Day grid */}
      <div className={styles.grid}>
        {days.map(({ date, currentMonth }, i) => {
          const isStart = isSameDay(date, start);
          const isEnd = isSameDay(date, end);
          const inRange = !!start && !!end && date > start && date < end;

          return (
            <div
              key={i}
              onClick={() => handleSelect(date)}
              className={[
                styles.day,
                !currentMonth ? styles.fade : "",
                inRange ? styles.inRange : "",
                isStart ? styles.startDay : "",
                isEnd ? styles.endDay : "",
              ]
                .filter(Boolean)
                .join(" ")}
              aria-label={date.toLocaleDateString("en-GB")}
              role="button"
              tabIndex={0}
              onKeyDown={(e) => e.key === "Enter" && handleSelect(date)}
            >
              {date.getDate()}
            </div>
          );
        })}
      </div>
    </div>
  );
}
