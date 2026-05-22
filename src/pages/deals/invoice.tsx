import styles from "./scss/deals.module.scss"
import logo from "../../assets/images/deals/worktuallogo.png"
import { useNavigate } from "react-router-dom";

function Invoice() {
    const navigate = useNavigate();


    return (
        <>
         <div className={styles.termscardinvoice}>
                        <div className={styles.invoiceleft}>
                            <svg  style={{position:"relative", top:"4px", cursor:"pointer"}} onClick={() => navigate(-1)} xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none"><path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#404040"></path></svg>
                            <div className={styles.containsdetails}>
                                <div className={styles.quotationtitle}>Quotation</div>
                                <div className={styles.quotationsubtitle}>This section contains the detailed quotation, pricing, and terms for the selected deal.</div>
                            </div>
                        </div>
                        <div className={styles.invoiceright}>
                            <button>Download</button>
                            <button>Send to Customer</button>
                        </div>
                    </div>
            <div className={`${styles.invoicecontainer} maincontainer`}>
                <div className={styles.termsinvoicelead}>
                   
                    <div className={styles.invoice}>


                        <div className={styles.invoiceheader}>
                            <div className={styles.lefts}>
                                <div className={styles.lefttop}>
                                    <img src={logo} />
                                    <div className={styles.namewithdiv}>
                                        <p className="title"> <span></span>Company Name</p>
                                        <p className="content">Enterprise Solutions Provider</p>
                                    </div>

                                </div>

                                <div className={styles.leftbottom}>


                                    <p className="content mt12">123 Business Avenue, Tech Park San Francisco, CA 94105</p>
                                    <p className="content">Phone: +1 (555) 123-4567</p>
                                    <p className="content">Email: sales@acmecorp.com</p></div>
                            </div>


                            <div className={styles.rights}>
                                <p className="content">Quote #: QT-2026-00142</p>
                                <p className="content">Deal #: DEAL-2026-00089</p>
                                <p className="content ">January 6, 2026</p>
                            </div>
                        </div>


                        <section className={styles.details}>
                            <label >Customer Details</label>
                            <div className={styles.contentflex}>
                                <div className={styles.half}>

                                    <p className="innertitle mb12">TechStart Innovations Inc</p>
                                    <p className="boldtxt ">Billing address</p>
                                    <p className="content">456 Innovation Drive, Suite 200</p>
                                    <p className="content">Austin, TX 78701</p>
                                    <p className="content">United States</p>
                                </div>


                                <div className={styles.half}>
                                    <p className="innertitle mb12">Primary Contact</p>
                                    <p className="content">Name: Sarah Mitchell</p>
                                    <p className="content">Title: VP of Sales Operations</p>
                                    <p className="content">Email: sarah.mitchell@techstart.com</p>
                                    <p className="content">Phone: +1 (555) 987-6543</p>
                                </div>
                            </div>

                        </section>
                        <section className={styles.items}>
                            <label style={{ display: "inline-block" }} className="mb12">Items</label>


                            <div className={styles.table}>
                                <div className={styles.rowHead}>
                                    <span>Product / Service</span>
                                    <span>Description</span>
                                    <span>Qty</span>
                                    <span>Unit Price</span>
                                    <span>Subtotal</span>
                                </div>


                                <div className={styles.row}>
                                    <span>Enterprise CRM License</span>
                                    <span>Annual subscription for up to 50 users</span>
                                    <span>1</span>
                                    <span>£2,400.00</span>
                                    <span>£2,400.00</span>
                                </div>


                                <div className={styles.row}>
                                    <span>Implementation & Setup</span>
                                    <span>Configuration, migration, training</span>
                                    <span>40</span>
                                    <span>£150.00</span>
                                    <span>£6,000.00</span>
                                </div>
                            </div>
                        </section>


                        <section className={styles.summary}>
                            <div className={styles.summaryBox}>
                                <div className={styles.header}>
                                    <p className="innertitle ">Financial Summary</p>
                                </div>



                                <div className={styles.summaryRow}>
                                    <span className="content">Subtotal</span>
                                    <span className="content">£17,399.00</span>
                                </div>
                                <div className={styles.summaryRow}>
                                    <span className="content">Discount (10%)</span>
                                    <span className={`${styles.negative} content`}>-£1,739.90</span>
                                </div>
                                <div className={styles.summaryRow}>
                                    <span className="content">VAT (18%)</span>
                                    <span className="content">£0.00</span>
                                </div>


                                <div className={styles.total}>
                                    <span className="title">Total Amount</span>
                                    <span className="title">£15,759.10</span>
                                </div>
                            </div>
                        </section>
                    </div>
                    <div className={styles.termscard}>
                        <label>Terms and condition</label>
                        <div className={styles.inner}>
                            <div className={styles.column}>
                                <h5 className="boldtxt mb12">Validity Period</h5>
                                <p>Valid until February 5, 2026</p>
                                <span style={{ fontSize: "12px" }}>(30 days from issue date)</span>
                            </div>
                            <div className={styles.column}>
                                <h5 className="boldtxt mb12">Payment Terms</h5>
                                <p>Valid until February 5, 2026</p>
                            </div>
                            <div className={styles.column}>
                                <h5 className="boldtxt mb12">Delivery Timeline</h5>
                                <p>Valid until February 5, 2026</p>
                            </div>
                        </div>

                    </div>
                    <div className={`${styles.termscard} ${styles.termscardnew}`}>
                        <div className="innertitle">Acceptance & Consent</div>
                        <div className={styles.block}>
                            <label>Digital signature</label>
                            <input type="text" placeholder="Type your full name to sign" />
                        </div>
                        <label >
                            <input     className="custom-checkbox" type="checkbox" id="subscribe" name="subscribe" value="newsletter" />
                            I hereby accept the terms and conditions of this quotation and authorize its approval, acknowledging that pricing will be locked upon confirmation.
                        </label>
                        <div className="btncontainer">
                            <button className="secondarybtn">Pay now</button>
                            <button>Pay now</button>
                        </div>

                    </div>
                </div>
            </div>
        </>
    )
} export default Invoice;

