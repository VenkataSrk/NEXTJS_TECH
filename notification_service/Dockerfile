FROM node:16.15.0

# Create app directory
ENV NODE_OPTIONS="--max-old-space-size=4096"
WORKDIR /usr/src/app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copie
# where available (npm@5+)
COPY package*.json ./
# Bundle app source
COPY . .
RUN yarn install --ignore-engines
#RUN yarn install
RUN  yarn build
# If you are building your code for production
# RUN npm ci --only=production

EXPOSE 5004
CMD [ "yarn", "start:prod" ]

