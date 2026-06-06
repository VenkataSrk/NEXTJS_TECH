
 
# stage 1 build the vuejs application
FROM node:lts-alpine as build-stage
# set the working directory
WORKDIR /usr/src/app
# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
COPY [ "package.json", "package-lock.json*", "npm-shrinkwrap.json*", "./" ]
# Install all the dependecies
RUN yarn install
# Copy all the dependencies
COPY . .
 
 
# Build the files
RUN yarn run vite build
 
# stage 2 production stage
FROM nginx:stable-alpine as production
# Copy the dist file from the working directory
COPY --from=build-stage /usr/src/app/dist /usr/share/nginx/html
# Copy the configuration file and set it to nginx
COPY ./nginx.conf /etc/nginx/nginx.conf
EXPOSE 80
 
