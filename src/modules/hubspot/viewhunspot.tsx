import React from 'react'
import styles from "./scss/hubspot.module.scss"
import { Link } from 'react-router-dom'

function Viewhubspot() {
  return (
    <>
      <div className='breadcrumbs'>
                        <ol>
                            <li>
                                <Link to=''>Connectors</Link>
                            </li>
                            <li>HubSpot</li>
                        </ol>
                    </div>
                    <div className={styles.topheading}>
                        <div className={styles.tophdlefts}>
                            <h2>Connect with HubSpot account</h2>
                            <p className={styles.headerdesctxt}>Enter Your connect name and API key to connect to your Worktual account</p>
                        </div>
                        <div className={styles.tophdrights}>
                                 <div className='formbtns'>
                            <button className='cancelbtn'>Cancel</button>
                            <button className='primarybtn'>Submit</button>
                        </div>
                        </div>
                    </div>
     <div className='viewtablecontainer'>
                   <div className='row'>
                        <div className='td first'>
                            <div className={styles.viewrow}>
                       <span className={styles.boldtxt}> Email</span>
                       <div>
 <span>Lat sync: now </span><span className={`${styles.statuselem} ${styles.processed}`}>active</span>
                       </div>
                        
                            </div>
   
                        </div>
                         <div className='td second'>

                        </div>
                         <div className='td last'>
<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.847607 11.6649C0.655207 11.6649 0.462808 11.5917 0.316408 11.4442C0.0239078 11.1512 0.0249076 10.6766 0.318408 10.3837L4.18271 6.53116C4.32481 6.38956 4.40291 6.20006 4.40291 5.99896C4.40291 5.79876 4.32481 5.61026 4.18271 5.46866L0.318008 1.61516C0.025008 1.32216 0.0241079 0.84756 0.316508 0.55466C0.608508 0.26076 1.08361 0.26076 1.37751 0.55266L5.24181 4.40616C5.66761 4.83096 5.90241 5.39636 5.90291 5.99896C5.90341 6.60156 5.66851 7.16696 5.24181 7.59366L1.37711 11.4462C1.23111 11.5917 1.03901 11.6649 0.847607 11.6649Z" fill="black"/>
</svg>
                        </div>
                   </div>
                     <div className='row'>
                        <div className='td first'>
                         SMS
                        </div>
                         <div className='td second'>

                        </div>
                         <div className='td last'>
<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.847607 11.6649C0.655207 11.6649 0.462808 11.5917 0.316408 11.4442C0.0239078 11.1512 0.0249076 10.6766 0.318408 10.3837L4.18271 6.53116C4.32481 6.38956 4.40291 6.20006 4.40291 5.99896C4.40291 5.79876 4.32481 5.61026 4.18271 5.46866L0.318008 1.61516C0.025008 1.32216 0.0241079 0.84756 0.316508 0.55466C0.608508 0.26076 1.08361 0.26076 1.37751 0.55266L5.24181 4.40616C5.66761 4.83096 5.90241 5.39636 5.90291 5.99896C5.90341 6.60156 5.66851 7.16696 5.24181 7.59366L1.37711 11.4462C1.23111 11.5917 1.03901 11.6649 0.847607 11.6649Z" fill="black"/>
</svg>
                        </div>
                   </div>
                     <div className='row'>
                        <div className='td first'>
                         WhatsApp
                        </div>
                         <div className='td second'>

                        </div>
                         <div className='td last'>
<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
  <path d="M0.847607 11.6649C0.655207 11.6649 0.462808 11.5917 0.316408 11.4442C0.0239078 11.1512 0.0249076 10.6766 0.318408 10.3837L4.18271 6.53116C4.32481 6.38956 4.40291 6.20006 4.40291 5.99896C4.40291 5.79876 4.32481 5.61026 4.18271 5.46866L0.318008 1.61516C0.025008 1.32216 0.0241079 0.84756 0.316508 0.55466C0.608508 0.26076 1.08361 0.26076 1.37751 0.55266L5.24181 4.40616C5.66761 4.83096 5.90241 5.39636 5.90291 5.99896C5.90341 6.60156 5.66851 7.16696 5.24181 7.59366L1.37711 11.4462C1.23111 11.5917 1.03901 11.6649 0.847607 11.6649Z" fill="black"/>
</svg>
                        </div>
                   </div>
                </div>
    </>
  )
}

export default Viewhubspot