FROM node:18
# ENV http_proxy "http://10.150.1.20:8080"
# ENV https_proxy "http://10.150.1.20:8080"
# Create app directory

ENV NODE_OPTIONS="--max-old-space-size=12288"
WORKDIR /usr/src/app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copie
# where available (npm@5+)
COPY package*.json ./
# Bundle app source
COPY . .
# COPY ../.git/HEAD .
RUN yarn add bson --ignore-engines
RUN yarn install --ignore-engines
RUN  yarn build
# If you are building your code for production
# RUN npm ci --only=production

EXPOSE 5007
CMD [ "yarn", "start" ]

