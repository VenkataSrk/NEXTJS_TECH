import React from 'react';
import styles from './scss/whatsapptemp.module.scss';
import BootstrapTable from 'react-bootstrap-table-next';
import { tableOptions } from '../../../components/custom_components/tableOptions';
import paginationFactory from 'react-bootstrap-table2-paginator';

const WhatsAppTemplates = () => {
    const columns = [
        {
            dataField: "template_name",
            text: "Template Name",
            sort: true,
            formatter: (item: string) => {
                return <span className='clickabletxt'>{item}</span>
            }
        },
        {
            dataField: "status",
            text: "Status",
            sort: true,
            formatter: (item: string) => {
                return <span className={`${styles.statuselem} ${styles.processed}`}>{item}</span>
            }
        },
        {
            dataField: "created_date",
            text: "Created Date",
            sort: true,
        },
    ];

    const data = [
        { template_name: 'Order_Confirmation_ENG', status: 'Approved', created_date: '03/08/2025' },
    ]

    const tableOpt = {
        ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
            text: "All",
            value: 20,
        }]
    }
    return (
        <div className={styles.whatsapptempsec}>
            {/* <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Whatsapp Templates</h2>
                    <p className={styles.headerdesctxt}>Manage and track your WhatsApp message templates for campaigns and customer communication.</p>
                </div>
                <div className={styles.tophdrights}>
                    <button className='primarybtn'>Create template</button>
                </div>
            </div> */}
             <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Whatsapp Templates</h2>
                    <p className={styles.headerdesctxt}>Manage and track your WhatsApp message templates for campaigns and customer communication.</p>
                </div>
                <div className='formbtns'>
                    <button className='primarybtn'>Create template</button>
                </div>
            </div>
            <div className='table-wrapper'>
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
            
        </div>
    )
}

export default WhatsAppTemplates
