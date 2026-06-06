FROM nginx:latest

WORKDIR /var/www

# Clean existing files (optional but safe)
RUN rm -rf /var/www/*

# Copy build output (React/Angular/Vue build) into /var/www
COPY ./build/ /var/www

# Copy your custom nginx.conf
COPY nginx.conf /etc/nginx/nginx.conf

# Expose correct port
EXPOSE 80

# Start nginx in foreground
CMD ["nginx"]
