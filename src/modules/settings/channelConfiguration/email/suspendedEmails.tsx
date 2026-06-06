import React from 'react'
import BootstrapTable from 'react-bootstrap-table-next'
import { tableOptions } from '../../../../components/custom_components/tableOptions';
import paginationFactory from 'react-bootstrap-table2-paginator';

const SuspendedEmails = () => {
  const columns = [
      {
        dataField: "date_time",
        text: "Date & Time",
        sort: true,
      },
      {
        dataField: "subject",
        text: "Subject",
        sort: true,
      },
      {
        dataField: "sender_email",
        text: "Sender Email",
        sort: true,
      },
      {
        dataField: "reason_for_suspension",
        text: "Reason for Suspension",
        sort: true,
      },
    ];
  
    const data = [
      { date_time: '15-07-2025•12:02 PM', subject: 'Order Inquiry',sender_email:'john.doe@example.com',reason_for_suspension:'Detected as spam' },
    ]
  
    const tableOpt = {
      ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
        text: "All",
        value: 20,
      }]
    }
  return (
    <div className='table-wrapper'>
         <div className='desktoptable desktoptablescroll'>
        <BootstrapTable
          keyField="id"
          data={data}
          columns={columns}
          pagination={paginationFactory(tableOpt)}
          // noDataIndication={<NoTableData />}
        />
      </div>
    </div>
  )
}

export default SuspendedEmails
