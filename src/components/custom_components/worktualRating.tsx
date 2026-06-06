// Rating.tsx
import React, { useState } from 'react';

interface RatingProps {
  max?: number;
  value?: number;
  onChange?: (rating: number) => void;
  size?: number;
  readOnly?: boolean;
}

const WorktualRating: React.FC<RatingProps> = ({
  max = 5,
  value = 0,
  onChange,
  size = 24,
  readOnly = false,
}) => {
  const [hovered, setHovered] = useState<number | null>(null);

  return (
    <div style={{ display: 'flex', gap: 4 }}>
      {Array.from({ length: max }, (_, i) => {
        const starValue = i + 1;
        const isFilled = hovered !== null ? starValue <= hovered : starValue <= value;

        return (
          <svg
            key={i}
            onMouseEnter={() => !readOnly && setHovered(starValue)}
            onMouseLeave={() => !readOnly && setHovered(null)}
            onClick={() => !readOnly && onChange?.(starValue)}
            width={size}
            height={size}
            viewBox="0 0 24 24"
            fill={isFilled ? '#facc15' : 'none'}
            stroke="#facc15"
            strokeWidth="2"
            strokeLinecap="round"
            strokeLinejoin="round"
            style={{ cursor: readOnly ? 'default' : 'pointer' }}
          >
            <polygon points="12 2 15 8.5 22 9.3 17 14 18.5 21 12 17.8 5.5 21 7 14 2 9.3 9 8.5 12 2" />
          </svg>
        );
      })}
    </div>
  );
};

export default WorktualRating;
