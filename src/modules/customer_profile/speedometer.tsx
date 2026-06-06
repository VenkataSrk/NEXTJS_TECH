import React from "react";
import Chart from "react-apexcharts";
import { ApexOptions } from "apexcharts";

interface GaugeProps {
  value: number; // 0–100
}

const GaugeChart: React.FC<GaugeProps> = ({ value }) => {
  const options: ApexOptions = {
    chart: {
      type: "radialBar",
      sparkline: { enabled: true },
    },
    plotOptions: {
      radialBar: {
        startAngle: -135,
        endAngle: 135,
        hollow: { size: "65%" },
        track: {
          background: "#eee",
          strokeWidth: "100%",
        },
        dataLabels: { show: false },
      },
    },
    fill: {
      type: "gradient",
      gradient: {
        shade: "dark",
        type: "horizontal",
        gradientToColors: ["#F22E57", "#575CFF"],
        stops: [0, 50, 100],
      },
    },
    stroke: { lineCap: "butt" },
  };

  const series = [100];

  // Needle angle: 0 → -135deg, 100 → +135deg
  const angle = (value / 100) * 270 - 135;

  return (
    <div
      style={{
        position: "relative",
        width: "250px",
        height: "250px",
        margin: "auto",
      }}
    >
      <Chart options={options} series={series} type="radialBar" height={250} />
         {/* Needle */}
      <div
        style={{
          position: "absolute",
          top: "30%",
          left: "50%",
          transform: `rotate(${angle}deg)`,
          transformOrigin: "bottom center",
          width: 0,
          height: 0,
          borderLeft: "3px solid transparent", 
          borderRight: "6px solid transparent", 
          borderBottom: "60px solid #575CFF", 
          borderRadius: '12px'  
        }}
      />

    </div>
  );
};

export default GaugeChart;
