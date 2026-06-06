import React, { useEffect, useState } from 'react'
import styles from './scss/importcsv.module.scss';
import { tableOptions } from '../../../components/custom_components/tableOptions';
import BootstrapTable from 'react-bootstrap-table-next';
import paginationFactory from 'react-bootstrap-table2-paginator';
import BlurToastDemo from '../../../components/custom_components/blurtast/blurtoast';
import WorktualModal from '../../../components/custom_components/worktualModal';
import { assets } from '../../../styles/assets/assets';
import { Link } from 'react-router-dom';
import { whatsappService } from '../../../base/service/apiservice';
import moment from 'moment';
import ContentLayout from '../../../layout/contentLayout';
function ImportHistory() {
     const columns = [
        {
            dataField: "fileName",
            text: "File Name",
            sort: true,
            formatter: (item: string,record :any) => {
                // return <span className='clickabletxt'>{item}</span>
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
        const [data,setData] = useState<any>([]);
    
        const tableOpt = {
            ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
                text: "All",
                value: 20,
            }]
        }

        const handleGetFileHistory=()=>{
              try {
                let body: any = {
                  limit: 100,
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
            
        useEffect(()=>{
            handleGetFileHistory();
        },[])

  return (
    <ContentLayout noPadding={true}>
        <>
          <div className={` ${styles.importcsvsec} importcsvsec`}>
              <div className='breadcrumbs'>
                        <ol>
                            <li>
                                <Link to=''>Import .csv files</Link>
                            </li>
                            <li>Import History</li>
                        </ol>
                    </div>
            <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Import History</h2>
                    <p className={styles.headerdesctxt}>View the latest files you’ve imported, along with their upload date, status, and record count.</p>
                </div>
                
            </div>
          <div className='table-wrapper'>
  <div className='desktoptable'>
                <BootstrapTable
                    keyField="fileId"
                    data={data}
                    columns={columns}
                    pagination={paginationFactory(tableOpt)}
                // noDataIndication={<NoTableData />}
                />
           

             
            </div>
          </div>
          
        </div>
        </>
        </ContentLayout>
  )
}

export default ImportHistory;