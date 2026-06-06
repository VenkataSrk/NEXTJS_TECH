
import styles from './scss/hubspot.module.scss';
import { assets } from '../../styles/assets/assets';



const Mapvalue = () => {

    const parametersMapping: any = [
        {
            "worktualParams": "firstName",
            "connectorsParams": "firstname"
        },
        {
            "worktualParams": "lastName",
            "connectorsParams": "lastname"
        },
        {
            "worktualParams": "primaryPhoneNumber",
            "connectorsParams": "phone"
        },
        {
            "worktualParams": "primaryEmail",
            "connectorsParams": "email"
        },

    ];

    return (
        <>
            <div className={styles.mainheader}>
                <h5>Hubspot to Campaign management</h5>
                <p>Microcopy for Hubspot to Campaign management</p>
            </div>
            <div className={`${styles.parameterssec} ${styles.parasechunspot}`}>

                <div className={styles.paramslefts}>
                    <h4 className={styles.headertxt}>Contacts</h4>
                    <p className={styles.subtxt}>Microcopy for Hubspot to Campaign management</p>
                    <div className={`${styles.paramsrow} ${styles.paramsrowheader}`}>
                        <div className={styles.parmbox}>
                            <h5>
                                HubSpot
                            </h5>
                        </div>
                        <div className={styles.parmbox}>
                            <h5>
                                Campaign Management
                            </h5>
                        </div>

                    </div>
                    {parametersMapping.map((data: any, indx: number) => {
                        return (
                            <div key={indx}>
                                <div className={styles.paramsrow}>
                                    <div className={styles.parmbox}>
                                        {data.worktualParams}
                                    </div>
                                    <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                                    <div className={styles.parmbox}>
                                        {data.connectorsParams}
                                    </div>
                                </div>
                            </div>
                        )

                    })}
                    {/* 
                    <div className={styles.paramsrow}>
                        <div className={styles.parmbox}>
                            lastname
                        </div>
                        <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                        <div className={styles.parmbox}>
                            lastName
                        </div>
                    </div>
                    <div className={styles.paramsrow}>
                        <div className={styles.parmbox}>
                            email
                        </div>
                        <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                        <div className={styles.parmbox}>
                            email
                        </div>
                    </div>
                    <div className={styles.paramsrow}>
                        <div className={styles.parmbox}>
                            phone
                        </div>
                        <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                        <div className={styles.parmbox}>
                            phoneNumber
                        </div>
                    </div> */}


                </div>


            </div>
            {/* <div className={`${styles.parameterssec} ${styles.parasechunspot}`}>
          
            <div className={styles.paramslefts}>
                <h4 className={styles.headertxt}>Contacts</h4>
                <p className={styles.subtxt}>Microcopy for Hubspot to Campaign management</p>
                <div className={`${styles.paramsrow} ${styles.paramsrowheader}`}>
                     <div className={styles.parmbox}>
                    <h5>
                          HubSpot 
                    </h5>
                    </div>
                    <div className={styles.parmbox}>
                         <h5>
                   Campaign Management
                    </h5>
                    </div>
                   
                </div>
                <div className={styles.paramsrow}>
                    <div className={styles.parmbox}>
                         Name
                    </div>
                    <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                    <div className={styles.parmbox}>
                        Scarlett
                    </div>
                </div>
                <div className={styles.paramsrow}>
                    <div className={styles.parmbox}>
                      Email
                    </div>
                    <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                    <div className={styles.parmbox}>
                        12345
                    </div>
                </div>
                <div className={styles.paramsrow}>
                    <div className={styles.parmbox}>
                        Source
                    </div>
                    <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                    <div className={styles.parmbox}>
                        20/08/2025
                    </div>
                </div>

             
            </div>
            
        </div> */}

        </>
    )
}

export default Mapvalue