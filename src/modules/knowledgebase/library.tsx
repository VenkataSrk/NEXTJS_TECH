import React, { useEffect, useRef, useState } from 'react';
import style from './scss/knowledgebase.module.scss'

import micIcon from '../../styles/images/customai/mic.svg'
import { assets } from '../../styles/assets/assets';
import { whatsappService } from '../../base/service/apiservice';
import { useDispatch, useSelector } from 'react-redux';
import { commonDataSliceActions } from '../../store/slice/commonSlice/commonSlice';
import MoveFolderModal from '../../components/custom_components/moveFolder/moveFolderModal';
import CreateFolderModal from '../../components/custom_components/moveFolder/createFolder';
import libicon from "../../styles/images/icons/foldericon.svg"
import PdfModal from './pdfModal';
import RenameFolderModal from '../../components/custom_components/moveFolder/RenameFolder';
// import Scrapedwebsite from './scrapedwebsite';
// import foldericonactivee from "../../styles/images/icons/foldericonacrivee.svg"

type DeleteConfirmationState = {
  isOpen: boolean;
  message: string;
  onConfirm: null | (() => void | Promise<void>);
};

const Library = () => {
  const dispatch = useDispatch();
  const dropdownRef = useRef<HTMLDivElement | null>(null);
  const [knowledgebase,setKnowledgebase] = useState<any>([]);
  console.log("kbase",knowledgebase)
  const [selectedDot,setSelectedDot] = useState<any>(-1);
  const [createModal,setcreateModal] = useState<any>(false);
  const [moveModal,setmoveModal] = useState<any>(false);
  const [showRename,setShowRename] = useState<any>(false);
   const[folderList,setFolderList] = useState<any>([]);
   const[selectedFolder,setSelectedFolder] = useState<any>(-1);
   const[selectedWeb,setSelectedWeb] = useState<any>(-1);
   const[selectedPDF,setSelectedPDF] = useState<any>("");
   const [selectedFolderData,setSelectedFolderData]=useState<any>(null);
   const [deleteConfirmation, setDeleteConfirmation] = useState<DeleteConfirmationState>({
    isOpen: false,
    message: '',
    onConfirm: null,
   });
   console.log("sellll",selectedFolderData)


  const fetchKnowledge :any =  useSelector((state :any)=>state.commonData.knowledgeBaseDoc);
  console.log("fetchknow::",fetchKnowledge)
  const [movefile,setMoveFile] = useState<any>(null);
  const [websiteUrlData,setWebsiteUrlData] = useState<any>([]);
  console.log("wb",websiteUrlData)
  const handleDotSelection =(index :any)=>{
      if(index ==selectedDot){
        setSelectedDot(-1);
      }else{
        setSelectedDot(index);
      }
  }

  // const downloadFile = (url: any) => {
  //   const link = document.createElement("a");
  //   link.href = url;
  //   link.download = ""; // lets browser decide filename
  //   document.body.appendChild(link);
  //   link.click();
  //   document.body.removeChild(link);
  // };

  const downloadFile = (url: string) => {
  const link = document.createElement("a");
  link.href = url;
  link.setAttribute("download", "");
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
};

const handleDocDelete = async (item: any) => {
  try {
    const body: any = {
      id: item?.id,
      uniqueId: item?.uniqueId,
      fileName: item?.filename,
      uploadUrl: item?.uploadUrl,
    };
    const res = await whatsappService.post(body, '/delete_knowledgeBase_doc');
    if (res?.data?.statusCode == 200) {
      console.log("deleteddoc",res)
      setKnowledgebase((prev: any) => prev.filter((doc: any) => doc?.id !== item?.id));
      fetchKnowledgeDoc();
    }
  } catch (error: any) {
    console.log("API ERROR:", error?.response?.data || error);
  }
}

const openDeleteConfirmation = (message: string, onConfirm: () => void | Promise<void>) => {
  setSelectedDot(-1);
  setSelectedFolder(-1);
  setSelectedWeb(-1);
  setDeleteConfirmation({
    isOpen: true,
    message,
    onConfirm,
  });
}

const closeDeleteConfirmation = () => {
  setDeleteConfirmation({
    isOpen: false,
    message: '',
    onConfirm: null,
  });
}

const handleConfirmDelete = async () => {
  if (deleteConfirmation.onConfirm) {
    await deleteConfirmation.onConfirm();
  }
  closeDeleteConfirmation();
}

const fetchKnowledgeDoc =()=>{
      try {
        whatsappService.get('/get_knowledge_docs').then((res: any) => {

      //     setKnowledgebase([
      // {
      //     "id": 3,
      //     "domainId": 7648,
      //     "uniqueId": "1a2b3c",
      //     "companyName": "worktual",
      //     "industrial_type": "saas",
      //     "filename": "Campaign Management-2.pdf",
      //     "minetype": "application/pdf",
      //     "uploadUrl": "https://ccaas-storage.worktual.co.uk/whatsapp/18677/Campaign Management-2.pdf",
      //     "size": 149983,
      //     "pdf_info": "[{\"product_name\": \"WhatsApp Campaign module\", \"product_features\": \"Integrates Meta’s WhatsApp Business API; enables sign-up of WhatsApp Business Accounts via Facebook Embedded Signup; allows viewing and managing connected WhatsApp numbers; can enable/disable accounts; supports sending template-based messages; provides file upload and media template management; built as a full-stack solution with React.js (TypeScript) frontend, Node.js (Express) backend, MySQL database via stored procedures, and Meta Graph API (WhatsApp Cloud API) for external integration.\"}, {\"product_name\": \"Frontend (React.js TypeScript)\", \"product_features\": \"Implemented with React.js using TypeScript; includes components such as WhatsappConfig.tsx (entry point for configuration), UserTable.tsx (list with search & pagination), UserForm.tsx (read-only account details), Signup.tsx (initial signup screen), SignupStatus.tsx (status display), AddAccountBtn.tsx (trigger account addition).\"}, {\"product_name\": \"Backend (Node.js Express)\", \"product_features\": \"Node.js server using Express framework; provides REST endpoints like /insert_update_whatsapp_config, /get_business_account/:mobileNumber, /change_account_status, /register_whatsapp_number, /get_business_meta_details/:mobileNumber; handles signup response, Meta data fetching, number registration, and account status toggling.\"}, {\"product_name\": \"Database (MySQL stored procedures)\", \"product_features\": \"MySQL database accessed via stored procedures; key procedures include wc_InsertUpdateBusinessAccountInfo, wc_GetBusinessAccountInfo, wc_updateBussinessConfigurationToggleAction, wc_get_business_meta_details; stores fields such as domainId, mobileNumber, businessName, status, isActive, messageLimit, appId, appSecret, authToken, phoneNumberId, wabaId, businessId.\"}, {\"product_name\": \"Meta Graph API (WhatsApp Cloud API)\", \"product_features\": \"External integration used to exchange auth code for access token, fetch WABA ID, retrieve WhatsApp numbers, register numbers, perform two-factor authentication, subscribe app to WABA webhooks, create/update/delete message templates, send template messages, and prepare/upload files.\"}]",
      //     "createdOn": "2025-12-18T15:18:26.000Z",
      //     "updatedOn": "2025-12-18T15:19:36.000Z"
      // }]);
          if (res?.data?.statusCode == 200) {
            setKnowledgebase(res?.data?.getKnowledgeResponse || []);
            // setAllArchive(res?.data?.getAllArchiveRes);
          }
          else {
            setKnowledgebase([]);
            // setAllArchive([]);
          }
        })
      } catch (error) {
        console.log("API ERROR:", error)
      }
}

const handleMoveFolder=(item :any)=>{
  setSelectedDot(-1);
  setmoveModal(true);
  setMoveFile(item);
}

const fetchFolders =()=>{
      try {
        whatsappService.get('/get_folder').then((res: any) => {

          if (res?.data?.statusCode == 200) {
            console.log("res",res)
            setFolderList(res?.data?.deleteDocResponse || []);
            // setAllArchive(res?.data?.getAllArchiveRes);
          } else {
            setFolderList([]);
          }
        })
      } catch (error) {
        console.log("API ERROR:", error)
      }
    
}

const handleRename=async(folder :any)=>{
  
        try {
            const body :any= {
                id: folder?.kbFolderId??null,
                domainId: folder?.domainId  ,
                folderName: folder?.foldername??"New Folder",
                collection:  JSON.stringify([])
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
          }
    
}

const handleFolderDelete=async (folder :any)=>{
  try {
    console.log("folder",folder)
    const body: any = {
           id: folder?.kbFolderId
         }
         await whatsappService.post(body, '/delete_knowledgebase_folder').then((res: any) => {
           if (res?.data?.statusCode == 200) {
                console.log("deleteddoc",res)
               setFolderList((prev: any) => prev.filter((item: any) => item?.kbFolderId !== folder?.kbFolderId));
               fetchFolders();
           }
         })
  } catch (error) {
    console.log("error",error);
  }
}

const fetchWebsite=async()=>{
    try {
      await whatsappService.get('/get_website_data').then((res :any)=>{
      
          if(res?.data?.statusCode==200){
            console.log("res",res)
            setWebsiteUrlData(res?.data?.deleteDocResponse || []);
          } else {
            setWebsiteUrlData([]);
          }
      })
    } catch (error) {
      console.log("error:",error)
    }
}

const deleteWebsite=async(item :any)=>{
  try {
    const body={
      id: item
    }
      await whatsappService.post(body,'/delete_knowledgebase_website').then((res :any)=>{
          console.log("resout",res)
          setWebsiteUrlData((prev: any) => prev.filter((website: any) => website?.id !== item));
          fetchWebsite();
      })
  } catch (error) {
    console.log("err",error)
  }
}

  useEffect(()=>{
    fetchKnowledgeDoc();
    fetchFolders();
    fetchWebsite();
    if(fetchKnowledge){
      dispatch(commonDataSliceActions.setKnowledgeBaseDocAdded(false));
    }
  },[fetchKnowledge])

  useEffect(() => {
  const handleClickOutside = (event: MouseEvent) => {
    if (
      dropdownRef.current &&
      !dropdownRef.current.contains(event.target as Node)
    ) {
      setSelectedDot(-1);
      setSelectedFolder(-1);
      setSelectedWeb(-1);
    }
  };

  document.addEventListener("mousedown", handleClickOutside);

  return () => {
    document.removeEventListener("mousedown", handleClickOutside);
  };
}, []);

    return(
        <>
         {!selectedPDF?<>
           <div className={style.ticketright}>
                <div className={style.constactright}>
                     <div className={`${style.elevatebrandbody} ${style.evelatelefts}`}>
            <div className={style.header}>
              <div className={style.lefts}>
                <h3>Knowledge base</h3>
                <p>Explore articles, FAQs, and best practices.</p>
              </div>
              <button onClick={()=>setcreateModal(true)}>Create folder</button>
              
            </div>
            <div className={style.innerbody}>
              <div className={style.promtabmain}>
                 <div className={style.recommendcard}>
Responses generated by the system are based entirely on the content, policies, and updates provided by your team.Our Campaign solution is not responsible for outdated, incorrect, or missing content in the knowledge base that has not been updated by your team
                 </div>
                <div className={style.filesection}>
   <p className={style.folfdert}>Uploaded files <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                       <path d="M13.6665 6.64191C13.4685 2.97131 10.326 0.137972 6.6502 0.342438C5.42166 0.408705 4.2578 0.820438 3.2512 1.51057L3.23486 1.04424C3.22246 0.684238 2.92693 0.401038 2.56953 0.401038C2.56173 0.401038 2.55393 0.401038 2.54546 0.401705C2.1776 0.414705 1.88986 0.723305 1.90286 1.09117L1.9712 3.05537C1.9836 3.41537 2.27913 3.69857 2.63653 3.69857C2.64433 3.69857 2.65213 3.69857 2.6606 3.69791L4.62546 3.62957C4.99333 3.61657 5.28106 3.30797 5.26806 2.9401C5.25566 2.57224 4.96206 2.27477 4.5786 2.29751L3.8482 2.32271C4.6958 1.74864 5.673 1.40564 6.70486 1.35024C9.82073 1.18291 12.4913 3.58004 12.6587 6.69657C12.8267 9.81184 10.4282 12.483 7.31293 12.6504C5.8084 12.7389 4.35326 12.2207 3.22893 11.2109C2.10393 10.2012 1.44053 8.81377 1.35913 7.30397C1.34353 7.02597 1.115 6.82351 0.827864 6.82744C0.549864 6.84244 0.33633 7.08004 0.35133 7.35871C0.447064 9.13671 1.2296 10.7714 2.55446 11.9616C3.7882 13.069 5.35526 13.668 7.0004 13.668C7.1228 13.668 7.24453 13.6647 7.3676 13.6582C11.0382 13.4603 13.8637 10.3131 13.6665 6.64191Z" fill="#505152"/>
                      <path d="M6.76318 3.74146C6.48712 3.74146 6.26318 3.96539 6.26318 4.24146V7.61385C6.26318 7.77205 6.33805 7.92052 6.46498 8.01492L8.88618 9.81572C8.97538 9.88279 9.08018 9.91465 9.18372 9.91465C9.33672 9.91465 9.48778 9.84499 9.58538 9.71285C9.75012 9.49152 9.70452 9.17832 9.48252 9.01365L7.26312 7.36259V4.24152C7.26312 3.96545 7.03925 3.74146 6.76318 3.74146Z" fill="black"/>
                    </svg></p>
                    {knowledgebase.length == 0 && <>
                      <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
                    </>}
                  <div className={style.foldersfma}>
                 
                    {knowledgebase?.map((item :any,index :any)=>{
                      return(
                          <div className={style.uploaderpdf}>
                            <div className={style.imgcontainer}>
                              <img src={assets.uploadpdf} alt="" />
                            </div>
                            <div className={style.dtls} onClick={()=>setSelectedPDF(item?.uploadUrl.replaceAll(" ","%20"))}>
                              <h4>{item?.filename}</h4>
                              <p>{`${(item?.size / (1024 * 1024)).toFixed(2)} MB`}</p>
                            </div>

                            <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none" onClick={()=>handleDotSelection(index)}>
                              <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526" />
                            </svg>
                             {index==selectedDot &&
                              <div
                                style={{
                                  position: "absolute",
                                  top: 36,
                                  right: 0,
                                  background: "white",
                                  boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
                                  borderRadius: "6px",
                                  zIndex: 9999999999,
                                  width: "120px",
                                  padding: "4px 0",
                                }}
                                ref={dropdownRef}
                              >
                                    <div className="dropdown-item" onClick={()=>downloadFile(item?.uploadUrl)}>
                                      Download
                                    </div>
                                     <div className="dropdown-item" onClick={()=>handleMoveFolder(item)}>
                                      Move To Folder
                                    </div>
                                    <div className="dropdown-item" onClick={()=>openDeleteConfirmation('Are you sure you want to delete this file?', () => handleDocDelete(item))} >
                                      Delete
                                    </div>
                                  </div> }
                          </div>)
                    })}
                      
{/* Scraped website sample */}
                      {/* <div className={style.uploaderpdf}>
                         <div className={style.imgcontainer}>
                            <img src={assets.uploadpdf} alt="" />
                           </div>
                           <div className={style.dtls}>
                               <h4>Scraped website <span className={style.langcontainer}>En</span></h4>
                               <p>110mb</p>
                           </div>
                        
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none">
                          <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526"/>
                        </svg>
                     </div> */}

                      {/* <div className={style.uploaderpdf}>
                         <div className={style.imgcontainer}>
                            <img src={assets.uploadpdf} alt="" />
                           </div>
                           <div className={style.dtls}>
                               <h4>Scraped website <span className={style.langcontainer}>En</span></h4>
                               <p>110mb</p>
                           </div>
                        
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none">
                          <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526"/>
                        </svg>
                     </div> */}

                  

                </div>
                 {websiteUrlData.length > 0 &&
              <div className={style.filesection}>
 {<p className={style.folfdert}>Scraped website 
  <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                       <path d="M13.6665 6.64191C13.4685 2.97131 10.326 0.137972 6.6502 0.342438C5.42166 0.408705 4.2578 0.820438 3.2512 1.51057L3.23486 1.04424C3.22246 0.684238 2.92693 0.401038 2.56953 0.401038C2.56173 0.401038 2.55393 0.401038 2.54546 0.401705C2.1776 0.414705 1.88986 0.723305 1.90286 1.09117L1.9712 3.05537C1.9836 3.41537 2.27913 3.69857 2.63653 3.69857C2.64433 3.69857 2.65213 3.69857 2.6606 3.69791L4.62546 3.62957C4.99333 3.61657 5.28106 3.30797 5.26806 2.9401C5.25566 2.57224 4.96206 2.27477 4.5786 2.29751L3.8482 2.32271C4.6958 1.74864 5.673 1.40564 6.70486 1.35024C9.82073 1.18291 12.4913 3.58004 12.6587 6.69657C12.8267 9.81184 10.4282 12.483 7.31293 12.6504C5.8084 12.7389 4.35326 12.2207 3.22893 11.2109C2.10393 10.2012 1.44053 8.81377 1.35913 7.30397C1.34353 7.02597 1.115 6.82351 0.827864 6.82744C0.549864 6.84244 0.33633 7.08004 0.35133 7.35871C0.447064 9.13671 1.2296 10.7714 2.55446 11.9616C3.7882 13.069 5.35526 13.668 7.0004 13.668C7.1228 13.668 7.24453 13.6647 7.3676 13.6582C11.0382 13.4603 13.8637 10.3131 13.6665 6.64191Z" fill="#505152"/>
                      <path d="M6.76318 3.74146C6.48712 3.74146 6.26318 3.96539 6.26318 4.24146V7.61385C6.26318 7.77205 6.33805 7.92052 6.46498 8.01492L8.88618 9.81572C8.97538 9.88279 9.08018 9.91465 9.18372 9.91465C9.33672 9.91465 9.48778 9.84499 9.58538 9.71285C9.75012 9.49152 9.70452 9.17832 9.48252 9.01365L7.26312 7.36259V4.24152C7.26312 3.96545 7.03925 3.74146 6.76318 3.74146Z" fill="black"/>
                    </svg>
                    
                    </p>}
                     {websiteUrlData.length == 0 && <>
                      <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
                    </>}

                  <div className={style.foldersfma}>
                    {websiteUrlData?.map((item :any,ind :any)=>{
                      return(
                       <div className={style.uploaderpdf}>
                         <div className={style.imgcontainer}>
                            <img src={assets.webscrpicon} alt="" />
                           </div>
                           <div className={style.dtls} style={{width:"calc(100% - 77px"}}>
                               <h4>{item?.websideUrl} 
                                  {/* <span className={style.langcontainer}></span> */}
                                </h4>
                               {/* <p>110mb</p> */}
                           </div>
                        
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none" onClick={()=>{setSelectedWeb(ind)}}>
                          <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526"/>
                        </svg>

                         { (ind==selectedWeb ) && (
                            <div
                              style={{
                                position: "absolute",
                                top: 36,
                                right: 0,
                                background: "white",
                                boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
                                borderRadius: "6px",
                                zIndex: 9999999999,
                                width: "120px",
                                padding: "4px 0",
                              }}
                              ref={dropdownRef}
                            >
                              {/* <div className="dropdown-item" onClick={()=>handleRename(item)} >Rename</div> */}
                              <div className="dropdown-item" onClick={()=>openDeleteConfirmation('Are you sure you want to delete this website?', () => deleteWebsite(item?.id))} >Delete</div>
                            </div>
                          )}
                     </div>
                      )
                    })
                    }
                  </div>
              </div>}
                 
                   
                 
              </div>
              {/* <Scrapedwebsite/> */}
          {folderList.length > 0 &&     
                <div className={style.filesection}>
   {
    <p className={style.folfdert}>folders<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                       <path d="M13.6665 6.64191C13.4685 2.97131 10.326 0.137972 6.6502 0.342438C5.42166 0.408705 4.2578 0.820438 3.2512 1.51057L3.23486 1.04424C3.22246 0.684238 2.92693 0.401038 2.56953 0.401038C2.56173 0.401038 2.55393 0.401038 2.54546 0.401705C2.1776 0.414705 1.88986 0.723305 1.90286 1.09117L1.9712 3.05537C1.9836 3.41537 2.27913 3.69857 2.63653 3.69857C2.64433 3.69857 2.65213 3.69857 2.6606 3.69791L4.62546 3.62957C4.99333 3.61657 5.28106 3.30797 5.26806 2.9401C5.25566 2.57224 4.96206 2.27477 4.5786 2.29751L3.8482 2.32271C4.6958 1.74864 5.673 1.40564 6.70486 1.35024C9.82073 1.18291 12.4913 3.58004 12.6587 6.69657C12.8267 9.81184 10.4282 12.483 7.31293 12.6504C5.8084 12.7389 4.35326 12.2207 3.22893 11.2109C2.10393 10.2012 1.44053 8.81377 1.35913 7.30397C1.34353 7.02597 1.115 6.82351 0.827864 6.82744C0.549864 6.84244 0.33633 7.08004 0.35133 7.35871C0.447064 9.13671 1.2296 10.7714 2.55446 11.9616C3.7882 13.069 5.35526 13.668 7.0004 13.668C7.1228 13.668 7.24453 13.6647 7.3676 13.6582C11.0382 13.4603 13.8637 10.3131 13.6665 6.64191Z" fill="#505152"/>
                      <path d="M6.76318 3.74146C6.48712 3.74146 6.26318 3.96539 6.26318 4.24146V7.61385C6.26318 7.77205 6.33805 7.92052 6.46498 8.01492L8.88618 9.81572C8.97538 9.88279 9.08018 9.91465 9.18372 9.91465C9.33672 9.91465 9.48778 9.84499 9.58538 9.71285C9.75012 9.49152 9.70452 9.17832 9.48252 9.01365L7.26312 7.36259V4.24152C7.26312 3.96545 7.03925 3.74146 6.76318 3.74146Z" fill="black"/>
                    </svg></p>}
                     {folderList.length == 0 && <>
                      <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
                    </>}
                    <div className={style.foldersfma}>
                 
                      {folderList.map((item: any, ind: any) => (
                        <div className={style.uploaderpdf} key={ind}>
                          <div className={style.imgcontainer}>
                            <img src={libicon} alt="" />
                          </div>
                          <div className={style.dtls}>
                            <h4>{item?.foldername}</h4>
                           
                          </div>

                          <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none" onClick={()=>setSelectedFolder(ind)}>
                            <path
                              fillRule="evenodd"
                              clipRule="evenodd"
                              d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z"
                              fill="#252526"
                            />
                          </svg>

                          { (ind==selectedFolder ) && (
                            <div
                              style={{
                                position: "absolute",
                                top: 36,
                                right: 0,
                                background: "white",
                                boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
                                borderRadius: "6px",
                                zIndex: 9,
                                width: "120px",
                                padding: "4px 0",
                              }}
                              ref={dropdownRef}
                            >
                              <div className="dropdown-item" onClick={()=>{setShowRename(true); setSelectedFolderData(item);}} >Rename</div>
                              <div className="dropdown-item" onClick={()=>openDeleteConfirmation('Are you sure you want to delete this folder?', () => handleFolderDelete(item))} >Delete</div>
                            </div>
                          )}
                        </div>
                      ))}

              
 
                  
                  </div>
                  </div>}
                                </div>
 

               
            </div>
          </div>
                 </div>
            </div>
            {moveModal &&

            <MoveFolderModal setmoveModal={()=>setmoveModal(false)} movefile={movefile} fetchFolders={fetchFolders} folderList={folderList}/>
            }
            {createModal &&
                      <CreateFolderModal setcreateModal={()=>setcreateModal(false)} fetchFolders={fetchFolders}/>
            }
            {showRename && 
                      <RenameFolderModal  setShowRenameModal={()=>setShowRename(false)} fetchFolders={fetchFolders} folderData={selectedFolderData} />

            }
            {deleteConfirmation.isOpen && (
              <div
                style={{
                  position: 'fixed',
                  inset: 0,
                  background: 'rgba(0, 0, 0, 0.45)',
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'center',
                  zIndex: 999999999,
                  padding: '16px',
                }}
              >
                <div
                  style={{
                    background: '#fff',
                    borderRadius: '12px',
                    width: '100%',
                    maxWidth: '360px',
                    padding: '24px',
                    boxShadow: '0 12px 30px rgba(0, 0, 0, 0.18)',
                  }}
                >
                  <h4 style={{ margin: '0 0 12px 0', fontSize: '18px', color: '#252526' }}>Delete Confirmation</h4>
                  <p style={{ margin: '0 0 20px 0', fontSize: '14px', color: '#505152', lineHeight: '20px' }}>
                    {deleteConfirmation.message}
                  </p>
                  <div
                    style={{
                      display: 'flex',
                      justifyContent: 'flex-end',
                      gap: '12px',
                    }}
                  >
                    <button
                      onClick={closeDeleteConfirmation}
                      style={{
                        border: '1px solid #d9d9d9',
                        background: '#fff',
                        color: '#252526',
                        borderRadius: '8px',
                        padding: '10px 18px',
                        cursor: 'pointer',
                      }}
                    >
                      No
                    </button>
                    <button
                      onClick={handleConfirmDelete}
                    >
                      Yes
                    </button>
                  </div>
                </div>
              </div>
            )}
          </>:
          <>

            <PdfModal pdfUrl={selectedPDF} onClose={()=>setSelectedPDF("")}/>
          </>
          }
        </>
    )
}
export default Library;
