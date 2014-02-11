# Mission Control for Raspberry Pi #
Version 1.4.2 - Released 2014-02-09  
Created by Andrew Hazelden   
email: [andrew@andrewhazelden.com](mailto:andrew@andrewhazelden.com)  blog: [http://andrewhazelden.com](http://andrewhazelden.com)


![The Mission Control Dashboard Updates in Real-time](screenshots/dashboard-6up.png)

Mission Control is a ground station program that allows you to plot the GPS based latitude, longitude, and altitude of your Raspberry Pi. The program has a graphical dashboard, and supports sharing the GPS position data via the internet using a Google Earth KMZ file. 

The live data for Mission Control is provided by a [MikroElektronika GPS Click board](http://www.mikroe.com/click/gps/ "GPS Click") that is connected to the [Raspberry Pi](http://www.raspberrypi.org/faqs "Raspberry Pi") via a USB connection on **/dev/ttyACM0**.

## Screenshots ##
![The Live Dashboard View](screenshots/mission_control_raspbian.png)  

You can track your position in real-time using Mission Control's SDL based dashboard display.

![Firefox Google Earth WebView](screenshots/firefox.png)  

If you have the Google Earth WebView plugin installed you can track the Raspberry Pi in a remote web browser.

![Google Earth Tracking](screenshots/google-earth.png)  

Mission Control also provides a KMZ export file that can be used in Google Earth. The KMZ file has a snapshot of the graphics dashboard and a waypoint log of the GPS Click's trail. The KMZ export is updated every 10 seconds.


## Installation ##

![Mission Control Folder](screenshots/program_folder.png)  

Once you unpack the Mission Control archive  you can install the software by running:  

    sudo sh ./install.sh  

This will copy the Mission Control software to the /opt/missioncontrol/ folder. The installer adds the SDL libraries and Apache using apt-get. This installer package is designed to be used with Raspbian.

The Apache web sharing folder is located at:

    /var/www/  

For this demo the raspberry pi should have the IP address set to **192.168.1.137** If you want to use a different IP address you need to update the address stored in the HTML file:  

    /var/www/index.html  
    
You also need to update the Raspberry IP address stored in the kml file:

    /var/www/link.kml

A MissionControl.desktop shortcut is placed in the /home/pi/Desktop folder. The desktop item makes it easy to run Mission Control. 

The Mission Control program will launch and connect to the GPS Click via the /dev/ttyACM0 serial port connection at 115200.

The preferences for the Mission Control program are stored in a plain text preference file:  

    /opt/missioncontrol/gps_prefs.usf  

You can change the serial port device, and the baud rate in the **gps_prefs.usf** file.

## Starting Mission Control ##

**Step 1.** Connect your [MikroElektronika GPS Click board](http://www.mikroe.com/click/gps/ "GPS Click") to the Raspberry Pi USB port.

**Step 2.** Double click on the "Mission Control" icon on your Pi's desktop to start the program.

![Double Click to start Mission Control](screenshots/desktop_link.png)  

If you are using the Raspbian LXDE window manager, you can also start Mission Control with the **Internet > Mission Control** menu item. You can start Mission Control with the debugging info visible by running the **Internet Menu > Mission Control Terminal** menu item.

![You can start Mission Control using the LXDE Menus](screenshots/menu-items.png)  

The raw NMEA GPS log file can be viewed with the **Internet Menu > GPS Tracklog** menu item. This command opens the /opt/missioncontrol/gps_log/tracklog.gps file in the nano text editor.

Mission Control can be started from the command line by typing the following command in the LXterminal:   

    /opt/missioncontrol/missioncontrol  

**Step 3.** At this point the Raspberry Pi system should show the live Mission Control gauges. 

You can now start your web browser on your desktop / laptop / mobile system and track the Raspberry Pi using the Mission Control web interface. (You need to have Google Earth installed on your desktop to access the Mission Control webgui. )

Open your desktop system's web browser and access the Raspberry Pi's internet address:    

    http://192.168.1.137

If you want to view the Mission Control KMZ file directly in Google Earth you can download the current file using the URL:
    
    http://192.168.1.137/mission_control.kmz

A live network Google Earth KMZ file is accessible at:

    http://192.168.1.137/link.kmz


The link.kmz file is neat because it refreshes Google Earth regularly and downloads the latest Mission Control data.


A plain text NMEA GPS tracklog is saved on the Raspberry Pi at:  

    /opt/missioncontrol/gps_logs/tracklog.gps

This logfile is overwritten every time the Mission Control program is launched.

## Options ##

### Fullscreen Mode ###

You can switch between windowed and fullscreen modes by tapping the TAB key on your keyboard, or by clicking in the Mission Control window.

![Fullscreen Mode](screenshots/fullscreen_gauge.png)

A single click brings Mission Control into fullscreen mode.

### View Layouts ###

The Mission Control preference file (/opt/missioncontrol/gps_prefs.usf) has an option that allows you to choose between three different window layouts:

    # View Layout 0=Vertical Block Layout, 1=Horizontal Layout 5up, 2=Horizontal Layout 6up
    @view_layout 1

Setting the **@view_layout** value to 0 uses a vertically arranged block layout. This vertical layout fits nicely on the Raspberry Pi screen if you decide to use the composite video connection and lower the screen resolution to 640x480.

![Vertical Block Layout](screenshots/dashboard-blocks.png)

----------

Setting the **@view_layout** to 1 uses a horizontal layout with 5 gauges. This layout fits nicely on a 1024x768 screen.

![Horizontal Layout 5 Up](screenshots/dashboard-5up.png)

----------

Setting the **@view_layout** to 2 uses a horizontal layout with 6 gauges.

![Horizontal Layout 6 Up](screenshots/dashboard-6up.png)

----------

### Changing the BAUD Rate ###

The Mission Control preference file (/opt/missioncontrol/gps_prefs.usf) has an option that allows you to select the serial port BAUD rate. A BAUD rate is the serial communication speed in bits-per-second.

    # GPS Baud Rate
    @baud_rate 115200

The following is a list of valid BAUD rates for Mission Control on Raspbian:

    @baud_rate 300
    @baud_rate 600
    @baud_rate 1200
    @baud_rate 1800
    @baud_rate 2400
    @baud_rate 4800
    @baud_rate 9600
    @baud_rate 19200
    @baud_rate 38400
    @baud_rate 57600
    @baud_rate 115200
    @baud_rate 230400
    @baud_rate 460800 
    @baud_rate 500000 
    @baud_rate 576000 
    @baud_rate 921600
    @baud_rate 1000000
    @baud_rate 1152000
    @baud_rate 2000000
    @baud_rate 3000000
    @baud_rate 4000000

### Making Mission Control a Startup Item ###
Here is a tip for Raspbian / LXDE users on making Mission Control a startup item by copying the program's LXDE desktop shortcut to you user account's autostart folder.

Paste the following commands into the terminal window:

    # Create an autostart folder
    mkdir -p ~/.config/autostart
    
    # Paste the desktop link in the folder
    cp /opt/missioncontrol/MissionControl.desktop ~/.config/autostart/
    
    # Make it executable
    chmod 755 ~/.config/autostart/MissionControl.desktop

If you want to remove Mission Control from the startup items run the following command:

	rm ~/.config/autostart/MissionControl.desktop

### Uninstalling Mission Control ###

If you want to remove Mission Control from your system you can run the following shell command:

    sudo sh /opt/missioncontrol/remove.sh



