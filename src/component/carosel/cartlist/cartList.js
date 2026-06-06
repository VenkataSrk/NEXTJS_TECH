import React from "react";

export const ShopingCartView = ({
  cartData,
  totalCost,
  hadCart,
  onCloseCart,
  onPurchase,
  onUpdateQty,
  showAlert,
  onCloseAlert,
  loading,
  onSelectItem
}) => {

  console.log("cartDatacartData", cartData)
  return (
    <div className="endchats endforshops">
      <div className="endchatsinner">
        <div className="endbagesin">
          <div className="endtagtitle">Cart Items ({cartData?.length})</div>
          <div className="endtag" style={{ fontSize: 14 }}>
            To Pay: &#8377; {`${totalCost || "-"}`}
          </div>
        </div>

        <div className="bagesmain">
          {cartData?.map((itm) => (
            <div className="bagesblock" key={itm.variant_id}>
              <div className="bagesblockleft">
                <div className="bagesblockleftimg" onClick={()=> onSelectItem?.(itm)}>
                  <img src={itm.image || itm.thumbnailImg} alt="" />
                </div>
                <div className="bagesblockleftcontent">
                  <div className="bagesblocktitle">
                    {itm?.title || itm?.productType || "-"}
                  </div>
                  <div style={{ display: "flex", gap: "14px" }}>

                    { itm?.size  ? <div style={{ fontSize: "12px" }}> Size: {itm?.size ?? 0} </div>
                      : itm?.weight ? <div style={{ fontSize: "12px" }}> Weight: {itm?.weight ?? 0} </div> 
                      : <></>
                    }

                    <div className="bagesblockcount">
                      <svg
                        onClick={() => onUpdateQty?.(itm, "minus")}
                        xmlns="http://www.w3.org/2000/svg"
                        width="11"
                        height="1"
                        viewBox="0 0 11 1"
                        fill="none"
                      >
                        <path
                          d="M10.1869 1H0.5C0.223933 1 0 0.776067 0 0.5C0 0.223933 0.223933 0 0.5 0H10.1869C10.4629 0 10.6869 0.223933 10.6869 0.5C10.6869 0.776067 10.4629 1 10.1869 1Z"
                          fill="#757676"
                        />
                      </svg>
                      {itm?.quantity ?? 0}
                      <svg
                        onClick={() => onUpdateQty?.(itm, "plus")}
                        xmlns="http://www.w3.org/2000/svg"
                        width="11"
                        height="11"
                        viewBox="0 0 11 11"
                        fill="none"
                      >
                        <path
                          d="M10.1688 4.83137L5.8309 4.83437V0.499797C5.8309 0.223776 5.60713 0 5.33111 0C5.05509 0 4.83132 0.223776 4.83132 0.499797V4.8351L0.499459 4.8381C0.22344 4.8383 -0.000199783 5.06221 1.33928e-07 5.33823C0.000200051 5.61425 0.224107 5.83789 0.500125 5.83769L4.83132 5.83469V10.1693C4.83132 10.4453 5.05509 10.6691 5.33111 10.6691C5.60713 10.6691 5.8309 10.4453 5.8309 10.1693V5.83403L10.1694 5.83103C10.4454 5.83083 10.6691 5.60692 10.6689 5.3309C10.6687 5.05488 10.4448 4.83117 10.1688 4.83137Z"
                          fill="#757676"
                        />
                      </svg>
                    </div>
                  </div>
                </div>
              </div>

              <div className="bagesblockright">
                <div className="bagesblockrightprice">
                  &#8377; {Number(itm.amount_rs) ?? 0}
                </div>
              </div>
            </div>
          ))}
        </div>

        <div className="bagesbottom">
          <div className="bagesbuttons" onClick={onCloseCart}>
            {hadCart ? "Cancel" : "Close"}
          </div>
          {hadCart && (
            <div className={`bagesbuttons ${loading ? "loadingcarosel" : ''}`} onClick={onPurchase} >
              Buy now
            </div>
          )}

          {showAlert.open && (
            <div className="bagescartav">
              <div className="bagescartavinner">
                Only {showAlert.count || 0} units available
                <svg
                  onClick={onCloseAlert}
                  xmlns="http://www.w3.org/2000/svg"
                  width="20"
                  height="20"
                  viewBox="0 0 20 20"
                  fill="none"
                >
                  <path
                    d="M11.0561 9.99623L14.377 6.67532C14.6699 6.38244 14.6699 5.90804 14.377 5.61526C14.0841 5.32248 13.6097 5.32238 13.317 5.61526L9.99604 8.93618L6.67512 5.61526C6.38224 5.32238 5.90784 5.32238 5.61506 5.61526C5.32228 5.90814 5.32218 6.38254 5.61506 6.67532L8.93598 9.99623L5.61506 13.3172C5.32218 13.61 5.32218 14.0844 5.61506 14.3772C5.7615 14.5236 5.95332 14.5968 6.14514 14.5968C6.33696 14.5968 6.52878 14.5236 6.67522 14.3772L9.99613 11.0563L13.3171 14.3772C13.4635 14.5236 13.6553 14.5968 13.8471 14.5968C14.039 14.5968 14.2308 14.5236 14.3772 14.3772C14.6701 14.0843 14.6701 13.6099 14.3772 13.3172L11.0561 9.99623Z"
                    fill="white"
                  />
                  <path
                    d="M9.99584 1.49938C14.6808 1.49938 18.4922 5.31089 18.4922 9.99584C18.4922 14.6808 14.6808 18.4923 9.99584 18.4923C5.31089 18.4923 1.49938 14.6808 1.49938 9.99584C1.49938 5.31089 5.31099 1.49938 9.99584 1.49938ZM9.99584 0C4.47524 0 0 4.47534 0 9.99584C0 15.5163 4.47524 19.9917 9.99584 19.9917C15.5164 19.9917 19.9916 15.5163 19.9916 9.99584C19.9916 4.47534 15.5164 0 9.99584 0Z"
                    fill="white"
                  />
                </svg>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};