import React, { useCallback, useEffect, useRef, useState } from 'react';
import styles from './scss/importcsv.module.scss';
import { tableOptions } from '../../../components/custom_components/tableOptions';
import BootstrapTable from 'react-bootstrap-table-next';
import paginationFactory from 'react-bootstrap-table2-paginator';
import BlurToastDemo from '../../../components/custom_components/blurtast/blurtoast';
import WorktualModal from '../../../components/custom_components/worktualModal';
import { assets } from '../../../styles/assets/assets';
import {  whatsappService } from '../../../base/service/apiservice';
import { useNavigate } from 'react-router-dom';
import moment from 'moment';
import exportFromJSON from 'export-from-json';
import ContentLayout from '../../../layout/contentLayout';
import NoTableData from '../../../components/custom_components/notabledata/notabledata';

const ImportCsv = () => {
    const navigate = useNavigate();
    const [toastOpen, setToastOpen] = useState(false);
      const [showInstructions, setShowInstructions] = useState(true);
    const [showCloud,setShowCloud]= useState<boolean>(false);

    // bulk import
    // const inputRef = useRef<HTMLInputElement>(null);
    const [isDragging, setIsDragging] = useState(false);
    const[fileName,setFileName]=useState<any>("");
    const [totalData, setTotalData] = useState<any>(0);
    const [notValids, setNotValid] = useState<any>(null);
    const [partialError,setPartialError] = useState<any>(false);
    console.log("notvalid",notValids);
    const[noDataFound,setNoDataFound]=useState<boolean>(false);
    const [validDataLength,setValidDataLength]=useState<number>(0);
    const [filePath,setFilePath]= useState<any>(null);
    const initialErrorData =[{
      listName:'',
      totalRecords:0,
      errorRecords:0
    }]
    const [errorData,setErrorData] = useState<any>(initialErrorData);

    const mapData = (data: any) => {
        const finalMapedData = [] as any
        data?.forEach((eachData: any) => {
            const each = eachData.split(/,(?=(?:(?:[^"]*"){2})*[^"]*$)/);
            const output = each?.map((part: any) => {
                if (part.startsWith('"') && part.endsWith('"')) {
                    return (part.slice(1, -1)).replace('\r', '').split(',')
                } else {
                    return part.replace('\r', '');
                }
            });


            if (output?.some((d: any) => d.length > 0)) {
                finalMapedData.push({
                    firstName: output[0] ? output[0] || '' : '',
                    lastName: output[1] ? output[1] || '' : '',
                    phoneNumber: output[2] !== '' ? (Array.isArray(output[2]) ? output[2] : [output[2]]) : [],
                    email: output[3] !== '' ? (Array.isArray(output[3]) ? output[3] : [output[3]]) : [],
                    location: output[4] || "",
                    address: output[5]?.toString() || "",
                    country: output[6] || "",
                    state: output[7] || "",
                    city: output[8] || "",
                    postalCode: output[9] || "",
                    facebook: output[10] || "",
                    instagram: output[11] || "",
                    whatsapp: output[12] !== undefined ? (output[12]) : '',
                    customerType: output[13] || "",
                    tags: output[14] || "",
                    source: output[15] || '',
                    deviceId:output[16]||'',
                    deviceOS:output[17]||''
                    // sipLoginId :output[17]||'',
                    // ext:output[18]||""
                })
            }
        })
        return finalMapedData
    };

     const validateImportData = (data: any) => {
        if (data) {
            const namePattern = /^[a-zA-Z\s]+$/
            const emailPattern = /^[a-zA-Z0-9_.-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
            const phonePattern = /^\+?\d{10,18}$/
            const firstName = data.firstName?.length ? data.firstName : ''
            const lastName = data.lastName?.length ? data.lastName : ''
            // const customerType = Array.isArray(data?.customerType)
            const whatsappNumber =data.whatsapp?.length?data.whatsapp:''

            return (
                namePattern.test(firstName)
                // && (namePattern.test(lastName) || lastName?.trim()==='')
                && (data?.email?.length > 0 ? (data?.email?.every((email: any) => emailPattern.test(email?.trim()))) : false
                && data?.phoneNumber?.length > 0 ? data?.phoneNumber?.every((num: any) => phonePattern.test(num)) : true)
                // && !customerType 
                && (!whatsappNumber?.trim() || phonePattern.test(whatsappNumber))
            )
        }
    }

     const mapForImport = (data: any) => {
        const importData :any= [];
        data?.forEach((d: any) => {
        try {
            const tags = d.tags ? d.tags.split(',') : []
            importData.push(
            {
                firstName: d.firstName,
                lastName: d.lastName,
                phoneNumber: d?.phoneNumber?.slice(1),
                email: d?.email?.slice(1),
                primaryPhoneNumber: d?.phoneNumber?.[0],
                primaryEmail: d?.email?.[0],
                tags: tags,
                customerType: d.customerType,
                location: d.location,
                address: d.address,
                country: d.country,
                state: d.state,
                city: d.city,
                postalCode: d.postalCode,
                facebook: d.facebook,
                instagram: d.instagram,
                whatsapp: d.whatsapp,
                source: 'Import',
                deviceId: d.deviceId,
                deviceOS: d.deviceOS
                // sipLoginId: d.sipLoginId,
                // ext: d.ext
            }
            )
        } catch (error) {
            console.log(error)
        }
        })
        return importData
    }

    const mapApiDataForExport = (datas: any) => {
        const dataArray = datas?.map((data: any) => {
            return ({
                FirstName: data?.firstName || '',
                LastName: data?.lastName || '',
                phoneNumber: Array.isArray(data?.phoneNumber) ? data?.phoneNumber?.join(',') : '',
                email: Array.isArray(data?.email) ? data?.email?.join(',') : '',
                Location: data?.location || '',
                Address: data?.address || '',
                Country: data?.country || '',
                State: data?.state || '',
                City: data?.city || '',
                PostalCode: data?.postalCode || '',
                Facebook: data?.facebook || '',
                Instagram: data?.instagram || '',
                WhatsApp: data?.whatsapp && data?.whatsapp !== 'undefined' ? data?.whatsapp : '',
                tags: Array.isArray(data?.tags) ? data?.tags?.join(',') : '',
                CustomerType: data?.customerType || '',
                Source: data?.source,
                DeviceId:data?.deviceId,
                deviceOS: data?.deviceOS
                // SipLoginId:data?.siploginId,
                // Ext:data?.ext
            })
        })
        return dataArray;
    };

        const handleResetImport=()=>{
      const delayClose=()=>{
          // setShowFail(false);
          // setShow1(true);
          setNoDataFound(false)
          setNotValid(null);
          setTotalData(0);
          // setShowInstruction(false);
          // setShowActionModal(false);
          // setUploadStatus(1);
      }
      delayClose();
        
    }

    const handleInsertFile=(fileName :any,filePath :any,status :any,records :any)=>{
      try {
            let body :any={
              fileName: fileName,
              filePath: filePath,
              status: status,
              records: records,
            }
            console.log("body????",body);
            whatsappService.post(body, "/insert_bulk_file_history").then((response: any) => {                  
                      console.log("res",response);
                  }).catch((error) => {                   
                    console.log(error);
                  })

      } catch (error) {
        console.log("err",error);
      }
         
    }

    const handleNFSUpload=async (file :any)=>{
      try{
      console.log("file??????nfs",file);
      let formData = new FormData();
      formData.append("doc", file[0]);
    //   let res :any= CoreServices.post(formData, "/upload_article")
    //         .then((resp: any) => {              
    //            return resp?.data?.insertUpdateArticleResult?.[0]?.articleUrl;              
    //         })
    //         .catch((err: any) => { });
    //   return res;
    // }

      let res: any = whatsappService.post(formData, '/upload_contact_file').then((resp: any) => {
        console.log("respppp", resp?.data?.fileList?.[0]?.url)
        return resp?.data?.fileList?.[0]?.url
      }).catch((err: any) => {
        console.log("err",err);
      });
      return res;
    }catch(error :any){
      console.log("API ERROR:",error)
    }
    }

    const handleCSVUpload = (file: File,filePath :any) => {
          console.log("setFileName:", file);
          setFileName({ name: file.name, size: file.size })
          // You can read the file content using FileReader if needed
          const reader = new FileReader();
          reader.onload = async(event :any) => {
            // console.log("cones here1")
              const notValid :any= [];
              const text = event?.target?.result?.split("\n");
           
              const newTextArray =text?.splice(1);
              
              setTotalData(newTextArray?.length?newTextArray?.length-1:0);
              
              const mappedData = mapData(newTextArray);
              const validData = mappedData.filter((data: any) => {
                  if (validateImportData(data)) {
                      return true
                  }
                  else {
                      notValid.push(data)
                      return false
                  }               
                  })
              const validDataForImport = mapForImport(validData);
              const exportNotValid = mapApiDataForExport(notValid);
              if (exportNotValid.length > 0) {
                 console.log("here comes -1 ???")
                setPartialError(true);
                setErrorData([{errorRecords:notValid?.length,listName:file.name,totalRecords:newTextArray?.length-1}]);
                setNotValid({
                  file: exportNotValid,
                  fulllength: mappedData.length,
                  validLen: validData.length
                })
              }

              if (validDataForImport.length === 0) {
                  console.log("here comes 0 ???")
                  // setShow1(true);
                  setNoDataFound(true)
                  // toast.error("No valid data found");             
                  // setUploadStatus(2);    
                  return
              }
              else {
                console.log("here comes 1???")
                setValidDataLength(validDataForImport?.length??0);
              
                // setUploadStatus(3);
                setNoDataFound(false);
                let lenDiv = validDataForImport.length / 1000
                if (lenDiv < 0) {
                  lenDiv = 1;
                }
                let error = false;
                let failed =0;
                const promises =[];
                for (let i = 0; i < lenDiv; i++) {
                  const data = validDataForImport.splice(0, 1000)
                  let promise = whatsappService.post(data, "/insert_bulk_contact").then((response: any) => {        
                    console.log("errressss",response)          ;
                      if(response?.data?.failure){
                          failed += response?.data?.failure;
                      }                     
                  }
                  ).catch((error) => {
                    error = true
                    console.log(error)
                  })
                  promises.push(promise);
                }

                  Promise.all(promises).then(() => {     
                        let status :any='Imported';
                      if (error) {
                          // toast.error("Try later");

                          handleResetImport();
                      }
                      else if (failed) {                     
                          // setShowFail(true);
                          // setContactExists(failed);
                          setShowCloud(false);
                          setToastOpen(true);                         
                          status="Partial-Imported";
                      }
                      else {
                          // setShow1(true);
                          // toast.success("Uploaded successfully");
                          setToastOpen(true);
                          
                          if (!(totalData - validDataLength)) {
                              // setUploadStatus(3);
                              // handleResetImport();  
                               status="Partial-Imported";
                                 
                          }

                            
                }
                   handleInsertFile(file.name,filePath,status,newTextArray?.length?newTextArray?.length-1:0,);       
                  // onContactAdded(); need to put fetch data function
                  });
              
              
              }
          };        
          
          // You can parse CSV here if needed
          reader.onerror = (error :any) => {
          console.error("Error reading file:", error);
          };

          reader.readAsText(file); // Read file as plain text
          
          
      };


    // const handleFiles = useCallback((files: FileList | null) => {
    //   if (!files || files.length === 0) return;

    //   const file = files[0];
    //   const isCSV = file.type === 'text/csv' || file.name.endsWith('.csv');
    //   if (isCSV) {
    //     handleCSVUpload(file);
    //   } else {
    //     alert('Please upload a valid CSV file.');
    //   }
    // }, [handleCSVUpload]);

      const handleFiles = async(files: FileList | null) => {
      if (!files || files.length === 0) return;

      const file = files[0];

      const isCSV = file.type === 'text/csv' || file.name.endsWith('.csv');
      if (isCSV) {
        let data: any = [];
        data.push(file)
        const filePath = await handleNFSUpload(data);       
       
        await handleCSVUpload(file,filePath);
      } else {
        alert('Please upload a valid CSV file.');
      }
    }

    const handleDragOver = (e: React.DragEvent) => {
      e.preventDefault();
      setIsDragging(true);
    };

    const handleDragLeave = () => {
      setIsDragging(false);
    };

    const handleDrop = (e: React.DragEvent) => {
      e.preventDefault();
      setIsDragging(false);
      handleFiles(e.dataTransfer.files);
    };

    const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
      handleFiles(e.target.files);
    };

    // const handleClick = () => {
    //   inputRef.current?.click();
    // };
    const downloadSampleCsv2 = async (url: any, name: any) => {
      let csvContent = "data:text/csv;charset=utf-8," + url?.map((e: any, index: any) => Object.values(e).join(",")).join("\n");
      var encodedUri = encodeURI(csvContent);
      var link = document.createElement("a");
      link.setAttribute("href", encodedUri);
      link.setAttribute("download", "my_data.csv");
      document.body.appendChild(link);
      link.click();
    };
    
    const downloadSampleCsv = () => {
      const data = ["First Name","Last Name","Phone Number","Email Id","Address","Country","State","City","Postal Code","WhatsApp","Source", "DeviceId","deviceOS"];
      const apidatas = [data];
      downloadSampleCsv2(apidatas, "sample");
    };

    const downloadCSV = async (url: any, name: any) => {
      try {

        const data = url
        const fileName = name.split('.')[0];
        const exportType = exportFromJSON.types.csv;
        exportFromJSON({ data, fileName, exportType })

      } catch (error) {
        console.log(error)
      }
    };

    const handleGetFileHistory=()=>{
      try {
        let body: any = {
          limit: 5,
          offset: 0
        }
        
        whatsappService.post(body, "/get_bulk_file_history").then((response: any) => {                  
                  console.log("res",response);
                  if(response?.data?.message=="File History fetched Successfully" && response?.data?.getFileHist?.[0]?.length>0){
                     setData(response?.data?.getFileHist?.[0]);
                  }
                 
              }).catch((error) => {                   
                console.log(error);
              })

      } catch (error) {
        console.log("err",error);
      }
    }


    //bulk import

    const columns = [
        {
            dataField: "fileName",
            text: "File Name",
            sort: true,
            formatter: (item: any,record :any) => {             
                return (<a  href={record?.filePath} download={record?.fileName ?? "download"}><span className='clickabletxt' >{item}</span></a>)
            }
        },
        {
            dataField: "createdOn",
            text: "Uploaded On",
            sort: true,
            formatter: (item: string) => {
                return <span >{moment(item).format("YYYY-MM-DD")}</span>
            }
        },
        {
            dataField: "status",
            text: "Status",
            formatter: (item: string) => {
                return <span className={`${styles.statuselem} ${styles.processed}`}>{item}</span>
            }
        },
        {
            dataField: "records",
            text: "Records",
            sort: true,
        },
    ];
  const columnsinner = [
        {
            dataField: "listName",
            text: "List Name",
            sort: true,
            formatter: (item: any) => {
                return <span className='clickabletxt'>{item}</span>
            }
        },
        {
            dataField: "totalRecords",
            text: "Total Records",
             formatter: (item: any) => {
                return <span >{item}</span>
            }
        },
        {
            dataField: "errorRecords",
            text: "Error records",
            formatter: (item: any) => {
                return <span className='errortxt'>{item}</span>
            }
        },
       
    ];
    const [data,setData] = useState<any>([]);
    console.log("data????",data);

    const tableOpt = {
        ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
            text: "All",
            value: 20,
        }]
    }

useEffect(()=>{
    handleGetFileHistory();
},[])


    return (
      <ContentLayout noPadding={true}>
        <div className={` ${styles.importcsvsec} importcsvsec`}>
            <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Import .csv files</h2>
                    <p className={styles.headerdesctxt}>Import customer or transaction data using the supported file format.</p>
                </div>
            </div>
            <div className={styles.subtopheading}>
                <h4>Upload your files</h4>
                <p className={styles.subtopdesctxt}>Before uploading, ensure your file matches the required format and contains valid data.</p>
            </div>
            <div className={styles.optionsbtns}>
                <button className='primarybtn' onClick={()=>setShowCloud(true)}>Upload</button>
             
                    {/* Toast */}
                    <BlurToastDemo open={toastOpen}  
                    message={
          <div style={{ display: "flex", alignItems: "center", gap: "8px" }}>
            <svg
              width="17"
              height="16"
              viewBox="0 0 17 16"
              fill="none"
              xmlns="http://www.w3.org/2000/svg"
            >
              <g clipPath="url(#clip0_6088_60288)">
                <path
                  d="M8.50065 1.33203C4.81872 1.33203 1.83398 4.31676 1.83398 7.9987C1.83398 11.6806 4.81878 14.6654 8.50065 14.6654C12.1825 14.6654 15.1673 11.6805 15.1673 7.9987C15.1673 4.3169 12.1825 1.33203 8.50065 1.33203ZM11.6865 6.14276L8.66852 10.0731C8.40325 10.4188 8.00152 10.631 7.56632 10.6564C7.53732 10.6584 7.50805 10.659 7.47905 10.659C7.07445 10.659 6.68638 10.4963 6.40258 10.204L5.35278 9.12256C5.16038 8.92463 5.16498 8.60756 5.36318 8.41556C5.56138 8.22356 5.87818 8.2287 6.07018 8.42596L7.11998 9.50736C7.22285 9.61283 7.35438 9.6695 7.50832 9.65843C7.65545 9.64996 7.78565 9.58096 7.87552 9.46376L10.8935 5.53343C11.0615 5.3147 11.3765 5.2737 11.5943 5.44163C11.8134 5.60963 11.8547 5.92336 11.6865 6.14276Z"
                  fill="#00E2A4"
                />
              </g>
              <defs>
                <clipPath id="clip0_6088_60288">
                  <rect width="16" height="16" fill="white" transform="translate(0.5)" />
                </clipPath>
              </defs>
            </svg>
            <span>Uploads done successfully</span>
            <svg style={{cursor:"pointer"}} onClick={()=>{setToastOpen(!toastOpen)}} xmlns="http://www.w3.org/2000/svg" width="11" height="10" viewBox="0 0 11 10" fill="none">
  <path d="M6.29984 4.99937L10.2032 1.09571C10.4245 0.874973 10.4245 0.51564 10.2032 0.294906C9.98184 0.0735729 9.62244 0.0735729 9.40237 0.294906L5.49904 4.19851L1.59571 0.294906C1.37564 0.0735729 1.01497 0.0735729 0.794906 0.294906C0.573573 0.51564 0.573573 0.874973 0.794906 1.09571L4.69824 4.99937L0.794906 8.90304C0.573573 9.12377 0.573573 9.48311 0.794906 9.70384C0.905573 9.81451 1.05011 9.86984 1.19531 9.86984C1.34051 9.86984 1.48564 9.81451 1.59571 9.70384L5.49904 5.80024L9.40237 9.70384C9.51237 9.81451 9.65757 9.86984 9.80277 9.86984C9.94797 9.86984 10.0925 9.81451 10.2032 9.70384C10.4245 9.48311 10.4245 9.12377 10.2032 8.90304L6.29984 4.99937Z" fill="white"/>
</svg>
          </div>
        } />
               
                <button className='cancelbtn' onClick={downloadSampleCsv}>Download Template</button>
            </div>
            <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Recent Uploads</h2>
                    <p className={styles.headerdesctxt}>View the latest files you’ve imported, along with their upload date, status, and record count.</p>
                </div>
                <div className={styles.tophdrights}>
                    <button className='cancelbtn' onClick={()=>navigate('/importHistory')}>Import History</button>
                </div>
            </div>
            {/* <BlurToastDemo
        trigger={<button style={{ padding: "8px 16px", borderRadius: "8px" }}>Click Me</button>}
      /> */}
      <div className='table-wrapper '>
<div className='desktoptable'>
                <BootstrapTable
                    keyField="fileId"
                    data={data}
                    columns={columns}
                    // pagination={paginationFactory(tableOpt)}
                noDataIndication={<NoTableData  />}
                />
           
            </div>
      </div>
            

                 {/* // cloud modal */}
                   <WorktualModal
                      show={showCloud}
                      mdsize='modalsm'
                      title='Instruction for data Upload'
                      onClose={()=>setShowCloud(false)}
                    >
        <>
         <div className='uploadsec'  onDragOver={handleDragOver}  onDragLeave={handleDragLeave}  onDrop={handleDrop} 
        //  onClick={handleClick}
         >
              <input type="file" name="" id=""                                    
                    // ref={inputRef}
                    accept=".csv"
                    onChange={handleFileChange}
              />
              <img className='upimg' src={assets.uploadcloud_icon} alt="uploadcloud_icon" />
              <div className='upinfosec'>
                <p className='upfiletxt'><span className='clickabletxt'>Click to Upload</span> or Drag and drop here</p>
                <p className='updesc'>JPEG or PNG format, up to 5 MB. <br /> Recommended size: 192×192 px for best display.</p>
              </div>
        </div>
        <div className={styles.instructions}>
          <div
            className={styles.instructionsheader}
            onClick={() => setShowInstructions(!showInstructions)}
          >
            <span>
              {showInstructions ? (
                <svg
                  xmlns="http://www.w3.org/2000/svg"
                  width="16"
                  height="9"
                  viewBox="0 0 16 9"
                  fill="none"
                >
                  <path d="M15.0821 8.7111C14.8526 8.7111 14.6231 8.6232 14.4473 8.4494L8.8955 2.9318C8.4014 2.4396 7.5976 2.4396 7.1035 2.9308L1.5527 8.4464C1.2002 8.796 0.629795 8.7941 0.279295 8.4425C-0.0713045 8.089 -0.0693049 7.5196 0.283195 7.1691L5.835 1.6535C7.0313 0.466004 8.9727 0.469904 10.1651 1.6545L15.7169 7.1721C16.0694 7.5227 16.0704 8.093 15.7208 8.4455C15.545 8.6223 15.3136 8.7111 15.0821 8.7111Z" fill="black"/>
                </svg>
              ) : (
                <svg
                  style={{ transform: "rotate(180deg)" }}
                  xmlns="http://www.w3.org/2000/svg"
                  width="16"
                  height="9"
                  viewBox="0 0 16 9"
                  fill="none"
                >
                  <path d="M15.0821 8.7111C14.8526 8.7111 14.6231 8.6232 14.4473 8.4494L8.8955 2.9318C8.4014 2.4396 7.5976 2.4396 7.1035 2.9308L1.5527 8.4464C1.2002 8.796 0.629795 8.7941 0.279295 8.4425C-0.0713045 8.089 -0.0693049 7.5196 0.283195 7.1691L5.835 1.6535C7.0313 0.466004 8.9727 0.469904 10.1651 1.6545L15.7169 7.1721C16.0694 7.5227 16.0704 8.093 15.7208 8.4455C15.545 8.6223 15.3136 8.7111 15.0821 8.7111Z" fill="black"/>
                </svg>
              )}
            </span>
            <p className={styles.bold}>
              Here are some important things to keep in mind when upload files
            </p>
          </div>

          <div
            className={`${styles.instructionsContent} ${
              showInstructions ? styles.open : ""
            }`}
          >
            <ul>
              <li>Verify that the provided email address is in the correct format.</li>
              <li>Check that the provided phone number matches the specified country code.</li>
              <li>Numbers are not allowed in the first name.</li>
              <li>
                Phone number does not contain invalid characters (letters or special
                symbols).
              </li>
              <li>
                The First Name field should not include special characters (e.g., @, #,
                $, %).
              </li>
              <li>The maximum character limit for the Name field is 100.</li>
            </ul>
          </div>
        </div>

        </>
      
      </WorktualModal>

                     {/* //table modal */}
      <WorktualModal
        show={partialError}
        mdsize='modalsm'
        title='Imported data list'
        onClose={()=> setPartialError(false)}
       
      >
        <>
      <BootstrapTable
                    keyField="fileId"
                    data={errorData}
                    columns={columnsinner}
                
                // noDataIndication={<NoTableData />}
                />
                <div style={{marginTop:"18px"}}>
   <button className='cancelbtn' onClick={()=>downloadCSV(notValids?.file,fileName?.name)}> <span style={{marginRight:"8px"}}><svg xmlns="http://www.w3.org/2000/svg" width="14" height="18" viewBox="0 0 14 18" fill="none">
  <path d="M5.39361 13.8398C5.82081 14.2763 6.39161 14.5166 7.00051 14.5166C7.60991 14.5166 8.18071 14.2764 8.60791 13.8398L12.8647 9.49408C13.1547 9.19818 13.1499 8.72358 12.854 8.43358C12.5581 8.14448 12.0835 8.14838 11.7935 8.44528L7.75681 12.5661L7.75051 1.23138C7.75051 0.817285 7.41461 0.481384 7.00051 0.481384C6.58641 0.481384 6.25051 0.817285 6.25051 1.23138L6.25681 12.5786L2.20751 8.44528C1.91751 8.14938 1.44291 8.14548 1.14701 8.43358C0.851114 8.72358 0.846214 9.19818 1.13631 9.49408L5.39361 13.8398Z" fill="#404040"/>
  <path d="M12.3292 16.0127L1.67151 16.0186C1.25741 16.0186 0.922014 16.3545 0.922014 16.7686C0.922014 17.1827 1.25791 17.5186 1.67201 17.5186L12.3297 17.5127C12.7438 17.5127 13.0792 17.1768 13.0792 16.7627C13.0792 16.3486 12.7433 16.0127 12.3292 16.0127Z" fill="#404040"/>
</svg></span>Download list of record with errors</button>
                </div>
             

        </>
      
      </WorktualModal>
        </div>
        
        </ContentLayout>
    )
}

export default ImportCsv
