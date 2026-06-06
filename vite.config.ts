import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  css: {
    devSourcemap: true,
  },
  build: {
    sourcemap: true,
    minify: false,
    rollupOptions: {
      treeshake: false
    }
  },
  server: {
    port: 3000,
    host: true, // or use '0.0.0.0' to expose to external IPs
    strictPort: true,
    cors: {
      origin: ['https://campaign.worktual.co.uk', 'https://qacampaign.worktual.co.uk'], // control cross-origin access
      credentials: true
    },
  },
})
