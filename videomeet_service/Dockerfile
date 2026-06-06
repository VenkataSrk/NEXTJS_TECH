FROM node:16.20.1
#ENV http_proxy "http://10.171.1.10:8080"
#ENV https_proxy "http://10.171.1.10:8080"

# Create app directory
WORKDIR /usr/src/app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
# where available (npm@5+)
COPY package*.json ./
# Bundle app source
COPY . .
RUN yarn install
#RUN yarn build
# If you are building your code for production
# RUN npm ci --only=production

EXPOSE 8080
CMD [ "yarn", "dev" ]


