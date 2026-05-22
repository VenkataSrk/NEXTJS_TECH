import React from "react";
import Nodata1 from '../../../assets/images/nodata.svg';

// const Nodata = ({ isChildren = false, content = 'No Data Available' } :any) =>{
//    return(
//       <>
//         <div className={isChildren ? "": "emptydiv"} style={{display:"flex",flexDirection: "column",alignItems: 'center',justifyContent: 'center', width:"100%" , height:"100%"}}>
//             <img src={Nodata1} alt="Nodata" />
//             <div>{content}</div>
//         </div>
//       </>
//    )
// }
// export default Nodata

const Nodata = ({ isChildren = false, content = 'No Data Available', subContent, image }: {
  isChildren?: boolean;
  content?: string;
  subContent?: string;
  image?: string | any;
}) => {
  return (
    <div
      className={isChildren ? "" : "emptydiv"}
      style={{ display: "flex", flexDirection: "column", alignItems: 'center', justifyContent: 'center', width: "100%", height: "100%" }}
    >
      <img src={image ?? Nodata1} alt="Nodata" />
      <div style={{ paddingTop: "12px" }}>{content}</div>
      {subContent && (
        <div style={{ paddingTop: "4px", color: "#6B7280", fontSize: "13px" }}>{subContent}</div>
      )}
    </div>
  );
};
export default Nodata