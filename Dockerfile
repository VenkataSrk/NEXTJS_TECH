# ---------- Build stage ----------
FROM node:lts-alpine AS build-stage

WORKDIR /usr/src/app

# Install git (needed for some npm deps)
RUN apk add --no-cache git

# Copy only dependency files first (better caching)
COPY package.json yarn.lock* package-lock.json* npm-shrinkwrap.json* ./

# Clean yarn cache + install dependencies safely
RUN yarn cache clean --force \
    && yarn install --frozen-lockfile || yarn install --no-cache

# Copy application source
COPY . .

# Build Vite project
RUN yarn run vite build


# ---------- Production stage ----------
FROM nginx:stable-alpine AS production

# Copy built files
COPY --from=build-stage /usr/src/app/dist /usr/share/nginx/html

# Copy nginx configuration
COPY nginx.conf /etc/nginx/nginx.conf

EXPOSE 80
