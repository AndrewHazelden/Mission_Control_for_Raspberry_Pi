#! /bin/sh

# Mission Control Installer V1.0 - 2014-02-06
# by Andrew Hazelden andrew@andrewhazelden.com

echo "Installing Mission Control on Raspbian"
echo "--------------------------------------"
echo " "

# Make the mission control folder
sudo mkdir -p /opt/missioncontrol/

# Copy the current files to the mission control folder
cp -R * /opt/missioncontrol/

# Move to the directory
cd /opt/missioncontrol/

# Set the file permissions
sudo chmod 777 -R /opt/missioncontrol/
sudo chmod 777 /opt/missioncontrol/install.sh
sudo chmod 777 /opt/missioncontrol/make_kmz.sh
sudo chmod 777 /opt/missioncontrol/missioncontrol

# Make the text documents easy to open
sudo chmod 666 /opt/missioncontrol/readme.html
sudo chmod 666 /opt/missioncontrol/readme.md
sudo chmod 666 /opt/missioncontrol/gps_prefs.usf

# Copy the MissionControl.desktop item to the LXDE Internet Menu
sudo cp /opt/missioncontrol/MissionControl.desktop /usr/share/applications/
sudo cp /opt/missioncontrol/MissionControlTerminal.desktop /usr/share/applications/
sudo cp /opt/missioncontrol/GPSlog.desktop /usr/share/applications/

# Copy the MissionControl.desktop item to the Pi User's Desktop folder
sudo cp /opt/missioncontrol/MissionControl.desktop /home/pi/Desktop/
#sudo cp /opt/missioncontrol/MissionControlTerminal.desktop /home/pi/Desktop/

# Give the pi user control of their desktop link
sudo chown pi /home/pi/Desktop/MissionControl.desktop

# Copy the MissionControl menu item to the Menu Folder
#sudo cp /opt/missioncontrol/resources/MissionControl /etc/menu

echo "Installing the SDL Graphics Library"
echo "-----------------------------------"
echo " "

# Update the apt-get package list
sudo apt-get update

# Add the standard SDL libraries
sudo apt-get -y install libsdl1.2debian libsdl-image1.2 libsdl-mixer1.2 libsdl-gfx1.2-4 libsdl-ttf2.0-0 zip

# Add the dev SDL libraries
#sudo apt-get -y install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-gfx1.2-dev libsdl-ttf2.0-dev zip

echo "Installing the Apache Webserver"
echo "-------------------------------"
echo " "

# Raspberry Pi Apache Installer
sudo apt-get -y install apache2 php5 libapache2-mod-php5

# Restart apache
sudo service apache2 restart

# Tip: The Apache shared folder is: 
# /var/www/

# Make the www folder writable
sudo chmod -R 777 /var/www/

# Add the Mission Control index.html page and live.kml file to the apache web folder
sudo cp /opt/missioncontrol/gps_logs/index.html /var/www/
sudo cp /opt/missioncontrol/gps_logs/live.kml /var/www/

# Add the favicon and mobile thumbnail icons to the apache web folder
sudo cp /opt/missioncontrol/resources/favicon.png /var/www/
sudo cp /opt/missioncontrol/resources/apple-touch-icon.png /var/www/

# Restart the LXDE Panel GUI to refresh the icons
lxpanelctl restart

echo "Installation Complete"
echo "---------------------"
echo " "

