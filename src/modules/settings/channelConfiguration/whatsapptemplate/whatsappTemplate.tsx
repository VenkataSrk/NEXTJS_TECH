import React, { useEffect, useRef, useState } from 'react'
// import styles from "./scss/whatsapp.module.scss"
import styles from "../whatsapp/scss/whatsapp.module.scss"
import BootstrapTable from 'react-bootstrap-table-next';
import paginationFactory from 'react-bootstrap-table2-paginator';
import BlurToastDemo from '../../../../components/custom_components/blurtast/blurtoast';

import Spinner from '../../../../components/custom_components/spinner/spinner';
import WorktualModal from '../../../../components/custom_components/worktualModal';
import { format } from 'date-fns';
import { assets } from '../../../../styles/assets/assets';
const  WhatsappTemplate=()=>{
const [loader,setLoader]=useState<any>(false);
const [modalshow,setModalShow]= useState<any>(false);

const accountDatas = [
  {
    id: 1,

    createdDate:"2026-03-15",
    status: "Active"
  },
  {
    id: 2,
    
    createdDate:"2026-03-15",
    status: "Active"
  },
  {
    id: 3,
   
    createdDate:"2026-03-15",
    status: "Inactive"
  },
  {
    id: 4,
    
    createdDate:"2026-03-15",
    status: "Active"
  },
  {
    id: 5,
    createdDate:"2026-03-15",
    status: "Blocked"
  }
];

const columns = [

  { dataField: "templatename", text: "Template Name",sort: true,
      formatter: (item: any, row: any) => (
        <span
          className="clickabletxt"
          style={{ cursor: "pointer" }}
        onClick={()=>setModalShow(true)}
        >
         jhgjjg
        </span>
      ) },
  { dataField: "status", text: "Status" ,sort: true,
      formatter: (item: number) =>
        item === 1 ? (
          <span className={`${styles.statuselem} ${styles.pending}`}>Connected</span>
          
        ) : (
        //   <span className={`${styles.statuselem} ${styles.formaterr}`}>Not Connected</span>
          <span className={`${styles.statuselem} ${styles.yellow}`}>Connected</span>
        )},
  { dataField: "createdDate", text: "created Date" },

];


  return (
<>

    
      {loader ? (
         <div className='loadersinner'><Spinner /></div>
        //  <div className='loadersinner'><Loaders /></div>
        // <Loaders />
      ) : (
        <div className={styles.overallbody}>
          <div className={styles.innerbody}>
            {/* <div className={styles.headerarea}>
              <div className={styles.lefts}>
                <h5>Whatsapp configuration</h5>
                <p>Microcopy for Whatsapp configuration</p>
              </div>
              <div className={styles.rights}>
                <button className='primarybtn' onClick={launchWhatsAppSignup} >Add account</button>
              </div>
            </div> */}
             <div className={`${styles.topheading} ${styles.header}`}>
                <div className={styles.tophdlefts}>
                    <h2>Whatsapp configuration</h2>
                    <p className={styles.headerdesctxt}>Set up WhatsApp to manage and send campaigns effectively.</p>
                </div>
                {/* <div className='formbtns'>
                        { accountDatas?.length > 0 && <button className='primarybtn' onClick={launchWhatsAppSignup} >Add account</button>}
                </div> */}
            </div>
            
              <div className='table-wrapper'>

              <div className='desktoptable'>
                <BootstrapTable
                  keyField="id"
                  data={accountDatas}
                  columns={columns}
                //   pagination={paginationFactory(tableOpt)}
                />
              </div>
              </div>
            
           
          </div>
        
        </div>
      )
      }

       <div className='closetting closettingheig lgbgmodal' >
                                            <WorktualModal show={modalshow} mdsize="modalxxl" customTitle={<>
                                              <div className={`${styles.emailsmsent} stickyheader `}>
                                                <div className={styles.leftcampaign}>
                                                  <h3>AI Campaign manager - App Push</h3>
                                                  <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                </div>
                                                <div className={styles.rightcampaign}>
                                                 
                                                  <button className={styles.schedulebtn} 
                                                  onClick={() => setModalShow(false)}
                                                  >
                                                    Close
                                                  </button>
                                                </div>
                                              </div>
                                            </>} 
                                            footer={
                                          
                                        false
                                        }>
      
                                            <>
                                            <div className={`${styles.mobilelayoutviewsec} ${false ? styles.mobileheightzoom : ''}`}>
                                      <div className={styles.bodywrapper}>
                                          <div className={styles.mobileheader}>
                                              <img className={styles.topbarimg} src={assets.mobile_topbar} alt="topbarimg" />
                                              <img className={styles.camerafront} src={assets.camera_front} alt="camerafront" />
                                          </div>
                                          <div className={`${styles.mobilemainsec} ${styles.mobilemargin} ${styles.whtsappmobmargin}`}>
                                              <div className={styles.userinfotop}>
                                                  <span className={styles.backicon}>←</span>
                                                  <div style={{ display: "flex", alignItems: "center", gap: "5px", width: "calc(100% - 54px)" }} className={styles.contactinfo}>
                                                      <span className={styles.avatorcon}>
                                                          <img src={assets.avatarimg} alt="avatar" />
                                                      </span>
                                                      <p style={{ margin: "0px" }}>+447441476247</p>
                                                  </div>
                                              </div>
                                              {(
                                                  <div className={`${styles.contentsec} ${styles.contentsecwhtapp}`}>
                                                      <div className={styles.msgsec}>
                                                          {/* Header */}
                                                          {/* {(msg?.campaign_data?.headerType=="Image" || campaignMessageData?.templeteView?.template?.[0]?.headerType === "Image" )&& ( */}
                                                              <div className={styles.imgcon}>
                                                                  <img
                                                                    //   src={msg?.campaign_data?.headerValue??campaignMessageData?.templeteView?.template?.[0]?.headerValue}
                                                                      alt="header image"
                                                                  />
                                                              </div>
                                                          {/* )} */}

                 

                                                          {/* Body */}
                                                          <div
                                                              className={styles.bodymessagewrapper}
                                                              dangerouslySetInnerHTML={{
                                                                  __html:""
                                                                    //   msg?.campaign_data?.bodymessage??campaignMessageData?.templeteView?.template?.[0]?.bodymessage ?? "",
                                                              }}
                                                          />
                                                          {/* Footer */}
                                                          <div
                                                              className={styles.footerwrapper}
                                                              dangerouslySetInnerHTML={{
                                                                  __html:""
                                                                //   msg?.campaign_data?.footer?? campaignMessageData?.templeteView?.template?.[0]?.footer ?? "",
                                                              }}
                                                          />
                                                          {/* Buttons */}
                                                          {/* {(msg?.campaign_data?.buttonType??campaignMessageData?.templeteView?.template?.[0]?.buttonType)?.map(
                                                              (button: any, index: number) => {
                                                                  let label = "";
                                                                  if (button?.action?.value === "calltoAction") {
                                                                      label = button?.website?.label ?? button?.buttonText;
                                                                  } else if (button?.action?.value === "quickReply") {
                                                                      label = button?.buttonText;
                                                                  }
                                                                  console.log("label>>>>>>>>", label)
                                                                  return ( */}
                                                                      <>
                                                                          <div className={styles.insidebtn} 
                                                                        //   key={index}
                                                                          >
                                                                              {/* {label === "Visit website" && */}
                                                                               <span className='link'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                  <path d="M13.227 11.9966H13.2231C12.811 11.9966 12.4751 12.3296 12.4731 12.7427L12.4643 14.3687C12.4643 15.1265 11.8442 15.7437 11.0825 15.7437H3.47897C2.71727 15.7437 2.09717 15.1236 2.09717 14.3619V6.75107C2.09717 5.98937 2.71727 5.36927 3.47897 5.36927H6.38917C6.80327 5.36927 7.13917 5.03337 7.13917 4.61927C7.13917 4.20517 6.80327 3.86927 6.38917 3.86927H3.47897C1.89007 3.86927 0.597168 5.16227 0.597168 6.75107V14.3619C0.597168 15.9508 1.89017 17.2437 3.47897 17.2437H11.0825C12.6714 17.2437 13.9643 15.9537 13.9643 14.3726L13.9731 12.7505C13.9751 12.3364 13.6411 11.9985 13.227 11.9966Z" fill="#0096DE" />
                                                                                  <path d="M16.8403 4.33297L13.4829 0.976067C13.1899 0.683067 12.7153 0.683067 12.4224 0.976067C12.1295 1.26907 12.1294 1.74367 12.4224 2.03657L15.4141 5.02767C15.3704 5.01867 15.3308 4.99957 15.2847 4.99847C8.10397 4.83537 7.22607 6.10927 6.81497 11.1977C6.78177 11.6108 7.08937 11.9721 7.50247 12.0063C7.52297 12.0073 7.54347 12.0083 7.56397 12.0083C7.95067 12.0083 8.27877 11.7114 8.31107 11.3188C8.64217 7.20547 8.71047 6.34607 15.2515 6.49847C15.2613 6.49847 15.2691 6.49367 15.2788 6.49347L12.4224 9.34907C12.1294 9.64207 12.1294 10.1167 12.4224 10.4096C12.5689 10.5561 12.7603 10.6293 12.9527 10.6293C13.1441 10.6293 13.3365 10.5561 13.483 10.4096L16.8404 7.05317C17.5904 6.30317 17.5903 5.08297 16.8403 4.33297Z" fill="#0096DE" />
                                                                              </svg>
                                                                              </span>
                                                                              {/* } */}
                                                                              {"label" === "Call Phone Number" && 
                                                                              <span className='phone'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                  <path d="M3.94222 1.51336C4.32432 1.51336 4.68842 1.69106 4.93932 1.99846L6.16772 3.51046C6.53772 3.96556 6.59042 4.60056 6.29562 5.09616L5.29662 6.79246C4.67362 7.83006 5.20752 9.11376 7.03942 10.9658C8.83422 12.7409 9.76552 12.9463 10.3099 12.9463C10.6154 12.9463 10.9126 12.8704 11.2031 12.7068L12.8929 11.7007C13.1043 11.5772 13.3463 11.512 13.5927 11.512C13.9174 11.512 14.2356 11.6255 14.4903 11.8328L15.9995 13.0594C16.5041 13.471 16.6326 14.1685 16.2831 14.7056L16.1591 14.9043C15.5306 15.8804 14.455 16.4866 13.3522 16.4866C13.1761 16.4866 12.9989 16.471 12.8296 16.4407C10.6775 16.0498 8.33012 14.6013 5.85772 12.1416C3.39092 9.66206 1.94512 7.31786 1.56072 5.17476C1.33572 3.91746 1.95282 2.57756 3.12472 1.82226L3.30322 1.70096C3.49452 1.57836 3.71522 1.51336 3.94222 1.51336ZM3.94222 0.0134583C3.44232 0.0134583 2.94002 0.151858 2.49332 0.438458L2.28412 0.580058C0.691623 1.60596 -0.259777 3.51556 0.0843235 5.43896C0.529723 7.92236 2.11552 10.5069 4.79452 13.1995C7.49382 15.8853 10.0716 17.4641 12.5617 17.9165C12.8263 17.9639 13.0905 17.9866 13.3523 17.9866C14.9935 17.9866 16.5358 17.0901 17.4203 15.7166L17.5553 15.5005C18.3112 14.3399 18.048 12.7944 16.9479 11.897L15.4364 10.6687C14.9016 10.2332 14.248 10.012 13.5929 10.012C13.0906 10.012 12.5875 10.1421 12.1365 10.4056L10.4359 11.4181C10.4023 11.4369 10.3602 11.4464 10.31 11.4464C9.94612 11.4464 9.15562 10.9491 8.09432 9.89946C6.89982 8.69166 6.42072 7.83456 6.58272 7.56476L7.58822 5.85746C8.20222 4.82496 8.09432 3.50226 7.33172 2.56426L6.10362 1.05276C5.54942 0.373558 4.74872 0.0134583 3.94222 0.0134583Z" fill="#0096DE" />
                                                                              </svg>
                                                                              </span>
                                                                              } 
                                                                              {"label" === "Copy Offer Code" && 
                                                                              <span className='copy'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                  <path d="M16.669 1.47352C15.5308 0.393423 13.959 0.210824 12.4312 0.174724C11.1871 0.142524 10.3062 0.142524 9.06597 0.174724C7.53717 0.210824 5.96537 0.392524 4.82717 1.47352C4.35597 1.92032 4.06827 2.46182 3.88657 3.03062C2.84447 3.22792 2.01987 3.60702 1.39107 4.20392C0.140572 5.39042 0.104972 7.21762 0.0790718 8.55252L0.0761719 11.9646L0.0790718 12.1121C0.104972 13.4461 0.140572 15.2732 1.39107 16.4695C2.57857 17.5965 4.22357 17.7869 5.82267 17.825C6.47647 17.8416 7.03457 17.8494 7.59217 17.8494C8.14977 17.8494 8.70837 17.8416 9.36027 17.825C10.9609 17.7869 12.6059 17.5965 13.7934 16.4695C14.4994 15.8001 14.8041 14.9314 14.9481 14.1516C15.5778 13.9602 16.1668 13.665 16.6689 13.1882C17.8652 12.0544 17.9008 10.3298 17.9243 9.17362V5.47342C17.9009 4.33282 17.8653 2.60832 16.669 1.47352ZM16.4119 5.06292C15.6554 5.05442 15.1092 5.02242 14.6787 4.89832C13.4585 4.56142 12.5376 3.43832 12.439 2.16202C12.4256 2.00042 12.4333 1.83852 12.4357 1.67692C13.8345 1.71232 14.9171 1.87812 15.6367 2.56142C16.2375 3.13172 16.3718 4.05822 16.4119 5.06292ZM12.7613 15.3817C11.9874 16.1161 10.8282 16.2899 9.32377 16.3251C8.04447 16.3583 7.14257 16.3583 5.85937 16.3251C4.35647 16.2899 3.19727 16.1151 2.42577 15.3837C1.62787 14.621 1.60207 13.2724 1.57907 12.0839L1.57617 8.73142L1.57907 8.58202C1.60197 7.39352 1.62787 6.04682 2.42327 5.29202C2.73227 4.99902 3.12217 4.78862 3.60487 4.63842C3.58137 4.98782 3.57427 5.32542 3.56827 5.63472L3.56537 8.87982L3.56827 9.02042C3.59267 10.2948 3.62637 12.0409 4.82657 13.1884C5.96477 14.2685 7.53657 14.4511 9.06437 14.4872C9.68647 14.5028 10.2172 14.5106 10.748 14.5106C11.2783 14.5106 11.8095 14.5028 12.4296 14.4872C12.729 14.4801 13.0296 14.4643 13.3288 14.4403C13.1957 14.8341 13.012 15.1439 12.7613 15.3817ZM15.6368 12.1005C14.9093 12.7909 13.8145 12.954 12.3931 12.9872C11.1763 13.0175 10.3199 13.0175 9.10107 12.9872C7.68117 12.954 6.58647 12.7909 5.86137 12.1024C5.11427 11.3876 5.08987 10.1151 5.06787 8.99202L5.06547 5.80652L5.06787 5.66292C5.08987 4.54082 5.11427 3.26842 5.85987 2.56132C6.58647 1.87092 7.68117 1.70782 9.10257 1.67462C9.71097 1.65902 10.2291 1.65122 10.7481 1.65122C10.8004 1.65122 10.8549 1.65222 10.9073 1.65242C10.9046 1.86382 10.8967 2.07622 10.9141 2.28402C11.063 4.21762 12.4126 5.86022 14.2632 6.37092C14.8599 6.54302 15.5245 6.58232 16.4243 6.59162V9.15802C16.4004 10.3377 16.3219 11.4511 15.6368 12.1005Z" fill="#0096DE" />
                                                                              </svg>
                                                                              </span>
                                                                              } 
                                                                              <span className={`${styles.clickabletxt} clickabletxt`}>mhjkjhhjjgh
                                                                                {/* {"ghfg"
                                                                              label
                                                                              } */}
                                                                              </span>
                                                                          </div>
                                                                      </>
                                                                       <>
                                                                          <div className={styles.insidebtn} 
                                                                        //   key={index}
                                                                          >
                                                                              {/* {label === "Visit website" && */}
                                                                               <span className='link'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                  <path d="M13.227 11.9966H13.2231C12.811 11.9966 12.4751 12.3296 12.4731 12.7427L12.4643 14.3687C12.4643 15.1265 11.8442 15.7437 11.0825 15.7437H3.47897C2.71727 15.7437 2.09717 15.1236 2.09717 14.3619V6.75107C2.09717 5.98937 2.71727 5.36927 3.47897 5.36927H6.38917C6.80327 5.36927 7.13917 5.03337 7.13917 4.61927C7.13917 4.20517 6.80327 3.86927 6.38917 3.86927H3.47897C1.89007 3.86927 0.597168 5.16227 0.597168 6.75107V14.3619C0.597168 15.9508 1.89017 17.2437 3.47897 17.2437H11.0825C12.6714 17.2437 13.9643 15.9537 13.9643 14.3726L13.9731 12.7505C13.9751 12.3364 13.6411 11.9985 13.227 11.9966Z" fill="#0096DE" />
                                                                                  <path d="M16.8403 4.33297L13.4829 0.976067C13.1899 0.683067 12.7153 0.683067 12.4224 0.976067C12.1295 1.26907 12.1294 1.74367 12.4224 2.03657L15.4141 5.02767C15.3704 5.01867 15.3308 4.99957 15.2847 4.99847C8.10397 4.83537 7.22607 6.10927 6.81497 11.1977C6.78177 11.6108 7.08937 11.9721 7.50247 12.0063C7.52297 12.0073 7.54347 12.0083 7.56397 12.0083C7.95067 12.0083 8.27877 11.7114 8.31107 11.3188C8.64217 7.20547 8.71047 6.34607 15.2515 6.49847C15.2613 6.49847 15.2691 6.49367 15.2788 6.49347L12.4224 9.34907C12.1294 9.64207 12.1294 10.1167 12.4224 10.4096C12.5689 10.5561 12.7603 10.6293 12.9527 10.6293C13.1441 10.6293 13.3365 10.5561 13.483 10.4096L16.8404 7.05317C17.5904 6.30317 17.5903 5.08297 16.8403 4.33297Z" fill="#0096DE" />
                                                                              </svg>
                                                                              </span>
                                                                              {/* } */}
                                                                              {"label" === "Call Phone Number" && 
                                                                              <span className='phone'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                  <path d="M3.94222 1.51336C4.32432 1.51336 4.68842 1.69106 4.93932 1.99846L6.16772 3.51046C6.53772 3.96556 6.59042 4.60056 6.29562 5.09616L5.29662 6.79246C4.67362 7.83006 5.20752 9.11376 7.03942 10.9658C8.83422 12.7409 9.76552 12.9463 10.3099 12.9463C10.6154 12.9463 10.9126 12.8704 11.2031 12.7068L12.8929 11.7007C13.1043 11.5772 13.3463 11.512 13.5927 11.512C13.9174 11.512 14.2356 11.6255 14.4903 11.8328L15.9995 13.0594C16.5041 13.471 16.6326 14.1685 16.2831 14.7056L16.1591 14.9043C15.5306 15.8804 14.455 16.4866 13.3522 16.4866C13.1761 16.4866 12.9989 16.471 12.8296 16.4407C10.6775 16.0498 8.33012 14.6013 5.85772 12.1416C3.39092 9.66206 1.94512 7.31786 1.56072 5.17476C1.33572 3.91746 1.95282 2.57756 3.12472 1.82226L3.30322 1.70096C3.49452 1.57836 3.71522 1.51336 3.94222 1.51336ZM3.94222 0.0134583C3.44232 0.0134583 2.94002 0.151858 2.49332 0.438458L2.28412 0.580058C0.691623 1.60596 -0.259777 3.51556 0.0843235 5.43896C0.529723 7.92236 2.11552 10.5069 4.79452 13.1995C7.49382 15.8853 10.0716 17.4641 12.5617 17.9165C12.8263 17.9639 13.0905 17.9866 13.3523 17.9866C14.9935 17.9866 16.5358 17.0901 17.4203 15.7166L17.5553 15.5005C18.3112 14.3399 18.048 12.7944 16.9479 11.897L15.4364 10.6687C14.9016 10.2332 14.248 10.012 13.5929 10.012C13.0906 10.012 12.5875 10.1421 12.1365 10.4056L10.4359 11.4181C10.4023 11.4369 10.3602 11.4464 10.31 11.4464C9.94612 11.4464 9.15562 10.9491 8.09432 9.89946C6.89982 8.69166 6.42072 7.83456 6.58272 7.56476L7.58822 5.85746C8.20222 4.82496 8.09432 3.50226 7.33172 2.56426L6.10362 1.05276C5.54942 0.373558 4.74872 0.0134583 3.94222 0.0134583Z" fill="#0096DE" />
                                                                              </svg>
                                                                              </span>
                                                                              } 
                                                                              {"label" === "Copy Offer Code" && 
                                                                              <span className='copy'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                  <path d="M16.669 1.47352C15.5308 0.393423 13.959 0.210824 12.4312 0.174724C11.1871 0.142524 10.3062 0.142524 9.06597 0.174724C7.53717 0.210824 5.96537 0.392524 4.82717 1.47352C4.35597 1.92032 4.06827 2.46182 3.88657 3.03062C2.84447 3.22792 2.01987 3.60702 1.39107 4.20392C0.140572 5.39042 0.104972 7.21762 0.0790718 8.55252L0.0761719 11.9646L0.0790718 12.1121C0.104972 13.4461 0.140572 15.2732 1.39107 16.4695C2.57857 17.5965 4.22357 17.7869 5.82267 17.825C6.47647 17.8416 7.03457 17.8494 7.59217 17.8494C8.14977 17.8494 8.70837 17.8416 9.36027 17.825C10.9609 17.7869 12.6059 17.5965 13.7934 16.4695C14.4994 15.8001 14.8041 14.9314 14.9481 14.1516C15.5778 13.9602 16.1668 13.665 16.6689 13.1882C17.8652 12.0544 17.9008 10.3298 17.9243 9.17362V5.47342C17.9009 4.33282 17.8653 2.60832 16.669 1.47352ZM16.4119 5.06292C15.6554 5.05442 15.1092 5.02242 14.6787 4.89832C13.4585 4.56142 12.5376 3.43832 12.439 2.16202C12.4256 2.00042 12.4333 1.83852 12.4357 1.67692C13.8345 1.71232 14.9171 1.87812 15.6367 2.56142C16.2375 3.13172 16.3718 4.05822 16.4119 5.06292ZM12.7613 15.3817C11.9874 16.1161 10.8282 16.2899 9.32377 16.3251C8.04447 16.3583 7.14257 16.3583 5.85937 16.3251C4.35647 16.2899 3.19727 16.1151 2.42577 15.3837C1.62787 14.621 1.60207 13.2724 1.57907 12.0839L1.57617 8.73142L1.57907 8.58202C1.60197 7.39352 1.62787 6.04682 2.42327 5.29202C2.73227 4.99902 3.12217 4.78862 3.60487 4.63842C3.58137 4.98782 3.57427 5.32542 3.56827 5.63472L3.56537 8.87982L3.56827 9.02042C3.59267 10.2948 3.62637 12.0409 4.82657 13.1884C5.96477 14.2685 7.53657 14.4511 9.06437 14.4872C9.68647 14.5028 10.2172 14.5106 10.748 14.5106C11.2783 14.5106 11.8095 14.5028 12.4296 14.4872C12.729 14.4801 13.0296 14.4643 13.3288 14.4403C13.1957 14.8341 13.012 15.1439 12.7613 15.3817ZM15.6368 12.1005C14.9093 12.7909 13.8145 12.954 12.3931 12.9872C11.1763 13.0175 10.3199 13.0175 9.10107 12.9872C7.68117 12.954 6.58647 12.7909 5.86137 12.1024C5.11427 11.3876 5.08987 10.1151 5.06787 8.99202L5.06547 5.80652L5.06787 5.66292C5.08987 4.54082 5.11427 3.26842 5.85987 2.56132C6.58647 1.87092 7.68117 1.70782 9.10257 1.67462C9.71097 1.65902 10.2291 1.65122 10.7481 1.65122C10.8004 1.65122 10.8549 1.65222 10.9073 1.65242C10.9046 1.86382 10.8967 2.07622 10.9141 2.28402C11.063 4.21762 12.4126 5.86022 14.2632 6.37092C14.8599 6.54302 15.5245 6.58232 16.4243 6.59162V9.15802C16.4004 10.3377 16.3219 11.4511 15.6368 12.1005Z" fill="#0096DE" />
                                                                              </svg>
                                                                              </span>
                                                                              } 
                                                                              <span className={`${styles.clickabletxt} clickabletxt`}>mhjkjhhjjgh
                                                                                {/* {"ghfg"
                                                                              label
                                                                              } */}
                                                                              </span>
                                                                          </div>
                                                                      </>
                                                                  {/* );
                                                              }
                                                          )} */}
                                                      </div>
                                                  </div>
                                              )}
                                          </div>
                                          <div className={styles.mobilefooter}>
                                              <img src={assets.message_bar} alt="msgbar" />
                                          </div>
                                      </div>
                                  </div>
                                            </>
      
                                            </WorktualModal>
                                          </div>
      </>

  );
}
export default WhatsappTemplate;