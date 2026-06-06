import React, { useCallback, useRef, useState } from "react";
import { useDispatch, useSelector } from "react-redux";
import { ShopingSiceActions } from "../../../store/slice/shoping_slice";
import { ShopingCartView } from "./cartList";
import { wixCheckoutPage } from '../utils'


export const ShopingCartNikys = () => {
    const { cartData } = useSelector((store) => store.shopingStore);
    const dispatch = useDispatch();
  
    const [showAlert, setShowAlert] = useState({ open: false, count: 0 });
    const alertTimer = useRef(null);
    const [loading, setLoading] = useState(false); 
    const triggerAlert = () => {
      if (alertTimer.current) {
        clearTimeout(alertTimer.current);
      }
      alertTimer.current = setTimeout(() => {
        setShowAlert({ open: false, count: 0 });
      }, 3000);
    };
  
    const closeCart = useCallback(() => {
      dispatch(ShopingSiceActions.setShowCartModel(false));
    }, [dispatch]);
  
 
    const purchase = useCallback(async () => {
      if (loading) return;
      setLoading(true)
      const res = await wixCheckoutPage(cartData, dispatch);
      setLoading(false)
      if (res?.statusCode === 200) {
        const checkouturl = res?.result?.checkoutUrl;
        window.open(checkouturl, '_blank')
      }
      closeCart();
    }, [cartData, loading]);
  
    const hadCart = useCallback(
      () => !cartData.every((i) => i.quantity === 0),
      [cartData]
    );
  
    const getCost = useCallback(() => {
      try {
        return cartData.reduce((acc, itm) => {
          if (itm.quantity > 0 && itm.amount_rs) {
            acc += Number(itm.quantity) * Number(itm.amount_rs);
          }
          return acc;
        }, 0);
      } catch {
        return 0;
      }
    }, [cartData]);
  
    const updateQty = useCallback(
      (item, type) => {
        try {
          let newCart = [...cartData];
          const index = newCart.findIndex((i) => i.variant_id === item.variant_id && i.product_id === item.product_id);
  
          if (index === -1) return;
          
          let qty = newCart[index].quantity;
  
          if (type === "plus") qty = qty + 1;
          if (type === "minus") qty = Math.max(qty - 1, 0);
  
          if (qty === 0) {
            newCart.splice(index, 1);
          } else {
            newCart[index] = {
              ...newCart[index],
              quantity: qty,
            };
          }
  
          dispatch(ShopingSiceActions.setCartData(newCart));
  
          if (newCart.length === 0) {
            closeCart();
          }
        } catch (error) {
          console.log(error);
        }
      },
      [cartData, closeCart, dispatch]
    );

    const handelSlectItem = useCallback((itm)=> {
      
    })
  
    return (
      <ShopingCartView
        cartData={cartData}
        totalCost={getCost()}
        hadCart={hadCart()}
        onCloseCart={closeCart}
        onPurchase={purchase}
        loading={loading}
        onUpdateQty={updateQty}
        showAlert={showAlert}
        onCloseAlert={() => setShowAlert({ open: false, count: 0 })}
        onSelectItem={handelSlectItem}
      />
    );
  };
  