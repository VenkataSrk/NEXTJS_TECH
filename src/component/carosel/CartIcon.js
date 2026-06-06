

import React from "react";
import "../../../src/chatPreview.scss";
import cartbagesgreen from "../../images/cartbagesgreen.png"
import cartbagesblack from "../../images/cartpngblack.png"
import { useDispatch, useSelector } from "react-redux";
import { ShopingSiceActions } from "../../store/slice/shoping_slice";



export const ShopingCartIcon = () => {
    const { itemDetails, cartData } = useSelector((store) => store.shopingStore);
    const dispatch = useDispatch()
    const openCart = () => {
      dispatch(ShopingSiceActions.setShowCartModel(true))
    }
  
    return (
      <div className="cartbages" onClick={openCart}>
        <img src={cartbagesblack} alt="" />
        <div className="cartbagescount">{cartData?.length}</div>
      </div>
    )
  }
  