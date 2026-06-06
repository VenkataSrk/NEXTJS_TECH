import React, { useCallback,useRef, useEffect, useState } from "react";
import { useDispatch, useSelector } from "react-redux";
import closeImg from "../../images/blackclose.png";
import { ShopingSiceActions } from "../../store/slice/shoping_slice";
import { WixOptimizedImage } from "./utils";
import { v4 as uuidv4 } from 'uuid'
import {CarosuelShopingImage } from '../carosel/caroselimages';
import { CarosuelShoping } from '../carosel/caroselSlider'

export const ShopingSelectedItem_Pothys = () => {
  const dispatch = useDispatch();
  const { itemDetails, cartData } = useSelector((store) => store.shopingStore);
  const [recommentedProd, setRecommentedProd] = useState([])
  const [isrecommentedProd, setIsRecommentedProd] = useState(true)
  const [caroselUuid, setCaroselUuid] = useState(uuidv4())
  const [showAlert, setShowAlert] = useState({open: false, count: 0})
  const alertTimer = useRef(null);

  // useEffect(() => {
  //   if (itemDetails?.product_id) {
  //     setIsRecommentedProd(false)
  //     shipifyPorductRecommendation({
  //       productId: itemDetails?.product_id ?? '0',
  //       variantId: "0"
  //     })
  //       .then((res) => {
  //         setRecommentedProd(res)
  //         // autoScroll()
  //         setCaroselUuid(uuidv4())
  //         setIsRecommentedProd(true)
  //       })
  //       .catch(() => {
  //         setIsRecommentedProd(true)
  //       })
  //   }

  // }, [itemDetails?.product_id])

  const autoScroll = () => {
    try {
      document.getElementById('auto-scroller-mainpage').scrollTo({
        top: 0,
        behavior: 'smooth'
      });
    } catch (error) {
      console.log(error)
    }
  }


  const {
    image,
    title,
    productType,
    selectedItem,
    selectedSize,
    sizes,
    availableColors,
  } = itemDetails;

  const cancelModel = () => {
    dispatch(
      ShopingSiceActions.setShowItemModel({
        itemDetails: {},
        toggle: false,
      })
    );
  };

  const openCart = () => {
    cancelModel();
    dispatch(ShopingSiceActions.setShowCartModel(true));
  }

  const onSelectSize = (size) => {
    const newColors = itemDetails.variants.filter((v) => v.size === size);
    const firstVariant = newColors[0];

    const existing = cartData.find(
      (c) => c.variant_id === firstVariant.variant_id
    );

    const updated = {
      ...itemDetails,
      selectedSize: size,
      availableColors: newColors.map((c) => ({
        ...c,
        quantity: c.quantity || 0,
      })),
      selectedItem: {
        ...firstVariant,
        quantity: existing ? existing.quantity : 0,
      },
    };

    dispatch(ShopingSiceActions.updateItemDetails(updated));
  };

  const onSelectColor = (variantId) => {
    const colorItem = availableColors.find((c) => c.variant_id === variantId);

    const existing = cartData.find((c) => c.variant_id === variantId);

    const updated = {
      ...itemDetails,
      selectedItem: {
        ...colorItem,
        quantity: existing ? existing.quantity : 0,
      },
    };

    dispatch(ShopingSiceActions.updateItemDetails(updated));
  };

  const triggerAlert = () => {
    if (alertTimer.current) {
      clearTimeout(alertTimer.current);
    }
    alertTimer.current = setTimeout(() => {
      setShowAlert({open: false, count: 0});
    }, 3000);
  };

  const updateQuantity = (type) => {
    let qty = selectedItem.quantity;
    const maxQty = Number(selectedItem.available_quantity) || 1;

    if (type === "plus" && qty >= maxQty) {
      setShowAlert({ open: true, count: maxQty})
      triggerAlert()
      return;
    }

    if (type === "plus") qty = Math.min(qty + 1, maxQty);
    if (type === "minus") qty = Math.max(qty - 1, 0);

    const updated = {
      ...itemDetails,
      selectedItem: {
        ...selectedItem,
        quantity: qty,
      },
    };

    dispatch(ShopingSiceActions.updateItemDetails(updated));

    let newCart = [...cartData];
    const index = newCart.findIndex(
      (x) => x.variant_id === selectedItem.variant_id
    );

    if (qty === 0) {
      if (index !== -1) newCart.splice(index, 1);
    } else {
      const cartVersion = {
        ...selectedItem,
        quantity: qty,
        title: itemDetails.title,
        productType: itemDetails.productType
      };

      if (index !== -1) {
        newCart[index] = cartVersion;
      } else {
        newCart.push(cartVersion);
      }
    }

    dispatch(ShopingSiceActions.setCartData(newCart));

  };

  const hadData = (d, type) => {
    if (Array.isArray(d)) 
        return d.filter(Boolean).length > (type === 'color' ? 1 : 0) ;
    return !!d;
  };

  const getCost = useCallback(() => {
    try {
      return cartData.reduce((acc, itm) => {
        if (itm.quantity > 0 && itm.amount_rs) {
          acc += Number(itm.quantity) * Number(itm.amount_rs)
        }
        return acc
      }, 0)
    } catch (error) {
      return 0
    }
  }, [cartData])

  if (!itemDetails?.product_id) return null;

  console.log("selectedItem", itemDetails)

  return (
    <div className="endchats endforshops">
      {isrecommentedProd &&
        <div className="endchatsinner endforpothys" style={!selectedItem.images_list?.length ? { maxHeight: '594px'} : {}}>
          <img onClick={cancelModel} src={closeImg} alt="" className="closeicom"/>

          <div className={`singlepro poopro ${recommentedProd?.length ? "pthyssinglepro" : ''}`} id="auto-scroller-mainpage">

            {/* <div className="singleprotop">
              <img src={selectedItem.image || image} alt=""/>
            </div> */}
            {!selectedItem.images_list?.length 
            ? 
            <div className="singleprotopimg">
                <WixOptimizedImage
                  src={selectedItem.image || image}
                  alt={'img'}
                />
                </div>
             : <CarosuelShopingImage carosuelData={selectedItem.images_list}/>}
            

            <div className="singleprocontent" style={{paddingBottom: showAlert?.open || cartData?.length > 0 ? "60px" : ""}}>
              {/* Price + Quantity */}
              <div className="singleproprice">
                <div className="singlepropricename">
                  ₹{Number(selectedItem.amount_rs)}
                </div>

                <div className="singleprocounts">
                  {/* Minus */}
                  <svg
                    onClick={() => updateQuantity("minus")}
                    xmlns="http://www.w3.org/2000/svg"
                    width="11"
                    height="1"
                    viewBox="0 0 11 1"
                    fill="none"
                  >
                    <path
                      d="M10.1869 1H0.5C0.223933 1 0 0.776067 0 0.5C0 0.223933 0.223933 0 0.5 0H10.1869C10.4629 0 10.6869 0.223933 10.6869 0.5C10.6869 0.776067 10.4629 1 10.1869 1Z"
                      fill="white"
                    />
                  </svg>

                  {selectedItem.quantity}

                  {/* Plus */}
                  <svg
                    onClick={() => updateQuantity("plus")}
                    xmlns="http://www.w3.org/2000/svg"
                    width="13"
                    height="13"
                    viewBox="0 0 13 13"
                    fill="none"
                  >
                    <path
                      d="M12.5 6.25C12.5 6.44891 12.421 6.63968 12.2803 6.78033C12.1397 6.92098 11.9489 7 11.75 7H7V11.75C7 11.9489 6.92098 12.1397 6.78033 12.2803C6.63968 12.421 6.44891 12.5 6.25 12.5C6.05109 12.5 5.86032 12.421 5.71967 12.2803C5.57902 12.1397 5.5 11.9489 5.5 11.75V7H0.75C0.551088 7 0.360322 6.92098 0.21967 6.78033C0.0790177 6.63968 0 6.44891 0 6.25C0 6.05109 0.0790177 5.86032 0.21967 5.71967C0.360322 5.57902 0.551088 5.5 0.75 5.5H5.5V0.75C5.5 0.551088 5.57902 0.360322 5.71967 0.21967C5.86032 0.0790177 6.05109 0 6.25 0C6.44891 0 6.63968 0.0790177 6.78033 0.21967C6.92098 0.360322 7 0.551088 7 0.75V5.5H11.75C11.9489 5.5 12.1397 5.57902 12.2803 5.71967C12.421 5.86032 12.5 6.05109 12.5 6.25Z"
                      fill="white"
                    />
                  </svg>
                </div>
              </div>

              <div className="singleprotitle">{productType}</div>
              <div className="singleprodesc">{title}</div>

              {/* Color Selector */}
              {hadData(availableColors, 'color') &&
              <div className="singleprocolor">
                  {availableColors?.map((c) => (
                    <div
                      key={c.variant_id}
                      className={
                        c.variant_id === selectedItem.variant_id
                          ? "singleprocolorinner singleprocolorinneractive"
                          : "singleprocolorinner"
                      }
                      style={{
                        background: c.color,
                        borderColor: c.color,
                      }}
                      onClick={() => onSelectColor(c.variant_id)}
                    ></div>
                  ))}
              </div>}

              {/* Size Selector */}
                {hadData(sizes, 'size') && <div className="singleprosize">
                  {sizes?.map((s) => (
                    <div
                      key={s}
                      className={
                        selectedSize === s
                          ? "singleprosizeinner singleprosizeinneractive"
                          : "singleprosizeinner"
                      }
                      onClick={() => onSelectSize(s)}
                    >
                      {s}
                    </div>
                  ))}
                  </div>} 

            </div>

            {/* {recommentedProd?.length > 0
              && <div className="pthyscsr" style={{paddingBottom: showAlert?.open || cartData?.length > 0 ? "60px" : ""}}>
                <CarosuelShoping carosuelData={recommentedProd} title="You might also like" uuid={caroselUuid} />
              </div>} */}
          </div>
          {/* 
           <div className="bagesbottom">
              <div  className="bagesbuttons" onClick={cancelModel}>  Close </div> */}
              {/* <div className="bagesbuttons" onClick={saveModel}> Done </div> */}
              {showAlert?.open
                ? (
                  <div className="bagesbottom newbadgesbottom">
                  <div className="bagescartav">
                    <div className="bagescartavinner">Only {showAlert?.count || 0} units available
                      <svg
                        onClick={() => setShowAlert({ open: false, count: 0 })}
                        xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                        <path d="M11.0561 9.99623L14.377 6.67532C14.6699 6.38244 14.6699 5.90804 14.377 5.61526C14.0841 5.32248 13.6097 5.32238 13.317 5.61526L9.99604 8.93618L6.67512 5.61526C6.38224 5.32238 5.90784 5.32238 5.61506 5.61526C5.32228 5.90814 5.32218 6.38254 5.61506 6.67532L8.93598 9.99623L5.61506 13.3172C5.32218 13.61 5.32218 14.0844 5.61506 14.3772C5.7615 14.5236 5.95332 14.5968 6.14514 14.5968C6.33696 14.5968 6.52878 14.5236 6.67522 14.3772L9.99613 11.0563L13.3171 14.3772C13.4635 14.5236 13.6553 14.5968 13.8471 14.5968C14.039 14.5968 14.2308 14.5236 14.3772 14.3772C14.6701 14.0843 14.6701 13.6099 14.3772 13.3172L11.0561 9.99623Z" fill="white" />
                        <path d="M9.99584 1.49938C14.6808 1.49938 18.4922 5.31089 18.4922 9.99584C18.4922 14.6808 14.6808 18.4923 9.99584 18.4923C5.31089 18.4923 1.49938 14.6808 1.49938 9.99584C1.49938 5.31089 5.31099 1.49938 9.99584 1.49938ZM9.99584 0C4.47524 0 0 4.47534 0 9.99584C0 15.5163 4.47524 19.9917 9.99584 19.9917C15.5164 19.9917 19.9916 15.5163 19.9916 9.99584C19.9916 4.47534 15.5164 0 9.99584 0Z" fill="white" />
                      </svg>
                    </div>
                  </div>
                  </div>
                ) : (
                  cartData?.length > 0
                    ? (
                      <div className="bagesbottom newbadgesbottom">
                      <div className="bagescartav" style={{background: "#00963E"}}>
                        <div className="bagescartavinner"> {cartData?.length} {cartData?.length === 1 ? "Item" : "Items"} | &#8377; {`${getCost() || '-'}`}
                          <div className='viewcartwithnav' onClick={openCart}>
                            View Cart
                            <div className='backnavicon'>
                              <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                                <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="white" />
                              </svg>
                            </div>
                          </div>
                        </div>
                      </div>
                      </div>
                    ) : <></>
                )}

            </div>
        // </div>
        }
    </div>
  );
};

export default ShopingSelectedItem_Pothys;