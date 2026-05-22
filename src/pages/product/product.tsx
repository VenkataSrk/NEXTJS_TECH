import React, { useMemo } from "react";
import { Outlet, useLocation, useNavigate } from "react-router-dom";
import styles from "./scss/product.module.scss";

function Product() {

  return (
    <div className={styles.content}>

      <Outlet />
    </div>
  );
}

export default Product;
