import { Bar, BarChart, CartesianGrid, Line, LineChart, ResponsiveContainer, Tooltip, XAxis, YAxis } from "recharts";
import { getCurrencyByDomain } from "../../base/utils";

function LineChartComponent({ data, xKey = "name", yKey = "value", color }: any) {
    const monthMap: Record<string, string> = {
        "01": "Jan",
        "02": "Feb",
        "03": "Mar",
        "04": "Apr",
        "05": "May",
        "06": "Jun",
        "07": "Jul",
        "08": "Aug",
        "09": "Sep",
        "10": "Oct",
        "11": "Nov",
        "12": "Dec",
    };
    const formatMonth = (value: any) => {
        return monthMap[value] || value;
    };

    return (
        <ResponsiveContainer width="100%" height="100%">
            <LineChart
                data={data}
                margin={{}}
            >
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis
                    dataKey={xKey}
                    interval={0}
                    padding={{ left: 0, right: 20 }}
                    label={{
                        position: "insideBottom",
                        offset: -10,
                        fontWeight: "bold",
                    }}
                    //   height={150}
                    angle={-45}
                    textAnchor="end"
                    tickFormatter={formatMonth}
                />
                <YAxis allowDecimals={false} />
                <Tooltip labelFormatter={formatMonth} />
                <Line type="monotone" dataKey={yKey} stroke={color} strokeWidth={2} />
            </LineChart>
        </ResponsiveContainer>
    );
}

function BarChartComponent({
    data,
    xKey = "name",
    yKey = "value",
    color = "#5B61F6",
    barCategoryGap = 20,
    height = 260,
    yDomain,
}: any) {
    const currency = getCurrencyByDomain();
    const compact = new Intl.NumberFormat("en-GB", {
        notation: "compact",
        maximumFractionDigits: 1,
    });
    const MAX_CHARS_PER_LINE = 14;
    const MAX_LINES = 2;

    const CustomXAxisTick = ({ x, y, payload }: any) => {
        const raw = String(payload?.value ?? "");
        const words = raw?.split(/\s+/).filter(Boolean);
        const lines: string[] = [];

        let current = "";
        for (const word of words) {
            const next = current ? `${current} ${word}` : word;
            if (next.length <= MAX_CHARS_PER_LINE) {
                current = next;
            } else {
                if (current) lines.push(current);
                current = word;
            }
        }
        if (current) lines?.push(current);

        const sliced = lines?.slice(0, MAX_LINES);
        if (lines.length > MAX_LINES) {
            sliced[MAX_LINES - 1] = `${sliced[MAX_LINES - 1]}...`;
        }

        return (
            <g transform={`translate(${x},${y + 10})`}>
                <text textAnchor="middle" fill="#666" fontSize={12}>
                    {sliced?.map((line, index) => (
                        <tspan key={index} x="0" dy={index === 0 ? 0 : 14}>
                            {line}
                        </tspan>
                    ))}
                </text>
            </g>
        );
    };

    return (
        <ResponsiveContainer width="100%" height={height}>
            <BarChart data={data} barCategoryGap={barCategoryGap} margin={{ top: 10, bottom: 20 }}>
                <CartesianGrid strokeDasharray="3 3" vertical={false} />
                <XAxis
                    dataKey={xKey}
                    tickLine={false}
                    axisLine={false}
                    interval={0}
                    tick={<CustomXAxisTick />}
                />
                <YAxis
                    tickLine={false}
                    axisLine={false}
                    domain={yDomain}
                    tickFormatter={(v: any) => `${currency}${compact.format(Number(v) || 0)}`}
                />
                <Tooltip formatter={(v: any) => `${currency}${Number(v || 0).toLocaleString()}`} />
                <Bar dataKey={yKey} fill={color} radius={[6, 6, 0, 0]} />
            </BarChart>
        </ResponsiveContainer>
    );
}


export {
    LineChartComponent,
    BarChartComponent,
};  
