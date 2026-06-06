// components/DonutChart.tsx
import React from 'react';
import Chart from 'react-apexcharts';

interface DonutChartProps {
    series: number[];
    labels: string[];
    colors?: string[];
}

const DonutChart: React.FC<DonutChartProps> = ({ series, labels, colors }) => {

    const options: ApexCharts.ApexOptions = {
        chart: {
            type: 'donut',
            toolbar: {
                show: false, // Shows export/download/zoom tools
            },
        },

        // Category labels corresponding to each value in the `series`
        labels: labels,

        // Customize how the legend is displayed
        legend: {
            show: false,
            position: 'bottom', // Positions legend below the chart
            horizontalAlign: 'center', // Centers legend items horizontally
            fontSize: '14px',
            // markers: {
            //   width: 12, // Size of color dot next to each legend item
            //   height: 12,
            //   radius: 12, // Makes the markers circular
            // },
        },

        // Enables percentage labels inside donut slices
        dataLabels: {
            enabled: false, // Show data values inside slices
            formatter: (val: number) => `${val.toFixed(1)}%`, // Format as percentage with 1 decimal
        },

        // Tooltip when hovering over slices
        tooltip: {
            enabled: true, // Enables tooltip on hover
            y: {
              formatter: (val: number) => `${val} %`, // Custom formatter for tooltip value
            },
        },

        // Define slice colors (optional, otherwise auto-assigned)
        colors: colors,

        // Controls specific donut/pie chart behavior
        plotOptions: {
            pie: {
                donut: {
                    size: '80%', // Size of the donut hole (percentage of radius)
                    labels: {
                        show: false, // Show labels in the center of the donut

                        // Label for the selected slice
                        name: {
                            show: true,
                            fontSize: '16px',
                            fontWeight: 600,
                            offsetY: -10,
                        },

                        // Value for the selected slice
                        value: {
                            show: true,
                            fontSize: '20px',
                            fontWeight: 700,
                            color: '#111',
                            offsetY: 10,
                            formatter: (val) => `${val}%`, // Show actual value
                        },

                        // Label in the center showing total sum of all slices
                        total: {
                            show: true,
                            label: 'Total', // Static label text
                            fontSize: '16px',
                            fontWeight: 600,
                            formatter: function (w) {
                                // Calculate and display the total of all series values
                                return w.globals.seriesTotals.reduce((a: number, b: number) => a + b, 0).toString();
                            },
                        },
                    },
                },
            },
        },

        // Responsive breakpoints 
        //   responsive: [
        //     {
        //       breakpoint: 480, 
        //       options: {
        //         chart: {
        //           width: 300,
        //         },
        //         legend: {
        //           position: 'bottom', // Adjust legend position for smaller screens
        //         },
        //       },
        //     },
        //   ],
    };


    return (
        <Chart options={options} series={series} type="donut" width="100%" height={200} />
    );
};

export default DonutChart;
