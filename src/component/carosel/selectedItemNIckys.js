import React, { useCallback, useEffect, useState } from "react";
import styles from "../newcaro.module.scss";
import "../../../src/chatPreview.scss";
import newdotleft from "../../images/newdotleft.png";
import newdotright from "../../images/newdotright.png";
import leftgreen from "../../images/leftgreen.png";
import rightgreen from "../../images/rightgreen.png";
import cartbages from "../../images/cartbages.png";
import cartbagesgreen from "../../images/cartbagesgreen.png";
import closeImg from "../../images/blackclose.png";
import { useDispatch, useSelector } from "react-redux";
import { ShopingSiceActions } from "../../store/slice/shoping_slice";
import store from "../../store/redux-store/root_store";
import { shipifyPorductRecommendation } from "../../chatbot/utils2.0";
import { v4 as uuidv4 } from "uuid";
import { getPerticularData, WixOptimizedImage } from "./utils";

const ShopingSelectedItem_nicky = () => {
  const dispatch = useDispatch();
  const { itemDetails, cartData, product } = useSelector((s) => s.shopingStore);

  const [selectedVariant, setSelectedVariant] = useState(null);
  
  const safeProduct = product || {};

  console.log("safeProductsafeProduct", selectedVariant)
  console.log("safeProductsafeProduct -- 2", safeProduct)
  const {
    name: productName = "",
    variants = [],
    customTextFields = [],
    media,
    productType = "",
    description,
    productPageUrl
  } = safeProduct;

  const mainImage = itemDetails?.image 
    ||  media?.mainMedia?.image?.url 
    ||  media?.mainMedia?.thumbnail?.url 
    ||  "";

  const pathToURL = productPageUrl?.base ? `${productPageUrl?.base}${productPageUrl?.path}` : '';
  
  const customeFieldData = customTextFields?.find((f) => f.title?.toLowerCase() === "message on cake");
  const { maxLength, title: messageTitle, mandatory: isMessageRequired  }  = customeFieldData ?? { maxLength: 100, title: "", mandatory : false };

  // const messageLimit = customTextFields?.find((f) => f.title?.toLowerCase() === "message on cake")?.maxLength || 100;

  const currentSelectedItem = itemDetails?.selectedItem || {};
  const showQty = Number(currentSelectedItem?.quantity || 0);
  const [desError, setError]= useState(false);
  const hasError = isMessageRequired && showQty > 0 && !currentSelectedItem?.message?.trim()

  useEffect(() => {
    try {
      if (!safeProduct?.id || !Array.isArray(variants) || !variants.length) {
        return;
      }

      // find cart item for this product (any variant)
      const cartItemForProduct = cartData.find(
        (c) => c.product_id === safeProduct.id
      );

      // choose initial variant:
      // 1) if cart has something, use that variant
      // 2) otherwise first variant
      let initialVariant =
        variants.find((v) => v.id === cartItemForProduct?.variant_id) ||
        variants[0];

      console.log("initialVariant", initialVariant)

      if (!initialVariant) return;

      setSelectedVariant(initialVariant);

      const quantity = cartItemForProduct?.quantity || 0;
      const message = cartItemForProduct?.message || "";

      dispatch(
        ShopingSiceActions.updateItemDetails({
          product_id: safeProduct.id,
          image: mainImage,
          selectedItem: {
            product_id: safeProduct.id,
            variant_id: initialVariant.id,
            weight: initialVariant?.choices?.Weight,
            amount_rs: initialVariant?.variant?.priceData?.price || 0,
            thumbnailImg: media?.mainMedia?.image?.url || media?.mainMedia?.thumbnail?.url,
            quantity,
            message,
            messageTitle,
          },
        })
      );
    } catch (e) {
      console.error("Hydration error", e);
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [safeProduct.id, variants.length]);

  const syncCart = useCallback((item) => {
    try {
      const { product_id, variant_id, quantity } = item || {};
      if (!product_id || !variant_id) return;

      const next = [...cartData];
      const idx = next.findIndex((c) => c.product_id === product_id && c.variant_id === variant_id);

      if (!quantity || quantity <= 0) {
        if (idx !== -1) {
          next.splice(idx, 1);
        }
      } else {
        const payload = {
          ...item,
          title: productName,
          productType,
        };

        if (idx !== -1) {
          next[idx] = payload;
        } else {
          next.push(payload);
        }
      }
      dispatch(ShopingSiceActions.setCartData(next));
    } catch (e) {
      console.error("syncCart error", e);
    }
  }, [cartData, dispatch, productName, productType]);

  const onSelectWeight = useCallback(
    (variant) => {
      try {
        if (!variant) return;

        setSelectedVariant(variant);

        const cartItemForVariant = cartData.find((c) => c.product_id === safeProduct.id && c.variant_id === variant.id);

        const quantity = cartItemForVariant?.quantity || 0;
        const message = cartItemForVariant?.message || "";

        dispatch(
          ShopingSiceActions.updateItemDetails({
            product_id: safeProduct.id,
            image: mainImage,
            selectedItem: {
              product_id: safeProduct.id,
              variant_id: variant.id,
              weight: variant?.choices?.Weight,
              amount_rs: variant?.variant?.priceData?.price || 0,
              thumbnailImg: media?.mainMedia?.image?.url || media?.mainMedia?.thumbnail?.url,
              quantity,
              message,
              messageTitle
            },
          })
        );
      } catch (e) {
        console.error("Variant select error", e);
      }
    },
    [
      cartData,
      currentSelectedItem?.message,
      dispatch,
      mainImage,
      safeProduct.id,
    ]
  );


  const onMessageChange = useCallback(
    (e) => {
      try {
        const value = e?.target?.value || "";
        setError(false)
        // 1. always update local selectedItem
        const updatedLocal = {
          ...currentSelectedItem,
          product_id: safeProduct.id,
          variant_id: selectedVariant?.id || currentSelectedItem?.variant_id,
          weight:
            selectedVariant?.choices?.Weight || currentSelectedItem?.weight,
          amount_rs:
            selectedVariant?.variant?.priceData?.price ||
            currentSelectedItem?.amount_rs ||
            0,
          message: value,
        };

        dispatch(
          ShopingSiceActions.updateItemDetails({
            product_id: safeProduct.id,
            image: mainImage,
            selectedItem: updatedLocal,
          })
        );

        // 2. if this variant is already in cart AND quantity > 0, update message in cart immediately
        const existing = cartData.find(
          (c) =>
            c.product_id === updatedLocal.product_id &&
            c.variant_id === updatedLocal.variant_id
        );

        if (existing && existing.quantity > 0) {
          syncCart({
            ...updatedLocal,
            quantity: existing.quantity, // keep same qty
          });
        }

      } catch (e) {
        console.error("Message change error", e);
      }
    },
    [
      cartData,
      currentSelectedItem,
      dispatch,
      mainImage,
      safeProduct.id,
      selectedVariant,
      syncCart,
    ]
  );


  const updateQuantity = useCallback(
    (type) => {
      try {
        setError(false)
        if (!selectedVariant?.stock?.inStock) return;

        const currentQty = Number(currentSelectedItem?.quantity || 0);
        const nextQty =
          type === "plus" ? currentQty + 1 : Math.max(currentQty - 1, 0);

        const updatedItem = {
          product_id: safeProduct.id,
          variant_id: selectedVariant.id,
          weight: selectedVariant?.choices?.Weight,
          amount_rs: selectedVariant?.variant?.priceData?.price || 0,
          quantity: nextQty,
          message: currentSelectedItem?.message || "",
          thumbnailImg: media?.mainMedia?.image?.url || media?.mainMedia?.thumbnail?.url,
          messageTitle,
        };

        // update local
        dispatch(
          ShopingSiceActions.updateItemDetails({
            product_id: safeProduct.id,
            image: mainImage,
            selectedItem: updatedItem,
          })
        );

        // this will insert/update/remove from cart based on qty
        syncCart(updatedItem);
      } catch (e) {
        console.error("Quantity update error", e);
      }
    },
    [
      currentSelectedItem?.message,
      currentSelectedItem?.quantity,
      dispatch,
      mainImage,
      safeProduct.id,
      selectedVariant,
      syncCart,
    ]
  );



  const getCost = useCallback(() => {
    try {
      return cartData.reduce(
        (t, i) => t + Number(i.quantity || 0) * Number(i.amount_rs || 0),
        0
      );
    } catch (e) {
      console.error("getCost error", e);
      return 0;
    }
  }, [cartData]);

  const cancelModel = useCallback(() => {
    try {
      setError(false)
      if(hasError){
        setError(true)
        return;
      };
      dispatch(
        ShopingSiceActions.setShowItemModel({
          itemDetails: {},
          toggle: false,
        })
      );
    } catch (e) {
      console.error("cancelModel error", e);
    }
  }, [dispatch, hasError]);


  const openCart = useCallback(() => {
    try {
      setError(false)
      if(hasError){
        setError(true)
        return;
      };
      setError(false)
      cancelModel();
      dispatch(ShopingSiceActions.setShowCartModel(true));
    } catch (e) {
      console.error("openCart error", e);
    }
  }, [cancelModel, dispatch, hasError]);

  const goToParentWebsite =()=> {
    try {
      window.top.location.href = pathToURL;
    } catch (error) {
      console.error('Redirect failed', error);
    }
  }

  if (!safeProduct?.id || !selectedVariant) return null;


  return (
    <div className="endchats endforshops">
      <div className="endchatsinner">
        <img onClick={cancelModel} src={closeImg} alt="" className="closeicom" />

        <div className="singlepro nickpro" id="auto-scroller-mainpage">
            <div className={isMessageRequired ? "singleprotopimg singleprotopimgformessage" : "singleprotopimg"}>
              {/* <img src={mainImage} style={{ aspectRatio: 1.5 }} alt="" /> */}
              <WixOptimizedImage
                src={mainImage}
                alt={'img'}
              />
            </div>

          <div className="singleprocontent" style={{paddingBottom: cartData?.length > 0 ? "60px" : ""}}>
            <div className="singleproprice">
              <div className="singlepropricename">
                ₹{selectedVariant?.variant?.priceData?.price || 0}
              </div>

              {selectedVariant?.stock?.inStock ? (
                <div className="singleprocounts">
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

                  {showQty}

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
              ) : <div className="nostock"> No Stock </div>}
            </div>

            <div className="singleprotitle" style={pathToURL ? { cursor: 'pointer' } : {}}
              onClick={goToParentWebsite}
            >
              {productName}
            </div>
            <div className="singleprodesc" dangerouslySetInnerHTML={{ __html: description }} />

            <div className="singleprosize">
              {variants?.map(
                (v) =>
                  v?.choices?.Weight && (
                    <div
                      key={v.id}
                      style={{ padding: "0 4px" }}
                      className={
                        selectedVariant?.id === v.id
                          ? "singleprosizeinner singleprosizeinneractive"
                          : "singleprosizeinner"
                      }
                      onClick={() => onSelectWeight(v)}
                    >
                      {v.choices.Weight}
                    </div>
                  )
              )}
            </div>

            {isMessageRequired
              ? (
                <textarea
                  className="formTextareas"
                  placeholder="Message on cake"
                  maxLength={maxLength}
                  value={currentSelectedItem?.message || ""}
                  onChange={onMessageChange}
                  style={{ padding: 6, fontSize: '14px' , borderColor: desError ? 'red' : '#D2D3D4' }}
                />
              )
              : <></>}

              {desError && <p style={{fontSize: 12, color: 'red'}}>Required field</p>}

          </div>
        </div>

        {cartData?.length > 0 && (
          <div className="bagesbottom newbadgesbottom">
            <div className="bagescartav">
              <div className="bagescartavinner">
                {cartData.length} {cartData.length === 1 ? "Item" : "Items"} | ₹
                {getCost() || "-"}
                <div className="viewcartwithnav" onClick={openCart}>
                  View Cart
                  <div className="backnavicon">{/* SVG */}</div>
                </div>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default ShopingSelectedItem_nicky;
