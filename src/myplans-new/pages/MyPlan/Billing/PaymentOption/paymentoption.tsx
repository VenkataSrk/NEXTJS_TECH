import React, { useState } from "react";
import styles from "../../../../assets/styles/paymentoption.module.scss";
import mastercard from "../../../../assets/images/myplan/mastercard.svg";

export default function PaymentOption() {

  const [selected, setSelected] = useState<string>("Visa");

  const cards = [
    {
      id: "Visa",
      name: "Visa Card",
      number: "**** **** **** 4567",
      expiry: "12/26",
      image: mastercard,
      primary: "yes"
    },
    {
      id: "MasterCard",
      name: "MasterCard",
      number: "**** **** **** 7890",
      expiry: "09/25",
      image: mastercard,
      primary: "no"
    },
    {
      id: "Amex",
      name: "American Express",
      number: "**** **** **** 1234",
      expiry: "03/27",
      image: mastercard,
      primary: "yes"
    },
  ];

  return (
    <>
      <div className={styles.createmaincontainer}>
        <div className={styles.headerole}>
          <h3>Apps</h3>
          <p>Microcopy</p>
        </div>

        <div className={styles.cardContainer}>
          {cards.map((card) => (
            <div className={styles.paymentoptions}>
              <label key={card.id} className={`${styles.card} ${selected === card.id ? styles.active : ""}`}>
                <div className={styles.cardblock}>
                  <div className={styles.cardblockleft}>
                    <img src={card.image} alt={card.name} className={styles.imgcard} />
                    {/* <input
                    type="radio"
                    name="payment"
                    value={card.id}
                    checked={selected === card.id}
                    onChange={() => setSelected(card.id)}
                  /> */}
                    {/* <span className={styles.customRadio}></span> */}
                    <div className={styles.content}>
                      <div className={styles.contentno}>{card.number}</div>
                      <div className={styles.contentname}>{card.name}</div>
                      <div className={styles.contentexp}>Expires on {card.expiry}</div>
                    </div>
                  </div>
                  {/* <div className={styles.cardblockright}>
                  <svg xmlns="http://www.w3.org/2000/svg" width="6" height="18" viewBox="0 0 6 18" fill="none">
                    <path fill-rule="evenodd" clip-rule="evenodd" d="M3 4.51434C4.2339 4.51434 5.2435 3.50474 5.2435 2.27084C5.2435 1.03694 4.2339 0.0273438 3 0.0273438C1.7661 0.0273438 0.7565 1.03694 0.7565 2.27084C0.7565 3.50474 1.7661 4.51434 3 4.51434ZM3 6.75784C1.7661 6.75784 0.7565 7.76744 0.7565 9.00134C0.7565 10.2352 1.7661 11.2448 3 11.2448C4.2339 11.2448 5.2435 10.2352 5.2435 9.00134C5.2435 7.76744 4.2339 6.75784 3 6.75784ZM0.7565 15.7318C0.7565 14.4979 1.7661 13.4883 3 13.4883C4.2339 13.4883 5.2435 14.4979 5.2435 15.7318C5.2435 16.9657 4.2339 17.9753 3 17.9753C1.7661 17.9753 0.7565 16.9657 0.7565 15.7318Z" fill="black" />
                  </svg>
                </div> */}
                </div>
              </label>
              {card.primary === "yes" &&
                <div className={styles.primary}>
                  <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                    <path d="M7 0.332031C3.31807 0.332031 0.333336 3.31676 0.333336 6.9987C0.333336 10.6806 3.31807 13.6654 7 13.6654C10.6819 13.6654 13.6667 10.6806 13.6667 6.9987C13.6667 3.31683 10.6819 0.332031 7 0.332031ZM7 12.6654C3.8754 12.6654 1.33334 10.1233 1.33334 6.9987C1.33334 3.8741 3.87547 1.33203 7 1.33203C10.1245 1.33203 12.6667 3.8741 12.6667 6.9987C12.6667 10.1233 10.1246 12.6654 7 12.6654Z" fill="black" />
                    <path d="M7.00003 5.24029C7.39646 5.24029 7.71783 4.91892 7.71783 4.52249C7.71783 4.12606 7.39646 3.80469 7.00003 3.80469C6.6036 3.80469 6.28223 4.12606 6.28223 4.52249C6.28223 4.91892 6.6036 5.24029 7.00003 5.24029Z" fill="black" />
                    <path d="M7 6.04297C6.70573 6.04297 6.4668 6.2819 6.4668 6.57617V10.1914C6.4668 10.4857 6.70573 10.7246 7 10.7246C7.29426 10.7246 7.5332 10.4857 7.5332 10.1914V6.57617C7.5332 6.2819 7.29426 6.04297 7 6.04297Z" fill="black" />
                  </svg>This is your default payment method</div>
              }
            </div>
          ))}
        </div>
      </div>
    </>
  );
}
