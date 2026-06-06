// components/StackedBarChart.tsx

import React from 'react';
import Chart from 'react-apexcharts';

interface BarChartProps {
    categories: string[];
    series: ApexAxisChartSeries;
    colors?: string[];
    xTitle?: string;
    yTitle?: string;
    height?: number;
    isStacked?: boolean
}

const ApexBarChart: React.FC<BarChartProps> = ({
    categories,
    series,
    colors,
    isStacked = false,
    xTitle = '',
    yTitle = '',
    height = 300,
}) => {
    const options: ApexCharts.ApexOptions = {
        chart: {
            type: 'bar',
            stacked: isStacked,
            toolbar: {
                show: false, // Hides the toolbar (zoom, download etc. – enable if needed)
            },
        },
        plotOptions: {
            bar: {
                horizontal: false, // Switch to horizontal layout (bars go left-right)
                borderRadius: 0, // No rounding on bar corners
                columnWidth: '35%', // Controls bar thickness
            },
        },
        dataLabels: {
            enabled: false, // Hides values above each bar
        },
        xaxis: {
            categories, // labels (e.g., user names, teams, products)
            title: {
                text: xTitle, // Label under X-axis (e.g., "Teams", "Departments")
                style: {
                    fontSize: '14px',
                    fontFamily: 'intermedium',
                    fontWeight: 'normal',
                    color: '#8D8D8D',
                },
            },
            labels: { show: true },
        },
        yaxis: {
            title: {
                text: yTitle,
                offsetX: -8,
                style: {
                    fontSize: '14px',
                    fontFamily: 'intermedium',
                    fontWeight: 'normal',
                    color: '#8D8D8D',
                },
            },
            labels: { show: true }, // Show Y-axis category labels
        },
        legend: {
            show: false,
        },
        fill: {
            opacity: 1, // Solid fill (1 = fully filled, 0.5 = 50% transparent)
        },
        colors, // Custom color array for each series (fallback if not provided: Apex default)
        tooltip: {
            y: {
                // formatter: (val: number) => `£${val.toLocaleString()}`, // ✅ Pound symbol
                formatter: (val: number) => `${val.toLocaleString()}`, // ✅ Pound symbol

            },
        },
    };

    return <Chart options={options} series={series} type="bar" height={height} />;
};

export default ApexBarChart;


// example
{/* <ApexBarChart
    isStacked={true}
    series={seriesbar}
    xTitle='Months'
    yTitle='Number of Messages Sent'
    colors={["#098F6A", "#E06E3A", "#E0B73A", "#E06E3A", "#3A9EE0"]}
    categories={["Jan", "Feb", "Mar", "Apr", "May", "June"]}
/> */}