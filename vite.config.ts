import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";

export default defineConfig({
  plugins: [react()],
  optimizeDeps: {
    include: ["recharts"],
  },
  css: {
    preprocessorOptions: {
      scss: {
        additionalData: `@use "/src/assets/styles/variables.scss" as *;`
      }
    }
  },
});
