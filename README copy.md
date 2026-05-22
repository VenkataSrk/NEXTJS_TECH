# MVNO Service - Installation & Setup Guide

This document explains how to install, configure, and run the MVNO service as a systemd daemon.  
Follow these steps when setting up a new server or onboarding new team members.

---

## Step 1: Copy systemd service file

Copy the service file into systemd directory:

cp mvno.service /etc/systemd/system/
chmod 644 /etc/systemd/system/mvno.service or chmod +x /etc/systemd/system/mvno.service

---

## Step 2: Ensure config directory exists

mkdir -p /etc/xgate/


The service uses the environment variable:

CONF_PATH=/etc/xgate/


(Note: No need to set this in `.bashrc`.)

---

## Step 3: Build and install the application

Go to your cloned project path:

cd <cloned_repo_path>
make
make install

Ensure the binary is installed at:

/usr/bin/mvno_service


---

## Step 4: Create log directory

mkdir -p /var/log/xgate/mvno/
chmod 755 /var/log/xgate/mvno/ or ihope so this not required 


---

## Step 5: Reload systemd

systemctl daemon-reload


---

## Step 6: Enable service at boot

systemctl enable mvno.service


---

## Step 7: Start the service

systemctl start mvno.service


---

## Step 8: Check service status

systemctl status mvno.service


---

## Step 9: View logs

Output log:

tail -f /var/log/xgate/mvno/mvno_service.out


Error log:

tail -f /var/log/xgate/mvno/mvno_service.err


---

## MVNO service is now installed and running.


