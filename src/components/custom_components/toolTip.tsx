import React, { useState } from "react";
import './scss/tooltip.scss';

type TooltipProps = {
  children: React.ReactNode;
  content: string;
  position?: "top" | "bottom" | "left" | "right";
  className?: string;
  // isHover:
};

const Tooltip: React.FC<TooltipProps> = ({ children, content,className, position = "top" }) => {
  const [visible, setVisible] = useState(false);

  return (
    <div className={`tooltipcontainer ${className}`} onMouseEnter={() => setVisible(true)} onMouseLeave={() => setVisible(false)}>
      {children}
      {visible && (
        <div className={`tooltipbox ${
            position === "top" ? "top" : ""
          } ${
            position === "bottom" ? "bottom" : ""
          } ${
            position === "left" ? "left" : ""
          } ${
            position === "right" ? "right" : ""
          }`}
        >
          {content}
        </div>
      )}
    </div>
  );
};

export default Tooltip;
