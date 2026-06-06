import React, { useState, useRef, useEffect, ReactNode } from "react";
import styles from "./scss/slider.module.scss";

interface SliderProps {
  children: ReactNode | ReactNode[];
  currentSlide:any
}

const Slider: React.FC<SliderProps> = ({ children,currentSlide }) => {
  const [activeIndex, setActiveIndex] = useState(0);
  const sliderRef = useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    const slider = sliderRef.current;
    if (!slider) return;

    const handleScroll = () => {
      const index = Math.round(slider.scrollLeft / slider.offsetWidth);
      setActiveIndex(index);
    };

    slider.addEventListener("scroll", handleScroll);
    return () => {
      slider.removeEventListener("scroll", handleScroll);
    };
  }, []);


  const childArray = React.Children.toArray(children);

  // 🔹 Function to scroll to a slide when dot is clicked
  const goToSlide = (index: number) => {
    const slider = sliderRef.current;
    if (!slider) return;
    slider.scrollTo({
      left: slider.offsetWidth * index,
      behavior: "smooth",
    });
    setActiveIndex(index);
    currentSlide(index)
  };

  return (
    <div className={styles.sliderContainer}>
      {/* Scrollable Wrapper */}
      <div className={styles.slider} ref={sliderRef}>
        {childArray.map((child, index) => (
          <div key={index} className={styles.slide}>
            {child}
          </div>
        ))}
      </div>

      {/* Dots with click functionality */}
      <div className={styles.dots}>
        {childArray.map((_, i) => (
          <span
            key={i}
            onClick={() => goToSlide(i)}
            className={`${styles.dot} ${activeIndex === i ? styles.active : ""}`}
          />
        ))}
      </div>
    </div>
  );
};

export default Slider;
