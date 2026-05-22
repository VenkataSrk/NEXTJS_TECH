import React, { useMemo, useState } from "react";
import { useSelector } from "react-redux";
import styles from "./scss/tracker.module.scss"
import search from "../../assets/images/product/searchicon.svg"
import list from "../../assets/images/product/list.svg"
import grid from "../../assets/images/product/grid.svg"
import prod from "../../assets/images/product/product.png"
import { useNavigate } from "react-router-dom";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import SkeletonBox from "../marketing/skeletonbox";
import Nodata from "../component/nodata";
type CatalogProps = {
  contact?: boolean;
  showProductName?: (pid :any)=>void;
};

function Catalog({contact=false,showProductName} :CatalogProps) {

  const [view, setView] = useState("list");
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const { productCatalog: catalog = [], productCatalogLoader: loading = false } = useSelector((state: any) => state.productStore);
  const [searchTerm, setSearchTerm] = useState("");

  const getIndividualProduct = (product: { id: number | string; name: string }) => {
    if(contact){
      showProductName?.(product.id);
    }else{

      const path = `/products/product-details?productId=${product.id}`;
      dispatch(
        commonDataSliceActions.setTabMenuPush({
          path,
          menuName: product.name || "Product",
          close: 1,
          type: "products",
        })
      );
      navigate(path);
    }
  };

  const renderProductCard = (product: any, index: number, isGrid: boolean) => (
    
    <div
      key={`${product?.product_id ?? index}`}
      className={`${styles.card} ${styles.prodcard}`}
      onClick={() =>
        getIndividualProduct({
          id: product?.product_id ?? index,
          name: product?.product_name ?? "Product",
        })
      }
    >
      <div className={styles.lefts}>
        <img src={product?.product_image} />
      </div>
      <div className={styles.rights}>
        <div className={styles.top}>
          <div className={styles.lefts}>
            <div className="boldtxt">{product?.product_name ?? "Product"}</div>
            <p>{product?.short_description ?? "-"}</p>
          </div>
          {!isGrid && (
            <div className="title" style={{ whiteSpace: "nowrap" }}>
              {product?.starting_price_plan ?? "-"}
            </div>
          )}
        </div>
        <div className={styles.bottom}>
          <div className={styles.half}>
            <label>Product Category</label>
            <div className={styles.tag}>
              {product?.product_category ?? "-"}
            </div>
          </div>
          <div className={styles.half}>
            <label>Product Sold</label>
            <div className={styles.tag}>
              {product?.rating_reviews ?? "-"}
            </div>
          </div>
        </div>
        {isGrid && (
          <div className="title">
            {product?.pricing ?? "-"}
          </div>
        )}
      </div>
    </div>
  );

  const filteredCatalog = useMemo(() => {
    const term = searchTerm?.trim().toLowerCase();
    if (!term) return catalog;
    return catalog?.filter((product) => {
      const name = String(product?.product_name ?? "").toLowerCase();
      const desc = String(product?.description ?? "").toLowerCase();
      const id = String(product?.product_id ?? "").toLowerCase();
      return name.includes(term) || desc.includes(term) || id.includes(term);
    });
  }, [catalog, searchTerm]);


  return (<>
    <div className={styles.trackerbodyinner}>

      {/* <div className={styles.wrapperhedaer}>
        <div className={styles.searchBox}>
          <img src={search} alt="search" />
          <input
            type="text"
            placeholder="Search products here"
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
          />
        </div>

        <div className={styles.viewToggle}>
          <button
            className={view === "grid" ? styles.active : ""}
            onClick={() => setView("grid")}
          >
            <img src={grid} alt="grid" />
          </button>

          <button
            className={view === "list" ? styles.active : ""}
            onClick={() => setView("list")}
          >
            <img src={list} alt="list" />
          </button>
        </div>
      </div> */}

      {/* {(filteredCatalog.length === 0 && !loading) && <Nodata />} */}

      {!loading && filteredCatalog.length > 0 && (view === "list" ? (
        <>
          {filteredCatalog.map((product, index) =>
            renderProductCard(product, index, false)
          )}
        </>
      ) : (
        <div className={styles.gridcontainer}>
          {filteredCatalog.map((product, index) =>
            renderProductCard(product, index, true)
          )}
        </div>
      ))}
    </div>
    {(filteredCatalog.length === 0 && !loading) && <Nodata />}
    {loading && (
      <div style={{ display: "flex", flexDirection: "column", gap: "20px" }} className={`maincontainer ${styles.skeletonStyles}`}>
        {Array.from({ length: 6 }, (_, index) => (
          // <div key={index} style={{ display: "flex", justifyContent: "center" }}>
          //   <SkeletonBox width="100%" height="120px" />
          // </div>

          <div className={`${styles.card} ${styles.prodcard}`}>
            <div className={styles.lefts}>
              <div className={styles.skeletonBadge}></div>
            </div>
            <div className={styles.rights}>
              <div className={styles.top}>
                <div className={styles.lefts}>
                  <div className="boldtxt">
                    <div className={styles.skeletonTitle}></div>
                  </div>
                  <div className={styles.skeletonText}></div>
                </div>
              </div>
              <div className={styles.bottom}>
                <div className={styles.half}>
                  <label>Product Category</label><span className={styles.skeletonText}></span>
                </div>
                <div className={styles.half}>
                  <label>Product Sold</label><span className={styles.skeletonText}></span>
                </div>
              </div>
            </div>
          </div>

        ))}
      </div>
    )}
  </>)
} export default Catalog
