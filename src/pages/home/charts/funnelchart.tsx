import React from "react";
import {
  ResponsiveContainer,
  FunnelChart,
  Funnel,
  LabelList,
  Tooltip,
  Cell
} from "recharts";

const Funnelchart = ({ data }) => {
  const colors = ["#7423D7", "#8650DE", "#9A6BFA", "#B894FF", "#E6D8FF"];

  return (
    <ResponsiveContainer width={320} height={250}>
      <FunnelChart>
        <Tooltip />

        <Funnel
          dataKey="value"
          data={data}
          isAnimationActive={true}
          stroke="#fff"
          lastShapeType="rectangle"
        >
          {/* Apply colors here */}
          {data.map((entry, index) => (
            <Cell
              key={`cell-${index}`}
              fill={colors[index] || colors[colors.length - 1]}
            />
          ))}

          <LabelList
            dataKey="name"
            position="right"
            fill="#000"
          />
        </Funnel>
      </FunnelChart>
    </ResponsiveContainer>
  );
};

export default Funnelchart;