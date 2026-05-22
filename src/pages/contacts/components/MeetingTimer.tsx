import { memo, useEffect, useState } from 'react';

const MeetingTimer = ({ startTime, fallbackText = 'Yet to start' }: any) => {
  const [text, setText] = useState<any>(null);

  useEffect(() => {
    if (!startTime) {
      setText(null);
      return;
    }

    let timeoutId: number | null = null;

    const update = () => {
      const now = new Date();
      const start = new Date(startTime);
      const startMs = start.getTime();

      if (Number.isNaN(startMs)) {
        setText(null);
        return;
      }

      const diff = startMs - now.getTime();
      if (diff <= 0) {
        setText(null);
        return;
      }

      const totalSeconds = Math.floor(diff / 1000);
      const totalMinutes = Math.floor(totalSeconds / 60);
      const totalHours = Math.floor(totalMinutes / 60);
      const minutes = totalMinutes % 60;

      const startDay = new Date(start.getFullYear(), start.getMonth(), start.getDate()).getTime();
      const nowDay = new Date(now.getFullYear(), now.getMonth(), now.getDate()).getTime();
      const dayDiff = Math.floor((startDay - nowDay) / (1000 * 60 * 60 * 24));

      const formatRemaining = () => {
        if (totalSeconds < 60) return `${totalSeconds} sec`;
        if (totalHours > 0) {
          return `${totalHours} hr${totalHours !== 1 ? 's' : ''} ${minutes} min`;
        }
        return `${minutes} min`;
      };

      if (dayDiff === 0) setText(`Today, ${formatRemaining()}`);
      else if (dayDiff === 1) setText(`Tomorrow, ${formatRemaining()}`);
      else if (dayDiff > 1) setText(`Starts in ${dayDiff} day${dayDiff === 1 ? '' : 's'}`);
      else setText(null);

      let nextDelay: number;
      if (dayDiff > 30) nextDelay = 24 * 60 * 60 * 1000;
      else if (dayDiff > 1) nextDelay = 60 * 60 * 1000;
      else if (totalHours > 1) nextDelay = 60 * 60 * 1000;
      else if (totalMinutes > 1) nextDelay = 60 * 1000;
      else nextDelay = 1000;

      timeoutId = window.setTimeout(update, nextDelay);
    };

    update();

    return () => {
      if (timeoutId) {
        window.clearTimeout(timeoutId);
      }
    };
  }, [startTime]);

  return <>{text ?? fallbackText}</>;
};

export default memo(MeetingTimer);
