import React, { useEffect, useState, useRef, useLayoutEffect } from 'react'
import { GetItemFromStorage } from '../../base/customhooks/useStorage';
import { LocalStorage } from '../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import styles from "./scss/reports.module.scss"
import { assets } from '../../styles/assets/assets'
import { useSelector } from 'react-redux';
import { useDispatcher } from '../../store/redux-store/dispatch';
import SocketService from '../../base/service/socketService';
import { reportSliceActions } from '../../store/slice/report';
import { v4 as uuidv4 } from "uuid";
import { config } from '../../base/constant';
import axios from 'axios';
import RightDrawer from '../../components/custom_components/drawer/drawer';
import WorktualDropdown from '../../components/custom_components/WroktualDropdown';
import WorktualSearch from '../../components/custom_components/woktualSearch';
import BootstrapTable from 'react-bootstrap-table-next';
import WorktualSelect from '../../components/custom_components/WorktualSelect';
import { tableOptions } from '../../components/custom_components/tableOptions';
import { set } from 'react-datepicker/dist/date_utils';
import { whatsappService } from '../../base/service/apiservice';
import moment from 'moment';
import paginationFactory from 'react-bootstrap-table2-paginator';
import {countryTimezones} from '../../base/countryConstants';
import CustomRangePicker from '../../components/custom_components/datepickercustom/customDatepicker';
import { CSVLink } from "react-csv";
import BlurToastDemo from '../../components/custom_components/blurtast/blurtoast';
import WorktualSearchableSelect from '../../components/custom_components/WorktualSearchableSelect';
import { jsPDF } from 'jspdf';
import Arrowlist from '../../styles/images/others/roundarrow.svg'
const Arrowright = () => (
    <img src={Arrowlist} alt="Arrowlist" />
);
function Reports() {
  const dispatch = useDispatcher();
  const reportData = useSelector((state: any) => state.reportData);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const [validation,setValidation] = useState<any>(false);

  const [createSchedule,setCreateSchedule] = useState<any>(false);
  const [getReccurrenceData,setGetReccurenceData] = useState<any>([]);
console.log("valid",validation)

  const [toastOpen,setToastOpen] = useState<any>(false);
  const [toastMessage,setToastMessage] = useState<any>("");
  const [showOptions,setShowOptions] =useState<any>(false);
  console.log("open,msg",toastMessage,toastOpen)
  const[firstTime,setFirstTime]=useState<any>(true);

  // regex

  const dateRegex: any = /^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$/;
  const emailRegex :any= /^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,63}$/;

  const hours :any=[
     { label: '00', value: 0 },
     { label: '01', value: 1 },
     { label: '02', value: 2 },
     { label: '03', value: 3 },
     { label: '04', value: 4 },
     { label: '05', value: 5 },
     { label: '06', value: 6 },
     { label: '07', value: 7 },
     { label: '08', value: 8 },
     { label: '09', value: 9 },
     { label: '10', value: 10 },
     { label: '11', value: 11 },
     { label: '12', value: 12 },
     { label: '13', value: 13 },
     { label: '14', value: 14 },
     { label: '15', value: 15 },
     { label: '16', value: 16 },
     { label: '17', value: 17 },
     { label: '18', value: 18 },
     { label: '19', value: 19 },
     { label: '20', value: 20 },
     { label: '21', value: 21 },
     { label: '22', value: 22 },
     { label: '23', value: 23 },
    
  ]

  const mins :any =[
     { label: '00', value: 0 },
     { label: '01', value: 1 },
     { label: '02', value: 2 },
     { label: '03', value: 3 },
     { label: '04', value: 4 },
     { label: '05', value: 5 },
     { label: '06', value: 6 },
     { label: '07', value: 7 },
     { label: '08', value: 8 },
     { label: '09', value: 9 },
     { label: '10', value: 10 },
     { label: '11', value: 11 },
     { label: '12', value: 12 },
     { label: '13', value: 13 },
     { label: '14', value: 14 },
     { label: '15', value: 15 },
     { label: '16', value: 16 },
     { label: '17', value: 17 },
     { label: '18', value: 18 },
     { label: '19', value: 19 },
     { label: '20', value: 20 },
     { label: '21', value: 21 },
     { label: '22', value: 22 },
     { label: '23', value: 23 },

      { label: '24', value: 24 },
     { label: '25', value: 25 },
     { label: '26', value: 26 },
     { label: '27', value: 27 },
     { label: '28', value: 28 },
     { label: '29', value: 29 },
     { label: '30', value: 30 },
     { label: '31', value: 31 },
     { label: '32', value: 32 },
     { label: '33', value: 33 },
     { label: '34', value: 34 },
     { label: '35', value: 35 },
     { label: '36', value: 36 },
     { label: '37', value: 37 },
     { label: '38', value: 38 },
     { label: '39', value: 39 },
     { label: '40', value: 40 },
     { label: '41', value: 41 },
     { label: '42', value: 42 },
     { label: '43', value: 43 },
     { label: '44', value: 44 },
     { label: '45', value: 45 },
     { label: '46', value: 46 },
     { label: '47', value: 47 },

      { label: '48', value: 48 },
     { label: '49', value: 49 },
     { label: '50', value: 50 },
     { label: '51', value: 51 },
      { label: '52', value: 52 },
     { label: '53', value: 53 },
     { label: '54', value: 54 },
     { label: '55', value: 55 },
      { label: '56', value: 56 },
     { label: '57', value: 57 },
     { label: '58', value: 58 },
     { label: '59', value: 59 },
     
    
  ]

  const dailyDays: any = [
    { label: '01', value: 1 },
    { label: '02', value: 2 },
    { label: '03', value: 3 },
    { label: '04', value: 4 },
    { label: '05', value: 5 },
    { label: '06', value: 6 },
    { label: '07', value: 7 },
    { label: '08', value: 8 },
    { label: '09', value: 9 },
    { label: '10', value: 10 },
    { label: '11', value: 11 },
    { label: '12', value: 12 },
    { label: '13', value: 13 },
    { label: '14', value: 14 },
    { label: '15', value: 15 },
  ]

  const monthlyDates: any = [
    { label: '01', value: 1 },
    { label: '02', value: 2 },
    { label: '03', value: 3 },
    { label: '04', value: 4 },
    { label: '05', value: 5 },
    { label: '06', value: 6 },
    { label: '07', value: 7 },
    { label: '08', value: 8 },
    { label: '09', value: 9 },
    { label: '10', value: 10 },
    { label: '11', value: 11 },
    { label: '12', value: 12 },
    { label: '13', value: 13 },
    { label: '14', value: 14 },
    { label: '15', value: 15 },
    { label: '16', value: 16 },
    { label: '17', value: 17 },
    { label: '18', value: 18 },
    { label: '19', value: 19 },
    { label: '20', value: 20 },
    { label: '21', value: 21 },
    { label: '22', value: 22 },
    { label: '23', value: 23 },

    { label: '24', value: 24 },
    { label: '25', value: 25 },
    { label: '26', value: 26 },
    { label: '27', value: 27 },
    { label: '28', value: 28 },
    { label: '29', value: 29 },
    { label: '30', value: 30 },
    { label: '31', value: 31 }
  ]

  const monthDays :any =[
    { label: '01', value: 1 },
    { label: '02', value: 2 },
    { label: '03', value: 3 },
    { label: '04', value: 4 },
    { label: '05', value: 5 },
    { label: '06', value: 6 },
    { label: '07', value: 7 }
  ]

  const weeklyOpt: any = [
    { label: '01', value: 1 },
    { label: '02', value: 2 },
    { label: '03', value: 3 },
    { label: '04', value: 4 },
  ]
  
    const reportOpt = [
    { label: 'Campaign report', value: 1 },
    { label: 'Email report', value: 2 },
    { label: 'Whatsapp report', value: 3 },
    { label: 'Sms report', value: 4 },
    { label: 'AppPush report', value: 5 },
    { label: 'WebPush report', value: 6 },
  ]

   const selectReportOpt = [
   { label: 'Campaign report', value: 1 },
    { label: 'Email report', value: 2 },
    { label: 'Whatsapp report', value: 3 },
    { label: 'Sms report', value: 4 },
    { label: 'AppPush report', value: 5 },
    { label: 'WebPush report', value: 6 },
  ]

  const monthTypeOpt: any = [
    { label: "ON", value: 0 },
    { label: "First", value: 1 },
    { label: "Second", value: 2 },
    { label: "Third", value: 3 },
    { label: "Fourth", value: 4 }
  ]

  const monthDaysOpt :any =[
     { label: '01', value: 1 },
    { label: '02', value: 2 },
    { label: '03', value: 3 },
    { label: '04', value: 4 },
    { label: '05', value: 5 },
    { label: '06', value: 6 },
    { label: '07', value: 7 },
    { label: '08', value: 8 },
    { label: '09', value: 9 },
    { label: '10', value: 10 },
    { label: '11', value: 11 },
    { label: '12', value: 12 },
    { label: '13', value: 13 },
    { label: '14', value: 14 },
    { label: '15', value: 15 },
    { label: '16', value: 16 },
    { label: '17', value: 17 },
    { label: '18', value: 18 },
    { label: '19', value: 19 },
    { label: '20', value: 20 },
    { label: '21', value: 21 },
    { label: '22', value: 22 },
    { label: '23', value: 23 },

    { label: '24', value: 24 },
    { label: '25', value: 25 },
    { label: '26', value: 26 },
    { label: '27', value: 27 },
    { label: '28', value: 28 },
    { label: '29', value: 29 },
    { label: '30', value: 30 },
    { label: '31', value: 31 }
  ]

  const monthWeekDaysOpt :any =[
    {label:"SUN",value:1},
    {label:"MON",value:2},
    {label:"TUE",value:3},
    {label:"WED",value:4},
    {label:"THU",value:5},
    {label:"FRI",value:6},
    {label:"SAT",value:7},

  ]

  const scrollerRef = useRef<HTMLDivElement>(null);

  const [viewReportStartDate,setViewReportStartDate] =useState<any>( moment(new Date()).format('YYYY-MM-DD'));
  const [viewReportEndDate,setViewReportEndDate] = useState<any>(moment(new Date()).format('YYYY-MM-DD'));
  const [resetCounter,setResetCounter]=useState<any>(0);
console.log("viewstartdate",viewReportStartDate,"viewReportend",viewReportEndDate)

  const [drawer,setDrawer]= useState(false);
  const initialReportOpt :any= { label: 'Campaign report', value: 1 };

  const initialColumnState: any = [
    {
      dataField: "Campaign_Name",
      text: "Campaign Name",
      sort: true,
      formatter: (item: string) => {
        // return <span className='clickabletxt'>{item}</span>
        return <span >{item}</span>
      }
    },
    {
      dataField: "Created_Date",
      text: "Created Date",
      sort: true,
      formatter: (item: string) => {
        return <span >{moment(item).format('YYYY-MM-DD')}</span>
      }

    },
    // {
    //   dataField: "campaign_duration",
    //   text: "Campaign duration (start date & Time -end date& Time)",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >{item}</span>
    //   }
    // },
    {
      dataField: "Targeted_Channels",
      text: "Targeted Channels ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "No_of_audience_targeted",
      text: "No.of audience targeted",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    // {
    //   dataField: "targeted_audience",
    //   text: "Targeted Audience ",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >{item}</span>
    //   }
    // },
    {
      dataField: "Overall_Delivery",
      text: "Overall delivery ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "Overall_Openrate",
      text: "Overall Openrate",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "Overall_Clickrate",
      text: "Overall Clickrate",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "EmailDelivery",
      text: "Email delivery ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "EmailOpenRate",
      text: "Email Openrate ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "EmailClickRate",
      text: "Email Clickrate ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    // {
    //   dataField: "emailBouncerate",
    //   text: "Email Bouncerate",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >{item}</span>
    //   }
    // },
    {
      dataField: "EmailUnsubscribeRate",
      text: "Email Unsubscribe rate ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "SMSDelivery",
      text: "SMS Delivery ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    // {
    //   dataField: "smsOptOut",
    //   text: "SMS Opt-out rate ",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >{item}</span>
    //   }
    // },
    {
      dataField: "WhatsappDelivery",
      text: "WhatsApp Delivery ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "WhatsappOpenRate",
      text: "Whatsapp Openrate ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    // {
    //   dataField: "appPushClickRate",
    //   text: "App Push Clickrate",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >{item}</span>
    //   }
    // },
    {
      dataField: "AppPushDelivery",
      text: "App Push Delivery ",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "WebPushDelivery",
      text: "Web Push Delivery",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    //     {
    //   dataField: "in-appClickRate",
    //   text: "In-App clickrate ",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >{item}</span>
    //   }
    // },
  ]



  const [selectedReport,setSelectedReport] = useState<any>(initialReportOpt);
  const [selectedHour,setSelectedHour] = useState<any>(hours?.[0]);
  const [selectedMin,setSelectedMin] = useState<any>(mins?.[0]);
  const[columns,setColumns]=useState<any>(initialColumnState);
  console.log("selectedreport",selectedReport)
  

  const [selectedRecordId,setSelectedRecordId] = useState<any>(null);
 console.log("recoid",selectedRecordId)

  const [columndata,setColumnData]= useState<any>([]);
  const [columnDataCopy,setColumnDataCopy] = useState<any>([]);
  const [searchInput,setSearchInput] = useState<any>('');
  const [pdfGenerating,setPdfGenerating] = useState(false);
console.log("searchinput",searchInput)
  const [selectedScheduleType,setSelectedScheduleType]= useState<any>(initialReportOpt);
console.log("selectedTyoe",selectedScheduleType)
const initialRecurrenceData ={
  recurrence:1,

}

  const tableOpt = {
      ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
          text: "All",
          value: columndata?.length,
      }]
  }
  
const initialSendMailData :any ={
  timezone:countryTimezones?.[0]?.value,
  isPdf:false,
  isCsv:false,
  isXls:false,
  isXlsx:false,
  mailto:"",
  subject:"",
  message:""
}
const [recurrenceData,setRecurrenceData]=useState<any>(initialRecurrenceData);
const [sendMailData,setSendMailData] = useState<any>(initialSendMailData);

const [dailyDay,setDailyDay] =useState<any>(dailyDays?.[0]);
const [weekly,setWeekly] = useState<any>(weeklyOpt?.[0]);
const [monthly,setMonthly] = useState<any>(monthlyDates?.[0]);
const [month,setMonth] = useState<any>(monthDays?.[0]);
const [monthType,setMonthType]=useState<any>(monthTypeOpt?.[0]);

const [selectedmonthDays,setSelectedMonthDays] = useState<any>(monthDaysOpt?.[0]);
const [editMonthDays,setEditMonthDays] = useState<any>(null);
const [childrenMonthDays,setChildrenMonthDays] = useState<any>(monthDaysOpt?.[0]?.label);

  useLayoutEffect(() => {
    if (!scrollerRef.current) return;
    scrollerRef.current?.scrollIntoView({ behavior: 'smooth', block: "end" });

  }, [reportData.reportMessage?.length]);


  const getWebPushLegacyReport =()=>{
    try {
          let body: any = {
            startDate: viewReportStartDate,
            endDate: viewReportEndDate,
          }
          whatsappService.post(body, '/get_webpush_legacy_report').then((res: any) => {
            if (res?.data?.statusCode == 200) {
            console.log("clg",res?.data?.response)


    const initialColumn: any = [
    {
      dataField: "Campaign_Name",
      text: "Campaign Name",
      sort: true,
      formatter: (item: string) => {
        
        // return <span className='clickabletxt' >{item}</span>
        return <span  >{item}</span>
      }
    },
    {
      dataField: "Created_Date",
      text: "Created Date",
      sort: true,
      formatter: (item: string) => {
        return <span >{moment(item).format("YYYY-MM-DD")}</span>
      }

    },
    {
      dataField: "Campaign_Duration",
      text: "Campaign duration (start date & Time -end date& Time)",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "No_of_audience_targeted",
      text: "No.of audience targeted",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    // {
    //   dataField: "targeted_audience",
    //   text: "Targeted Audience ",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >ewe</span>
    //   }
    // },
   
    {
      dataField: "Web_Push_Delivery",
      text: "Web Push Delivery",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "Web_Push_Openrate",
      text: "Web Push OpenRate",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
  ]
setColumns(initialColumn);
setColumnData(res?.data?.response);
setColumnDataCopy(res?.data?.response);

            }
          })
            } catch (error) {
                console.log("error", error);
            }
  } 

  const getAppPushReport=()=>{
     try {
          let body: any = {
            startDate: viewReportStartDate,
            endDate: viewReportEndDate,
          }
          whatsappService.post(body, '/get_apppush_legacy_report').then((res: any) => {
            if (res?.data?.statusCode == 200) {
            console.log("clg",res?.data?.response)


    const initialColumn: any = [
    {
      dataField: "Campaign_Name",
      text: "Campaign Name",
      sort: true,
      formatter: (item: string) => {
        
        // return <span className='clickabletxt'>{item}</span>
        return <span >{item}</span>
      }
    },
    {
      dataField: "Created_Date",
      text: "Created Date",
      sort: true,
      formatter: (item: string) => {
        return <span >{moment(item).format("YYYY-MM-DD")}</span>
      }

    },
    {
      dataField: "Campaign_duration",
      text: "Campaign duration (start date & Time -end date& Time)",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "No_of_audience_targeted",
      text: "No.of audience targeted",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    // {
    //   dataField: "targeted_audience",
    //   text: "Targeted Audience ",
    //   sort: true,
    //   formatter: (item: string) => {
    //     return <span >ewe</span>
    //   }
    // },
   
    {
      dataField: "App_Push_Delivery",
      text: "App Push Delivery",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
    {
      dataField: "App_Push_Openrate",
      text: "App Push OpenRate",
      sort: true,
      formatter: (item: string) => {
        return <span >{item}</span>
      }
    },
  ]
setColumns(initialColumn);
setColumnData(res?.data?.response);
setColumnDataCopy(res?.data?.response);

            }
          })
            } catch (error) {
                console.log("error", error);
            }
  }

  const getSmsReport = () => {
    try {
      let body: any = {
        startDate: viewReportStartDate,
        endDate: viewReportEndDate,
      }
      whatsappService.post(body, '/get_sms_legacy_report').then((res: any) => {
        if (res?.data?.statusCode == 200) {
          console.log("clg", res?.data?.response)


          const initialColumn: any = [
            {
              dataField: "Campaign_Name",
              text: "Campaign Name",
              sort: true,
              formatter: (item: string) => {

                // return <span className='clickabletxt'>{item}</span>
                return <span >{item}</span>
              }
            },
            {
              dataField: "Created_Date",
              text: "Created Date",
              sort: true,
              formatter: (item: string) => {
                return <span >{moment(item).format("YYYY-MM-DD")}</span>
              }

            },
            {
              dataField: "Campaign_Duration",
              text: "Campaign duration (start date & Time -end date& Time)",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "No_of_audience_targeted",
              text: "No.of audience targeted",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            // {
            //   dataField: "targeted_audience",
            //   text: "Targeted Audience ",
            //   sort: true,
            //   formatter: (item: string) => {
            //     return <span >ewe</span>
            //   }
            // },

            {
              dataField: "SMS_Delivery",
              text: "SMS Push Delivery",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
           
          ]
          setColumns(initialColumn);
          setColumnData(res?.data?.response);
          setColumnDataCopy(res?.data?.response);

        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const getWhatsappReport = () => {
    try {
      let body: any = {
        startDate: viewReportStartDate,
        endDate: viewReportEndDate,
      }
      whatsappService.post(body, '/get_whatsapp_legacy_report').then((res: any) => {
        if (res?.data?.statusCode == 200) {
          console.log("clg", res?.data?.response)


          const initialColumn: any = [
            {
              dataField: "Campaign_Name",
              text: "Campaign Name",
              sort: true,
              formatter: (item: string) => {

                // return <span className='clickabletxt'>{item}</span>
                return <span >{item}</span>
              }
            },
            {
              dataField: "Created_Date",
              text: "Created Date",
              sort: true,
              formatter: (item: string) => {
                return <span >{moment(item).format("YYYY-MM-DD")}</span>
              }

            },
            {
              dataField: "Campaign_Duration",
              text: "Campaign duration (start date & Time -end date& Time)",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "No_of_audience_targeted",
              text: "No.of audience targeted",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            // {
            //   dataField: "targeted_audience",
            //   text: "Targeted Audience ",
            //   sort: true,
            //   formatter: (item: string) => {
            //     return <span >ewe</span>
            //   }
            // },

            {
              dataField: "WhatsApp_Delivery",
              text: "WhatsApp Delivery",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
             {
              dataField: "Whatsapp_Openrate",
              text: "Whatsapp Openrate",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
           
          ]
          setColumns(initialColumn);
          setColumnData(res?.data?.response);
          setColumnDataCopy(res?.data?.response);

        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const getEmailReport = () => {
    try {
      let body: any = {
        startDate: viewReportStartDate,
        endDate:viewReportEndDate,
      }
      whatsappService.post(body, '/get_email_legacy_report').then((res: any) => {
        if (res?.data?.statusCode == 200) {
          console.log("clg", res?.data?.response)


          const initialColumn: any = [
            {
              dataField: "Campaign_Name",
              text: "Campaign Name",
              sort: true,
              formatter: (item: string) => {

                return <span >{item}</span>
              }
            },
            {
              dataField: "Created_Date",
              text: "Created Date",
              sort: true,
              formatter: (item: string) => {
                return <span >{moment(item).format("YYYY-MM-DD")}</span>
              }

            },
            {
              dataField: "Campaign_Duration",
              text: "Campaign duration (start date & Time -end date& Time)",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "No_of_audience_targeted",
              text: "No.of audience targeted",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "Targeted_Audience",
              text: "Targeted Audience ",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },

            {
              dataField: "Email_Delivery",
              text: "Email Delivery",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "Email_Openrate",
              text: "Email Openrate",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "Email_Clickrate",
              text: "Email Clickrate",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },
            {
              dataField: "Email_Unsubscribe_Rate",
              text: "Email Unsubscribe_Rate",
              sort: true,
              formatter: (item: string) => {
                return <span >{item}</span>
              }
            },

          ]
          setColumns(initialColumn);
          setColumnData(res?.data?.response);
          setColumnDataCopy(res?.data?.response);

        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const getAllCampaignReport=()=>{
     try {
      let body: any = {
        startDate: viewReportStartDate,
        endDate: viewReportEndDate,
      }
      whatsappService.post(body, '/get_all_legacy_report').then((res: any) => {
        if (res?.data?.statusCode == 200) {
          console.log("clg", res?.data?.response);
          setColumns(initialColumnState);
          setColumnData(res?.data?.response);
          setColumnDataCopy(res?.data?.response);

        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

const handleSelected=(opt :any)=>{
  console.log("opt",opt)
  setSelectedReport(opt);
  setSearchInput("");
}

const handleHour =(opt :any)=>{
 setSelectedHour(opt);
}
const handleMin =(opt :any)=>{
  setSelectedMin(opt);
}

const handleSelectedReport=(val :any)=>{
  console.log("check",val)
  setSelectedScheduleType(selectReportOpt[val-1]);
}

const handleTimeZone=(val : any)=>{
  console.log("valll",val)
    setSendMailData({...sendMailData,timezone:val})
}

const handleDailyDays=(val :any)=>{
    setDailyDay(val);
}

const handleWeekly=(val :any)=>{
    setWeekly(val);
}

const handleMonthRecurrence=(opt :any)=>{
    setMonth(opt);
}

const handleMonthType =(opt :any)=>{
  if(opt!=monthType){
    setMonthType(opt);
    setSelectedMonthDays(opt?.value==0?monthDaysOpt?.[0]:monthWeekDaysOpt?.[0]);
  }
}

const handleMonthDays=(opt :any)=>{
    setSelectedMonthDays(opt);
    setChildrenMonthDays(opt?.label);
}

const handleError =async(body :any)=>{
    let error: any = 0;
    if (!body?.reportId || !body?.recurrenceType ||!body?.reportStartDate||!body?.reportEndDate||!body?.timeZone||(!body?.isPdf && !body?.isCSV && !body?.isXls && !body?.isXLSX)||!body?.email?.trim()||!body?.message?.trim()) {
      error = -1;
    }

    if((recurrenceData?.recurrence==1 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) || (recurrenceData?.recurrence==1 && recurrenceData?.reportEndDate>new Date(Date.now() - 86400000).toISOString().split("T")[0]) || (recurrenceData?.recurrence==1 && recurrenceData?.reportStartDate>new Date(Date.now() - 86400000).toISOString().split("T")[0]) || (recurrenceData?.recurrence==1 && recurrenceData?.scheduleDate<new Date(Date.now() - 86400000).toISOString().split("T")[0]) ||
       (recurrenceData?.recurrence==2 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) ||  (recurrenceData?.recurrence==2 && recurrenceData?.reportEndDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]) || (recurrenceData?.recurrence==2 && recurrenceData?.reportStartDate<= new Date(Date.now() - 86400000).toISOString().split("T")[0]) ||
       (recurrenceData?.recurrence==3 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) || (recurrenceData?.recurrence==3 && recurrenceData?.reportEndDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]) || (recurrenceData?.recurrence==3 && recurrenceData?.reportStartDate<= new Date(Date.now() - 86400000).toISOString().split("T")[0]) ||
       (recurrenceData?.recurrence==4 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate)|| (recurrenceData?.recurrence==4 && recurrenceData?.reportEndDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]) || (recurrenceData?.recurrence==4 && recurrenceData?.reportStartDate<= new Date(Date.now() - 86400000).toISOString().split("T")[0]) ){
       error = -1;
    }

    if (error == 0) {
      setValidation(false);
      return 0;
    } else {
      return 1;
    }

}

const closeToast=()=>{
  setToastOpen(false);
   setToastMessage(""); 
}

const handleToast=(msg :any)=>{
  setToastOpen(true);
  setToastMessage(msg);
}

const downloadReportPdf = async () => {
  if (!columndata?.length) {
    handleToast("No data to export");
    return;
  }

  setPdfGenerating(true);
  try {
    const doc = new jsPDF({
      orientation: "landscape",
      unit: "pt",
      format: "a4"
    });

    const margin = 40;
    const pageWidth = doc.internal.pageSize.getWidth();
    const pageHeight = doc.internal.pageSize.getHeight();
    let cursorY = margin;

    doc.setFontSize(16);
    doc.text(`Report export ${moment().format("YYYY-MM-DD HH:mm")}`, margin, cursorY);
    cursorY += 24;

    const preparedColumns = (columns ?? []).filter((col: any) => col?.dataField);
    const fallbackKeys = Object.keys(columndata?.[0] ?? {});
    const columnDefs = preparedColumns.length
      ? preparedColumns
      : fallbackKeys.map((key) => ({ dataField: key, text: key }));

    const formatCellValue = (value: any) => {
      if (value === null || value === undefined) {
        return "";
      }
      if (typeof value === "object") {
        try {
          return JSON.stringify(value);
        } catch {
          return String(value);
        }
      }
      return String(value);
    };

    columndata.forEach((row: any, rowIndex: number) => {
      if (cursorY > pageHeight - margin) {
        doc.addPage();
        cursorY = margin;
      }

      doc.setFontSize(12);
      doc.text(`Record ${rowIndex + 1}`, margin, cursorY);
      cursorY += 18;

      columnDefs.forEach((colDef: any) => {
        if (!colDef?.dataField) {
          return;
        }

        if (cursorY > pageHeight - margin) {
          doc.addPage();
          cursorY = margin;
        }

        const label = colDef.text ?? colDef.dataField;
        const value = formatCellValue(row[colDef.dataField]);
        const line = `${label}: ${value}`;
        doc.setFontSize(10);
        doc.splitTextToSize(line, pageWidth - margin * 2).forEach((fragment: string) => {
          if (cursorY > pageHeight - margin) {
            doc.addPage();
            cursorY = margin;
          }
          doc.text(fragment, margin, cursorY);
          cursorY += 12;
        });
      });

      cursorY += 10;
    });

    doc.save(`Reports-${moment().format("YYYYMMDD-HHmmss")}.pdf`);
    handleToast("PDF download started");
  } catch (error) {
    console.error("PDF export failed", error);
    handleToast("PDF export failed");
  } finally {
    setPdfGenerating(false);
  }
};


const handleSubmit=async()=>{
     try {
          setValidation(true);
          const body={
            recordId:selectedRecordId??null,
            reportId:selectedScheduleType?.value??1,
            recurrenceType:recurrenceData?.recurrence,
            reportStartDate:recurrenceData?.reportStartDate,
            reportEndDate:recurrenceData?.reportEndDate,
            timeZone:sendMailData?.timezone,
            isPdf:sendMailData?.isPdf?1:0,
            isCSV:sendMailData?.isCsv?1:0,
            isXls:sendMailData?.isXls?1:0,
            isXLSX:sendMailData?.isXlsx?1:0,
            email:sendMailData?.mailto,
            subject:sendMailData?.subject,
            message:sendMailData?.message,
            scheduleDate:recurrenceData?.recurrence==1?recurrenceData?.scheduleDate:null,
            hour:selectedHour?.value,
            min:selectedMin?.value,
            repeatEvery:recurrenceData?.recurrence==2?dailyDay?.value:recurrenceData?.recurrence==3?weekly?.value:recurrenceData?.recurrence==4?month?.value:null,
            repeatDateNum:(recurrenceData?.recurrence==4 && monthType?.value==0)?selectedmonthDays?.value:null,
            ordinalNum:recurrenceData?.recurrence==4 ?monthType?.value:null,
            weekDay: (recurrenceData?.recurrence==4 && monthType?.value!=0)?selectedmonthDays?.value:null,
            isSun:recurrenceData?.recurrence==3?recurrenceData?.isSun?1:null:null,
            isMon:recurrenceData?.recurrence==3?recurrenceData?.isMon?2:null:null,
            isTue:recurrenceData?.recurrence==3?recurrenceData?.isTue?3:null:null,
            isWed:recurrenceData?.recurrence==3?recurrenceData?.isWed?4:null:null,
            isThu:recurrenceData?.recurrence==3?recurrenceData?.isThu?5:null:null,
            isFri:recurrenceData?.recurrence==3?recurrenceData?.isFri?6:null:null,
            isSat:recurrenceData?.recurrence==3?recurrenceData?.isSat?7:null:null,
          }
       
       const isErr: any = await handleError(body);
       if (isErr) {
         return;
       }
       let url: any;
       if (body?.recordId) {
         url = '/update_recurrence_config'
       } else {
         url = `/insert_recurrence_Schedule`
       }
      const response = await whatsappService.post(body,url );
      if (response?.data?.statusCode === 200) {
        console.log("success", response)
        setCreateSchedule(false);
        setValidation(false);
        init();
      }
    } catch (error) {
      console.log(error);
    }

}

const csvHeaders = columns.map(col => ({
  label: col.text ?? col.dataField,  // must be string
  key: col.dataField                 // must exist in data
}));


const init=()=>{
    try {
      whatsappService.get('/get_recurrence_Schedule').then((res: any) => {
        console.log('smtp', res)
        if (res?.data?.statusCode == 200 && res?.data?.listAndFilterData.length >= 0) {
          setGetReccurenceData(res?.data?.listAndFilterData??[]);
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const handleDatePicker=(startDate :any,endDate :any)=>{
      setViewReportStartDate(moment(startDate).format('YYYY-MM-DD'));
      setViewReportEndDate(moment(endDate).format('YYYY-MM-DD'));
  }


  const scheduleReset = () => {
    try {
      
      setRecurrenceData(initialRecurrenceData);
      setSendMailData(initialSendMailData);
      setDailyDay(dailyDays?.[0]);
      setWeekly(weeklyOpt?.[0]);
      setMonthly(monthlyDates?.[0]);
      setMonth(monthDays?.[0]);
      setMonthType(monthTypeOpt?.[0]);
      setSelectedMonthDays(monthDaysOpt?.[0]);
      setSelectedHour(hours?.[0]);
      setSelectedMin(mins?.[0]);
      setEditMonthDays(null);
    } catch (error) {
      console.log("err:",error)
    }
  }
  
  const handleScheduleEdit=(item :any)=>{
    try {
      setSelectedRecordId(item?.Id);
      setCreateSchedule(true);
      let body :any={
        recordId:item?.Id
      }
      whatsappService.post(body,'/get_scheduled_report_details').then((res: any) => {
        console.log('get_scheduled_report_details1', res)
        if (res?.data?.statusCode == 200 ) {
          console.log("get_scheduled_report_details2",res?.data?.listAndFilterData?.[0])

  
          const result :any= res?.data?.listAndFilterData?.[0];
          const recurrenceDataSet :any= {
              recurrence:result?.recurrenceType,
              reportStartDate:moment(result?.reportResultStartDate).format("YYYY-MM-DD"),
              reportEndDate:moment(result?.reportResultEndDate).format("YYYY-MM-DD"),
              scheduleDate:moment(result?.userSetTime).format("YYYY-MM-DD"),
              isSun:result?.isSunday,
              isMon:result?.isMonday,
              isTue:result?.isTueday,
              isWed:result?.isWednesday,
              isThu:result?.isThursday,
              isFri:result?.isFriday,
              isSat:result?.isSaturday,

          }
          setRecurrenceData({...recurrenceDataSet});

          const sendMailData :any ={
            timezone:result?.timeZone,
            isPdf:result?.fileTypeIsPDF,
            isCsv:result?.fileTypeIsCSV,
            isXls:result?.fileTypeIsXLS,
            isXlsx:result?.fileTypeIsXLSL,
            mailto:result?.emailId,
            subject:result?.subject,
            message:result?.message
          }
          
          setDailyDay((prev :any)=>dailyDays?.[result?.repeatEvery-1]);
          setMonth((prev :any)=>monthDays?.[result?.repeatEvery-1]);
          setMonthType((prev :any)=>monthTypeOpt?.[result?.ordinalNumber]);
          setTimeout(()=>{
            setSelectedMonthDays((prev :any)=>result?.weekDays?monthWeekDaysOpt?.[result?.weekDays-1]:monthDaysOpt?.[result?.DateNo-1])
          }
            ,100);
          setEditMonthDays((prev :any)=>result?.weekDays?monthWeekDaysOpt?.[result?.weekDays-1]:monthDaysOpt?.[result?.DateNo-1]);
          setSelectedHour((prev :any)=>hours?.[result?.recurrenceType==1?result?.TimeHour:result?.frequenceHour]);
          setSelectedMin((prev :any)=>mins?.[result?.recurrenceType==1?result?.TimeMinute:result?.frequenceMinute]);
          handleSelectedReport(result?.reportId);
          handleTimeZone(result?.timeZone);
          setSendMailData((prev :any)=>({...prev,...sendMailData}));
        }
      })
    } catch (error) {
      console.log("ERROR:",error)
    }
   

  }

  const clearFilter=()=>{
    console.log("erer")
     setSelectedReport(initialReportOpt);
     setSearchInput("");
     setViewReportStartDate(moment(new Date()).format('YYYY-MM-DD'))
     setViewReportEndDate(moment(new Date()).format('YYYY-MM-DD'))
     setColumnData([])
     setColumnDataCopy([])
     setResetCounter(resetCounter+1);
  }

useEffect(()=>{

  if (selectedReport?.value == 1) {
    getAllCampaignReport();
  }
  if (selectedReport?.value == 2) {
    getEmailReport();
  }
  if (selectedReport?.value == 3) {

    getWhatsappReport();
  }
  if (selectedReport?.value == 4) {
    getSmsReport();
  }
  if (selectedReport?.value == 5) {
    
    getAppPushReport();
  }
  if (selectedReport?.value == 6) {
    getWebPushLegacyReport();
  }

},[selectedReport,viewReportStartDate])

useEffect(()=>{
  if(searchInput?.length==0){
    setColumnData(columnDataCopy);
  }
  if(searchInput?.length>0){
    const lowerSearch = searchInput.toLowerCase();

    const result = columnDataCopy.filter((item :any) =>
      Object.values(item).some((value) =>
        String(value).toLowerCase().includes(lowerSearch)
      )
    );
    setColumnData(result);
  }
},[searchInput])


useEffect(()=>{
  init();
},[])

useEffect(()=>{
  try {
    if(!toastOpen) return;
    let timer:any ;
    clearTimeout(timer);
    timer =setTimeout(()=>{ closeToast();},3000);
    return ()=>clearTimeout(timer);
  } catch (error) {
    console.log("err:",error)
  }
},[toastOpen])


useEffect(()=>{
try {
  setChildrenMonthDays(monthType?.value==0?monthDaysOpt?.[0]?.label:monthWeekDaysOpt?.[0]?.label);
} catch (error) {
  console.log("error:",error)
}
},[monthType?.value])


const outsideclick = useRef<HTMLDivElement | null>(null);

useEffect(() => {
  const handleClickOutside = (event: MouseEvent) => {
    if (
      outsideclick.current &&
      !outsideclick.current.contains(event.target as Node)
    ) {
      setShowOptions(false);
    }
  };

  document.addEventListener("mousedown", handleClickOutside);

  return () => {
    document.removeEventListener("mousedown", handleClickOutside);
  };
}, []);
 const scrollRef = useRef(null);
const [showLeft, setShowLeft] = useState(false);
const [showRight, setShowRight] = useState(false);
 useEffect(() => {
        checkArrows();
        const el: any = scrollRef.current;
        el?.addEventListener("scroll", checkArrows);
        return () => el?.removeEventListener("scroll", checkArrows);
    }, []);

    const checkArrows = () => {
        const el: any = scrollRef.current;
        setShowLeft(el.scrollLeft > 10);
        setShowRight(el.scrollWidth - el.clientWidth - el.scrollLeft > 10);
    };

    const scroll = (direction: any) => {
      setFirstTime(false);
        const el: any = scrollRef.current;
        const scrollAmount = 250;
        direction === "left"
            ? el?.scrollTo({ left: el.scrollLeft - scrollAmount, behavior: "smooth" })
            : el?.scrollTo({ left: el.scrollLeft + scrollAmount, behavior: "smooth" });
    };

  return (
    <div className={` reportspage ${styles.reportspagecontainer}`}>
      <div className={styles.aipilotsec}>
        <div className={styles.lefts}>
          <h4 className={styles.aipheader}>Reports</h4>
          <p className={styles.aipdesc}>
            Track, analyze, and understand your campaign performance with AI-powered insights.
          </p>
        </div>
        {/* <div className={styles.rights}>
          <span className={styles.leg}>Legacy</span>
          <label className="switch">

            <input
              type="checkbox"
              name="checkbox"
              checked={false}


            />
            <span className="slider round"></span>
          </label>
        </div> */}


      </div>

      <div className={styles.staticpagesec}>
        <div className={styles.filtersec}>
          <div className={styles.lefts}>
            <div className={`commonsearch ${styles.commonsearchinput}`}>
              <WorktualSearch onChange={(e: any) => setSearchInput(e?.target?.value)} value={searchInput} id="search" name="search"></WorktualSearch>
            </div>
            
            {/* <div className='commondropdown'>
              <WorktualDropdown options={reportOpt} onChange={(option :any)=>handleSelected(option) } defaultValue={initialReportOpt}
                dropdownImage={true}
              ></WorktualDropdown>
            </div> */}

            

          </div>
          <div className={styles.rights} ref={outsideclick}>
            <button className='cancelbtn exportbtn'  onClick={()=>setShowOptions(true)}>
              Export
            {showOptions &&
            
              <div className='exportdropdwn'>
                <div className='list'>
  <CSVLink data={columndata} headers={csvHeaders} onClick={() => handleToast("Export CSV Successfully")}>
                CSV
              </CSVLink>
                </div>

              <div
           className='list'
              onClick={downloadReportPdf}
         
            >
              {"PDF"}
            </div>
            <div className='list'>
               <CSVLink data={columndata} headers={csvHeaders} onClick={() => handleToast("Export  XLSX Successfully")}>
                XLSX
              </CSVLink>
            </div>
             
              </div>
            }
            
              
            </button>
           


         

          
            <button className='cancelbtn' onClick={() => { setDrawer(true); setSelectedScheduleType(initialReportOpt) }}>Schedule report</button>

          </div>
        </div>
        <div className={styles.filtersec}>
<div className={styles.lefts}>
   
    <div className='commondropdown commoninputswidth'>
              <WorktualDropdown options={reportOpt} onChange={(option: any) => handleSelected(option)} defaultValue={initialReportOpt}
                dropdownImage={true} reset={resetCounter}
              ></WorktualDropdown>
            </div>
            <div className='commoninputswidth datepicker'>
<CustomRangePicker handleDatePicker={(startDate: any, endDate: any) => handleDatePicker(startDate, endDate)} startDate={viewReportStartDate} endDate={viewReportEndDate} reset={resetCounter} />
            </div>
            <span className='clearfilter' onClick={()=>clearFilter()}>Clear Filter</span>
           

</div>
<div className={styles.rights}>

</div>
        </div>
        <div className={styles.scrolldiv}>
        
        <div className={`desktoptable columnstableheight ${styles.columnstable} ${styles.tableheightafterafilter} customscroll`} ref={scrollRef}>
          {columndata?.length == 0 ? <>
            <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
          </> :<>
           {showLeft && (
                <div className={styles.arrowleft}><div  onClick={() => scroll("left")}><Arrowright /></div></div>
        )}
            <BootstrapTable
              keyField="Sno"
              data={columndata}
              columns={columns}
            pagination={paginationFactory(tableOpt)}
            // noDataIndication={  <>
            //                       <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
            //                     </>}
            />
             {((columndata?.length>0 && firstTime) || showRight) && (
                <div className={styles.arrowright}><div onClick={() => scroll("right")}><Arrowright /></div></div>
            )}
            </>
            }

        </div>
         
            </div>
        <RightDrawer open={drawer} onClose={() => setDrawer(false)} >
          <>
            <div className={styles.topheading}>
              <div style={{ padding: "0 16px" }} className={styles.tophdlefts}>
                <h2>Schedule Reports</h2>
                <p className={styles.headerdesctxt}>Manage your automated report schedules and delivery preferences.</p>
              </div>
            </div>
            {!createSchedule &&
              <>
                {/* {getReccurrenceData?.map((item: any) => {
                return ( */}
                <div className={styles.content}>
                  <div className={styles.cardcontainer}>
                    {getReccurrenceData?.map((item: any) => {
                      return (

                        <div className={styles.schedulecard} onClick={() => handleScheduleEdit(item)}>
                          <div className={styles.header}>
                            <h3 className={styles.title}>{reportOpt?.[item?.ReportName - 1]?.label ?? ""}</h3>
                            <span className='badge'>{item?.Type}</span>
                          </div>

                          <div className={styles.dates}>

                            <span className={styles.firstspan}>
                              <span><svg style={{ display: "grid" }} xmlns="http://www.w3.org/2000/svg" width="12" height="13" viewBox="0 0 12 13" fill="none">
                                <path d="M11.4925 5.5804C11.4756 4.424 11.4756 2.96373 10.5978 2.06893C9.95412 1.41233 9.07965 1.18387 8.18785 1.10067V0.500067C8.18785 0.224 7.96392 6.67572e-05 7.68785 6.67572e-05C7.41178 6.67572e-05 7.18785 0.224 7.18785 0.500067V1.054C6.71172 1.04393 6.23445 1.037 5.75685 1.037C5.27492 1.037 4.79325 1.04387 4.31285 1.05413V0.5C4.31285 0.223933 4.08892 0 3.81285 0C3.53678 0 3.31285 0.223933 3.31285 0.5V1.105C2.42558 1.19053 1.55645 1.41547 0.91585 2.06887C0.08085 2.91187 0.0403166 4.2612 0.0171833 5.38187L0.0169833 5.38287L0.0171165 5.38367C0.0157165 5.452 0.0141167 5.52247 0.01265 5.58887C-0.00421667 6.45833 -0.00421667 7.32773 0.01265 8.1972C0.0295167 9.35367 0.0295166 10.8139 0.907383 11.7087C1.75152 12.5697 2.99232 12.6963 4.14878 12.7216C4.71765 12.7352 5.28665 12.7439 5.85545 12.7439C6.35018 12.7439 6.84472 12.7373 7.33952 12.7216C8.49592 12.6879 9.73678 12.5697 10.5809 11.7087C11.4588 10.8139 11.4503 9.35367 11.4756 8.1972C11.4756 7.80047 11.4925 7.41213 11.4925 7.01547H11.501C11.501 6.53427 11.501 6.0616 11.4925 5.5804ZM3.31285 2.1112V2.4376C3.31285 2.71367 3.53678 2.9376 3.81285 2.9376C4.08892 2.9376 4.31285 2.71367 4.31285 2.4376V2.05427C4.79318 2.0438 5.27492 2.03707 5.75685 2.03707C6.23392 2.03713 6.71098 2.0438 7.18792 2.05413V2.4376C7.18792 2.71367 7.41185 2.9376 7.68792 2.9376C7.96398 2.9376 8.18792 2.71367 8.18792 2.4376V2.10627C8.91045 2.183 9.48392 2.36133 9.88398 2.76947C10.3395 3.2338 10.4448 4.076 10.4761 4.88293H1.03252C1.06585 4.06 1.16785 3.2356 1.62992 2.76907C2.02345 2.36773 2.58125 2.191 3.31285 2.1112ZM10.4925 7.01553C10.4925 7.20667 10.4884 7.3958 10.4843 7.58493C10.4799 7.78833 10.4757 7.9916 10.4759 8.17533L10.4727 8.32647C10.4525 9.3148 10.4297 10.435 9.86678 11.0087C9.32985 11.5563 8.49052 11.6877 7.30778 11.7221C6.84585 11.7368 6.37078 11.7439 5.85552 11.7439C5.29458 11.7439 4.73358 11.7353 4.17072 11.7219C3.03598 11.6971 2.16818 11.5663 1.62125 11.0083C1.04432 10.4203 1.02732 9.22507 1.01372 8.2646L1.01245 8.17787C0.99765 7.41573 1.00078 6.64533 1.01245 5.88293H10.4945C10.4983 6.21707 10.5009 6.54967 10.501 6.88473C10.4954 6.92753 10.4925 6.9712 10.4925 7.01553Z" fill="#404040" />
                              </svg></span>
                              {item?.Report_Start_Date ?? ""}
                            </span>
                          </div>

                          <hr />

                          <div className={styles.timezones}>

                            <span className={styles.firstspan}><span style={{ display: "grid" }}><svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                              <path d="M13.3299 6.665C13.3294 2.98407 10.3461 0.000333333 6.665 0C2.9838 0.000333333 0.0004 2.98393 0 6.66507C0.000333333 10.346 2.98373 13.3294 6.665 13.3297C10.3461 13.3294 13.3294 10.3458 13.3299 6.665C13.3299 6.66513 13.3299 6.6648 13.3299 6.665ZM12.1859 6.10707H9.62947C9.49653 4.36487 8.90613 2.69727 7.92327 1.26433C8.94947 1.5024 9.86733 2.0202 10.5888 2.74107C11.4705 3.62353 12.0553 4.79667 12.1859 6.10707ZM4.81767 6.10707C4.9646 4.39933 5.6054 2.77427 6.665 1.42813C7.7246 2.77427 8.36547 4.39933 8.51233 6.10707H4.81767ZM8.51227 7.22293C8.36527 8.93047 7.72453 10.5555 6.665 11.9015C5.60547 10.5555 4.96467 8.93047 4.81767 7.22293H8.51227ZM2.74107 2.74107C3.46253 2.02027 4.38033 1.50247 5.4066 1.26433C4.42373 2.69727 3.8334 4.36487 3.7004 6.10707H1.14393C1.2746 4.79673 1.85947 3.62353 2.74107 2.74107ZM1.14393 7.22293H3.70047C3.83347 8.96513 4.42373 10.6325 5.40653 12.0653C4.38033 11.8273 3.46247 11.3095 2.741 10.5886C1.8594 9.70613 1.2746 8.53307 1.14393 7.22293ZM10.5887 10.5886C9.8672 11.3094 8.94947 11.8272 7.9232 12.0653C8.906 10.6326 9.4964 8.9652 9.62933 7.22293H12.1858C12.0551 8.53307 11.4703 9.70613 10.5887 10.5886Z" fill="#404040" />
                            </svg></span>{item?.TimeZone ?? ""}</span>
                          </div>
                        </div>


                      )
                    })
                    }
                  </div>
                  <div className={styles.footer}>
                    <button className='primarybtn' onClick={() => { setCreateSchedule(true); scheduleReset(); setSelectedRecordId(null); }}> Create new schedule</button>
                  </div>
                </div>

              </>
            }
            {createSchedule &&
              <div className={styles.content}>
                <form action="" className='formsec reportsdrawerform'>
                  <div className='formrow'>
                    <div className='formlabels'>
                      <label htmlFor="domainname">Schedule Report Name<span className='required'>*</span></label>
                    </div>
                    <div className='formelements'>
                      <WorktualSelect options={selectReportOpt} defaultValue={selectedScheduleType} onChange={(value: any) => handleSelectedReport(value)}></WorktualSelect>
                    </div>
                  </div>
                  <div className='formrow greybox'>
                    <div className='formlabels'>
                      <label htmlFor="domainname">Reccurence<span className='required'>*</span></label>
                    </div>
                    <div className={`formelements ${styles.radiorowinp}`}>
                      <label htmlFor="sameemail">
                        <input id="once" name="once" type="radio" checked={recurrenceData.recurrence === 1}
                          onChange={() => {
                            if (recurrenceData?.recurrence !== 1) {
                              setRecurrenceData({ recurrence: 1 });
                              setSelectedHour(hours?.[0]);
                              setSelectedMin(mins?.[0]);
                              setDailyDay(dailyDays?.[0]);
                            }
                          }} />
                        Once
                      </label>
                      <label htmlFor="Daily">
                        <input id="Daily" name="Daily" type="radio" checked={recurrenceData.recurrence === 2} onChange={() => {
                          if (recurrenceData?.recurrence !== 2) {
                            setRecurrenceData({ recurrence: 2 });
                            setSelectedHour(hours?.[0]);
                            setSelectedMin(mins?.[0]);
                          }
                        }} />
                        Daily
                      </label>
                      <label htmlFor="Weekly">
                        <input id="Weekly" name="Weekly" type="radio" checked={recurrenceData.recurrence === 3} onChange={() => {
                          if (recurrenceData?.recurrence !== 3) {
                            setRecurrenceData({ recurrence: 3 });
                            setSelectedHour(hours?.[0]);
                            setSelectedMin(mins?.[0]);
                            setDailyDay(dailyDays?.[0]);
                          }
                        }} />
                        Weekly
                      </label>
                      <label htmlFor="Monthly">
                        <input id="Monthly" name="Monthly" type="radio" checked={recurrenceData.recurrence === 4} onChange={() => {
                          if (recurrenceData?.recurrence !== 4) {
                            setRecurrenceData({ recurrence: 4 });
                            setSelectedHour(hours?.[0]);
                            setSelectedMin(mins?.[0]);
                            setDailyDay(dailyDays?.[0]);
                          }
                        }} />
                        Monthly
                      </label>
                    </div>

                    {/* // once */}
                    {recurrenceData?.recurrence == 1 &&
                      <>

                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Start date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportEndDate && recurrenceData?.reportEndDate >= e?.target?.value) || !recurrenceData?.reportEndDate) {

                                setRecurrenceData({ ...recurrenceData, reportStartDate: e?.target?.value })
                              // } else {
                              //   console.log("error")
                              // }
                            }
                            }
                              max={new Date(Date.now() - 86400000).toISOString().split("T")[0]}
                              value={recurrenceData?.reportStartDate ?? ""}></input>
                          </div>
                          
                          {(validation && recurrenceData.recurrence == 1) && (!dateRegex?.test(recurrenceData?.reportStartDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                                 :  (validation && (recurrenceData?.recurrence==1 && recurrenceData?.reportStartDate>new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be less than Today</span> : <></>}
                        </div>
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">End date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportStartDate && recurrenceData?.reportStartDate <= e?.target?.value) || !recurrenceData?.reportStartDate) {
                                setRecurrenceData({ ...recurrenceData, reportEndDate: e?.target?.value })
                              // } else {
                              //   console.log("err")
                              // }
                            }}
                              max={new Date(Date.now() - 86400000).toISOString().split("T")[0]}
                              
                              value={recurrenceData?.reportEndDate ?? ""}></input>
                          </div>
                          
                          {(validation && recurrenceData.recurrence == 1) && (!dateRegex?.test(recurrenceData?.reportEndDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                               :(validation && (recurrenceData?.recurrence==1 && recurrenceData?.reportEndDate>new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be less than Today</span>
                                                                                                                               :(validation && recurrenceData.recurrence == 1 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) ? <span className='errorvalidate'>The End date must be Greater than StartDate</span>:<></>}
                        </div>

                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Schedule date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" min={new Date(Date.now()).toISOString().split("T")[0]} onChange={(e: any) => {

                              setRecurrenceData({ ...recurrenceData, scheduleDate: e?.target?.value })

                            }}
                              value={recurrenceData?.scheduleDate ?? ""}
                              max="9999-12-31"
                            ></input>
                          </div>
                        </div>
                        {(validation && recurrenceData.recurrence == 1) && (!dateRegex?.test(recurrenceData?.scheduleDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                            :(validation && (recurrenceData?.recurrence==1 && recurrenceData?.scheduleDate<new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be greater than or equal to Today</span> 
                                                                                                                            : <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Time<span className='required'>*</span></label>
                          </div>
                          <div className={`formelements ${styles.doubleselelement} `}>
                            <div className={styles.first}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={hours} onChange={
                                  (option: any) => handleHour(option)
                                }
                                  defaultValue={selectedHour}


                                ></WorktualDropdown>
                              </div> Hours
                            </div>

                            <div className={styles.scnd}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={mins} onChange={
                                  (option: any) => handleMin(option)
                                }
                                  defaultValue={selectedMin}
                                ></WorktualDropdown>
                              </div> mins
                            </div>


                          </div>
                        </div>
                      </>
                    }
                    {/* // daily */}
                    {recurrenceData?.recurrence == 2 &&
                      <>

                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Repeat every<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <div className='commondropdown widthautodrpdwn'>
                              <WorktualDropdown options={dailyDays} onChange={
                                (option: any) => handleDailyDays(option)
                              }
                                defaultValue={dailyDay}


                              ></WorktualDropdown>
                            </div>
                          </div>
                        </div>
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Start date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportEndDate && recurrenceData?.reportEndDate >= e?.target?.value) || !recurrenceData?.reportEndDate) {

                                setRecurrenceData({ ...recurrenceData, reportStartDate: e?.target?.value })
                              // } else {
                              //   console.log("error")
                              // }
                            }
                            }
                              min={new Date(Date.now()).toISOString().split("T")[0]}
                              max="9999-12-31"
                              value={recurrenceData?.reportStartDate ?? ""}></input>
                          </div>
                        </div>
                        {(validation && recurrenceData.recurrence == 2) && (!dateRegex?.test(recurrenceData?.reportStartDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                               :(validation && (recurrenceData?.recurrence==2 && recurrenceData?.reportStartDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be Greater than Today</span>
                                                                                                                               : <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">End date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportStartDate && recurrenceData?.reportStartDate <= e?.target?.value) || !recurrenceData?.reportStartDate) {
                                setRecurrenceData({ ...recurrenceData, reportEndDate: e?.target?.value })
                              // } else {
                              //   console.log("err")
                              // }
                            }}
                              min={new Date(Date.now()).toISOString().split("T")[0]}                           
                              max="9999-12-31"
                              value={recurrenceData?.reportEndDate ?? ""}></input>
                          </div>
                        </div>
                        
                        {(validation && recurrenceData.recurrence == 2) && (!dateRegex?.test(recurrenceData?.reportEndDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                             :(validation && recurrenceData.recurrence == 2 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) ? <span className='errorvalidate'>The End date must be Greater than StartDate</span> 
                                                                                                                             :(validation && (recurrenceData?.recurrence==2 && recurrenceData?.reportEndDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be Greater than Today</span>
                                                                                                                             : <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Frequency occurs once at <span className='required'>*</span></label>
                          </div>
                          <div className={`formelements ${styles.doubleselelement} `}>
                            <div className={styles.first}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={hours} onChange={
                                  (option: any) => handleHour(option)
                                }
                                  defaultValue={selectedHour}

                                ></WorktualDropdown>
                              </div> Hours
                            </div>
                            <div className={styles.scnd}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={mins} onChange={
                                  (option: any) => handleMin(option)
                                }
                                  defaultValue={selectedMin}

                                ></WorktualDropdown>
                              </div> mins
                            </div>


                          </div>
                        </div>
                      </>
                    }
                    {/* //weekly */}
                    {recurrenceData?.recurrence == 3 &&
                      <>

                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Repeat every<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <div className='commondropdown widthautodrpdwn'>
                              <WorktualDropdown options={weeklyOpt} onChange={
                                (option: any) => handleWeekly(option)
                              }
                                defaultValue={dailyDay}

                              ></WorktualDropdown>
                            </div>
                          </div>
                        </div>
                        <div style={{ marginTop: "16px" }} className='formrow '>

                          <div className={`formelements ${styles.radiorowinp}`}>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isSun == 1} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isSun: e?.target?.checked ? 1 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">SUN</label>
                            </span>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isMon == 2} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isMon: e?.target?.checked ? 2 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">MON</label>
                            </span>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isTue == 3} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isTue: e?.target?.checked ? 3 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">TUE</label>
                            </span>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isWed == 4} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isWed: e?.target?.checked ? 4 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">WED</label>
                            </span>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isThu == 5} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isThu: e?.target?.checked ? 5 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">THU</label>
                            </span>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isFri == 6} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isFri: e?.target?.checked ? 6 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">FRI</label>
                            </span>
                            <span className={styles.chkboxspan}>
                              <input type="checkbox" checked={recurrenceData?.isSat == 7} onChange={(e: any) => setRecurrenceData({ ...recurrenceData, isSat: e?.target?.checked ? 7 : 0 })} />
                              <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">SAT</label>
                            </span>
                          </div>
                          {((validation && recurrenceData?.recurrence == 3) && !recurrenceData?.isSun && !recurrenceData?.isMon && !recurrenceData?.isTue && !recurrenceData?.isWed && !recurrenceData?.isThu && !recurrenceData?.isFri && !recurrenceData?.isSat) ? <span className='errorvalidate'>Day must be selected</span> : <></>}
                        </div>

                        <div >

                        </div>
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Start date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportEndDate && recurrenceData?.reportEndDate >= e?.target?.value) || !recurrenceData?.reportEndDate) {

                                setRecurrenceData({ ...recurrenceData, reportStartDate: e?.target?.value })
                              // } else {
                              //   console.log("error")
                              // }
                            }
                            }
                              min={new Date(Date.now()).toISOString().split("T")[0]}
                              max="9999-12-31"
                              value={recurrenceData?.reportStartDate ?? ""}></input>
                          </div>
                        </div>
                        {(validation && recurrenceData.recurrence == 3) && (!dateRegex?.test(recurrenceData?.reportStartDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                               :(validation && (recurrenceData?.recurrence==3 && recurrenceData?.reportStartDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be Greater than Today</span>
                                                                                                                               : <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">End date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportStartDate && recurrenceData?.reportStartDate <= e?.target?.value) || !recurrenceData?.reportStartDate) {
                                setRecurrenceData({ ...recurrenceData, reportEndDate: e?.target?.value })
                              // } else {
                              //   console.log("err")
                              // }
                            }}
                              min={new Date(Date.now()).toISOString().split("T")[0]}
                              max="9999-12-31"
                              value={recurrenceData?.reportEndDate ?? ""}></input>
                          </div>
                        </div>
                        
                        {(validation && recurrenceData.recurrence == 3) && (!dateRegex?.test(recurrenceData?.reportEndDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                             :(validation && (recurrenceData?.recurrence==3 && recurrenceData?.reportEndDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be Greater than Today</span>
                                                                                                                             :(validation && recurrenceData.recurrence == 3 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) ? <span className='errorvalidate'>The End date must be Greater than StartDate</span> : <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Frequency occurs once at <span className='required'>*</span></label>
                          </div>
                          <div className={`formelements ${styles.doubleselelement} `}>
                            <div className={styles.first}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={hours} onChange={
                                  (option: any) => handleHour(option)
                                }
                                  defaultValue={selectedHour}

                                ></WorktualDropdown>
                              </div> Hours
                            </div>
                            <div className={styles.scnd}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={mins} onChange={
                                  (option: any) => handleMin(option)
                                }
                                  defaultValue={selectedMin}

                                ></WorktualDropdown>
                              </div> mins
                            </div>


                          </div>
                        </div>
                      </>
                    }
                    {/* //Monthly */}
                    {recurrenceData?.recurrence == 4 &&
                      <>

                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Repeat every<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <div className='commondropdown widthautodrpdwn'>
                              <WorktualDropdown options={monthDays} onChange={
                                (option: any) => handleMonthRecurrence(option)
                              }
                                defaultValue={month}


                              ></WorktualDropdown>
                            </div>
                          </div>
                        </div>
                        <div style={{ display: "flex", alignItems: "center", gap: "12px", fontFamily: "interregular", marginTop: "12px" }} className='formrow '>

                          <div className='commondropdown widthautodrpdwn'>
                            <WorktualDropdown options={monthTypeOpt} onChange={
                              (option: any) => handleMonthType(option)
                            }

                              defaultValue={monthType}
                            ></WorktualDropdown>
                          </div>
                          <div className='commondropdown widthautodrpdwn'>
                            <WorktualDropdown options={editMonthDays ? (!isNaN(Number(editMonthDays?.label)) ? monthDaysOpt : monthWeekDaysOpt) : monthType?.value == 0 ? monthDaysOpt : monthWeekDaysOpt} onChange={
                              (option: any) => handleMonthDays(option)
                            }
                              defaultValue={editMonthDays ? editMonthDays : monthType?.value == 0 ? monthDaysOpt?.[0] : monthWeekDaysOpt?.[0]}

                              children={childrenMonthDays}
                            ></WorktualDropdown>
                          </div> Day

                        </div>
                        <div >

                        </div>
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Start date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportEndDate && recurrenceData?.reportEndDate >= e?.target?.value) || !recurrenceData?.reportEndDate) {

                                setRecurrenceData({ ...recurrenceData, reportStartDate: e?.target?.value })
                              // } else {
                              //   console.log("error")
                              // }
                            }
                            }
                              min={new Date(Date.now()).toISOString().split("T")[0]}
                              max="9999-12-31"
                              value={recurrenceData?.reportStartDate ?? ""}
                            ></input>
                          </div>
                        </div>
                        {(validation && recurrenceData.recurrence == 4) && (!dateRegex?.test(recurrenceData?.reportStartDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                                :(validation && (recurrenceData?.recurrence==4 && recurrenceData?.reportStartDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be Greater than Today</span>
                                                                                                                                : <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">End date<span className='required'>*</span></label>
                          </div>
                          <div className='formelements'>
                            <input type="date" id="birthday" name="birthday" onChange={(e: any) => {
                              // if ((recurrenceData?.reportStartDate && recurrenceData?.reportStartDate <= e?.target?.value) || !recurrenceData?.reportStartDate) {

                                setRecurrenceData({ ...recurrenceData, reportEndDate: e?.target?.value })
                              // } else {
                              //   console.log("error")
                              // }
                            }
                            }
                              min={new Date(Date.now()).toISOString().split("T")[0]}
                              max="9999-12-31"
                              value={recurrenceData?.reportEndDate ?? ""}></input>
                          </div>
                        </div>
                        
                        {(validation && recurrenceData.recurrence == 4) && (!dateRegex?.test(recurrenceData?.reportEndDate)) ? <span className='errorvalidate'>The date is Invalid</span> 
                                                                                                                             :(validation && (recurrenceData?.recurrence==4 && recurrenceData?.reportEndDate<=new Date(Date.now() - 86400000).toISOString().split("T")[0]))?<span className='errorvalidate'>The date must be Greater than Today</span>
                                                                                                                             :(validation && recurrenceData.recurrence == 4 && recurrenceData?.reportEndDate<=recurrenceData?.reportStartDate) ? <span className='errorvalidate'>The End date must be Greater than StartDate</span> 
                                                                                                                             :  <></>}
                        <div className={`formrow ${styles.innerformrow}`}>
                          <div className='formlabels'>
                            <label htmlFor="domainname">Frequency occurs once at <span className='required'>*</span></label>
                          </div>
                          <div className={`formelements ${styles.doubleselelement} `}>
                            <div className={styles.first}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={hours} onChange={
                                  (option: any) => handleHour(option)
                                }
                                  defaultValue={selectedHour}

                                ></WorktualDropdown>
                              </div> Hours
                            </div>
                            <div className={styles.scnd}>
                              <div className='commondropdown widthautodrpdwn'>
                                <WorktualDropdown options={mins} onChange={
                                  (option: any) => handleMin(option)
                                }
                                  defaultValue={selectedMin}

                                ></WorktualDropdown>
                              </div> mins
                            </div>


                          </div>
                        </div>
                      </>
                    }
                  </div>
                  <div className='formrow'>
                    <div className='formlabels'>
                      <label htmlFor="domainname">Time zone<span className='required'>*</span></label>
                    </div>
                    <div className='formelements'>
                      <WorktualSearchableSelect  options={countryTimezones} onChange={(value: any) => handleTimeZone(value) } />
                      {/* <WorktualSelect options={countryTimezones} defaultValue={sendMailData?.timezone ? { label: sendMailData?.timezone, value: sendMailData?.timezone } : countryTimezones?.[0]} onChange={(value: any) => handleTimeZone(value)}></WorktualSelect> */}
                    </div>
                  </div>
                  <div className='formrow '>
                    <div className='formlabels'>
                      <label htmlFor="domainname">File Type<span className='required'>*</span></label>
                    </div>
                    <div className={`formelements ${styles.radiorowinp}`}>
                      <span className={styles.chkboxspan}>
                        <input type="checkbox" checked={sendMailData?.isPdf ?? false} onChange={(e: any) => setSendMailData({ ...sendMailData, isPdf: e?.target?.checked })} />
                        <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">PDF</label>
                      </span>
                      <span className={styles.chkboxspan}>
                        <input type="checkbox" checked={sendMailData?.isCsv ?? false} onChange={(e: any) => setSendMailData({ ...sendMailData, isCsv: e?.target?.checked })} />
                        <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">CSV</label>
                      </span>
                      <span className={styles.chkboxspan}>
                        <input type="checkbox" checked={sendMailData?.isXls ?? false} onChange={(e: any) => setSendMailData({ ...sendMailData, isXls: e?.target?.checked })} />
                        <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">XLS</label>
                      </span>
                      <span className={styles.chkboxspan}>
                        <input type="checkbox" checked={sendMailData?.isXlsx ?? false} onChange={(e: any) => setSendMailData({ ...sendMailData, isXlsx: e?.target?.checked })} />
                        <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">XLSL</label>
                      </span>
                    </div>
                    {(validation && (!sendMailData?.isPdf && !sendMailData?.isCsv && !sendMailData?.isXls && !sendMailData?.isXlsx)) ? <span className='errorvalidate'>Choose any one of the export format.</span> : <></>}

                  </div>
                  <div className='formrow'>
                    <div className='formlabels'>
                      <label htmlFor="domainname">TO<span className='required'>*</span></label>
                    </div>
                    <div className='formelements'>
                      <input type='text' placeholder='Enter email id' value={sendMailData?.mailto ?? ""} onChange={(e: any) => setSendMailData({ ...sendMailData, mailto: e?.target?.value })}></input>
                    </div>

                  </div>
                  {(validation && !sendMailData?.mailto?.trim()) ? <span className='errorvalidate'>Email is Mandatory</span> : (validation && !(emailRegex?.test(sendMailData?.mailto))) ? <span className='errorvalidate'>Email is Invalid</span> : <></>}
                  <div className='formrow'>
                    <div className='formlabels'>
                      <label htmlFor="domainname">Subject<span className='required'>*</span></label>
                    </div>
                    <div className='formelements'>
                      <input type='text' placeholder='Enter Subject' value={sendMailData?.subject ?? ""} onChange={(e: any) => setSendMailData({ ...sendMailData, subject: e?.target?.value })}></input>
                    </div>
                  </div>
                  {(validation && !sendMailData?.subject?.trim()) ? <span className='errorvalidate'>Subject is Mandatory</span> : <></>}
                  <div className='formrow'>
                    <div className='formlabels'>
                      <label htmlFor="domainname">Message<span className='required'>*</span></label>
                    </div>
                    <div className='formelements'>
                      <textarea placeholder='Enter message' value={sendMailData?.message ?? ""} onChange={(e: any) => setSendMailData({ ...sendMailData, message: e?.target?.value })}></textarea>
                    </div>
                  </div>
                  {(validation && !sendMailData?.message?.trim()) ? <span className='errorvalidate'>Message is Mandatory</span> : <></>}
                </form>
                <div style={{ padding: "20px" }} className='formbtns'>
                  <button type='button' className='cancelbtn' onClick={() => { setDrawer(false); setCreateSchedule(false); setValidation(false); }}> Cancel</button>
                  <button type='button' className='primarybtn' onClick={() => handleSubmit()}
                  >Submit </button>
                </div>
              </div>
            }
          </>
        </RightDrawer>
      </div>
      {toastOpen &&
        <BlurToastDemo
          open={toastOpen}
          message={
            <div style={{ display: "flex", alignItems: "center", gap: "8px" }}>
              <span>{toastMessage}</span>
              <svg
                style={{ cursor: "pointer" }}
                onClick={closeToast}
                xmlns="http://www.w3.org/2000/svg"
                width="11"
                height="10"
                viewBox="0 0 11 10"
                fill="none"
              >
                <path
                  d="M6.29984 4.99937L10.2032 1.09571C10.4245 0.874973 10.4245 0.51564 10.2032 0.294906C9.98184 0.0735729 9.62244 0.0735729 9.40237 0.294906L5.49904 4.19851L1.59571 0.294906C1.37564 0.0735729 1.01497 0.0735729 0.794906 0.294906C0.573573 0.51564 0.573573 0.874973 0.794906 1.09571L4.69824 4.99937L0.794906 8.90304C0.573573 9.12377 0.573573 9.48311 0.794906 9.70384C0.905573 9.81451 1.05011 9.86984 1.19531 9.86984C1.34051 9.86984 1.48564 9.81451 1.59571 9.70384L5.49904 5.80024L9.40237 9.70384C9.51237 9.81451 9.65757 9.86984 9.80277 9.86984C9.94797 9.86984 10.0925 9.81451 10.2032 9.70384C10.4245 9.48311 10.4245 9.12377 10.2032 8.90304L6.29984 4.99937Z"
                  fill="white"
                />
              </svg>
            </div>
          }
        />}
    </div>
  )
}
export default Reports
