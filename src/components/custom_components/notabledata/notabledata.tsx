import React from "react";
import styles from "./scss/notabledata.module.scss";
import nodataimg from "../../../styles/images/icons/nodata.svg"

interface NoTableDataProps {
  title?: string;
  description?: string;
  image?: string;
}

const NoTableData: React.FC<NoTableDataProps> = ({
  title = "No Data Found",
  description = "No records are currently available.",
  image =nodataimg,
}) => {
  return (
    <div className={styles.noDataWrapper}>
      <img src={image} alt="No data" className={styles.noDataImage} />
      <h3 className={styles.noDataTitle}>{title}</h3>
      <p className={styles.noDataDesc}>{description}</p>
    </div>
  );
};

export default NoTableData;
