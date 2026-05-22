import React, { useEffect, useState } from "react";
import { BarChartComponent, LineChartComponent } from "./chartComponents";

type ChartData = {
  [key: string]: string | number;
};

interface DynamicrechartData {
  dynamicrechartdata?: {
    chartname?: string;
    data?: ChartData[];
  };
}

interface StateData {
  xKey: string;
  yKey: string;
  chartname: string;
  data: ChartData[];
}

function Dynamicrechart({ dynamicrechartdata }: DynamicrechartData) {
  const [keyData, setKeyData] = useState<StateData>({
    data: [],
    xKey: "",
    yKey: "",
    chartname: "",
  });

  useEffect(() => {
    if (dynamicrechartdata?.chartname && dynamicrechartdata?.data?.length) {
      const obj = dynamicrechartdata.data[0];

      if (obj) {
        const keys = Object.keys(obj);

        setKeyData({
          xKey: keys[0] || "",
          yKey: keys[1] || "",
          data: dynamicrechartdata.data,
          chartname: dynamicrechartdata.chartname || "",
        });
      }
    }
  }, [dynamicrechartdata]);

  if (!keyData.chartname || !keyData.data.length) {
    return <></>;
  }

  return (
    <div style={{ width: "100%", height: "100%" }}>

      {/* ✅ LINE CHART */}
      {keyData.chartname === "SimpleLineChart" && (
        <LineChartComponent
          xKey={keyData.xKey}
          yKey={keyData.yKey}
          data={keyData.data}
          color="#ff6a2a"
        />
      )}

      {keyData.chartname === "SimpleBarChart" && (
        <BarChartComponent
          xKey={keyData.xKey}
          yKey={keyData.yKey}
          data={keyData.data}
          color="#5B61F6"
          barCategoryGap={20}
        />
      )}

    </div>
  );
}

export default Dynamicrechart;
