FROM node:22
# ENV http_proxy "http://10.150.1.23:8080"
# ENV https_proxy "http://10.150.1.23:8080"
# Create app directory
#RUN apk update && apk upgrade && \
 #   apk add --no-cache bash git openssh

WORKDIR /usr/src/app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copie
# where available (npm@5+)
COPY package*.json ./
# Bundle app source
COPY . .
# RUN yarn install --ignore-engines
#RUN apt-get update -y
#RUN apt-get install -y
#RUN yarn add puppeteer --ignore-scripts
RUN yarn add puppeteer@10.0.0 --ignore-scripts
RUN yarn install
RUN yarn build
# If you are building your code for production
# RUN npm ci --only=production

EXPOSE 3001
CMD [ "yarn", "start" ]
    
