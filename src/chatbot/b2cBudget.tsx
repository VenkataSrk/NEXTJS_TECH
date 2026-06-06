import React from "react"

const BudgetTable = ({ budgetData, explanation }: any) => {
    return (
        <>
        <div>
        {explanation ? <p className="leftagentchat">{explanation}</p> : ''}
        <div className="b2ctable">
            <div className="b2ctablehead">
                <div className="b2ctableth">Seller</div>
                <div className="b2ctableth">Product</div>
                <div className="b2ctableth">Qty</div>
                <div className="b2ctableth lqstt">Price</div>
            </div>
            {Object?.keys(budgetData)?.map((key: any, keyIndex: any) => {
                let seller=''
                if (key !== 'total')
                    return (
                        <div className="b2ctablebody">
                            {budgetData[key].map((item: any, itemIndex: any) => {
                                const isSame=seller!==item?.p_sellerInformation
                                seller=item?.p_sellerInformation
                                return (
                                    <div className="b2ctabletr">
                                        {isSame ?<div className="b2ctabletd">{item?.p_sellerInformation}</div>: <div className="b2ctabletd empty"></div>}
                                        <div className="b2ctabletd">{item?.p_productName}</div>
                                        <div className="b2ctabletd">{item?.quantity}</div>
                                        <div className="b2ctabletd lqstt">£ {item?.p_price}</div>
                                    </div>
                                )
                            })}
                        </div>
                    )
            })}
        </div>
        </div>
        </>
    )
}

export default BudgetTable;