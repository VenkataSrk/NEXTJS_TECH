import React, { useEffect, useState } from "react";
import WorktualModal from "../../components/custom_components/worktualModal";

export default function PdfModal({ pdfUrl,onClose } :any) {
  const [open, setOpen] = useState(false);

  useEffect(()=>{
    setOpen(true);
  },[])
  return (
    <>
      {/* <button onClick={() => setOpen(true)}>Open PDF</button> */}

      {/* {open && (
        <div style={styles.overlay}>
          <div style={styles.modal}>
            <button style={styles.close} onClick={() => {setOpen(false); onClose();}}>
              ✖
            </button>

            <iframe
            //   src={"https://academicpages.github.io/files/paper1.pdf"}
              src={pdfUrl?.replaceAll("//","//qa")}
              title="PDF Viewer"
              style={styles.iframe}
            />
          </div>
        </div>
      )} */}
<div className="filepreviwmodal">
      <WorktualModal
              show={true}
              mdsize="modallg"
              customTitle={<><h5 style={{fontSize:"20px", fontFamily:"intermedium"}}>Meeting notes</h5><div><span onClick={()=>onClose()}><svg className='modelclose'  xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 10 10" fill="none">
                                <path d="M5.80079 4.99937L9.70412 1.09571C9.92545 0.874973 9.92545 0.51564 9.70412 0.294906C9.48279 0.0735729 9.12339 0.0735729 8.90332 0.294906L4.99999 4.19851L1.09665 0.294906C0.876586 0.0735729 0.515919 0.0735729 0.295852 0.294906C0.074519 0.51564 0.074519 0.874973 0.295852 1.09571L4.19919 4.99937L0.295852 8.90304C0.074519 9.12377 0.074519 9.48311 0.295852 9.70384C0.406519 9.81451 0.551052 9.86984 0.696252 9.86984C0.841452 9.86984 0.986586 9.81451 1.09665 9.70384L4.99999 5.80024L8.90332 9.70384C9.01332 9.81451 9.15852 9.86984 9.30372 9.86984C9.44892 9.86984 9.59345 9.81451 9.70412 9.70384C9.92545 9.48311 9.92545 9.12377 9.70412 8.90304L5.80079 4.99937Z" fill="#989898" />
                            </svg></span></div></>}
          >


              <iframe
                    src={"https://academicpages.github.io/files/paper1.pdf"}
                //   src={pdfUrl?.replaceAll("//", "//qa")}
                  title="PDF Viewer"
            
              />

    {/* <div > */}
      {/* <div > */}
        {/* <button style={closeButton} onClick={onClose}>✖</button> */}
        {/* <object
        //   data={pdfUrl}
           data={"https://academicpages.github.io/files/paper1.pdf"}
          type="application/pdf"
          width="100%"
          height="100%"
        >
          <p>
            PDF cannot be displayed. <a href={pdfUrl} target="_blank">Download PDF</a>
          </p>
        </object> */}
      {/* </div> */}
    {/* </div> */}



          </WorktualModal>
</div>
        
    </>
  );
}

