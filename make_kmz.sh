#! /bin/sh

# KMZ Packager V1.0 - 2014-02-06
# by Andrew Hazelden andrew@andrewhazelden.com

# Package a folder with images and a kml file into a zipped .kmz
# This script requires the zip module

# KMZ Output 
KMZ_PACKAGE="mission_control.kmz"
MISSION_CONTROL_FOLDER="/opt/missioncontrol/"
GPS_LOGS_FOLDER="${MISSION_CONTROL_FOLDER}gps_logs/"
GPS_TEMP_FOLDER="${MISSION_CONTROL_FOLDER}temp_kmz/"

# Update the apt-get package list
#sudo apt-get update

# Add the standard zip libraries
#sudo apt-get -y install  zip

# Create the working directory
cd /opt/missioncontrol/
mkdir -p /opt/missioncontrol/temp_kmz

# Add the kmz resource files
cp ${GPS_LOGS_FOLDER}dashboard.bmp ${GPS_TEMP_FOLDER}
cp ${GPS_LOGS_FOLDER}tracklog.kml ${GPS_TEMP_FOLDER}doc.kml

# Move into the kmz folderlo
cd ${GPS_TEMP_FOLDER}

# Remove the old KMZ zip file
rm -f ${GPS_LOGS_FOLDER}${KMZ_PACKAGE}

# Zip the files
zip ${GPS_LOGS_FOLDER}${KMZ_PACKAGE} *

# Move back down to the mission control project directory
cd ${MISSION_CONTROL_FOLDER}

# KMZ Complete
echo "Created KMZ package:  ${KMZ_PACKAGE}"

# Copy files to apache folder: /var/www
echo "Copying files to Apache folder"
#cp ${GPS_LOGS_FOLDER}index.html /var/www
cp ${GPS_LOGS_FOLDER}${KMZ_PACKAGE} /var/www



