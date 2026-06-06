

import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import styles from "./newcaro.module.scss";
import "../../src/chatPreview.scss";
import leftgreen from "../images/leftgreen.png";
import rightgreen from "../images/rightgreen.png";
import { useDispatch, useSelector } from "react-redux";
import { ShopingSiceActions } from "../store/slice/shoping_slice";
import { v4 as uuidv4 } from 'uuid';
import { getPerticularData } from './carosel/utils'
import { RightArrow, LeftArrow } from './carosel/caroselSlider'


export const CarosuelShoping_nicky = ({ carosuelData, title, uuid }) => {

  const [productList, setProductList] = useState([])
  const [isHovered, setIsHovered] = useState(false);

  const autoplaySpeed = 2000;
  const transitionSpeed = 600;

  const getSlidesToShow = () => {
    if (window.innerWidth <= 768) return 2;
    if (window.innerWidth <= 1024) return 2;
    return 2;
  };

  const [slidesToShow, setSlidesToShow] = useState(2);

  useEffect(() => {
    setSlidesToShow(getSlidesToShow());
    const handleResize = () => setSlidesToShow(getSlidesToShow());
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

  const maxIndex = Math.max(productList.length - slidesToShow, 0) + 1;

  const [current, setCurrent] = useState(0);
  const autoplayRef = useRef(null);

  useEffect(() => {
    if(!productList.length || isHovered) return ;
    startAutoplay();
    return stopAutoplay;
  }, [productList?.length, isHovered]);

  const startAutoplay = () => {
    stopAutoplay();
    autoplayRef.current = window.setInterval(() => goNext(), autoplaySpeed);
  };

  const stopAutoplay = () => {
    if (autoplayRef.current) {
      clearInterval(autoplayRef.current);
      autoplayRef.current = null;
    }
  };

  const goNext = () => {
    //startAutoplay()
    setCurrent((prev) => {
      if (prev >= maxIndex - 1) return prev;
      return prev + 1;
    });
  };

  const goPrev = () => {
   // startAutoplay()
    setCurrent((prev) => {
      if (prev <= 0) return prev;
      return prev - 1;
    });
  };

  const trackStyle = {
    display: "flex",
    transition: `transform ${transitionSpeed}ms ease`,
    transform: `translateX(-${(current * 100) / slidesToShow}%)`,
  };

  const trackStyle1 = {
    display: "flex",
    transition: `transform ${transitionSpeed}ms ease`,
    transform: `translateX(-${(current * 100) / slidesToShow}% - 12px)`,
  };
  

  const slideStyle = {
    flex: `0 0 calc(${100 / slidesToShow}% - 12px)`,
    width: `calc(${100 / slidesToShow}% - 12px)`,
    cursor: 'pointer'
  };

  
  const dispatch = useDispatch()
  const { itemDetails, cartData } = useSelector((store) => store.shopingStore);

  const selectedItem = async (item) => {

    const existing = cartData.find(c => c.variant_id === item.selectedItem?.variant_id);
    const updatedItem = {
      ...item, 
      selectedItem: {
        ...item.selectedItem,
        ...(existing || {})
      }
    };

    const res = await getPerticularData(item.product_id, dispatch);
    if(!Object.keys(res)?.length){
      dispatch( ShopingSiceActions.setShowItemModel({ itemDetails: {}, toggle: false })  );
      dispatch(ShopingSiceActions.setProduct({}))
      return;
    };

    dispatch(ShopingSiceActions.setProduct(res ?? {}));
    dispatch(
      ShopingSiceActions.setShowItemModel({
        itemDetails: updatedItem,
        toggle: true
      })
    );
  };

  useEffect(() => {
    if (!Array.isArray(carosuelData) || carosuelData.length === 0) {
      // dispatch(ShopingSiceActions.setProductList([]));
      setProductList([])
      return;
    }
    console.log("carosuelData", carosuelData)
    setProductList(carosuelData)
    setCurrent(0)

  }, [carosuelData?.length]);

  return (

    <>
      <div className={styles.homeproduct}>
        <div className={styles.maintitle}>{title ?? "Results"}</div>

        <div className={styles.sliderContainer}
         onMouseEnter={() => setIsHovered(true)}
         onMouseLeave={() => setIsHovered(false)}
         style={current >= maxIndex - 1 ? { padding: "0 0 0 20px" } : {}}
         >
          <div className={styles.sliderTrack} style={current >= maxIndex  ? trackStyle1 : trackStyle}>
            {productList.map((row, idx) => (
              <div
                className={styles.slide}
                key={idx}
                style={slideStyle}
                onClick={() => selectedItem(row)}
              >
                <div className={styles.slideinner}>
                  <div className={styles.imageWrap}>
                    <img src={row.image} className={styles.image} />
                  </div>

                  <div className={styles.content}>

                    <div className={styles.proname}>{row.title}</div>

                    <div className={styles.pronameprice}> &#8377; { 
                      row.min_price ? Number(row.min_price) 
                      : row.max_price ? Number(row.max_price) 
                      : row.price ? Number(row.price) 
                      : 0 }</div>
                  </div>
                </div>
              </div>
            ))}
          </div>

          {/* Arrows with hide/show */}
          <div className={`${styles.dots} pthysdots`}>
            {current > 0 && (
              // <img src={leftgreen} alt="" onClick={goPrev} className={styles.leftbages} />
              <LeftArrow onClick={goPrev} className={styles.leftbages}/>
            )}

            {current < maxIndex - 1 && (
              // <img src={rightgreen} alt="" onClick={goNext} className={styles.rightbages} />
              <RightArrow onClick={goNext} className={styles.rightbages}/>
            )}
          </div>
        </div>
      </div>
    </>
  );
}
