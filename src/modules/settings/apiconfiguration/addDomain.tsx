import React from 'react'
import styles from './scss/adddomain.module.scss';
import WorktualSelect from '../../../components/custom_components/WorktualSelect';
import { assets } from '../../../styles/assets/assets';
import { Link } from 'react-router-dom';
import ContentLayout from '../../../layout/contentLayout';
import WorktualModal from '../../../components/custom_components/worktualModal';

const AddDomain = () => {
    const selectOpt = [
        { value: 'one', label: 'One' }
    ]
    return (
        <ContentLayout noPaddingBlock={true} >
            <div className={styles.adddomainsec}>
                <div className='breadcrumbs'>
                    <ol>
                        <li>
                            <Link to=''>API configuration</Link>
                        </li>
                        <li>
                            Add Domain
                        </li>
                    </ol>
                </div>
                <div className={styles.topheading}>
                    <div className={styles.tophdlefts}>
                        <h2>Domain Details</h2>
                        <p className={styles.headerdesctxt}>Configure your domain to securely connect external data sources for building a unified customer view.</p>
                    </div>
                </div>
                <div className='formsec'>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="domainname">Domain Name <span className='required'>*</span></label>
                            <span className='formelemdesc'>Enter a unique name to identify this domain in your integrations list.</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="" id="domainname" placeholder='e.g., Marketing Data API' />
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="description">Description<span className='required'>*</span></label>
                            <span className='formelemdesc'>Provide a short description of the domain’s purpose or the type of data it will supply.</span>
                        </div>
                        <div className='formelements'>
                            <textarea name="" id="description" placeholder='e.g., Connects ad campaign performance data to CVM'></textarea>
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="description">Authentication type<span className='required'>*</span></label>
                            <span className='formelemdesc'>Select how this domain will authenticate data requests.</span>
                        </div>
                        <div className='formelements'>
                            <div>
                                <WorktualSelect onChange={() => { }} options={selectOpt} />
                            </div>
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="header">Header<span className='required'>*</span></label>
                            <span className='formelemdesc'>Enter the authentication header key.</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="" id="header" placeholder='e.g., Authorization' />
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="value">Value<span className='required'>*</span></label>
                            <span className='formelemdesc'>Enter the authentication header value.</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="" id="value" placeholder='e.g., Bearer abc123token' />
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="domainurl">Domain URL<span className='required'>*</span></label>
                            <span className='formelemdesc'>Enter the base domain URL, starting with https://</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="" id="domainurl" placeholder='e.g., https://api.companyname.com' />
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="description">Add End point details</label>
                            <span className='formelemdesc'>Add the specific API endpoints you want to connect.</span>
                        </div>
                        <div className='formelements'>
                            <div className={styles.endpointgroups}>
                                <div className={styles.apiendpoint}>/api/campaigns/&#x7B; id &#x7D;/analytics</div>
                                <div className={styles.apiendpoint}>/api/campaigns/&#x7B; id &#x7D;/analytics</div>
                                <div className={styles.addendpoint}><img src={assets.pluscircle} alt="pluscircle" />Add End point URL</div>
                            </div>
                        </div>
                    </div>
                    <div className='formbtns'>
                        <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Save</button>
                    </div>
                </div>
            </div>
            <WorktualModal
                show={false}
                title='Add Endpoint Details'
                footer={
                    <div className={styles.footerbtns}>
                        <button className='cancelbtn'>Cancel</button>
                        <button className='primarybtn'>Save</button>
                    </div>
                }
            >
                <form className='formsec' style={{maxWidth:'100%'}}>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Endpoint Name<span className='required'>*</span></label>
                            <span className='formelemdesc'>Use descriptive names to easily identify the endpoint later.</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="" id="" placeholder='Enter a clear, unique name' />
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Description<span className='required'>*</span></label>
                            <span className='formelemdesc'>Provide a short summary of this endpoint’s purpose.</span>
                        </div>
                        <div className='formelements'>
                            <textarea name="" id="" placeholder='Enter the description	'></textarea>
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Request Method<span className='required'>*</span></label>
                            <span className='formelemdesc'>Select the HTTP method (GET, POST, PUT, DELETE). <br />
                                💡 Choose based on the type of operation this endpoint performs.</span>
                        </div>
                        <div className='formelements'>
                            <div>
                                <WorktualSelect placeholder='Select a request method' options={selectOpt} onChange={() => { }} />
                            </div>
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Endpoint URL<span className='required'>*</span></label>
                            <span className='formelemdesc'>Enter the full API URL, including the protocol (http/https).</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="" id="" placeholder='https://api.example.com/v1/users' />
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Keywords<span className='required'>*</span></label>
                            <span className='formelemdesc'>Add tags to make this endpoint easier to find. <br />
                                💡 Use Enter, Space, Comma, or Hyphen to add multiple keywords. <br />
                                Example: Recharge, Mobile Credit, Balance, Prepaid, Refill, Pay Bill, Top-up</span>
                        </div>
                        <div className='formelements'>
                           <div className={styles.manualmultiselect}>
                            <span className={styles.floattxt}>Enter name</span>
                                <input type="text" name="" id="" />
                                <div className={styles.multiselvalues}>
                                    <p className={styles.multielem}><span>Recharge</span><img src={assets.multi_close} alt="multi_close" /></p>
                                </div>
                           </div>
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Parameters</label>
                            <span className='formelemdesc'>Mapping parameters ensures consistent functionality across applications.</span>
                        </div>
                        <div className='formelements'>
                            <div className={styles.reqsec}>
                                <div className={styles.requiredrow}>
                                    <span className={styles.reqelem}><input type="checkbox" name="" id="req1" /><label htmlFor="req1">Required</label></span> <input type="text" name="" id="" placeholder='First name' />
                                </div>
                                <div className={styles.requiredrow}>
                                    <span className={styles.reqelem}><input type="checkbox" name="" id="req2" /><label htmlFor="req2">Required</label></span> <input type="text" name="" id="" placeholder='Email Id' />
                                </div>
                            </div>
                        </div>
                    </div>
                </form>
            </WorktualModal>
        </ContentLayout>
    )
}

export default AddDomain
