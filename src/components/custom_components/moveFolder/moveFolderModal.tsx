import { useEffect, useState } from "react";
import styles from "./MoveFolderModal.module.scss";
import { whatsappService } from "../../../base/service/apiservice";
import { jwtDecode } from "jwt-decode";
import { GetItemFromStorage } from "../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../base/customhooks/localStorageKeys";

const folders = [
  "Attachments",
  "Knowledge",
  "Documents",
  "Shared Folder",
  "Draft Folder",
  "Company Folder"
];

const MoveFolderModal = ({setmoveModal,movefile,fetchFolders,folderList} :any) => {
    console.log("mvfile",movefile)
    const [folders,setFolders]=useState<any>(folderList);
    console.log("folders,",folders)
    const [foldersCopy,setFoldersCopy] =useState<any>(folderList);
    const [searchName,setSearchName] =useState<any>("");
    const [selectedFolder,setSelectedFolder] = useState<any>(-1);

    const loginUserDetails: any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    const [folderName, setFolderName] = useState<any>("");
    const [folderId,setFolderId]= useState<any>(null);
    const handleCloseMoveModal=()=>{
        setSelectedFolder(-1);
        setmoveModal();
    }

const handleMoveFolder=async()=>{
    try {
      const body: any = {
        id: folderId ?? null,
        domainId: loginUserDetails?.domainId,
        folderName: folderName ?? "",
        collection: movefile ? JSON.stringify([movefile]) : JSON.stringify([])
      }

        console.log("body",body)

      await whatsappService.post(body, '/insert_update_new_folder').then((res: any) => {

        if (res?.data?.statusCode == 200) {
          console.log("res", res)

        }
        })
      } catch (error) {
        console.log("API ERROR:", error)
      }finally{
          setSelectedFolder(-1);
          fetchFolders();
          handleCloseMoveModal();
      }
}

const handleSearch=(e :any)=>{
    if(e?.target?.value=="" || !e?.target?.value){

        setFolders(folderList);
    }
   setFolders(foldersCopy.filter((item :any)=>item?.foldername.includes(e?.target?.value)));
}

    useEffect(()=>{
        try {
            
            fetchFolders();
        } catch (error) {
            console.log("Err",error)
        }
        
    },[])

  return (
    <div className={styles.overlay}>
      <div className={styles.modal}>

        {/* Header */}
        <div className={styles.header}>
          <h4>Move Folder</h4>
          <span className={styles.close}  onClick={()=>handleCloseMoveModal()}>×</span>
        </div>

        {/* Search */}
        <div className={styles.search}>
          <input placeholder="Search folder" onChange={(e :any)=>handleSearch(e)}/>
        </div>

        {/* Folder list */}
        <div className={styles.list}>
          {folders.map((folder :any, index :any) => (
            <div key={index} className={`${styles.item} ${index==selectedFolder?styles.active:""}`} onClick={()=>{setFolderName(folder?.foldername); setSelectedFolder(index);setFolderId(folder?.kbFolderId);}}>
                <div className={styles.lefts}>
                         <span className={styles.icon}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="16" viewBox="0 0 19 16" fill="none">
  <path d="M15.0957 2.3811L11.7277 2.3328C11.2704 2.3262 10.8411 2.1114 10.5617 1.7493L10.0039 1.0266C9.4786 0.3772 8.6882 0 7.8531 0H2.3851C1.07 0 0.00399995 1.0662 0.00399995 2.3811V6.7065C0.00389995 6.7136 0 6.7194 0 6.7265C0 6.7336 0.00389995 6.7394 0.00399995 6.7465V12.8429C0.00399995 14.4999 1.3471 15.8429 3.004 15.8429H15.0957C16.7526 15.8429 18.0957 14.4999 18.0957 12.8429V5.381C18.0957 3.7243 16.7526 2.3811 15.0957 2.3811ZM1.504 2.3811C1.504 1.8953 1.8993 1.5 2.3851 1.5H7.8531C8.2373 1.5 8.5961 1.6714 8.8165 1.9431L9.3742 2.6658C9.9363 3.3941 10.7862 3.8194 11.7062 3.8325L15.0957 3.8811C15.9228 3.8811 16.5957 4.554 16.5957 5.3811V5.9766H1.5039L1.504 2.3811ZM15.0957 14.343H3.0039C2.1768 14.343 1.5039 13.6701 1.5039 12.843V7.4765H16.5956V12.843C16.5956 13.6701 15.9228 14.343 15.0957 14.343Z" fill="#2D2B2F"/>
</svg></span>
              <span className={styles.name} >{folder?.foldername}</span>
                  </div>
                  <span className={styles.none}>

<svg width="25" height="24" viewBox="0 0 25 24" fill="none" xmlns="http://www.w3.org/2000/svg" style={{rotate:"270deg"}} >
<path d="M12.2703 14.9023C11.6678 14.9023 11.1023 14.6679 10.677 14.2412L6.82349 10.3769C6.53099 10.0839 6.53199 9.60931 6.82499 9.31641C7.11889 9.02441 7.59399 9.02441 7.88599 9.31841L11.739 13.1827C12.0227 13.4659 12.5183 13.4659 12.801 13.1827L16.6545 9.31841C16.9465 9.02451 17.4216 9.02451 17.7155 9.31641C18.0085 9.60941 18.0094 10.084 17.717 10.3769L13.8635 14.2412C13.4382 14.668 12.8724 14.9023 12.2703 14.9023Z" fill="#757676"/>
</svg>

                  </span>
         
            </div>
          ))}
        </div>

        {/* Footer */}
        <div className={styles.footer}>
          <button className={"cancelbtn"} onClick={()=>handleCloseMoveModal()}>Cancel</button>
          <button onClick={()=>handleMoveFolder()}>Add</button>
        </div>

      </div>
    </div>
  );
};

export default MoveFolderModal;