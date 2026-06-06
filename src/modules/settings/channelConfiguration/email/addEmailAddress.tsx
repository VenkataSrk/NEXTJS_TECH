import React from 'react'
import { Link } from 'react-router-dom';
import styles from './scss/addemailadress.module.scss';
import WorktualSelect from '../../../../components/custom_components/WorktualSelect';
import ContentLayout from '../../../../layout/contentLayout';


const AddEmailAddress = () => {
    const selectOpt = [
        { value: 'one', label: 'One' }
    ]
    return (
        <ContentLayout noPaddingBlock={true}>
        <div className={styles.addemailaddresssec}>
            <div className='breadcrumbs'>
                <ol>
                    <li>
                        <Link to='/Email configuration '>API configuration</Link>
                    </li>
                    <li>Add Email Address</li>
                </ol>
            </div>
            <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Set Up Private Domain Email</h2>
                    <p className={styles.headerdesctxt}>Add an email address from your private or non-shared domain for system use.</p>
                </div>
            </div>
            <form className='formsec'>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="">Domain<span className='required'>*</span></label>
                        <span className='formelemdesc'>Select or enter the domain where this email address will be configured.</span>
                    </div>
                    <div className='formelements'>
                        <div>
                            <WorktualSelect placeholder='Enter domain name' onChange={() => { }} options={selectOpt} />
                        </div>
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="">Email<span className='required'>*</span></label>
                        <span className='formelemdesc'>Enter the local part of the email (before @). The domain will be appended automatically.</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="" id="" placeholder='eg: @privatedomain.com' />
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">Ailas name</label>
                        <span className='formelemdesc'>A friendly name to identify this email (e.g., “Support”, “Sales”).</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="" id="ailasname" placeholder='Enter alias' />
                    </div>
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">Bot assistance</label>
                    </div>
                    <div className='formelements'>
                        <div className={styles.botassist}>
                            <span className={styles.bttxt}>Enable your Bot for automated and regenerative assistance</span>
                            <label className="switch">
                                <input
                                    type="checkbox"
                                    name="checkbox"
                                    onChange={() => { }}
                                />
                                <span className="slider round"></span>
                            </label>
                        </div>
                    </div>
                </div>
                <div className='formbtns'>
                    <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Next</button>
                </div>
            </form>
        </div>
        </ContentLayout>
    )
}

export default AddEmailAddress
