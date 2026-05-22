import React from "react";
import styles from "./scss/dynamicdata.module.scss";
import Mgscontent from "../../../assets/images/projectinsights/contactsicon.svg"

interface NoDataProps {
  title?: string;
  message?: string;
  image?: string;
  imageAlt?: string;
  Imageicon?: string;
  Buttonmessage?: string;
  buttonCallback?: () => void
}

const Dynamicnodata: React.FC<NoDataProps> = ({
  title = "No recordings available",
  message = "There are no call recordings to display at the moment.",
  imageAlt = "No data",
  Buttonmessage="Create Contact",
  Imageicon=Mgscontent,
  buttonCallback,
}) => {
  return (
    <div className={`${styles.nodatacontainer} nodatacontainer`}>
      <div className={styles.nodatabox}>
         <img src={Imageicon} alt={imageAlt} />
        <div className={styles.connected}>
          <div className={styles.callstitle}>{title}</div>
          <div className={styles.callmessage}>{message}</div>
          <button onClick={()=> buttonCallback?.()}>{Buttonmessage}</button>
        </div>
      </div>
    </div>
  );
};

export default Dynamicnodata;
