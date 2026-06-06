import React from "react";
import { useSelector } from "react-redux";
import { nickysDomainAI, pothysDomain } from '../../../base/constants'
import { ShopingCart as ShopingCartPothys } from '../../CarouselPothys'
import { ShopingCartNikys } from './ShopingCartNikys'

export const HandelCartList =()=> {
    const { domainId } = useSelector((state) => state.message?.personalisationAll);

    if(`${domainId}` === nickysDomainAI){
        return (
            <ShopingCartNikys/>
        )
    }
    else if(`${domainId}` === pothysDomain){
        return (
            <ShopingCartPothys/>
        )
    }

    return <></>
}