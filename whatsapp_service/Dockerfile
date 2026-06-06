FROM node:18
# ENV http_proxy "http://10.150.1.23:8080"
# ENV https_proxy "http://10.150.1.23:8080"

WORKDIR /usr/src/app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copie
# where available (npm@5+)
COPY package*.json ./
# Bundle app source
COPY . .
# RUN yarn install --ignore-engines
RUN yarn install --ignore-engines
RUN  yarn build
# If you are building your code for production
# RUN npm ci --only=production

EXPOSE 5006
CMD [ "yarn", "start" ]
    
