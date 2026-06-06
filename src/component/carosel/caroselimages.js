import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import styles from "../newcaro.module.scss";
import "../../../src/chatPreview.scss";
import newdotleft from "../../images/newdotleft.png";
import newdotright from "../../images/newdotright.png";
import leftgreen from "../../images/leftgreen.png";
import rightgreen from "../../images/rightgreen.png";
import cartbages from "../../images/cartbages.png"
import cartbagesgreen from "../../images/cartbagesgreen.png"
import closeImg from "../../images/blackclose.png"
import { useDispatch, useSelector } from "react-redux";
import { ShopingSiceActions } from "../../store/slice/shoping_slice";
import store from '../../store/redux-store/root_store'
import { shipifyPorductRecommendation } from '../../chatbot/utils2.0'
import { v4 as uuidv4 } from 'uuid';
import { WixOptimizedImage } from './utils'
import { RightArrow, LeftArrow } from './caroselSlider'

export const CarosuelShopingImage = ({ carosuelData }) => {
    const [productList, setProductList] = useState([]);
    const [current, setCurrent] = useState(0);
    const [isHovered, setIsHovered] = useState(false);
    const autoplayRef = useRef(null);
  
    const autoplaySpeed = 2000;
    const transitionSpeed = 1000;
  
    useEffect(() => {
      if (!Array.isArray(carosuelData) || carosuelData.length === 0) {
        setProductList([]);
        setCurrent(0);
        return;
      }
      setProductList(carosuelData);
      setCurrent(0);
    }, [carosuelData]);
  
    const maxIndex = productList.length - 1;
  
    useEffect(() => {
      if (productList.length <= 1 || isHovered) return;
      autoplayRef.current = setInterval(() => {
        setCurrent((prev) => (prev >= maxIndex ? 0 : prev + 1));
      }, autoplaySpeed);
  
      return () => clearInterval(autoplayRef.current);
    }, [productList, maxIndex, isHovered]);
  
    const goNext = () => {
      setCurrent((prev) => (prev >= maxIndex ? prev : prev + 1));
    };
  
    const goPrev = () => {
      setCurrent((prev) => (prev <= 0 ? prev : prev - 1));
    };
  
    const trackStyle = {
      display: "flex",
      transition: `transform ${transitionSpeed}ms ease`,
      transform: `translateX(-${current * 100}%)`,
    };
  
    const slideStyle = {
      flex: `0 0 100%`,
      width: "100%",
      cursor: "pointer",
      boxSizing: "border-box", 
    };
  
    return (
      <div className={styles.homeproduct}>
        <div 
          className={styles.sliderContainer} 
          style={{ overflow: "hidden", padding: 0 }}
          onMouseEnter={() => setIsHovered(true)}
          onMouseLeave={() => setIsHovered(false)}
        >
          <div style={trackStyle}>
            {/* {productList.map((img, idx) => (
              <div key={idx} style={slideStyle}>
                <div className={styles.slideinner}>
                  <div className={styles.imageWrap}>
                    <img src={img} className={styles.image} alt={`slide-${idx}`} />
                  </div>
                </div>
              </div>
            ))} */}
  
            {productList.map((img, idx) => (
              <div
              className={styles.slide}
              key={idx}
              style={slideStyle}
            >
              <div className={`${styles.slideinner} ${styles.bgnone} bgnonenew`}>
                <div className={`${styles.imageWrap} ${styles.singleimgwrapnew} bgnonenewinner`}>
                      <div className={styles.singleimgwrapnewimg}>
                        <WixOptimizedImage
                          src={img}
                          className={styles.image}
                          alt={'img'}
                        />
                      </div>
                </div>
              </div>
              </div>
            ))}
  
  
          </div>
  
          <div className={`${styles.dots} pthysdots ${styles.navbtns}`}>
            {current > 0 && (
              // <img
              //   src={leftgreen}
              //   alt="prev"
              //   onClick={goPrev}
              //   className={styles.leftbages}
              // />

              <LeftArrow onClick={goPrev} className={styles.leftbages}/>

            )}
            {current < maxIndex && (
              // <img
              //   src={rightgreen}
              //   alt="next"
              //   onClick={goNext}
              //   className={styles.rightbages}
              // />
              <RightArrow onClick={goNext} className={styles.rightbages}/>
            )}
  
            
          </div>
        </div>
      </div>
    );
  };