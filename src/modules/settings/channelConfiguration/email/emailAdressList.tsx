import React, { useContext, useEffect, useState } from 'react';
import paginationFactory from 'react-bootstrap-table2-paginator';
import BootstrapTable from 'react-bootstrap-table-next';
import { tableOptions } from '../../../../components/custom_components/tableOptions';
import { WorktualContext } from '.';
import { whatsappService } from '../../../../base/service/apiservice';
import NoTableData from '../../../../components/custom_components/notabledata/notabledata';
import { assets } from '../../../../styles/assets/assets';
import { useDispatch, useSelector } from 'react-redux';
import { commonDataSliceActions } from '../../../../store/slice/commonSlice/commonSlice';
const EmailAdressList = () => {
  const dispatch = useDispatch();
  const [columnData,setColumnData]=useState<any>([]);
  const emailConfigRefresh =useSelector((state :any)=>state.commonData.emailConfigRefresh);
  console.log("emailConfig",columnData)
  const columns = [
    {
      dataField: "value",
      text: "Value",
      sort: true,
      // formatter: (item: string) => {
      //   return <span className='clickabletxt' >{item?.emailId}</span>
      // }
    },
    {
      dataField: "alias_name",
      text: "Alias name",
      sort: true,
    }
  ];

  const data = [
    // { value: 'support@worktual.com', alias_name: 'Support' },
    ...columnData
  ]

  const tableOpt = {
    ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
      text: "All",
      value: 20,
    }]
  }

  const fetchMailConfig=()=>{
      try {
            // whatsappService.get('/ec_get_smtp_config_dtl').then((res: any) => {
            whatsappService.get('/get_worktual_address').then((res: any) => {
              console.log('smtp',res)
              if(res?.data?.statusCode==200 && res?.data?.listAndFilterData.length>0){
                let smtpData :any= {
                  value:res?.data?.listAndFilterData?.[0]?.emailId,
                  alias_name:res?.data?.listAndFilterData?.[0]?.aliasName
                }
                console.log("smtoData",smtpData);
                setColumnData([smtpData]);
              }else{
                setColumnData([]);
              }
            })
          } catch (error) {
            console.log("error", error);
          }
    }

  useEffect(()=>{
    fetchMailConfig();
  },[])

  useEffect(() => {
    if(emailConfigRefresh){

      fetchMailConfig();
      dispatch(commonDataSliceActions.setEmailConfigRefresh(false));
    }
  }, [emailConfigRefresh])
  
  return (
    <div className='table-wrapper'>
      <div className='desktoptable desktoptablescroll'>
        {columnData.length>0 ?
        <BootstrapTable
          keyField="value"
          data={data}
          columns={columns}
          pagination={paginationFactory(tableOpt)}
          // noDataIndication={
          // <NoTableData />
           
        // }
        />
        :
        <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
        }
      </div>
    </div>
  )
}

export default EmailAdressList
