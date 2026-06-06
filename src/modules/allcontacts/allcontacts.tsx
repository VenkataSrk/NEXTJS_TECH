import React, { useEffect, useRef, useState } from "react";
import styles from "./scss/allcontacts.module.scss"
import { whatsappService } from "../../base/service/apiservice";
import { assets } from "../../styles/assets/assets";
import moment from "moment";
import { GetItemFromStorage } from "../../base/customhooks/useStorage";
import { LocalStorage } from "../../base/customhooks/localStorageKeys";
import { jwtDecode } from "jwt-decode";
import { useDispatch, useSelector } from "react-redux";
import { commonDataSliceActions } from "../../store/slice/commonSlice/commonSlice";
function Allcontacts() {
    const LIMIT = 100;
    const loaderRef = useRef<HTMLDivElement | null>(null);
    const ticketRightRef = useRef<any>(null);
    const [contactDatas,setContactDatas] = useState<any>([]);
      console.log("comtactDatas",contactDatas);
    const [contactDatasCopy,setContactDatasCopy]=useState<any>([]);
    const [archiveContactData,setArchiveContactData] = useState<any>([]);
      console.log("comtactDatas",contactDatas);
    const [archiveContactDataCopy,setArchiveContactDataCopy]=useState<any>([]);
    const [loader,setLoader]= useState<any>(false);
    const [loaderArchive,setLoaderArchive]= useState<any>(false);
    const [contactSummary,setContactSummary] = useState<any>([]);
    const [hasMore,setHasMore] = useState<any>(true);
    const [hasMoreArchive,setHasMoreArchive] = useState<any>(true);
    const [selectedContactData,setSelectedContactData] = useState<any>(null);
    console.log("sledta",selectedContactData)
    const [archiveOpen,setArchiveOpen] = useState<any>(false);
    const [selectedArchiveContact,setSelectedArchiveContact] = useState<any>(null);
    const [archivedContactsData,setArchivedContactsData]=useState<any>([]);
    const [selectedArchiveItem,setSelectedArchiveItem]=useState<any>(null);
    const [unArchiveOpen,setUnArchiveOpen]=useState<any>(false);
    const[selectedUnArchiveContact,setSelectedUnArchiveContact]=useState<any>(null);
    const [showArchivePage,setShowArchivePage]=useState<any>(false);
    const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
    const data: any = jwtDecode(userDetails);
    console.log("data",data);
    const noMore = useRef<any>(true);
    console.log("noMore",noMore)
    console.log("hasmore",hasMore)
    const offsetRef = useRef<any>(0);
    const offsetRefArchive =useRef<any>(0);
    const [archiveorUnarchive,setArchiveOrUnarchive] = useState<any>(0);
    console.log("archiveorunarchive",archiveorUnarchive)
    const firstTime =useRef<any>(0);
    console.log("comtactDatas",contactDatas);
    console.log("contactdatacpy",contactDatasCopy);
    console.log("archiveContactDataCopy",archiveContactDataCopy);
    console.log("contactsummary",contactSummary);
    const searchfilter :any =useSelector((state :any)=>state.commonData.search);
    const contactRefresh :any =useSelector((state :any)=>state.commonData.contactRefresh);
    const dispatch=useDispatch();

    const fetchMore =()=>{
         try {
            //   if (loader || !hasMore) return;
              if ( !hasMore) return;
             let body :any={
                 offset: offsetRef.current,
                 limit: LIMIT,
                 isArchive:0,
                 search:searchfilter??null
             }
              setLoader(true);
              whatsappService
                      .post(body,`/get_contact`)
                      .then((response) => {
                        if (
                          response.data.statusCode === 200 &&
                          response.data.contacts?.length > 0
                        ) {
                            console.log("res",response.data.contacts.length)
                            if (response.data.contacts.length < LIMIT) {
                                setHasMore(false);
                            }else{
                                
                                offsetRef.current+=LIMIT;
                            }
                            setContactDatas(prev => [...prev, ...response.data.contacts]);
                            setContactDatasCopy(prev => [...prev, ...response.data.contacts]);
                            setContactSummary(prev => [...prev, response.data.contacts[0]]);
                            setLoader(false);
                        } else {
                          setContactDatas([...contactDatas]);
                          setLoader(false);
                           setHasMore(false);
                        }
                      })
                      .catch(() => setContactDatas([...contactDatas]));
        } catch (error) {
            console.log("API ERROR:",error)
        }finally{
            setLoader(false);
           
        }
    }

    

const fetchMoreArchive =()=>{
         try {
            //   if (loader || !hasMore) return;
              if ( !hasMoreArchive) return;
             let body :any={
                offset: offsetRefArchive.current,
                limit: LIMIT,
                isArchive:1
            }
              setLoaderArchive(true);
              whatsappService
                      .post(body,`/get_contact`)
                      .then((response) => {
                        if (
                          response.data.statusCode === 200 &&
                          response.data.contacts?.length > 0
                        ) {
                            console.log("res",response.data.contacts.length)
                            if (response.data.contacts.length < LIMIT) {
                                setHasMoreArchive(false);
                            }else{
                                offsetRefArchive.current+=LIMIT;
                            }
                            setArchiveContactData(prev => [...prev, ...response.data.contacts]);
                            setArchiveContactDataCopy(prev => [...prev, ...response.data.contacts]);
                            setContactSummary(prev => [...prev, response.data.contacts[0]]);
                            setLoaderArchive(false);
                        } else {
                          setArchiveContactData([...archiveContactData]);
                          setLoaderArchive(false);
                           setHasMoreArchive(false);
                        }
                      })
                      .catch(() => setArchiveContactData([...archiveContactData]));
        } catch (error) {
            console.log("API ERROR:",error)
        }finally{
            setLoaderArchive(false);
        }
    }

    const formatDateTime = (isoString: string) => {
        const date = new Date(isoString);

        return date.toLocaleString("en-GB", {
            day: "2-digit",
            month: "short",
            year: "numeric",
            hour: "numeric",
            minute: "2-digit",
            hour12: true,
        });
    }

    const handleArchiveContact=async()=>{
        
        try {
             console.log("archivecontact",selectedArchiveContact)
              
             const body :any={
                 id: selectedArchiveContact?.id,
                 isArchive:1,
                 archivedBy:data?.profileName??"ADMIN"
             }
             
              await whatsappService
                      .post(body,`/update_archive_contact`)
                      .then((response) => {
                        console.log("resupdate_archive_contact",response)
                        if (

                          response.data.statusCode === 200 
                         
                        ) {
                            console.log("res",response)

                           
                        } 
                       setArchiveOrUnarchive(archiveorUnarchive+1);
                      })
                      .catch((err :any) =>console.log("err",err));
            console.log("hi")
        } catch (error) {
            console.log("err",error)
        }finally{
            setSelectedContactData(null);
            handleArchiveCancel();
        }
    }

      const handleUnArchiveContact=async()=>{
        
        try {
             console.log("archivecontact",selectedUnArchiveContact)
              
             const body :any={
                 id: selectedUnArchiveContact?.id,
                 isArchive:0,
                 archivedBy:data?.profileName??"ADMIN"
             }
             
              await whatsappService
                      .post(body,`/update_archive_contact`)
                      .then((response) => {
                        if (
                          response.data.statusCode === 200 &&
                          response.data.contacts?.length > 0
                        ) {
                            console.log("res",response)
                        } 
                        setArchiveOrUnarchive(archiveorUnarchive+1);
                      })
                      .catch((err :any) =>console.log("err",err));
            console.log("hi")
        } catch (error) {
            console.log("err",error)
        }finally{
            handleUnArchiveCancel();
            setSelectedArchiveItem(null);
        }
    }

    const handleArchiveCancel=()=>{
        setArchiveOpen(false);
        setSelectedArchiveContact(null);
    }
    
    const handleUnArchiveCancel=()=>{
        setUnArchiveOpen(false);
        setSelectedUnArchiveContact(null);
    }


// useEffect(() => {
//   const observer = new IntersectionObserver(
//     entries => {
//       if (entries[0].isIntersecting) {
//         fetchMore();
//       }
//     },
//     { threshold: 0.3 }
//   );

//   if (loaderRef.current) {
//     observer.observe(loaderRef.current);
//   }


//   return () => observer.disconnect();
// }, [offset, hasMore]);


useEffect(() => {
  const currentLoader = loaderRef.current;
  if (!currentLoader) return;

  const observer = new IntersectionObserver(
    ([entry]) => {
      if (entry.isIntersecting && !loader && hasMore && !showArchivePage) {
        fetchMore();
      }

      if (entry.isIntersecting && !loaderArchive && hasMoreArchive && showArchivePage) {
        fetchMoreArchive();
      }
    },
    { threshold: 0.3 }
  );

  observer.observe(currentLoader);

  return () => observer.disconnect();
}, [loader, hasMore, loaderArchive, hasMoreArchive]);


useEffect(()=>{
    if(firstTime.current || archiveorUnarchive>0){

        if(!showArchivePage ){
            offsetRef.current=0;
            setHasMore(true);
            setContactDatas([]);
            setContactDatasCopy([]);
            
            fetchMore();
        }else{
             offsetRefArchive.current = 0;
             setHasMoreArchive(true);
            setArchiveContactData([]);
            setArchiveContactDataCopy([]);
           
            fetchMoreArchive();
            
        }
    }
    if(!firstTime.current && showArchivePage){
            offsetRefArchive.current = 0;
             setHasMoreArchive(true);
            setArchiveContactData([]);
            setArchiveContactDataCopy([]);
            fetchMoreArchive();
    }
},[showArchivePage, archiveorUnarchive])

useEffect(() => {
  ticketRightRef.current?.scrollTo({ top: 0, behavior: "auto" });
}, [selectedContactData?.id]);

useEffect(()=>{
if(searchfilter || contactRefresh){
    offsetRef.current = 0;
    setHasMore(true);
    setContactDatas([]);
    setContactDatasCopy([]);
    fetchMore();
    setTimeout(()=>{
        dispatch(commonDataSliceActions.setContactFilter(null));
        dispatch(commonDataSliceActions.setContactRefresh(false));
    
    },100);
}
},[searchfilter,contactRefresh])



    return (<>
        <div className={styles.ticketright} ref={ticketRightRef}>
           
{!showArchivePage &&<>
            {!selectedContactData?.id ?
            <div className={styles.maincontainer}>
                <div className={styles.header}>
                    <div className={styles.lefts}>
                        <h3>Contacts</h3>
                        <p>Keep things organised by archiving what’s no longer active.</p>
                    </div>
                   {/* {archivedContactsData.length>0 &&  */}
                   {true && 
                   <div className={styles.rights} onClick={()=>setShowArchivePage(true)}>
                        <button className="cancelbtn">View Archived</button>
                    </div>}

                </div>

                {/* <div className={styles.gradientcard}>
                    <div className={styles.bluetxt}>
                        Contact Update
                    </div>
                    <div className={styles.head}>
                        New Contacts Added
                    </div>
                    <div className={styles.body}>
                        <b>47 new contacts </b>have been uploaded to your database, including <b> Sarah Chen </b>,<b> Marcus Williams </b> and <b> Elena Rodriguez </b>.
                    </div>
                    <div className={styles.footer}>
                        Download Full List  <span><svg xmlns="http://www.w3.org/2000/svg" width="9" height="12" viewBox="0 0 9 12" fill="none">
                            <path d="M2.98116 8.9056C3.26596 9.1966 3.64649 9.3568 4.05243 9.3568C4.45869 9.3568 4.83923 9.19667 5.12403 8.9056L7.96189 6.00847C8.15523 5.8112 8.15203 5.4948 7.95476 5.30147C7.75749 5.10873 7.44109 5.11133 7.24776 5.30927L4.55663 8.05647L4.55243 0.5C4.55243 0.223934 4.32849 0 4.05243 0C3.77636 0 3.55243 0.223934 3.55243 0.5L3.55663 8.0648L0.857093 5.30927C0.663759 5.112 0.347359 5.1094 0.150093 5.30147C-0.0471741 5.4948 -0.0504406 5.8112 0.142959 6.00847L2.98116 8.9056Z" fill="#7423D7" />
                            <path d="M7.60489 10.3542L0.499759 10.3581C0.223693 10.3581 9.25093e-05 10.5821 9.25093e-05 10.8581C9.25093e-05 11.1342 0.224026 11.3581 0.500093 11.3581L7.60523 11.3542C7.88129 11.3542 8.10489 11.1303 8.10489 10.8542C8.10489 10.5781 7.88096 10.3542 7.60489 10.3542Z" fill="#7423D7" />
                        </svg></span>
                    </div>
                </div> */}

                {/* // gradient card home */}
                 {/* <div className={styles.gradientcard}>
                    <div className={styles.dimtxt}>
                    CONTACT REMINDER
                    </div>
                    <div className={styles.head}>
                        New Contacts Added
                    </div>
                    <div className={styles.body}>
                        <b>47 new contacts </b>have been uploaded to your database, including <b> Sarah Chen </b>,<b> Marcus Williams </b> and <b> Elena Rodriguez </b>.
                    </div>
                    <div className={styles.footer}>
                        Resume Creation <span style={{display:"grid"}}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
  <path d="M10 0C4.4772 0 0 4.4771 0 10C0 15.5229 4.4772 20 10 20C15.5228 20 20 15.5228 20 10C20 4.4772 15.5229 0 10 0ZM10 18.5C5.3131 18.5 1.5 14.687 1.5 10C1.5 5.313 5.3131 1.5 10 1.5C14.6869 1.5 18.5 5.3131 18.5 10C18.5 14.6869 14.687 18.5 10 18.5Z" fill="#7423D7"/>
  <path d="M13.7364 8.9517L8.874 6.1444C8.0671 5.6785 7.0583 6.2609 7.0583 7.1927V12.8073C7.0583 13.7391 8.067 14.3215 8.874 13.8556L13.7364 11.0483C14.5433 10.5824 14.5433 9.4177 13.7364 8.9517Z" fill="#7423D7"/>
</svg></span>
                    </div>
                </div> */}

                <div className={styles.contactdetails}>
                    <div className={styles.headertopmost}>
                        All contacts
                    </div>
                      {contactDatas.length ==0 && <div className={"nodataclass"}><img src={assets.nodataimg}/>No Data found</div>

                    }
                    <div className={styles.contactlist}>
                      
                        {contactDatas?.map((item :any,ind :any)=>{
                            return(
                        <div className={styles.contactype}
                                 onClick={()=>{
                                   
                                    setSelectedContactData(item);}} 
                        >
                            <div className={styles.contactleft}>
                                <div className={styles.contactdata}>
                                    <span style={{ display: "grid", position: "relative", top: "4px" }}>
                                        <svg
                                            xmlns="http://www.w3.org/2000/svg"
                                            width="17"
                                            height="18"
                                            viewBox="0 0 17 18"
                                            fill="none"
                                        >
                                            <path
                                                d="M15.9966 13.4301C15.0459 11.9674 13.2851 10.8319 11.1001 10.285L10.8821 10.2586C10.6914 10.2586 10.5015 10.3162 10.3317 10.4107C9.69464 10.7652 8.96244 10.9683 8.18224 10.9683C7.85564 10.9683 7.52064 10.9327 7.18074 10.8577C6.74914 10.7622 6.34494 10.6009 5.97434 10.3882C5.80104 10.2888 5.60574 10.2276 5.40954 10.2276L5.19704 10.2527C2.98284 10.7922 1.17514 11.9346 0.180743 13.4012C-0.0549569 13.7487 -0.0622568 14.195 0.171343 14.544C1.28204 16.2038 3.43464 17.4409 6.06924 17.8465C6.74444 17.9505 7.41004 18.0001 8.05884 18.0001C11.5895 18.0001 14.6223 16.5349 15.9962 14.4297C16.1976 14.1211 16.1975 13.7391 15.9966 13.4301ZM8.05884 16.5001C7.47594 16.5001 6.88344 16.4542 6.29754 16.3641C4.47474 16.0834 2.89224 15.3352 1.90724 14.3132C1.73174 14.1311 1.73234 13.83 1.90784 13.6478C2.71834 12.8064 3.92994 12.1389 5.35514 11.7603C5.83134 12.0181 6.33584 12.2071 6.85734 12.3226C7.29594 12.4193 7.74164 12.4684 8.18224 12.4684C9.13804 12.4684 10.0853 12.2348 10.9317 11.7914C12.2908 12.1621 13.4415 12.799 14.216 13.5973C14.3927 13.7794 14.3929 14.0817 14.2177 14.2653C12.9036 15.6428 10.5907 16.5001 8.05884 16.5001Z"
                                                fill="black"
                                            />
                                            <path
                                                d="M6.72344 8.5636C7.17764 8.6982 7.62654 8.7613 8.06024 8.7613C10.8067 8.7613 12.9606 6.2372 12.3303 3.3809C11.9921 1.8487 10.9066 0.6472 9.40284 0.1994C8.94674 0.0635999 8.49584 0 8.06024 0C5.31384 0 3.15994 2.5238 3.79024 5.38C4.12874 6.914 5.21734 8.1172 6.72344 8.5636ZM5.81244 2.5781C6.36274 1.893 7.18204 1.5 8.06014 1.5C8.42204 1.5 8.79654 1.5669 9.15754 1.6977C10.0526 2.022 10.6634 2.7802 10.8669 3.7103C11.0639 4.6111 10.8655 5.489 10.3079 6.1831C9.75764 6.8682 8.93834 7.2612 8.06024 7.2612C7.69754 7.2612 7.32234 7.194 6.96204 7.0638C6.06684 6.7402 5.45704 5.9805 5.25364 5.0507C5.05664 4.1501 5.25494 3.272 5.81244 2.5781Z"
                                                fill="black"
                                            />
                                        </svg>
                                    </span>


                                    <div className={styles.namelist}>

                                        <h3>
                                            {item?.Name??item?.firstName??"-"}
                                            {/* <span>dyer</span> */}

                                        </h3>

                                        <div className={styles.emaildetails}>
                                            {item?.whatsappNumber?<p>+{item?.whatsappNumber??"-"}
                                               
                                            </p>:<p>-</p>}
                                            <p>{item?.email??'-'}
                                            </p>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div   className={styles.contactright}>
                                {item?.createdAt?<p>Added On {item?.createdAt?formatDateTime(item?.createdAt):"-"}  
                                    <img src={assets.archiveicon} onClick={(e :any)=>{e.stopPropagation(); setArchiveOpen(true);setSelectedArchiveContact(item);}}></img>
                                    </p>:<p></p>}
                            </div>
                        </div>
                        )
                        })
                        }
                       
                        <>
                        
                            {(hasMore && !searchfilter)&& (
                            <div ref={loaderRef}>
                                {/* {loader ? "Loading..." : "Scroll to load more"} */}
                            </div>
                            )}
                        </>


                    </div>
                </div>
            </div>
:
          <> <div className={styles.yourinfo}>
                <div className={styles.toplayerwithnav}>
            <div style={{margin:"0px"}} className={styles.titlearea}>
                    <div
                        className={styles.desc}
                        // 👈 go to previous page
                        style={{ cursor: "pointer" }}   // optional: show clickable cursor
                        onClick={()=>setSelectedContactData(null)}
                    >
                        All Contacts
                        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                        </svg>
                    </div>

                    <div className={styles.title}>{selectedContactData?.firstName??"New Customer"} </div>
            </div>
                 <div className={styles.rights}>
                        {/* <button className="cancelbtn"  onClick={()=>{setArchiveOpen(true);setSelectedArchiveContact(selectedContactData);}}>Archive</button> */}
                        <button className="cancelbtn"  onClick={(e: any)=>{e.stopPropagation(); setArchiveOpen(true);setSelectedArchiveContact(selectedContactData);}}>Archive</button>
                </div>
             </div>
               
                <div className={styles.yourinfotop}>
                    <div className={styles.profile}>
                        <div className={styles.profileleft}>
                            <span>
                                {(selectedContactData?.firstName?.trim()?.length > 0 ||
                                    selectedContactData?.lastName?.trim()?.length > 0)
                                    ? `${selectedContactData?.firstName?.charAt(0)?.toUpperCase() || ""}${selectedContactData?.lastName?.charAt(0)?.toUpperCase() || ""}`
                                    : "NC"}
                            </span>
                        </div>
                        <div className={styles.profileright}>

                            <div 
                                className={styles.profiletext} 
                               
                               >{selectedContactData?.firstName??"New Customer"}&nbsp;
                                 is an enterprise-level customer with a strong engagement history. He values premium service and fast issue resolution. His recent interactions indicate high satisfaction, with consistent positive feedback.
                             </div>

                            {/* <div className={styles.recentactive}>
                                <div className={styles.activesa}>Active</div>
                                <div className={styles.activesa}>New</div> : <div className={styles.activesa}>Old</div>
                            </div> */}
                        </div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>First Name</div>
                        <div className={styles.value}>{selectedContactData?.firstName??""} </div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Last Name</div>
                        <div className={styles.value}>{selectedContactData?.lastName??""} </div>
                    </div>
                </div>
                <div className={styles.yourinfotop}>
                    <div className={styles.yourtitle}>Work Details</div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Email</div>
                        <div className={styles.value}>
                          {selectedContactData?.email??""}
                            </div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Phone</div>
                        <div className={styles.value}>  
                           {selectedContactData?.phoneNumber??""}
                        </div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Customer Type</div>
                        <div className={styles.value}>{selectedContactData?.customerType??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Customer Tags</div>
                        <div className={styles.value}>{selectedContactData?.tags??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Customer Id</div>
                        <div className={styles.value}>{selectedContactData?.id??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Location</div>
                        <div className={styles.value}>{selectedContactData?.location??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>City</div>
                        <div className={styles.value}>{selectedContactData?.city??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Address</div>
                        <div className={styles.value}>{selectedContactData?.address??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>State</div>
                        <div className={styles.value}>{selectedContactData?.state??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Country</div>
                        <div className={styles.value}>{selectedContactData?.country??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Postal code</div>
                        <div className={styles.value}>{selectedContactData?.postCode??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Whatsapp</div>
                        <div className={styles.value}>{selectedContactData?.whatsappNumber??""}</div>
                        <div className={styles.icon}></div>
                    </div>
                    {/* <div className={styles.fieldareamain}>
                        <div className={styles.label}>Instagram</div>
                        <div className={styles.value}>eer</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Facebook</div>
                        <div className={styles.value}>er</div>
                        <div className={styles.icon}></div>
                    </div> */}
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Date of added</div>
                        <div className={styles.value}>  {selectedContactData?.createdAt?moment(selectedContactData?.createdAt).format('YYYY-MM-DD'):""}</div>
                        <div className={styles.icon}></div>
                    </div>
                </div>

               
            </div>
</>
            }
            </>}

            {showArchivePage && !selectedArchiveContact && !selectedArchiveItem &&
               <div className={styles.maincontainer}>
               
 <div  style={{margin:"16px 0"}} className={styles.titlearea}>
                    <div
                        className={styles.desc}
                        // 👈 go to previous page
                        style={{ cursor: "pointer" }}   // optional: show clickable cursor
                        // onClick={()=>setSelectedContactData(null)}
                        onClick={()=>{firstTime.current=1;setShowArchivePage(false); }}
                    >
                        All Contacts
                        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                        </svg>
                    </div>

                    <div className={styles.title}>Archived </div>
            </div>
                {/* <div className={styles.gradientcard}>
                    <div className={styles.bluetxt}>
                        Contact Update
                    </div>
                    <div className={styles.head}>
                        New Contacts Added
                    </div>
                    <div className={styles.body}>
                        <b>47 new contacts </b>have been uploaded to your database, including <b> Sarah Chen </b>,<b> Marcus Williams </b> and <b> Elena Rodriguez </b>.
                    </div>
                    <div className={styles.footer}>
                        Download Full List  <span><svg xmlns="http://www.w3.org/2000/svg" width="9" height="12" viewBox="0 0 9 12" fill="none">
                            <path d="M2.98116 8.9056C3.26596 9.1966 3.64649 9.3568 4.05243 9.3568C4.45869 9.3568 4.83923 9.19667 5.12403 8.9056L7.96189 6.00847C8.15523 5.8112 8.15203 5.4948 7.95476 5.30147C7.75749 5.10873 7.44109 5.11133 7.24776 5.30927L4.55663 8.05647L4.55243 0.5C4.55243 0.223934 4.32849 0 4.05243 0C3.77636 0 3.55243 0.223934 3.55243 0.5L3.55663 8.0648L0.857093 5.30927C0.663759 5.112 0.347359 5.1094 0.150093 5.30147C-0.0471741 5.4948 -0.0504406 5.8112 0.142959 6.00847L2.98116 8.9056Z" fill="#7423D7" />
                            <path d="M7.60489 10.3542L0.499759 10.3581C0.223693 10.3581 9.25093e-05 10.5821 9.25093e-05 10.8581C9.25093e-05 11.1342 0.224026 11.3581 0.500093 11.3581L7.60523 11.3542C7.88129 11.3542 8.10489 11.1303 8.10489 10.8542C8.10489 10.5781 7.88096 10.3542 7.60489 10.3542Z" fill="#7423D7" />
                        </svg></span>
                    </div>
                </div> */}

                {/* // gradient card home */}
              

                <div className={styles.contactdetails}>
                    <div className={styles.headertopmost}>
                       
                    </div>
                      {archiveContactData.length ==0 && <div className={"nodataclass"}><img src={assets.nodataimg}/>No Data found</div>

                    }
                    <div className={styles.contactlist}>
                      
                        {archiveContactData?.map((item :any,ind :any)=>{
                            return(
                        <div className={styles.contactype}
                               onClick={()=>setSelectedArchiveItem(item)}
                        >
                            <div className={styles.contactleft} >
                                <div className={styles.contactdata}>
                                    <span style={{ display: "grid", position: "relative", top: "4px" }}>
                                        <svg
                                            xmlns="http://www.w3.org/2000/svg"
                                            width="17"
                                            height="18"
                                            viewBox="0 0 17 18"
                                            fill="none"
                                        >
                                            <path
                                                d="M15.9966 13.4301C15.0459 11.9674 13.2851 10.8319 11.1001 10.285L10.8821 10.2586C10.6914 10.2586 10.5015 10.3162 10.3317 10.4107C9.69464 10.7652 8.96244 10.9683 8.18224 10.9683C7.85564 10.9683 7.52064 10.9327 7.18074 10.8577C6.74914 10.7622 6.34494 10.6009 5.97434 10.3882C5.80104 10.2888 5.60574 10.2276 5.40954 10.2276L5.19704 10.2527C2.98284 10.7922 1.17514 11.9346 0.180743 13.4012C-0.0549569 13.7487 -0.0622568 14.195 0.171343 14.544C1.28204 16.2038 3.43464 17.4409 6.06924 17.8465C6.74444 17.9505 7.41004 18.0001 8.05884 18.0001C11.5895 18.0001 14.6223 16.5349 15.9962 14.4297C16.1976 14.1211 16.1975 13.7391 15.9966 13.4301ZM8.05884 16.5001C7.47594 16.5001 6.88344 16.4542 6.29754 16.3641C4.47474 16.0834 2.89224 15.3352 1.90724 14.3132C1.73174 14.1311 1.73234 13.83 1.90784 13.6478C2.71834 12.8064 3.92994 12.1389 5.35514 11.7603C5.83134 12.0181 6.33584 12.2071 6.85734 12.3226C7.29594 12.4193 7.74164 12.4684 8.18224 12.4684C9.13804 12.4684 10.0853 12.2348 10.9317 11.7914C12.2908 12.1621 13.4415 12.799 14.216 13.5973C14.3927 13.7794 14.3929 14.0817 14.2177 14.2653C12.9036 15.6428 10.5907 16.5001 8.05884 16.5001Z"
                                                fill="black"
                                            />
                                            <path
                                                d="M6.72344 8.5636C7.17764 8.6982 7.62654 8.7613 8.06024 8.7613C10.8067 8.7613 12.9606 6.2372 12.3303 3.3809C11.9921 1.8487 10.9066 0.6472 9.40284 0.1994C8.94674 0.0635999 8.49584 0 8.06024 0C5.31384 0 3.15994 2.5238 3.79024 5.38C4.12874 6.914 5.21734 8.1172 6.72344 8.5636ZM5.81244 2.5781C6.36274 1.893 7.18204 1.5 8.06014 1.5C8.42204 1.5 8.79654 1.5669 9.15754 1.6977C10.0526 2.022 10.6634 2.7802 10.8669 3.7103C11.0639 4.6111 10.8655 5.489 10.3079 6.1831C9.75764 6.8682 8.93834 7.2612 8.06024 7.2612C7.69754 7.2612 7.32234 7.194 6.96204 7.0638C6.06684 6.7402 5.45704 5.9805 5.25364 5.0507C5.05664 4.1501 5.25494 3.272 5.81244 2.5781Z"
                                                fill="black"
                                            />
                                        </svg>
                                    </span>


                                    <div className={styles.namelist}>

                                        <h3>
                                            {item?.Name??item?.firstName??"-"}
                                            {/* <span>dyer</span> */}

                                        </h3>

                                        <div className={styles.emaildetails}>
                                            {item?.whatsappNumber?<p>+{item?.whatsappNumber??"-"}
                                               
                                            </p>:<p>-</p>}
                                            <p>{item?.email??'-'}
                                            </p>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div className={styles.contactright}>
                                {item?.createdAt?<p>Added On {item?.createdAt?formatDateTime(item?.createdAt):"-"}  <img src={assets.archiveicon} onClick={(e :any)=>{e.stopPropagation(); setUnArchiveOpen(true);setSelectedUnArchiveContact(item);}}></img></p>:<p></p>}
                            </div>
                        </div>)
                        })
                        }
                       
                        <>
                        
                            {hasMoreArchive && (
                            <div ref={loaderRef}>
                                {/* {loader ? "Loading..." : "Scroll to load more"} */}
                            </div>
                            )}
                        </>


                    </div>
                </div>
            </div>
}

{showArchivePage && !selectedArchiveContact && selectedArchiveItem && 
<div className={styles.yourinfo}>
                <div className={styles.toplayerwithnav}>
            <div className={styles.titlearea} onClick={()=>setSelectedArchiveItem(null)}>
                    <div
                        className={styles.desc}
                        // 👈 go to previous page
                        style={{ cursor: "pointer" }}   // optional: show clickable cursor
                        
                    >
                        All Contacts
                        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                        </svg>
                    </div>

                    <div className={styles.desc} style={{cursor:"pointer"}}>Archived 
                        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                        </svg>
                    </div>
                    <div className={styles.desc}>{selectedArchiveItem?.firstName??"New Customer"} 
                        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                        </svg>
                    </div>
            </div>
                 <div className={styles.rights}>
                        {/* <button className="cancelbtn"  onClick={()=>{setUnArchiveOpen(true);setSelectedUnArchiveContact(selectedContactData);}}>UnArchive</button> */}
                        <button className="cancelbtn"  onClick={(e :any)=>{e.stopPropagation(); setUnArchiveOpen(true);setSelectedUnArchiveContact({...selectedArchiveItem});}}>UnArchive</button>
                </div>
             </div>
               
                <div className={styles.yourinfotop}>
                    <div className={styles.profile}>
                        <div className={styles.profileleft}>
                            <span >
                             
             
                                {selectedArchiveItem?.firstName?.[0]?.toUpperCase()??"NC"}
                            </span>
                        </div>
                        <div className={styles.profileright}>

                            <div 
                                className={styles.profiletext} 
                               
                               >{selectedArchiveItem?.firstName??"New Customer"}&nbsp;
                                 is an enterprise-level customer with a strong engagement history. He values premium service and fast issue resolution. His recent interactions indicate high satisfaction, with consistent positive feedback.
                             </div>

                            {/* <div className={styles.recentactive}>
                                <div className={styles.activesa}>Active</div>
                                <div className={styles.activesa}>New</div> : <div className={styles.activesa}>Old</div>
                            </div> */}
                        </div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>{selectedArchiveItem?.firstName ??"-"}</div>
                        <div className={styles.value}>{selectedArchiveItem?.lastName??"-"} </div>
                    </div>
                   
                </div>
                <div className={styles.yourinfotop}>
                    <div className={styles.yourtitle}>Details</div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Archive Date</div>
                        <div className={styles.value}>
                          {moment(selectedArchiveItem?.archived_date).format("YYYY-MM-DD HH:mm:ss")??"-"}
                            </div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Archive By</div>
                        <div className={styles.value}>  
                           {selectedArchiveItem?.archived_by??"-"}
                        </div>
                        <div className={styles.icon}></div>
                    </div>
                </div>
            </div>
}
        </div>

        
                    
        {
            archiveOpen &&   <div className={styles.overlay}>
            <div className={styles.modal}>

                {/* Header */}
                <div className={styles.header}>
                    <h4>Archive ?</h4>
                    <span className={styles.close} onClick={()=>handleArchiveCancel()} >×</span>
                </div>

                <div className={styles.body}>
                    <div className={styles.block}>
                        <p>Doing this action will move this contact to archive and stop future campaign sends to them.</p>
                    </div>
              
                </div>
          

                {/* Footer */}
                <div className={styles.footer}>
                    <button className={"cancelbtn"} onClick={()=>handleArchiveCancel()}>Cancel</button>
                    <button  onClick={()=>handleArchiveContact()}>Archive</button>
                </div>

            </div>
        </div>
        }
       
         {
            unArchiveOpen &&   <div className={styles.overlay}>
            <div className={styles.modal}>

                {/* Header */}
                <div className={styles.header}>
                    <h4>UnArchive ?</h4>
                    <span className={styles.close} onClick={()=>handleUnArchiveCancel()} >×</span>
                </div>

                <div className={styles.body}>
                    <div className={styles.block}>
                        <p>Doing this action will move this contact to unArchive and start future campaign sends to them.</p>
                    </div>
              
                </div>
          

                {/* Footer */}
                <div className={styles.footer}>
                    <button className={"cancelbtn"} onClick={()=>handleUnArchiveCancel()}>Cancel</button>
                    <button  onClick={()=>handleUnArchiveContact()}>UnArchive</button>
                </div>

            </div>
        </div>
        }

        

    </>)
} export default Allcontacts
