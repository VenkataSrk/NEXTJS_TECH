import React from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";

const Linechart = ({ data }) => {

  // ✅ check if all values are zero
  const hasOnlyZero = data?.every((d) => Number(d.revenue) === 0);

  return (
    <div style={{ width: "100%", height: 300 }}>
      <ResponsiveContainer>
        <LineChart data={data}>

          <CartesianGrid strokeDasharray="3 3" vertical={false} />

          <XAxis dataKey="month" />

          {/* ✅ FIX 1: force Y-axis range */}
          <YAxis
            tickFormatter={(value) => `£${value}`}
            domain={hasOnlyZero ? [0, 10] : [0, "dataMax + 10"]}
          />

          <Tooltip formatter={(value) => `£${value}`} />

          {/* ✅ FIX 2: show dots */}
          <Line
            type="monotone"
            dataKey="revenue"
            stroke="red"
            strokeWidth={2}
            dot={{ r: 4 }}   // ⭐ important
          />

        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default Linechart;