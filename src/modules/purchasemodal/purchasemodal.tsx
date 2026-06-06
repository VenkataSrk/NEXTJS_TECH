import React from 'react'
import styles from "./scss/purchasemodal.module.scss"
import WorktualModal from '../../components/custom_components/worktualModal'
import Elevatebrand from '../home/campaign/createcampaign'
import Chatcomponent from '../../components/chatcomponent/chatcomponent'
import { assets } from '../../styles/assets/assets'

function Purchasemodal() {
  console.log("purchase modal")
  return (
    <div className='closenonemdal'>
      <WorktualModal
        show={true}
        mdsize='modalxxl'
      >
        <>
          <Chatcomponent
            headerTitle={
              <div >
                <h5>Welcome aboard! Lets Customise Your flow</h5>
                <span style={{ fontSize: "13px", color: "#404040", fontFamily: "interregular", fontWeight: "400" }}>
                </span>
              </div>
            }
            rightIcon={assets.threedot}
          />
        </>
      </WorktualModal>
    </div>
  )
}

export default Purchasemodal;