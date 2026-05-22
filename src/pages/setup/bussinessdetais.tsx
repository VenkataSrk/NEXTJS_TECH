import React, { useRef } from 'react';
import styles from './scss/set.module.scss';
import { useLocation, useNavigate } from 'react-router-dom';

const Businessdetails = ({data, open, setIsOpen} : any) => {
  const navigate = useNavigate();

  const weekDays = ["Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"];

  const splitFunction = (v?: string) => v?.split(',').map(d => d.trim()) || [];

  const workingDays = splitFunction(data?.workingDays);
  const dayOffDays = splitFunction(data?.dayOff);
  const hours = splitFunction(data?.operatingHours);

  const dayHourMap = workingDays.reduce<Record<string, string[]>>(
    (acc, day, i) => ({ ...acc, [day]: [hours[i]] }),
    {}
  );

  const getTimeDiff = (from?: string, to?: string) => {
    try {
      if (!from || !to) return "00 Hrs";
  
      const [fh, fm] = from.split(':').map(Number);
      const [th, tm] = to.split(':').map(Number);
  
      if (isNaN(fh) || isNaN(fm) || isNaN(th) || isNaN(tm)) {
        return "00 Hrs";
      }
  
      let start = fh * 60 + fm;
      let end = th * 60 + tm;
      if (end < start) end += 24 * 60;
  
      const diff = end - start;
      const hrs = Math.floor(diff / 60);
      const mins = diff % 60;
  
      return `${hrs}${mins ? `:${mins.toString().padStart(2, '0')}` : ''} Hrs`;
    } catch (error) {
      return "00 Hrs";
    }
  };
  

  return (
    
      <div className={styles.businesscontainer}>
          <div className={`${styles.breadcrumscontainer} ${styles.maincontainer}`}>
            <div className={styles.breadtitle} onClick={() => setIsOpen()}>Business Hours</div>
            <div className={`${styles.breadtitle} ${styles.active}`}>{data?.name}</div>
          </div>


          {weekDays.map(day => {
            const isDayOff = dayOffDays?.includes(day);
            const slots = dayHourMap?.[day] || [];

            return (
              <div className={styles.daycontainer} key={day}>
                <div className={styles.daytitle}>{day}</div>

                <div className={styles.hourcontainer}>
                  {(isDayOff || !slots?.length)
                    ? <div className={styles.dayoffcontent}>Day Off</div>
                    : slots?.map((slot : any, index : any) => {
                        const [from, to] = slot?.split(' - ');
                        return (
                          <div className={styles.inputfieldbox} key={index}>
                            <input value={from} readOnly />
                            <div className={styles.totitle}>To</div>
                            <input value={to} readOnly />
                            <div className={styles.totitle}>{getTimeDiff(from, to)}</div>
                          </div>
                        );
                      })}
                </div>
              </div>
            );
          })}
        </div>
  );
};

export default Businessdetails;
