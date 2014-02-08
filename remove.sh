#! /bin/sh

# Mission Control Uninstaller V1.0 - 2014-02-08
# by Andrew Hazelden andrew@andrewhazelden.com

echo "Uninstalling Mission Control on Raspbian"
echo "----------------------------------------"
echo " "

# Remove the MissionControl.desktop items from the LXDE Internet Menu
sudo rm -f /opt/missioncontrol/MissionControl.desktop /usr/share/applications/MissionControl.desktop
sudo rm -f /usr/share/applications/MissionControlTerminal.desktop
sudo rm -f /usr/share/applications/GPSlog.desktop 

# Remove the MissionControl.desktop item from the Pi User's Desktop folder
sudo rm -f /home/pi/Desktop/MissionControl.desktop

# Tip: The Apache shared folder is: 
# /var/www/

# Remove the Mission Control index.html page, KMZ, and KML files from the apache web folder
sudo rm -f /var/www/index.html
sudo rm -f /var/www/live.kml
sudo rm -f /var/www/mission_control.kmz

# Remove the favicon and mobile thumbnail icons from the apache web folder
sudo rm -f /var/www/favicon.png
sudo rm -f /var/www/apple-touch-icon.png

# Remove the Mission Control folder
sudo rm -rf /opt/missioncontrol/

# Restart the LXDE Panel GUI to refresh the icons
lxpanelctl restart

echo "Uninstallation Complete"
echo "---------------------"
echo " "

