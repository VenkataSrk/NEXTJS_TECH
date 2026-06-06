import { useState } from "react";
import { whatsappService } from "../../../base/service/apiservice";
import styles from "./MoveFolderModal.module.scss";
import { jwtDecode } from "jwt-decode";
import { GetItemFromStorage } from "../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../base/customhooks/localStorageKeys";

const RenameFolderModal = ({setShowRenameModal,fetchFolders,folderData} :any) => {
    const [folderName,setFolderName]=useState<any>("");
    const [validation,setValidation]=useState<any>("");

    const handleCancel=()=>{
        setShowRenameModal();
    }


    const handleRename=async(folder :any)=>{
      
            try {
                setValidation(true)
                if(!folderName){
                    return;
                }
                const body :any= {
                    id: folder?.kbFolderId??null,
                    domainId: folder?.domainId  ,
                    folderName: folderName,
                    collection:  folder?.collection_info
                }
                
       
                await whatsappService.post(body,'/insert_update_new_folder').then((res: any) => {
        
                  if (res?.data?.statusCode == 200) {
                    console.log("res",res)
                    // setFolderList(res?.data?.getKnowledgeResponse);
                    // // setAllArchive(res?.data?.getAllArchiveRes);
                   
                   
                  }
                })
              } catch (error) {
                console.log("API ERROR:", error)
              }finally{
                  fetchFolders();
                  setValidation(false);
                  handleCancel();
              }
        
    }
    // const handleNewFolder=async()=>{
    //     try {
    //         setValidation(true);
    //         const body :any= {
    //             id: null,
    //             domainId:loginUserDetails?.domainId ,
    //             folderName: folderName??"New Folder",
    //             collection:  JSON.stringify([])
    //         }
    
            
    //         if(!folderName){
    //             return;
    //         }
    //         else{
    //             setValidation(false);
    //         }
    //         await whatsappService.post(body,'/insert_update_new_folder').then((res: any) => {
    
    //           if (res?.data?.statusCode == 200) {
    //             console.log("res",res)
    //             // setFolderList(res?.data?.getKnowledgeResponse);
    //             // // setAllArchive(res?.data?.getAllArchiveRes);
    //           }
    //         })
    //       } catch (error) {
    //         console.log("API ERROR:", error)
    //       }finally{
    //           fetchFolders();
    //           handleCancel();
             
    //       }
    // }

    // const handleFolder=async()=>{
    //     console.log("heee")
    //     await handleNewFolder();
    // }

    return (
        <div className={styles.overlay}>
            <div className={styles.modal}>

                {/* Header */}
                <div className={styles.header}>
                    <h4>Rename Folder</h4>
                    <span className={styles.close} onClick={()=>handleCancel()}>×</span>
                </div>

                <div className={styles.body}>
                    <div className={styles.block}>
     <label>Folder name <span>*</span></label>
                    <input type="text" placeholder="Folder name" onChange={(e :any)=>setFolderName(e?.target?.value)} />
                    </div>
              
                </div>
                {(validation  && !folderName) && <p className="errortxt">Enter a valid Folder Name</p>}

                {/* Footer */}
                <div className={styles.footer}>
                    <button className={"cancelbtn"} onClick={()=>handleCancel()}>Cancel</button>
                    <button onClick={()=>handleRename(folderData)}>Apply</button>
                </div>

            </div>
        </div>
    )
}
export default RenameFolderModal;
