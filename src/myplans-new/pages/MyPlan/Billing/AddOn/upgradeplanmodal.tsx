import React, { useState } from "react";
import styles from "../../../../assets/styles/upgradeplan.module.scss";

const plans = [
  {
    name: "Growth",
    price: "£64",
    desc: "per agent /month",
    features: [
      "Starter",
      "Call whispering, Barging & Monitoring",
      "Conference Calling",
      "Voice Transcript",
      "Internal chat",
      "Conversational Trigger."
    ],
    btnText: "Renew",
    color: "#3B82F6",
    h4bg:"#3B82F6"
    
  },
  {
    name: "Pro",
    price: "£86",
    desc: "per agent /month",
    features: [
      "Growth +",
      "Omnichannel channel Intelligence",
      "AI powered Routing",
      "Workflow Builder",
      "Agent assist",
      "CRM integration"
    ],
    btnText: "Upgrade",
    color: "#FB923C",
     h4bg: "#FB923C",
  },
  {
    name: "Enterprise",
   
    desc: "Best for many large teams",
    features: [
      "Pro +",
      "Conversational omni channel support",
      "Virtual Agent",
      "Agent-Bot priority management",
      "AI analytics",
      "Conversation to Transaction."
    ],
    btnText: "Contact us",
    color: "#15803D",
    h4bg: "#15803D",
  },
];

export default function UpgradePlan() {
  const [isAnnual, setIsAnnual] = useState(true);

  return (
    <div className={styles.upgradeModal}>
      <h3>Upgrade plan</h3>

      <p className={styles.subtitle}>
        Built for business, priced for people
        <br />
        <span>
          Whether you're serving hundreds or thousands of customers
        </span>
      </p>

      <div className={styles.toggleSection}>
        <span>Monthly</span>
        <label className={styles.switch}>
          <input
            type="checkbox"
            checked={isAnnual}
            onChange={() => setIsAnnual(!isAnnual)}
          />
          <span className={styles.slider}></span>
        </label>
        <span>Annually</span>
      </div>

      <div className={styles.plans}>
        {plans.map((plan, i) => (
          <div
            key={i}
            className={styles.planCard}
            style={{ borderColor: plan.color }}
          >
         
           

            <h4 style={{ color: plan.color , background:plan.h4bg }}>{plan.name}</h4>
               <div className={styles.dtls}>
<div className={styles.content}>
            <p className={styles.price}>
              {plan.price}{" "}
              {plan.price !== "Custom" && <span>{plan.desc}</span>}
            </p>
            {plan.price === "Custom" && (
              <p className={styles.subtext}>{plan.desc}</p>
            )}

            <h5>Key features</h5>
            <ul>
              {plan.features.map((feature, j) => (
                <li key={j}>
                  <span className={styles.check}>✔</span> {feature}
                </li>
              ))}
            </ul>
           </div>
            <button
             
            >
              {plan.btnText}
            </button>
            </div>
          </div>
        ))}
      </div>

      <a href="#" className={styles.compare}>
        Compare plan
      </a>
    </div>
  );
}
