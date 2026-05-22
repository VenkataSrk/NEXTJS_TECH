import React from "react";
import { PieChart, Pie, Cell, Tooltip, ResponsiveContainer, Label } from "recharts";

type DonutChartItem = {
  name: string;
  value: number;
};

type DonutChartProps = {
  data?: DonutChartItem[];
  colors?: string[];
  centerText?: string;
};

const defaultColors = ["#0088FE", "#00C49F", "#FFBB28", "#FF8042"];

const DonutChart = ({ data = [], colors = defaultColors, centerText }: DonutChartProps) => {
  return (
    <ResponsiveContainer width={300} height={250}>
      <PieChart>
        <Pie
          data={data}
          cx="50%"
          cy="50%"
          innerRadius={60}
          outerRadius={90}
          paddingAngle={3}
          dataKey="value"
        >
          {data.map((entry, index) => (
            <Cell key={`cell-${entry.name}-${index}`} fill={colors[index] || colors[colors.length - 1]} />
          ))}
          {centerText ? (
            <Label
              value={centerText}
              position="center"
              fill="#5E5E69"
              style={{ fontSize: "14px", fontWeight: 500 }}
            />
          ) : null}
        </Pie>
        <Tooltip />
      </PieChart>
    </ResponsiveContainer>
  );
};

export default DonutChart;
