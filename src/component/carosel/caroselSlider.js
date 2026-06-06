

import React, { useCallback, useEffect, useMemo, useRef, useState } from "react";
import styles from "../newcaro.module.scss";
import "../../../src/chatPreview.scss";
import leftgreen from "../../images/leftgreen.png";
import rightgreen from "../../images/rightgreen.png";
import closeImg from "../../images/blackclose.png"
import { useDispatch, useSelector } from "react-redux";
import { ShopingSiceActions } from "../../store/slice/shoping_slice";
import store from '../../store/redux-store/root_store'
import { shipifyPorductRecommendation } from '../../chatbot/utils2.0'
import { v4 as uuidv4 } from 'uuid'
import { nickysDomainAI } from '../../base/constants'
import { getPerticularData, WixOptimizedImage, stripHtml } from './utils'

export const RightArrow = ({onClick, className}) => {

    return (
        <svg 
            className={className}
            onClick={onClick}
            width="42" height="42" viewBox="0 0 42 42" fill="none" xmlns="http://www.w3.org/2000/svg">
            <g filter="url(#filter0_d_4463_897)">
                <rect x="2" y="1" width="38" height="38" rx="19" fill={'#00963E'}/>
                <rect x="2.3" y="1.3" width="37.4" height="37.4" rx="18.7" stroke={'#fff'} stroke-width="0.6" />
                <g clip-path="url(#clip0_4463_897)">
                    <path d="M18.8471 25.6654C18.6547 25.6654 18.4623 25.5922 18.3159 25.4447C18.0234 25.1517 18.0244 24.6771 18.3179 24.3842L22.1822 20.5317C22.3243 20.3901 22.4024 20.2006 22.4024 19.9995C22.4024 19.7993 22.3243 19.6108 22.1822 19.4692L18.3175 15.6157C18.0245 15.3227 18.0236 14.8481 18.316 14.5552C18.608 14.2613 19.0831 14.2613 19.377 14.5532L23.2413 18.4067C23.6671 18.8315 23.9019 19.3969 23.9024 19.9995C23.9029 20.6021 23.668 21.1675 23.2413 21.5942L19.3766 25.4467C19.2306 25.5922 19.0385 25.6654 18.8471 25.6654Z" fill="white" />
                </g>
            </g>
            <defs>
                <filter id="filter0_d_4463_897" x="0" y="0" width="42" height="42" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                    <feFlood flood-opacity="0" result="BackgroundImageFix" />
                    <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                    <feOffset dy="1" />
                    <feGaussianBlur stdDeviation="1" />
                    <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.051 0" />
                    <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_4463_897" />
                    <feBlend mode="normal" in="SourceGraphic" in2="effect1_dropShadow_4463_897" result="shape" />
                </filter>
                <clipPath id="clip0_4463_897">
                    <rect width="24" height="24" fill="white" transform="translate(9 8)" />
                </clipPath>
            </defs>
        </svg>
    )
}

export const LeftArrow = ({ onClick, className}) => {
    return (
        <svg 
            className={className}
            onClick={onClick}
            width="42" height="42" viewBox="0 0 42 42" fill="none" xmlns="http://www.w3.org/2000/svg">
            <g filter="url(#filter0_d_4463_896)">
                <rect x="2" y="1" width="38" height="38" rx="19" fill={'#00963E'} />
                <rect x="2.3" y="1.3" width="37.4" height="37.4" rx="18.7" stroke={'#fff'} stroke-width="0.6" />
                <g clip-path="url(#clip0_4463_896)">
                    <path d="M23.1528 25.6655C22.9614 25.6655 22.7695 25.5923 22.623 25.4467L18.7583 21.5932C18.3325 21.1684 18.0977 20.603 18.0972 20.0004C18.0967 19.3978 18.3316 18.8324 18.7583 18.4057L22.6235 14.5532C22.9165 14.2612 23.3916 14.2612 23.684 14.5552C23.9765 14.8482 23.9755 15.3228 23.682 15.6157L19.8173 19.4682C19.6752 19.6098 19.5971 19.7993 19.5971 20.0004C19.5971 20.2006 19.6752 20.3891 19.8173 20.5307L23.6825 24.3842C23.9755 24.6772 23.9764 25.1518 23.684 25.4447C23.5375 25.5922 23.3451 25.6655 23.1528 25.6655Z" fill="white" />
                </g>
            </g>
            <defs>
                <filter id="filter0_d_4463_896" x="0" y="0" width="42" height="42" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                    <feFlood flood-opacity="0" result="BackgroundImageFix" />
                    <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                    <feOffset dy="1" />
                    <feGaussianBlur stdDeviation="1" />
                    <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.051 0" />
                    <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_4463_896" />
                    <feBlend mode="normal" in="SourceGraphic" in2="effect1_dropShadow_4463_896" result="shape" />
                </filter>
                <clipPath id="clip0_4463_896">
                    <rect width="24" height="24" fill="white" transform="translate(9 8)" />
                </clipPath>
            </defs>
        </svg>
    )
}



export const CarosuelShoping = ({ carosuelData, title, uuid }) => {
    const messageList = useSelector((store)=> store.message);
    const [productList, setProductList] = useState([])
    const [compoentUUID, setCompoentUUID] = useState(uuid ?? uuidv4())
    const [isHovered, setIsHovered] = useState(false);

    const checkDomin = (domainId)=> `${messageList.personalisationAll?.domainId}` === domainId;

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
        if (!productList.length || isHovered) return;
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
    const [loading, setLoading] = useState(false);

    const selectedItem = async (item) => {

        const existing = cartData.find(c => c.variant_id === item.selectedItem?.variant_id);
        const updatedItem = {
            ...item,
            selectedItem: {
                ...item.selectedItem,
                ...(existing || {})
            }
        };

        if(checkDomin(nickysDomainAI)){
            setLoading(true)
            const res = await getPerticularData(item.product_id, dispatch);
            setLoading(false)
            if (!Object.keys(res)?.length) {
                dispatch(ShopingSiceActions.setShowItemModel({ itemDetails: {}, toggle: false }));
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
        }else {
            dispatch(
                ShopingSiceActions.setShowItemModel({
                    itemDetails: updatedItem,
                    toggle: true
                })
            );
        }
    };
    

    useEffect(() => {
        if(checkDomin(nickysDomainAI)){ 
            formatNicks()
        }else {
            formatPothys()
        }
    }, [carosuelData?.length]);

    const formatPothys =()=> {
        const carosuelData_filter = [...carosuelData].filter((item) => item?.image)
        const formatted = carosuelData_filter?.map((product) => {
            const variants = Array.isArray(product.variants) ? product.variants : [];

            if (variants.length === 0) {
                return { ...product, sizes: [], availableColors: [], selectedSize: null, selectedItem: null };
            }

            const sizes = [...new Set(variants.map((v) => v.size))];
            const defaultSize = sizes[0] ?? null;

            const colors = variants
                .filter((v) => v.size === defaultSize)
                .map((v) => ({
                    ...v,
                    // normalize types:
                    variant_id: v.variant_id,
                    amount_rs: Number(v.amount_rs),
                    image: v.image || product.image,
                    available_quantity: Number(v.available_quantity ?? 0),
                    quantity: 0,
                }));

            const selectedItem = colors[0] ?? null;

            return {
                ...product,
                variants,
                sizes,
                selectedSize: defaultSize,
                availableColors: colors,
                selectedItem,
            };
        });

        setProductList(formatted)
        setCurrent(0)
    }

    const formatNicks =() => {
        if (!Array.isArray(carosuelData) || carosuelData.length === 0) {
            setProductList([])
            return;
        }
        console.log("carosuelData", carosuelData)
        setProductList(carosuelData)
        setCurrent(0)
    }

    return (

        <>
            <div className={`${styles.homeproduct}`}>
                <div className={styles.maintitle}>{title ?? "Results"}</div>

                <div className={styles.sliderContainer}
                    onMouseEnter={() => setIsHovered(true)}
                    onMouseLeave={() => setIsHovered(false)}
                    style={current >= maxIndex - 1 ? { padding: "0 0 0 20px" } : {}}
                >
                    <div className={styles.sliderTrack} style={current >= maxIndex ? trackStyle1 : trackStyle}>
                        {productList.map((row, idx) => {
                            return (
                                <div
                                    className={styles.slide}
                                    key={idx}
                                    style={slideStyle}
                                    onClick={() => selectedItem(row)}
                                >
                                    <div className={`${styles.slideinner} ${loading ? "loadingcarosel" : ''} `}>
                                        <div className={styles.imageWrap}>
                                            <WixOptimizedImage
                                                src={row.image}
                                                className={styles.image}
                                                alt={row?.productType || row.title}
                                            />
                                        </div>
    
                                        <div className={styles.content}>

                                            {checkDomin(nickysDomainAI) 
                                            ? (
                                            <>
                                                <div className={styles.proname}>{row.title}</div>
                                                <div 
                                                    className={styles.pronamesub} 
                                                    dangerouslySetInnerHTML={{ __html: row.description }}
                                                />
                                            </> 
                                            ) : (
                                            <>
                                                <div className={styles.proname}>{row?.productType}</div>
                                                <div className={styles.pronamesub}>{row.title}</div>
                                            </>)}


                                            <div className={styles.pronameprice}> &#8377; {
                                                row.min_price ? Number(row.min_price)
                                                    : row.max_price ? Number(row.max_price)
                                                        : row.price ? Number(row.price)
                                                            : 0}</div>
                                        </div>
                                    </div>
                                </div>
                            )
                        })}
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