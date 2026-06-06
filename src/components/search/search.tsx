import React, { useEffect, useState } from 'react'
import WorktualModal from '../custom_components/worktualModal'
import chatMsg from '../../styles/assets/images/chatmsg.svg'
import { whatsappService } from '../../base/service/apiservice';
import { useNavigate } from 'react-router-dom';
import { commonDataSliceActions } from '../../store/slice/commonSlice/commonSlice';
import { useDispatch } from 'react-redux';

function Search({open,setOpen}:any) {
    const [liveCampaign,setLiveCampaign]= useState<any>([]);
    console.log("live",liveCampaign)
    const [liveCampaignCopy,setLiveCampaignCopy]= useState<any>([]);
    const [futureCampaign,setFutureCampaign]= useState<any>([]);
    const [futureCampaignCopy,setFutureCampaignCopy]= useState<any>([]);
    const [allDraftData,setAllDraftData]= useState<any>([]);
    const [allDraftDataCopy,setAllDraftDataCopy]= useState<any>([]);
    const [searchName,setSearchName] = useState<any>('');
    const navigate = useNavigate();
    const dispatch = useDispatch();

    const handleGetLiveCampaign = async () => {
        try {
            await whatsappService.get(`/get_live_campaign_list`)
                .then((response) => {
                if (response?.data?.statusCode === 200 && response?.data?.getLiveCampaignListResponse?.length > 0) {
                    console.log("res???:",response?.data?.getLiveCampaignListResponse)
                    setLiveCampaign(response?.data?.getLiveCampaignListResponse);
                    setLiveCampaignCopy(response?.data?.getLiveCampaignListResponse);
                }
                }).catch((error) => {
                console.log(error)
                })
        } catch (error) {
        console.log("error", error);
        }
        }
    
    const handleGetFutureCampaign = async () => {
        try {
            await whatsappService.get(`/get_Future_campaign_list`)
            .then((response) => {
                if (response?.data?.statusCode === 200 && response?.data?.getFutureCampaignListResponse?.length > 0) {
                console.log("res???:", response?.data?.getFutureCampaignListResponse)
                setFutureCampaign(response?.data?.getFutureCampaignListResponse);
                setFutureCampaignCopy(response?.data?.getFutureCampaignListResponse);
                }
            }).catch((error) => {
                console.log(error)
            })
        } catch (error) {
            console.log("error", error);
        }
    }

    const getAllDraft = async () => {
        try {
            whatsappService.get('/get_all_draft_campaign').then((res: any) => {
                console.log("getAllDraftres", res)
                if (res?.data?.statusCode == 200) {

                    // getIndividualDraft(res?.data?.getAllDraftRes?.[0], 0)
                    setAllDraftData(res?.data?.getAllDraftRes);
                    setAllDraftDataCopy(res?.data?.getAllDraftRes);
                }

            })

        } catch (error: any) {
            console.log("err", error)
        }
    }
    
    const setSearchCampaignName = (data: string) => {
        setSearchName(data);
        if (data && data.trim() !== '') {
            console.log("hehehe")
            const lowerCaseData = data.toLowerCase();
            const liveResult = liveCampaignCopy?.filter((list: any) =>
               JSON.parse(list?.campaign_Details)?.[0]?.CampaignName?.toLowerCase()?.includes(lowerCaseData)
            // list?.conversationName.includes(lowerCaseData)
            );
            console.log("liveResult",liveResult)
             const futureResult = futureCampaignCopy?.filter((list: any) =>
                JSON.parse(list?.campaign_Details)?.[0]?.CampaignName?.toLowerCase()?.includes(lowerCaseData)
            //  list?.conversationName.includes(lowerCaseData)
            );
            console.log("futureResult",futureResult)
             const draftResult = allDraftDataCopy?.filter((list: any) =>            
                list?.conversationName?.toLowerCase()?.includes(lowerCaseData)
            );
            console.log("draftResult??",draftResult)
            setLiveCampaign([...liveResult]);
            setFutureCampaign([...futureResult]);
            setAllDraftData([...draftResult]);
        } else {
            setLiveCampaign(liveCampaignCopy);
            setFutureCampaign(futureCampaignCopy);
            setAllDraftData(allDraftDataCopy);
        }
    };

    const handleRoute=(data :any,ind :any,campType :any)=>{
        dispatch(commonDataSliceActions.setMultiSearchData({"data":data,"campType":campType,"index":ind}));
        setOpen()
       
        
    }

    useEffect(()=>{
        handleGetLiveCampaign();
        handleGetFutureCampaign();
        getAllDraft();
    },[])
  return (
      <div >
        <WorktualModal
          show={true}
          title='Search'
          onClose={()=>setOpen()}
          // show={false}
          mdsize='modalsm'
        >
       <div  style={{marginBottom:"8px"}} className={` inputsearch inptbordered`}>
            <input type='search' placeholder='Search' onChange={(e :any)=>setSearchCampaignName(e?.target?.value)} value={searchName??''} />
          </div>
          {((liveCampaign?.length==0 && futureCampaign?.length==0 && allDraftData?.length==0))?
            <>
            <div >
                        <div style={{ display: "grid", placeItems: "center" }}>
                            <svg width="60" height="60" viewBox="0 0 60 60" fill="none" xmlns="http://www.w3.org/2000/svg">
                                <path d="M28.4469 59.9999C36.6597 59.9999 43.3175 59.7776 43.3175 59.5034C43.3175 59.2292 36.6597 59.0068 28.4469 59.0068C20.2341 59.0068 13.5763 59.2292 13.5763 59.5034C13.5763 59.7776 20.2341 59.9999 28.4469 59.9999Z" fill="#F4F4F4" />
                                <path d="M28.2922 55.6883C43.0198 55.6883 54.9588 43.7492 54.9588 29.0216C54.9588 14.2941 43.0198 2.35498 28.2922 2.35498C13.5646 2.35498 1.62549 14.2941 1.62549 29.0216C1.62549 43.7492 13.5646 55.6883 28.2922 55.6883Z" fill="#F4F4F4" />
                                <path d="M40.1849 11.2512H20.1671C17.3553 11.2512 15.0758 13.5307 15.0758 16.3425V36.8156C15.0758 39.6274 17.3553 41.9069 20.1671 41.9069H40.1849C42.9967 41.9069 45.2761 39.6274 45.2761 36.8156V16.3425C45.2761 13.5307 42.9967 11.2512 40.1849 11.2512Z" fill="#D2D3D4" />
                                <path d="M44.745 35.7204L40.4685 17.4788C39.7712 14.5042 36.7945 12.6581 33.8199 13.3554L15.5783 17.6318C12.6037 18.3292 10.7576 21.3059 11.4549 24.2805L15.7314 42.5221C16.4287 45.4967 19.4054 47.3428 22.38 46.6454L40.6216 42.369C43.5962 41.6717 45.4423 38.695 44.745 35.7204Z" fill="#BDBEBF" stroke="#001F16" stroke-width="0.166667" stroke-miterlimit="10" />
                                <path d="M34.5892 13.1753L16.1282 17.5032C13.5641 18.1043 11.9728 20.6702 12.5739 23.2343L16.9018 41.6953C17.5029 44.2594 20.0688 45.8507 22.6329 45.2496L41.0939 40.9217C43.658 40.3206 45.2493 37.7547 44.6482 35.1906L40.3204 16.7296C39.7193 14.1655 37.1533 12.5742 34.5892 13.1753Z" fill="#F8FFFC" stroke="#001F16" stroke-width="0.166667" stroke-miterlimit="10" />
                                <path d="M33.0365 31.9424C32.0354 30.99 30.4612 30.4352 28.6611 30.4092L28.4866 30.4282C28.338 30.4626 28.2003 30.5412 28.0847 30.6444C27.6512 31.0317 27.1168 31.3199 26.5088 31.4606C26.2542 31.5196 25.9868 31.5526 25.7086 31.5563C25.3552 31.5609 25.0116 31.5098 24.6849 31.4133C24.5322 31.3682 24.3691 31.3563 24.2162 31.3917L24.055 31.4494C22.4259 32.2631 21.2207 33.4665 20.7063 34.7722C20.5827 35.0861 20.6666 35.4374 20.915 35.6655C22.0771 36.7326 23.9683 37.2899 26.0855 37.1269C26.6302 37.085 27.1577 37.003 27.6633 36.886C30.4139 36.2492 32.5163 34.5773 33.2133 32.7124C33.3125 32.4471 33.2418 32.1376 33.0365 31.9424Z" fill="#D9D9DE" />
                                <path d="M25.0032 29.6229C25.2872 29.62 25.5605 29.5867 25.8204 29.5265C28.0085 29.0199 29.259 26.6118 28.2298 24.4525C27.6404 23.216 26.3915 22.4366 25.0218 22.4506C24.7377 22.4535 24.4644 22.4868 24.2046 22.547C22.0165 23.0535 20.766 25.4617 21.7952 27.621C22.3846 28.8575 23.6335 29.6368 25.0032 29.6229Z" fill="#D9D9DE" />
                                <path d="M41.8256 38.4644L36.3646 33.4844" stroke="#001F16" stroke-width="2.33333" stroke-miterlimit="10" stroke-linecap="round" />
                                <path d="M31.2687 34.0525C34.3295 34.0525 36.8107 31.5713 36.8107 28.5105C36.8107 25.4497 34.3295 22.9685 31.2687 22.9685C28.208 22.9685 25.7267 25.4497 25.7267 28.5105C25.7267 31.5713 28.208 34.0525 31.2687 34.0525Z" fill="#EFFAF7" />
                                <path d="M36.1118 32.4724C35.4768 33.4214 34.6177 34.1977 33.5926 34.6935C33.5786 34.7007 33.5671 34.7044 33.553 34.7115L33.5501 34.7117C33.2199 34.8732 32.8729 34.9973 32.5158 35.0933C30.7213 35.5676 28.9184 35.1504 27.4842 34.108C27.4574 34.0934 27.4364 34.082 27.418 34.0639C27.3908 34.0429 27.3606 34.0224 27.3363 34.0014C26.9551 33.7102 26.6031 33.3695 26.2865 32.9887C26.2825 32.9869 26.2814 32.9848 26.2831 32.9825C25.8554 32.4774 25.4961 31.8984 25.2205 31.2605C25.1476 31.0958 25.0807 30.9305 25.025 30.7616L25.0816 30.7362C25.2702 30.6876 25.4747 30.7147 25.6593 30.8033C25.8199 30.8773 25.9828 30.9416 26.1449 30.99C26.2246 31.0143 26.3038 31.0323 26.383 31.0502C26.389 31.0532 26.3977 31.0527 26.4039 31.0586C26.4565 31.0684 26.5121 31.0781 26.5677 31.0847C26.6848 31.1069 26.8072 31.1193 26.9263 31.1223C27.2718 31.1321 27.6031 31.0948 27.9199 31.0042C28.6818 30.784 29.3573 30.3075 29.9122 29.6432C30.0587 29.4601 30.2344 29.3294 30.4229 29.2809L30.6419 29.2561C32.455 29.3828 34.0729 30.1286 35.259 31.3534C35.5294 31.6316 35.7751 31.9366 35.9988 32.2619C36.0429 32.3264 36.0815 32.3945 36.1118 32.4724Z" fill="#D2D3D4" />
                                <path d="M28.0945 29.7899C27.818 29.9296 27.5201 30.0448 27.1971 30.1315C27.0166 30.182 26.837 30.2149 26.6577 30.2379C26.6325 30.2414 26.6098 30.2444 26.5846 30.2479C26.5767 30.2474 26.5718 30.2486 26.5657 30.2451C26.1624 30.2895 25.7014 30.1812 25.3151 30.111C25.3051 30.0912 25.354 30.1625 25.3435 30.1404C25.3324 30.1159 25.322 30.0939 25.3115 30.0718C24.9841 29.3311 24.8516 28.5562 24.8915 27.7945C24.9066 27.5302 24.9392 27.2741 24.9944 27.0249C25.0106 26.9399 25.0323 26.8561 25.0521 26.7752C25.1202 26.5254 25.2033 26.2843 25.3054 26.0462C25.3067 26.041 25.3061 26.0386 25.3099 26.0328C25.3319 25.9832 25.3541 25.9338 25.3816 25.8854C25.4083 25.8225 25.4398 25.7585 25.4714 25.6944C25.5011 25.6332 25.5333 25.5715 25.5684 25.5114C25.7732 25.148 26.023 24.8082 26.3104 24.4964C26.4293 24.3722 26.5553 24.2462 26.6843 24.132C27.1315 23.7254 27.6566 23.3841 28.2482 23.1252C28.25 23.1244 28.2517 23.1237 28.2534 23.123C28.5917 22.9801 28.9839 23.0837 29.2461 23.366C29.2856 23.4084 29.3243 23.452 29.3622 23.4966C29.4454 23.5949 29.5237 23.6941 29.5947 23.7953C31.0674 25.8588 30.3068 28.6533 28.0945 29.7899Z" fill="#D2D3D4" />
                                <path d="M38.0869 27.4156C37.599 24.3724 35.1928 22.1196 32.3027 21.6796C32.2808 21.679 32.2589 21.6723 32.2403 21.6718C31.7902 21.6065 31.3266 21.5849 30.8584 21.6134C30.6323 21.6262 30.4057 21.6517 30.1758 21.6864C29.3849 21.8161 28.6474 22.0727 27.9821 22.4379C27.7912 22.5395 27.6 22.6569 27.418 22.7745C26.9742 23.0733 26.5665 23.4202 26.205 23.8121C26.1437 23.8765 26.0854 23.9439 26.0303 24.0116C25.9719 24.0822 25.9137 24.1528 25.8616 24.2235C25.8132 24.2756 25.7709 24.3308 25.7287 24.3862C25.7222 24.3923 25.7221 24.3955 25.7189 24.4017C25.5206 24.6693 25.3438 24.947 25.1823 25.2408C25.1328 25.3367 25.08 25.4356 25.0333 25.538C24.8875 25.8353 24.7663 26.149 24.6636 26.4788C24.3719 27.4309 24.282 28.4575 24.4364 29.5062C24.4419 29.5377 24.4473 29.5691 24.4527 29.6038C24.4581 29.6354 24.4636 29.6668 24.4691 29.6953C24.5123 29.9566 24.5717 30.2092 24.6467 30.4558C24.6924 30.6264 24.7507 30.7943 24.8153 30.9622C25.0586 31.6117 25.3973 32.2104 25.8162 32.742C25.8141 32.7441 25.8151 32.7462 25.8192 32.7484C26.1286 33.1486 26.4798 33.5124 26.8666 33.83C26.8911 33.8526 26.9219 33.8753 26.9495 33.8981C26.9678 33.9174 26.9895 33.9304 27.0173 33.9469C28.4772 35.0895 30.3886 35.648 32.3669 35.3299C32.7607 35.265 33.146 35.1717 33.5164 35.0402L33.5195 35.0403C33.5354 35.0344 33.548 35.0317 33.5638 35.0258C34.7131 34.623 35.707 33.931 36.4735 33.0508C37.7738 31.5574 38.4266 29.52 38.0869 27.4156ZM36.4011 30.5583C36.2161 31.0333 35.9596 31.4783 35.6479 31.881C35.5538 32.0009 35.4566 32.1175 35.3533 32.2277C35.1305 32.4729 34.8797 32.7046 34.6139 32.9077C33.9128 33.4477 33.0747 33.824 32.1424 33.9689C31.4777 34.0769 30.8162 34.0625 30.1828 33.933C30.0142 33.9003 29.8398 33.8581 29.6748 33.8067C29.3387 33.7037 29.0159 33.5729 28.7127 33.4144C27.7071 32.886 26.8726 32.0547 26.3445 31.0243C26.3237 30.983 26.3027 30.9417 26.2851 30.9005C26.282 30.8972 26.2821 30.891 26.2792 30.8878C26.1808 30.6907 26.0984 30.4814 26.0256 30.2663C26.0141 30.2346 26.0023 30.2029 25.9937 30.1682C25.9879 30.1523 25.9852 30.1366 25.9794 30.1208C25.9073 29.8804 25.8509 29.6374 25.8073 29.3854C25.5973 28.0686 25.8759 26.7804 26.5033 25.7053C26.546 25.6343 26.5951 25.5602 26.6345 25.4891C26.7257 25.363 26.8137 25.2397 26.9077 25.123C26.9567 25.0491 27.0116 24.9878 27.0666 24.9234L27.0699 24.9234C27.1281 24.8528 27.1894 24.7886 27.2507 24.7211C27.6995 24.2373 28.2403 23.8312 28.8472 23.5398C29.0283 23.4505 29.2155 23.3708 29.4086 23.3037C29.7221 23.1895 30.0506 23.1041 30.3908 23.0534C30.7373 22.9967 31.083 22.9745 31.4216 22.9834L31.572 22.9874C31.622 22.9918 31.6722 22.9931 31.7224 22.9944C34.0761 23.1944 36.105 24.8884 36.6481 27.2585C36.6763 27.3784 36.701 27.5108 36.7259 27.6369C36.8903 28.6577 36.7604 29.6642 36.4011 30.5583Z" fill="#001F16" />
                            </svg>
                            <span style={{ fontFamily: "intermedium", fontSize: "14px" }}> No data found</span>
                        </div>
                    </div>
            </>:
          <div className='searchoverallbody' style={{"cursor":"pointer"}}>
              {liveCampaign?.length>0 &&
              <div className='sectionn'>
                  <h4>Live Campaigns</h4>
                  {liveCampaign.map((item :any,ind :any)=> <p  onClick={()=>handleRoute(item,ind,"live")}><span><img src={chatMsg} alt=""/></span>{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName?? "New Campaign"}</p>)}
                  
              </div>}
              {futureCampaign?.length>0 &&

              <div className='sectionn'>
                  <h4>Scheduled Campaigns</h4>
                   {futureCampaign.map((item :any,ind :any)=> <p    onClick={()=>handleRoute(item,ind,"schedule")}><span><img src={chatMsg} alt=""  /></span>{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName??"New Campaign"}</p>)}
                  
              </div>
              }
              {allDraftData?.length>0 &&

              <div className='sectionn'>
                  <h4>Drafts </h4>
                   {allDraftData.map((item :any,ind :any)=> <p  onClick={()=>handleRoute(item,ind,"draft")}><span><img src={chatMsg} alt=""  /></span>{item?.conversationName??"New Campaign"}</p>)}
                  
              </div>
              }
          </div>
          }
        </WorktualModal>
      </div>
  )
}

export default Search