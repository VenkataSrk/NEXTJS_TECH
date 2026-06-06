import React, { useState } from "react";
import "./scss/stepper.scss";

const steps = [
  { label: "A/B Testing" },
  { label: "Schedule" },
  { label: "Launch" },
];

const CustomStepper = () => {
  const [currentStep, setCurrentStep] = useState(1);

  const handleNext = () => {
    if (currentStep < steps.length) setCurrentStep((prev) => prev + 1);
  };

  const handlePrev = () => {
    if (currentStep > 1) setCurrentStep((prev) => prev - 1);
  };

  return (
    <div className="stepper-container">
      {/* Stepper */}
      <div className="stepper">
        {steps.map((step, index) => {
          const stepNumber = index + 1;
          const isActive = currentStep === stepNumber;
          const isCompleted = currentStep > stepNumber;

          return (
            <div key={index} className="step">
              <div className="step-content">
                <div style={{display:"flex", flexDirection:"column" , alignItems:"center"}}>
                <div
                  className={`circle ${
                    isCompleted
                      ? "completed"
                      : isActive
                      ? "active"
                      : "pending"
                  }`}
                >
                  {isCompleted ? "✔" : stepNumber}
                </div>
                 <span
                className={`label ${
                  isCompleted ? "completed" : isActive ? "active" : "pending"
                }`}
              >
                {step.label}
                </span></div>

                {index < steps.length - 1 && (
                  <div
                    className={`line ${isCompleted ? "completed" : "pending"}`}
                  ></div>
                )}
              </div>

             
            </div>
          );
        })}
      </div>

      

      {/* Navigation */}
      <div className="nav-buttons">
        <button  className='cancelbtn' onClick={handlePrev} disabled={currentStep === 1}>
          Previous
        </button>
        <button  className={`primarybtn`}onClick={handleNext} disabled={currentStep === steps.length}>
          Next
        </button>
      </div>
    </div>
  );
};

export default CustomStepper;
