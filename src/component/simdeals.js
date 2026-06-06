import React, { useState } from "react";
import "../../src/chatPreview.scss";
import offericon1 from "../images/offericon1.png";
import offericon2 from "../images/offericon2.png";
import simdownarrow from "../images/simdownarrow.png";
import simuparrow from "../images/simuparrow.png";

const SIMDeals = (props) => {
  const [arrow, setArrow] = useState(false);
  const [currentIndex, setCurrentIndex] = useState(0);

  const arrowClick = () => {
    setArrow(!arrow)
  }

  const handleNext = () => {
    if (currentIndex < props?.b2cSimData?.length - 1) {
      setCurrentIndex(currentIndex + 1);
    }
  };

  const handlePrev = () => {
    if (currentIndex > 0) {
      setCurrentIndex(currentIndex - 1);
    }
  };


  return (
    <>
      <div className="simblockmain">
        {/* <svg width="63" height="81" viewBox="0 0 63 81" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handlePrev}
          className={`${currentIndex === 0 ? "disable" : ""}`}>
          <g filter="url(#filter0_dd_2415_135155)">
            <rect x="5.5" y="0.377319" width="40" height="40" rx="20" fill="white" />
            <path d="M27.6533 26.0428C27.4619 26.0428 27.27 25.9696 27.1235 25.824L23.2588 21.9705C22.833 21.5457 22.5982 20.9803 22.5977 20.3777C22.5972 19.7751 22.8321 19.2097 23.2588 18.783L27.124 14.9305C27.417 14.6385 27.8921 14.6385 28.1845 14.9325C28.477 15.2255 28.476 15.7001 28.1825 15.993L24.3178 19.8455C24.1757 19.9871 24.0976 20.1766 24.0976 20.3777C24.0976 20.5779 24.1757 20.7664 24.3178 20.908L28.183 24.7615C28.476 25.0545 28.4769 25.5291 28.1845 25.822C28.038 25.9695 27.8456 26.0428 27.6533 26.0428Z" fill="#757676" />
          </g>
          <defs>
            <filter id="filter0_dd_2415_135155" x="-14.5" y="0.377319" width="80" height="80" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
              <feFlood flood-opacity="0" result="BackgroundImageFix" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect1_dropShadow_2415_135155" />
              <feOffset dy="10" />
              <feGaussianBlur stdDeviation="5" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.04 0" />
              <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_2415_135155" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect2_dropShadow_2415_135155" />
              <feOffset dy="20" />
              <feGaussianBlur stdDeviation="12.5" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
              <feBlend mode="normal" in2="effect1_dropShadow_2415_135155" result="effect2_dropShadow_2415_135155" />
              <feBlend mode="normal" in="SourceGraphic" in2="effect2_dropShadow_2415_135155" result="shape" />
            </filter>
          </defs>
        </svg> */}

        <svg width="46" height="46" viewBox="0 0 46 46" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handlePrev}
          className={`${currentIndex === 0 ? "disable" : ""}`}>
          <g filter="url(#filter0_dd_3515_117502)">
            <rect x="41" y="37" width="36" height="36" rx="18" transform="rotate(-180 41 37)" fill="#F8F8F8" />
            <rect x="40.5" y="36.5" width="35" height="35" rx="17.5" transform="rotate(-180 40.5 36.5)" stroke="#BDBDBD" />
            <g clip-path="url(#clip0_3515_117502)">
              <path d="M24.8128 14.2291C24.9748 14.2291 25.1368 14.2907 25.2601 14.4149C25.5064 14.6617 25.5056 15.0613 25.2584 15.308L22.0043 18.5522C21.8846 18.6714 21.8189 18.831 21.8189 19.0004C21.8189 19.169 21.8846 19.3277 22.0043 19.4469L25.2588 22.692C25.5055 22.9387 25.5063 23.3384 25.26 23.585C25.0141 23.8325 24.6141 23.8325 24.3666 23.5867L21.1124 20.3417C20.7539 19.9839 20.5561 19.5078 20.5557 19.0004C20.5553 18.4929 20.7531 18.0168 21.1124 17.6575L24.3669 14.4133C24.4899 14.2907 24.6516 14.2291 24.8128 14.2291Z" fill="black" />
            </g>
          </g>
          <defs>
            <filter id="filter0_dd_3515_117502" x="0" y="0" width="46" height="46" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
              <feFlood flood-opacity="0" result="BackgroundImageFix" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="1" operator="erode" in="SourceAlpha" result="effect1_dropShadow_3515_117502" />
              <feOffset dy="2" />
              <feGaussianBlur stdDeviation="2" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.06 0" />
              <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_3515_117502" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="1" operator="erode" in="SourceAlpha" result="effect2_dropShadow_3515_117502" />
              <feOffset dy="4" />
              <feGaussianBlur stdDeviation="3" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
              <feBlend mode="normal" in2="effect1_dropShadow_3515_117502" result="effect2_dropShadow_3515_117502" />
              <feBlend mode="normal" in="SourceGraphic" in2="effect2_dropShadow_3515_117502" result="shape" />
            </filter>
            <clipPath id="clip0_3515_117502">
              <rect width="20.2105" height="20.2105" fill="white" transform="matrix(-1 0 0 -1 33.1052 29.1052)" />
            </clipPath>
          </defs>
        </svg>


        {props?.b2cSimData?.map((sim, index) => (
          <div
            className={`simblock ${currentIndex === index ? "active" : "inactive"
              }`}
          >
            <div className="simblocktop">
              <div className="simblocktopinner">
                <p>Contract Length</p>
                <h4>{sim?.contractLength === 1 ? '30 days' : `${sim?.contractLength} Months`}</h4>
              </div>
              <div className="simblocktopinner">
                <p>Monthly Cost</p>
                <h4>£ {parseFloat(sim?.monthlyCost).toFixed(2)}</h4>
              </div>
              <div className="simblocktopinner">
                <p>Data</p>
                <h4>{sim?.data === '999999' ? sim?.dataMetric : `${sim?.data} ${sim?.dataMetric}`}</h4>
              </div>
            </div>

            <div className="simblocktopnew">
              <div className="simblocktopinnermid">
                <p>What you get</p>
                <div className="simpro">
                  {sim?.callMinutes !== null &&
                    <div className="simproinner">Calls-{sim?.callMinutes}</div>
                  }
                  {sim?.texts !== null &&
                    <div className="simproinner">Texts-{sim?.texts}</div>
                  }
                  {sim?.perks?.includes('EU') &&
                    <div className="simproinner">EU roaming included</div>
                  }
                </div>
              </div>
            </div>
            {(sim?.critique !== null || sim?.offers !== null) &&
              <div className="simblocktopnewone">
                <div
                  className="simblocktopinnermidmain"
                  onClick={arrowClick}
                >
                  <div className="simblocktopinnermid">
                    {sim?.offers !== null &&
                      <p>
                        <img src={offericon1} alt="" /> Offers Available
                      </p>
                    }
                    {sim?.critique !== null &&
                      <p>
                        <img src={offericon2} alt="" /> Reality Check Available
                      </p>
                    }
                  </div>
                  <img
                    src={arrow ? simuparrow : simdownarrow}
                    alt=""
                  />
                </div>

                {arrow && (
                  <div className="simblockbottombottom">
                    {sim?.offers !== null &&
                      <div className="simblockbottombottomright">
                        <h4>Offers</h4>
                        <p>{sim?.offers}</p>
                      </div>
                    }
                    {sim?.critique !== null &&
                      <div className="simblockbottombottomleft">
                        <h4>Reality Check by BeatMyShop</h4>
                        <p>{sim?.critique}</p>
                      </div>
                    }
                  </div>
                )}
              </div>
            }

            <div className="simblockbottom">
              <div className="simblockbottomtop">
                <div className="simblockbottomleft">
                  <img src={sim?.businessLogo} alt="" />
                  <h4>{sim?.companyName}</h4>
                </div>
                {/* <div className="simblockbottomright">
                  <button onClick={() => window.open(sim?.websiteURL, "_blank")}>Get Deal</button>
                </div> */}
              </div>
            </div>
          </div>
        ))};
        {/* <svg width="63" height="81" viewBox="0 0 63 81" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handleNext} className={`${props?.b2cSimData?.length === currentIndex + 1 ? "disable" : ""}`}>
          <g filter="url(#filter0_dd_2429_135185)">
            <rect x="8.5" y="0.877319" width="40" height="40" rx="20" fill="white" />
            <path d="M26.3476 26.5427C26.1552 26.5427 25.9628 26.4695 25.8164 26.322C25.5239 26.029 25.5249 25.5544 25.8184 25.2615L29.6827 21.409C29.8248 21.2674 29.9029 21.0779 29.9029 20.8768C29.9029 20.6766 29.8248 20.4881 29.6827 20.3465L25.818 16.493C25.525 16.2 25.5241 15.7254 25.8165 15.4325C26.1085 15.1386 26.5836 15.1386 26.8775 15.4305L30.7418 19.284C31.1676 19.7088 31.4024 20.2742 31.4029 20.8768C31.4034 21.4794 31.1685 22.0448 30.7418 22.4715L26.8771 26.324C26.7311 26.4695 26.539 26.5427 26.3476 26.5427Z" fill="#757676" />
          </g>
          <defs>
            <filter id="filter0_dd_2429_135185" x="-11.5" y="0.877319" width="80" height="80" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
              <feFlood flood-opacity="0" result="BackgroundImageFix" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect1_dropShadow_2429_135185" />
              <feOffset dy="10" />
              <feGaussianBlur stdDeviation="5" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.04 0" />
              <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_2429_135185" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="5" operator="erode" in="SourceAlpha" result="effect2_dropShadow_2429_135185" />
              <feOffset dy="20" />
              <feGaussianBlur stdDeviation="12.5" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
              <feBlend mode="normal" in2="effect1_dropShadow_2429_135185" result="effect2_dropShadow_2429_135185" />
              <feBlend mode="normal" in="SourceGraphic" in2="effect2_dropShadow_2429_135185" result="shape" />
            </filter>
          </defs>
        </svg> */}
        <svg width="46" height="46" viewBox="0 0 46 46" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handleNext} className={`${props?.b2cSimData?.length === currentIndex + 1 ? "disable" : ""}`}>
          <g filter="url(#filter0_dd_3515_117504)">
            <rect x="5" y="1" width="36" height="36" rx="18" fill="#F8F8F8" />
            <rect x="5.5" y="1.5" width="35" height="35" rx="17.5" stroke="#CCCCCC" />
            <g clip-path="url(#clip0_3515_117504)">
              <path d="M21.1872 23.7709C21.0252 23.7709 20.8632 23.7093 20.7399 23.5851C20.4936 23.3383 20.4944 22.9387 20.7416 22.692L23.9957 19.4478C24.1154 19.3286 24.1811 19.169 24.1811 18.9996C24.1811 18.831 24.1154 18.6723 23.9957 18.5531L20.7412 15.308C20.4945 15.0613 20.4937 14.6616 20.74 14.415C20.9859 14.1675 21.3859 14.1675 21.6334 14.4133L24.8876 17.6583C25.2461 18.0161 25.4439 18.4922 25.4443 18.9996C25.4447 19.5071 25.2469 19.9832 24.8876 20.3425L21.6331 23.5867C21.5101 23.7093 21.3484 23.7709 21.1872 23.7709Z" fill="black" />
            </g>
          </g>
          <defs>
            <filter id="filter0_dd_3515_117504" x="0" y="0" width="46" height="46" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
              <feFlood flood-opacity="0" result="BackgroundImageFix" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="1" operator="erode" in="SourceAlpha" result="effect1_dropShadow_3515_117504" />
              <feOffset dy="2" />
              <feGaussianBlur stdDeviation="2" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.06 0" />
              <feBlend mode="normal" in2="BackgroundImageFix" result="effect1_dropShadow_3515_117504" />
              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
              <feMorphology radius="1" operator="erode" in="SourceAlpha" result="effect2_dropShadow_3515_117504" />
              <feOffset dy="4" />
              <feGaussianBlur stdDeviation="3" />
              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
              <feBlend mode="normal" in2="effect1_dropShadow_3515_117504" result="effect2_dropShadow_3515_117504" />
              <feBlend mode="normal" in="SourceGraphic" in2="effect2_dropShadow_3515_117504" result="shape" />
            </filter>
            <clipPath id="clip0_3515_117504">
              <rect width="20.2105" height="20.2105" fill="white" transform="translate(12.8948 8.89478)" />
            </clipPath>
          </defs>
        </svg>

      </div>
    </>
  );
};

export default SIMDeals;
