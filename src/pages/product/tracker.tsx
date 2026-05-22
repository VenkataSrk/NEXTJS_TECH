import React from "react";
import styles from "./scss/tracker.module.scss"
import chartimg from "../../assets/images/product/trendchartimg.svg"
import {
    LineChart,
    Line,
    XAxis,
    YAxis,
    CartesianGrid,
    ResponsiveContainer
} from "recharts";
import {
    PieChart,
    Pie,
    Cell,

} from "recharts";
function Tracker() {
    const dataline = [
        { month: "Jan", value: 15000 },
        { month: "Feb", value: 20000 },
        { month: "Mar", value: 22000 },
        { month: "Apr", value: 20000 },
        { month: "May", value: 18000 },
        { month: "Jun", value: 19000 },
        { month: "Jul", value: 25000 },
        { month: "Aug", value: 27000 },
        { month: "Sep", value: 25000 },
        { month: "Oct", value: 23000 },
        { month: "Nov", value: 26000 },
        { month: "Dec", value: 30000 }
    ];
    const data = [
        { name: "Not Renewed", value: 26, color: "#4C84FF" },
        { name: "Renewed", value: 74, color: "#D63CE3" }
    ];
    return (<>
        <div className={styles.trackerbodyinner}>
            <div className={styles.overalltrackercard}>
                <div className={styles.header}>
                    <div className={styles.lefts}>
                        <img src={chartimg} />
                    </div>
                    <div className={styles.rights}>
                        <div className="subtitle">
                            Revenue Tracking
                        </div>
                        <div className="content">
                            Short descriptive sentence explaining revenue
                        </div>
                    </div>
                </div>
                <div className={styles.card}>
                    <div className={styles.pricetxt}>
                        £297,400
                    </div>
                    <div className={styles.bottom}>
                        <div className={"boldtxt"}>
                            Total Revenue (2024)
                        </div>
                        <p>Revenue performance shows strong upward momentum, with a 12.5% increase compared to last month, indicating healthy growth and improved monetization efficiency.</p>
                    </div>
                </div>
                <div className={styles.cardflex}>
                    <div className={styles.card}>
                        <div className="subtitle">
                            08
                        </div>
                        <div className={styles.bottomlayer}>
                            <p className="boldtxt">Active Products</p>
                            <span className={styles.link}>View all <span><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="none">
                                <path d="M3.3335 8H12.6668" stroke="#7423D7" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M8 3.33331L12.6667 7.99998L8 12.6666" stroke="#7423D7" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span></span>
                        </div>
                    </div>
                    <div className={styles.card}>
                        <div className="subtitle">
                            08
                        </div>
                        <div className={styles.bottomlayer}>
                            <p className="boldtxt">Active Products</p>
                            <span className={styles.link}>View all <span><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="none">
                                <path d="M3.3335 8H12.6668" stroke="#7423D7" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M8 3.33331L12.6667 7.99998L8 12.6666" stroke="#7423D7" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span></span>
                        </div>
                    </div>

                </div>
                <div className={styles.card}>
                    <div className={styles.pricetxt}>
                        £297,400
                    </div>
                    <div className={styles.bottom}>
                        <div className={"boldtxt"}>
                            Total Revenue (2024)
                        </div>
                        <p>Revenue performance shows strong upward momentum, with a 12.5% increase compared to last month, indicating healthy growth and improved monetization efficiency.</p>
                    </div>
                </div>
            </div>
            <div className={styles.gradientcard}>
                <div className="title">
                    Total Revenue Growth
                </div>
                <div style={{ width: "100%", height: 300, marginTop: "32px" }}>
                    <ResponsiveContainer>
                        <LineChart data={dataline}>
                            {/* Dashed grid */}
                            <CartesianGrid
                                strokeDasharray="4 4"
                                vertical={false}
                                stroke="#e5e5e5"
                            />

                            {/* X Axis */}
                            <XAxis
                                dataKey="month"
                                axisLine={false}
                                tickLine={false}
                                tick={{ fontSize: 12 }}
                            />

                            {/* Y Axis */}
                            <YAxis
                                axisLine={false}
                                tickLine={false}
                                tick={{ fontSize: 12 }}
                                domain={[0, 48000]}
                                ticks={[9000, 18000, 27000, 36000, 48000]}
                            />

                            {/* Line */}
                            <Line
                                type="monotone"
                                dataKey="value"
                                stroke="#ff6a2a"
                                strokeWidth={2}
                                dot={false}
                                activeDot={false}
                            />
                        </LineChart>
                    </ResponsiveContainer>
                </div>
            </div>
            <div className={styles.gradientcard}>
                <div className="title">
                    Revenue by Product Category
                </div>
                <div style={{ display: "flex", alignItems: "center", gap: 32 }}>
                    {/* Chart */}
                    <div style={{ width: 220, height: 220 }}>
                        <ResponsiveContainer>
                            <PieChart>
                                <Pie
                                    data={data}
                                    dataKey="value"
                                    innerRadius={70}
                                    outerRadius={90}
                                    startAngle={90}
                                    endAngle={-270}
                                    paddingAngle={4}
                                >
                                    {data.map((entry, index) => (
                                        <Cell key={index} fill={entry.color} />
                                    ))}
                                </Pie>
                            </PieChart>
                        </ResponsiveContainer>
                    </div>

                    {/* Legend */}
                    <div>
                        {data.map((item) => (
                            <div
                                key={item.name}
                                style={{
                                    display: "flex",
                                    alignItems: "center",
                                    gap: 8,
                                    marginBottom: 12,
                                    fontSize: 14
                                }}
                            >
                                <span
                                    style={{
                                        width: 10,
                                        height: 10,
                                        borderRadius: "50%",
                                        background: item.color
                                    }}
                                />
                                <span>
                                    {item.name}: {item.value}%
                                </span>
                            </div>
                        ))}
                    </div>
                </div>
            </div>
            <div className={styles.gradientcard}>
                <div className="title">
                    Renewal Rate
                </div>
                <div style={{ display: "flex", alignItems: "center", gap: 32 }}>
                    {/* Chart */}
                    <div style={{ width: 220, height: 220 }}>
                        <ResponsiveContainer>
                            <PieChart>
                                <Pie
                                    data={data}
                                    dataKey="value"
                                    innerRadius={70}
                                    outerRadius={90}
                                    startAngle={90}
                                    endAngle={-270}
                                    paddingAngle={4}
                                >
                                    {data.map((entry, index) => (
                                        <Cell key={index} fill={entry.color} />
                                    ))}
                                </Pie>
                            </PieChart>
                        </ResponsiveContainer>
                    </div>

                    {/* Legend */}
                    <div>
                        {data.map((item) => (
                            <div
                                key={item.name}
                                style={{
                                    display: "flex",
                                    alignItems: "center",
                                    gap: 8,
                                    marginBottom: 12,
                                    fontSize: 14
                                }}
                            >
                                <span
                                    style={{
                                        width: 10,
                                        height: 10,
                                        borderRadius: "50%",
                                        background: item.color
                                    }}
                                />
                                <span>
                                    {item.name}: {item.value}%
                                </span>
                            </div>
                        ))}
                    </div>
                </div>
            </div>
        </div>
    </>)
} export default Tracker