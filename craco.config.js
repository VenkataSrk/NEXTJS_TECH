module.exports = {
  webpack: {
    configure: (webpackConfig) => {
      // Find the babel-loader rules and exclude papaparse from transformation
      const oneOfRule = webpackConfig.module.rules.find((rule) => rule.oneOf);
      if (oneOfRule) {
        oneOfRule.oneOf.forEach((rule) => {
          // Handle rules with 'loader' property
          if (rule.loader && rule.loader.includes('babel-loader')) {
            const existingExclude = rule.exclude;
            rule.exclude = (modulePath) => {
              if (/node_modules[\\/]papaparse/.test(modulePath)) {
                return true;
              }
              if (existingExclude) {
                if (typeof existingExclude === 'function') {
                  return existingExclude(modulePath);
                }
                if (existingExclude instanceof RegExp) {
                  return existingExclude.test(modulePath);
                }
              }
              return false;
            };
          }
          // Handle rules with 'use' array (common in CRA 5)
          if (rule.use && Array.isArray(rule.use)) {
            rule.use.forEach((useEntry) => {
              if (
                useEntry.loader &&
                useEntry.loader.includes('babel-loader')
              ) {
                const existingExclude = rule.exclude;
                rule.exclude = (modulePath) => {
                  if (/node_modules[\\/]papaparse/.test(modulePath)) {
                    return true;
                  }
                  if (existingExclude) {
                    if (typeof existingExclude === 'function') {
                      return existingExclude(modulePath);
                    }
                    if (existingExclude instanceof RegExp) {
                      return existingExclude.test(modulePath);
                    }
                  }
                  return false;
                };
              }
            });
          }
        });
      }
      return webpackConfig;
    },
  },
};
