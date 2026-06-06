import React from 'react';
import styles from './scss/apiconfiguration.module.scss';
import paginationFactory from 'react-bootstrap-table2-paginator';
import BootstrapTable from 'react-bootstrap-table-next';
import { tableOptions } from '../../../components/custom_components/tableOptions';

const ApiConfiguration = () => {

    const columns = [
        {
            dataField: "api_name",
            text: "API Name",
            sort: true,
            formatter:(item:string)=>{
                return <span className='clickabletxt'>{item}</span>
            }
        },
        {
            dataField: "method",
            text: "Method",
            sort: true,
        },
        {
            dataField: "endpoint",
            text: "Endpoint",
            sort: true,
        },
        {
            dataField: "description",
            text: "Description",
            sort: true,
        },
    ];

    const data = [
        { api_name: 'Get All Customers', method: 'GET', endpoint: '/api/customers', description: 'Fetch 360-degree profiles of all customers' },
    ]

    const tableOpt = {
        ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
            text: "All",
            value: 20,
        }]
    }

    return (
        <div className={styles.apiconfigsec}>
            {/* <div className={styles.apisectop}>
                <div className={styles.toplefts}>
                    <h4>API configuration</h4>
                    <p>Microcopy for API configuration</p>
                </div>
                <button className='primarybtn'>Create API</button>
            </div> */}
             <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>API configuration</h2>
                    <p className={styles.headerdesctxt}>Set up and manage APIs for seamless data integration and connectivity.</p>
                </div>
                <div className='formbtns'>
                   <button className='primarybtn'>Create API</button>
                </div>
            </div>
            <div>
                <div className='table-wrapper '>
   <div className='desktoptable'>
                    <BootstrapTable
                        keyField="id"
                        data={data}
                        columns={columns}
                        pagination={paginationFactory(tableOpt)}
                        // noDataIndication={<NoTableData />}
                    />
                </div>
                </div>
             
                 {/* <div className='mobiletable'>
        <BootstrapTable
          keyField="id"
            data={userCopy}
          columns={columns}
            expandRow={expandRow}
            noDataIndication={<NoTableData/>}           />       
                  </div>  */}
            </div>

        </div>
    )
}

export default ApiConfiguration
