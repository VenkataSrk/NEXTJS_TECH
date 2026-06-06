const fs = require("fs");
const path = require("path");

const projectRoot = path.resolve(__dirname, "..");
const targetPath = path.join(
  projectRoot,
  "node_modules",
  "ua-parser-js",
  "src",
  "ua-parser.js"
);
const sourcePath = path.join(
  projectRoot,
  "node_modules",
  "ua-parser-js",
  "src",
  "main",
  "ua-parser.js"
);

try {
  if (!fs.existsSync(targetPath) && fs.existsSync(sourcePath)) {
    fs.copyFileSync(sourcePath, targetPath);
    process.stdout.write(
      "patch-ua-parser: created missing src/ua-parser.js\n"
    );
  }
} catch (error) {
  process.stdout.write(
    `patch-ua-parser: failed to create src/ua-parser.js (${error.message})\n`
  );
}
