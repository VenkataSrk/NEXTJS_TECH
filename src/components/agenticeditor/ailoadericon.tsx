import React from "react";

const Ailoadericon: React.FC = () => {
  const loaderStyle: React.CSSProperties = {
    position: "relative",
    width: "30px",
    height:"30px",
    minWidth: "30px",
    transformStyle: "preserve-3d",
    perspective: "400px",
  };

  const arcStyle: React.CSSProperties = {
    position: "absolute",
    top: 0,
    left: 0,
    width: "100%",
    height: "100%",
    borderRadius: "50%",
    borderBottom: "5px solid #6c00ff",
  };

  const containerStyle: React.CSSProperties = {
    display: "flex",
    justifyContent: "center",
    alignItems: "center",
    margin: 0,
  };

  return (
    <div style={containerStyle}>
      <style>
        {`
          @keyframes rotate1 {
            from { transform: rotateX(35deg) rotateY(-45deg) rotateZ(0); }
            to { transform: rotateX(35deg) rotateY(-45deg) rotateZ(1turn); }
          }
          @keyframes rotate2 {
            from { transform: rotateX(50deg) rotateY(10deg) rotateZ(0); }
            to { transform: rotateX(50deg) rotateY(10deg) rotateZ(1turn); }
          }
          @keyframes rotate3 {
            from { transform: rotateX(35deg) rotateY(55deg) rotateZ(0); }
            to { transform: rotateX(35deg) rotateY(55deg) rotateZ(1turn); }
          }
        `}
      </style>

      <div style={loaderStyle}>
        <div
          style={{
            ...arcStyle,
            animation: "rotate1 1.15s linear infinite",
            animationDelay: "-0.8s",
          }}
        />
        <div
          style={{
            ...arcStyle,
            animation: "rotate2 1.15s linear infinite",
            animationDelay: "-0.4s",
          }}
        />
        <div
          style={{
            ...arcStyle,
            animation: "rotate3 1.15s linear infinite",
            animationDelay: "0s",
          }}
        />
      </div>
    </div>
  );
};

export default Ailoadericon;
