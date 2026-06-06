const path = require('path');
const Dotenv = require('dotenv-webpack');
// const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;
const TerserPlugin = require('terser-webpack-plugin');
module.exports = {
  entry: './src/index.js', // entry point of your application
  output: {
    filename: 'worktual_widget.js', // name of the output file
    path: path.resolve(__dirname, 'dist'), // output directory
  },
  module: {
    rules: [
      {
        test: /\.(js|jsx|ts|tsx)$/,
        exclude: /node_modules/,
        use: {
          loader: 'babel-loader', // use Babel for transpiling JavaScript and TypeScript
          options: {
            presets: ['@babel/preset-env', '@babel/preset-react', '@babel/preset-typescript'],
          },
        },
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader'], // use style-loader and css-loader for CSS files
      },
      {
        test: /\.scss$/,
        use: ['style-loader', 'css-loader', 'sass-loader'], // use style-loader, css-loader, and sass-loader for SCSS files
      },
      {
        test: /\.svg$/,
        use: 'svg-inline-loader', // use svg-inline-loader for SVG files
      },
      {
        test: /\.(png|jpg|gif)$/i,
        use: [
          {
            loader: 'file-loader',
          },
        ],
      },
    ],
  },
  resolve: {
    extensions: ['.js', '.jsx', '.ts', '.tsx'],
  },
  plugins: [
    new Dotenv(), // load environment variables from .env file
  ],
  optimization: {
    minimize: true,
    minimizer: [
      new TerserPlugin({
        terserOptions: {
          compress: {
            drop_console: true,   // Remove console.log statements
          },
          format: {
            comments: false,      // Remove comments
          },
        },
        extractComments: false,   // Do not generate separate LICENSE files
      }),
    ],
  }
};
