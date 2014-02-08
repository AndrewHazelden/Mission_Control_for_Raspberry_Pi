// Mission Control for RPi + GPS Click v1.4 2014-02-08 9am
// By Andrew Hazelden 
//
// email: andrew@andrewhazelden.com
// blog: http://www.andrewhazelden.com/


//Add termios and gps reader header
#include "missioncontrol.h"

//Add universal settings format header
#include "usf.h"

int main (int argc, const char * argv[]) {
  int fd;
	char gps_string[255];
	long line_count = 0;
	
  //USF settings vars
  char port_name[255]="";
  int baud_rate = 0;
  long int kml_log_limit = 0;
  long int read_lines_count = 0;
      
  startup();  // Clear the struct Data
  start_time = time(NULL);

 	printf("Mission Control v1.4 - Released 2014-02-06\n");
 	printf("By Andrew Hazelden andrew@andrwhazelden.com\n");
 	printf("-------------------------------------------\n");
		
	// Load the settings file
  usf_read_init(USF_PREFERENCE_FILE_NAME);
	
	  
  //Scan for settings
  if (USF_error == 0){
    //print the current settings file data
    //usf_debug(usf_file_pointer);

    //Read the settings values
    usf_read_string("port_name", port_name);
    usf_read_int("baud_rate", &baud_rate);
    usf_read_string("gps_tracklog_filepath", gps_tracklog_filepath);
    usf_read_string("kml_tracklog_filepath", kml_tracklog_filepath);
    usf_read_string("bmp_snapshot_filepath", bmp_snapshot_filepath);
    usf_read_long("kml_log_limit", &kml_log_limit);
    usf_read_long("read_lines_count", &read_lines_count);
    usf_read_int("gauge_aa_mode", &rotozoom_aa_quality_mode );
    usf_read_int("max_fps", &fps );
    
    //Print the current settings
    printf("\n\nThe %s serial port will be used to open a %d speed serial GPS connection.\n\n", port_name, baud_rate);
  } else {
    printf("Error: The Mission Control preferences file: %s is missing.\n", USF_PREFERENCE_FILE_NAME);
    return 1;
  }
  

  // Start SDL an create a new window
  init_gfx();
  load_sprites();
  
  // Set the FramesPerSecond
  FPSmanager fps_manager;
	SDL_initFramerate( &fps_manager );
	SDL_setFramerate( &fps_manager, fps );   

	// Open the serial port
	fd = open_port(port_name, baud_rate); 
	printf("[fd]:%d\n",fd);
	//Quit if the serial port could not be opened
	if(fd == -1){
	  printf("The serial port could not be opened.\n\n");
    return 1;
  }
  
  // Open the gps log filepointer
  open_log();
	
  //Run until there is a click
  while(!done && fd){
  
    // Read the mouse and keyboard input
    get_input(); 

    // Process the serial GPS data
	  for(line_count=0;line_count<read_lines_count; line_count++){
      get_string(fd, gps_string);	
      printf("%s",gps_string);
      parseGPS(gps_string);
      line_count++;
	  }

    //Get last GPS time
    sscanf(gpgga.fix_time_utc, "%d", &fix_time_utc);
    elapsed_update_interval = fix_time_utc - previous_fix_time_utc;
    printf("Elapsed Time since last GPS update:%d\n",elapsed_update_interval); 

    // Update the gauges
    render_screen();
    
    // Write the Google Earth KML tracklog file every 10 seconds
    if( (frame_counter % (fps * 10)) == 1){   
      kml_update();

      // Write the Google Earth KML dashboard snapshot BMP image every 10 seconds
      kml_save_bmp_snapshot();
    }
    
    // Increment the frame counter
    frame_counter++;
    
    // Save the reading from the last cycle
	  save_previous();
	  
	  // Slow down the graphics update to 10 fps
    SDL_framerateDelay( &fps_manager); // this will delay execution for a while
  }
  
  // Close the serial port
  close_port(fd);
  
	// Release the SDL_ttf font files
  TTF_CloseFont( fnt );
	
  return 0;
}


// Open the NMEA GPS logfile for writing
int open_log(void){
  // Write modes "a+" = append new data, "w" = write to a new text file
  gps_log_fp = fopen(gps_tracklog_filepath, "w");
  
  if(!gps_log_fp){
    printf("Cannot write to the NMEA tracklog file: %s\n", gps_tracklog_filepath);
  } else {
    printf("Saving the NMEA tracklog file: %s\n", gps_tracklog_filepath);
  }
}


// Open the serial port
int open_port(char* port_name, int baud_rate){
	int fd;
	
  // /dev/tty.usbserial
	// /dev/ttyACM0

	fd = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{	
		perror("unable to open port");
	}
	else
	{
		fcntl(fd,F_SETFL, 0);
		printf("Serial port %s opened.\n", port_name);
		
		//Set the baud rate for the port
		set_baud_rate(fd, baud_rate);
	}
	return (fd);
}


// Close the serial port
void close_port(int fd){

	if(close(fd) == 0){
		printf("Serial port closed.\n"); // close serial port
	} else {
		printf("Serial port not closed.\n"); // close serial port error
	}
	
	if(fclose(gps_log_fp) == 0){
		printf("Tracklog file closed.\n"); // close NMEA tracklog
	} else {
		printf("Tracklog file not closed.\n"); // close close NMEA tracklog
	}
}


void set_baud_rate(int fd, int baud_rate){
  speed_t baud;
  switch(baud_rate){
    case 300: baud = B300; break;
    case 600: baud = B600; break;
    case 1200: baud = B1200; break;
    case 1800: baud = B1800; break;
    case 2400: baud = B2400; break;
    case 4800: baud = B4800; break;
    //case 7200: baud = B7200; break;
    case 9600: baud = B9600; break;
    //case 14400: baud = B14400; break;
    case 19200: baud = B19200; break;
    case 38400: baud = B38400; break;
    case 57600: baud = B57600; break;
    case 115200: baud = B115200; break;
    case 230400: baud = B230400; break;
    case 460800: baud = B460800; break;
    case 500000: baud = B500000; break;
    case 576000: baud = B576000; break;
    case 921600: baud = B921600; break;
    case 1000000: baud = B1000000; break;
    case 1152000: baud = B1152000; break;
    case 2000000: baud = B2000000; break;
    case 3000000: baud = B3000000; break;
    case 4000000: baud = B4000000; break;
    default: baud = B115200; break;
  }
  
	//fcntl(fd,F_SETFL, FNDELAY); //read characters immediately without blocking.
	
	struct termios options;
	
	tcgetattr(fd, &options);
	
	// Set the serial i/o speed
	cfsetispeed(&options, baud);
	cfsetospeed(&options, baud);
	
	// Disable local TTY override for input
	options.c_cflag |= (CLOCAL | CREAD);
	
	// Setup 8N1 settings
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	
	// Disable HW flow control
	options.c_cflag &= ~CRTSCTS;
	
	// RAW mode
	options.c_lflag &= (ICANON | ECHO | ECHOE | ISIG);
	
	// Disable SW flow control
	options.c_iflag &= ~(IXON |IXOFF | IXANY);
	
	// Choose raw output
	options.c_oflag &= ~OPOST;
	
	// Make changes immediately
	tcsetattr(fd, TCSANOW, &options);
}


void save_previous(void){
  // save previous time for reference
  previous_fix_time_utc = fix_time_utc;
  
  //Track the variometer change per update cycle
  previous_altitude = altitude_in_feet;
  //Debug altitude
  //previous_altitude = 15;
}


void startup(void){
	rssi = 0;
	ping_ms = 0;
	latitude = 0.0;
	longitude = 0.0;
	altitude_in_meters = 0.0;
	previous_altitude = 0.0;
	
	previous_fix_time_utc = 0;
	fix_time_utc = 0;
	elapsed_update_interval = 0;
	
	heading_degrees = 0.0;
	heading_waypoint_degrees = 0.0;
	
	altitude_in_feet = 0.0;
	
	altitude_hundreds_degrees = 0.0;
	altitude_thousands_degrees = 0.0;
	
	airspeed_in_km = 0.0;
	variometer_altitude_change = 0.0;
	
	fuel_percentage = -125.0;
	
	//blackbox_connected = 0;
	
	//waypoint timer uses this
	program_begin_time = time(NULL);
	
	strcpy(gpgga.fix_time_utc, "       "); 
	strcpy(gpgga.latitude, "       "); 
	strcpy(gpgga.northing, " "); 
	strcpy(gpgga.longitude, "       "); 
	strcpy(gpgga.easting, " ");
	strcpy(gpgga.fix_quality, " ");
	strcpy(gpgga.number_of_satellites, " ");
	strcpy(gpgga.hdop, " ");
	strcpy(gpgga.altitude, "       "); 
	strcpy(gpgga.altitude_units, " ");
	strcpy(gpgga.checksum, " ");
	
	strcpy(gprmc.status, " "); 
	strcpy(gprmc.speed_over_ground, "       ");
	strcpy(gprmc.track_angle, "       ");
	strcpy(gprmc.date, "  ");
	strcpy(gprmc.mode, " "); 
	strcpy(gprmc.checksum, " "); 

  //Start tracking the program uptime
	begin_program = time(NULL);
}



// Setup SDL
void init_gfx(void){
  char font_name[500];
  int font_size = 12;

  //static int flags=0;
  flags=0;

  /* Define the program icon */
  #ifdef _WIN32
    program_icon = SDL_LoadBMP("/opt/missioncontrol/resources/mc-icon-32px.bmp");
    //key the blue background from the sprite
    //SDL_SetColorKey( program_icon, SDL_SRCCOLORKEY, SDL_MapRGB(program_icon->format, 4, 98, 171) );
  #else
    program_icon = SDL_LoadBMP("/opt/missioncontrol/resources/mc-icon-64px.bmp");
    //key the blue background from the sprite
    //SDL_SetColorKey( program_icon, SDL_SRCCOLORKEY, SDL_MapRGB(program_icon->format, 4, 98, 171) );
  #endif


  char *msg;
  
  // Initialize SDL
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
  {
    sprintf (msg, "Couldn't initialize SDL: %s\n", SDL_GetError ());
    /*MessageBox (0, msg, "Error", program_icon);*/
    free (msg);
    exit (1);
  }
  atexit (SDL_Quit);
  
  // Set video mode
  screen = SDL_SetVideoMode (screen_width, screen_height, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  
  if (screen == NULL)
  {
    sprintf (msg, "Couldn't set %dx%dx16 video mode: %s\n", screen_width, screen_height, SDL_GetError ());
    printf("%s",msg);
    free (msg);
    exit (2);
  }
  
  flags = screen->flags;
  
  // Check if the enable fullscreen argument is turned on
  if(enable_fullscreen){
    SwitchFullscreen();
  }
  
    
  SDL_WM_SetIcon(program_icon, NULL);
  SDL_WM_SetCaption ("Mission Control", "Mission Control");
  
  // Create background screen colors
  screen_color = SDL_MapRGB(screen->format, 66, 66, 231);
  
  //Define the navputer text area
  display_area_rect.x = 10;
  display_area_rect.y = 26;
  display_area_rect.w = 234;
  display_area_rect.h = 74;
  

  // Set up SDL_TTF
  TTF_Init();
  atexit(TTF_Quit);
  
  //Read the font name from the preference file
  usf_read_string("font_name", font_name);
  usf_read_int("font_size", &font_size);
  
  fnt = TTF_OpenFont( font_name, font_size );
  
  // Raspbian Fonts:
  // /usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf
  // /usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono-Bold.ttf	
  // /usr/share/fonts/truetype/droid/DroidSansMono.ttf
  // resources/Xolonium-Regular.otf

  // Get the size of a character in pixels
  if(TTF_SizeText(fnt,"B",&character_with,&character_height)) {
    printf("SDL_TTF error: %s", TTF_GetError());
  } else {
    //total_text_rows = (display_area_rect.h/character_height)-1;
    //total_text_cols = (display_area_rect.w/character_with)-1;
    total_text_rows = (display_area_rect.h/character_height);
    total_text_cols = (display_area_rect.w/character_with);
    printf("Current Screen Size: width=%d height=%d\n", screen_width, screen_height);
    printf("Text Settings: %d=points rows=%d columns=%d xheight=%d\n", font_size, total_text_rows, total_text_cols, character_height);
    printf("\n");
  }
  
  
  // Wait a moment for the screen to switch
  if(enable_fullscreen)
  {
    SDL_Flip(screen);
    SDL_Delay(1500);
  }
  
  /*-------------------------------
  * Fill the screen
  * -------------------------------
  */
  
  //Set the ocean background color
  //bgColor = SDL_MapRGB(screen->format, 4, 98, 171); //Blue Color
  bgColor = SDL_MapRGB(screen->format, 0, 0, 0); //Black Color
  SDL_FillRect(screen, NULL, bgColor);
  
  clickColor = SDL_MapRGB(screen->format, 171, 98, 4);

  // Make sure everything is displayed on screen
  SDL_Flip(screen);
  
}


//Switch the display fullscreen
void SwitchFullscreen(void){

  // Check if the enable fullscreen argument is turned on
  if(enable_fullscreen){
    screen = SDL_SetVideoMode(0, 0, 0, screen->flags ^ SDL_FULLSCREEN);
    screen_width = screen->w;
    screen_height = screen->h;
    
    if(screen == NULL) 
    {
      screen = SDL_SetVideoMode(0, 0, 0, flags); /* If toggle FullScreen failed, then switch back */
    }
    if(screen == NULL)
    {
      exit(1); /* If you can't switch back for some reason, then fail */
    }
  }
}

void ToggleFullscreen(void){

  //Enter Fullscreen Mode
  if(enable_fullscreen){
    enable_fullscreen = 0;
    //UART1_Write_Line("Entering Fullscreen Mode");
  }
  else if(!enable_fullscreen){
    //Exit Fullscreen Mode
    enable_fullscreen = 1;
    //UART1_Write_Line("Exiting Fullscreen Mode");
  }
  
  SwitchFullscreen();
  
  //Set the ocean background color
  bgColor = SDL_MapRGB(screen->format, 4, 98, 171);
  SDL_FillRect(screen, NULL, bgColor);

  // Make sure everything is displayed on screen
  SDL_Flip(screen);
  
  SDL_Delay(200);
}


//Check for user input
void get_input(void){
  SDL_Event event;
  
  while (SDL_PollEvent (&event))
  {
    switch (event.type)
    {  
    case SDL_KEYUP:
      switch( event.key.keysym.sym ){
      case SDLK_LEFT:
        leftButton=0; //Left
        break;
      case SDLK_RIGHT:
        rightButton=0; //Right
        break;
      case SDLK_UP:
        upButton=0; //Up
        break;
      case SDLK_DOWN:
        downButton=0; //Down
        break;
      case SDLK_ESCAPE:
        done = 1;  // Quit when a key is pressed
        printf("Escape key pressed.\n");
        break;
      }
    case SDL_MOUSEBUTTONDOWN:
      isbutton_down=1;
      //get the mouse position when the button is pressed
      SDL_GetMouseState(&mousex, &mousey);
      //SDL_FillRect(screen, NULL, clickColor);
      //SDL_Flip(screen);
      break;
    case SDL_MOUSEBUTTONUP:
      //SDL_FillRect(screen, NULL, bgColor);
      //SDL_Flip(screen);
      break;	
    case SDL_QUIT:
      done = 1;
      break;
    default:
      break;
    }
  }
}


void load_sprites(void){
  SDL_RWops *rwop;
  
  //------------------
  //navputer
  //------------------
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/navputer.png", "rb");
  navputer_png = IMG_LoadPNG_RW(rwop);
  if(navputer_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  //Position the image
  navputer_rect.x = 0;
  navputer_rect.y = 0;
  navputer_rect.w = navputer_png->w;
  navputer_rect.h = navputer_png->h;
  
  //------------------
  //attitude
  //------------------
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/attitude_dial.png", "rb");
  //rwop=SDL_RWFromFile("/opt/missioncontrol/resources/attitude.png", "rb");
  attitude_dial_png = IMG_LoadPNG_RW(rwop);
  if(attitude_dial_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/attitude_horizon.png", "rb");
  attitude_horizon_png = IMG_LoadPNG_RW(rwop);
  if(attitude_horizon_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/attitude_crosshair.png", "rb");
  attitude_crosshair_png = IMG_LoadPNG_RW(rwop);
  if(attitude_crosshair_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/specular.png", "rb");
  specular_png = IMG_LoadPNG_RW(rwop);
  if(specular_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    
  
  //Position the image
  attitude_rect.x = DIAL_WIDTH*2;
  attitude_rect.y = 0;
  attitude_rect.w = attitude_dial_png->w;
  attitude_rect.h = attitude_dial_png->h;

  //------------------
  //heading
  //------------------
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/heading_bg.png", "rb");
  heading_bg_png = IMG_LoadPNG_RW(rwop);
  if(heading_bg_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  //Position the image
  heading_rect.x = DIAL_WIDTH*3;
  heading_rect.y = 0;
  heading_rect.w = heading_bg_png->w;
  heading_rect.h = heading_bg_png->h;
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/heading_dial.png", "rb");
  heading_dial_png = IMG_LoadPNG_RW(rwop);
  if(heading_dial_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/heading_arrow.png", "rb");  
  heading_arrow_png = IMG_LoadPNG_RW(rwop);
  if(heading_arrow_png  == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/heading_waypoint.png", "rb");  
  heading_waypoint_png = IMG_LoadPNG_RW(rwop);
  if(heading_waypoint_png  == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    
  //------------------
  //altitude
  //------------------
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/altitude_dial.png", "rb");
  altitude_dial_png = IMG_LoadPNG_RW(rwop);
  if(altitude_dial_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/altitude_hundreds_needle.png", "rb");
  altitude_hundreds_needle_png = IMG_LoadPNG_RW(rwop);
  if(altitude_hundreds_needle_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
    
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/altitude_thousands_needle.png", "rb");
  altitude_thousands_needle_png = IMG_LoadPNG_RW(rwop);
  if(altitude_thousands_needle_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  //Position the image
  altitude_rect.x = DIAL_WIDTH*4;
  altitude_rect.y = 0;
  altitude_rect.w = altitude_dial_png->w;
  altitude_rect.h = altitude_dial_png->h;
  
  //------------------
  //variometer
  //------------------
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/variometer_dial.png", "rb");
  variometer_dial_png = IMG_LoadPNG_RW(rwop);
  if(variometer_dial_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/variometer_needle.png", "rb");
  variometer_needle_png = IMG_LoadPNG_RW(rwop);
  if(variometer_needle_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  //Position the image
  variometer_rect.x = DIAL_WIDTH*5;
  variometer_rect.y = 0;
  variometer_rect.w = variometer_dial_png->w;
  variometer_rect.h = variometer_dial_png->h;
    
  //------------------
  //airspeed
  //------------------
  
  rwop=SDL_RWFromFile("/opt/missioncontrol/resources/airspeed_dial.png", "rb");
  airspeed_dial_png = IMG_LoadPNG_RW(rwop);
  if(airspeed_dial_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
    rwop=SDL_RWFromFile("/opt/missioncontrol/resources/airspeed_needle.png", "rb");
  airspeed_needle_png = IMG_LoadPNG_RW(rwop);
  if(airspeed_needle_png == NULL)
    fprintf(stderr, "Unable to load image: %s\n", IMG_GetError());
  
  //Position the image
  airspeed_rect.x = DIAL_WIDTH*6;
  airspeed_rect.y = 0;
  airspeed_rect.w = airspeed_dial_png->w;
  airspeed_rect.h = airspeed_dial_png->h;
  
    
}

// Free the PNG and BMP images
void free_sprites(void){
  SDL_FreeSurface(navputer_png);
  SDL_FreeSurface(variometer_dial_png);
  SDL_FreeSurface(variometer_needle_png);
  SDL_FreeSurface(heading_bg_png);
  SDL_FreeSurface(heading_dial_png);
  SDL_FreeSurface(heading_arrow_png);
  SDL_FreeSurface(heading_waypoint_png);
  SDL_FreeSurface(airspeed_dial_png);
  SDL_FreeSurface(airspeed_needle_png);
  SDL_FreeSurface(attitude_dial_png);
  SDL_FreeSurface(attitude_horizon_png);
  SDL_FreeSurface(attitude_crosshair_png);
  SDL_FreeSurface(altitude_dial_png);
  SDL_FreeSurface(altitude_hundreds_needle_png);
  SDL_FreeSurface(altitude_thousands_needle_png);
  SDL_FreeSurface(specular_png);
}

// Draw the graphics onscreen
void render_screen(void){
  // Clear the full screen
  SDL_FillRect(screen, NULL, bgColor);

  // Redraw the images
  render_navputer();
  
  
  // Variometer Gauge
  SDL_BlitSurface(variometer_dial_png, NULL, screen, &variometer_rect);
  
  // Rotate the needle sprite
  variometer_needle_png_rotated = rotozoomSurface(variometer_needle_png, ceil((-1)*variometer_degrees), 1, rotozoom_aa_quality_mode); 
  
  variometer_rotated_rect.x = ceil(variometer_rect.x + (variometer_needle_png->w - variometer_needle_png_rotated->w)/2.0);
  variometer_rotated_rect.y = ceil(variometer_rect.y + (variometer_needle_png->h - variometer_needle_png_rotated->h)/2.0)-1;
  variometer_rotated_rect.w = variometer_rect.w;
  variometer_rotated_rect.h = variometer_rect.h;
  SDL_BlitSurface(variometer_needle_png_rotated, NULL, screen, &variometer_rotated_rect); 
  SDL_FreeSurface(variometer_needle_png_rotated);
  
  //Static Dial Mode
  //SDL_BlitSurface(variometer_needle_png, NULL, screen, &variometer_rect);
  
  
  // Heading Gauge
  SDL_BlitSurface(heading_bg_png, NULL, screen, &heading_rect); 
  
  // Debug heading angle
  //heading_degrees += (frame_counter % 10);
  //heading_degrees = (int)heading_degrees % 360;
  
  
  // Rotate the heading dial sprite
  heading_dial_png_rotated = rotozoomSurface(heading_dial_png, ceil(heading_degrees), 1, rotozoom_aa_quality_mode); 
  
  heading_dial_rotated_rect.x = ceil(heading_rect.x + (heading_dial_png->w - heading_dial_png_rotated->w)/2.0);
  heading_dial_rotated_rect.y = ceil(heading_rect.y + (heading_dial_png->h - heading_dial_png_rotated->h)/2.0)-1;
  heading_dial_rotated_rect.w = heading_rect.w;
  heading_dial_rotated_rect.h = heading_rect.h;
  SDL_BlitSurface(heading_dial_png_rotated, NULL, screen, &heading_dial_rotated_rect); 
  SDL_FreeSurface(heading_dial_png_rotated);
  
  //Static Dial Mode
  //SDL_BlitSurface(heading_dial_png, NULL, screen, &heading_rect); 
  
  SDL_BlitSurface(heading_arrow_png, NULL, screen, &heading_rect); 
  SDL_BlitSurface(heading_waypoint_png, NULL, screen, &heading_rect); 
  
  
  // Airspeed Gauge
  SDL_BlitSurface(airspeed_dial_png, NULL, screen, &airspeed_rect);
  
  // Rotate the needle sprite
  airspeed_needle_png_rotated = rotozoomSurface(airspeed_needle_png, ceil(airspeed_degrees), 1, rotozoom_aa_quality_mode); 
  
  airspeed_rotated_rect.x = ceil(airspeed_rect.x + (airspeed_needle_png->w - airspeed_needle_png_rotated->w)/2.0);
  airspeed_rotated_rect.y = ceil(airspeed_rect.y + (airspeed_needle_png->h - airspeed_needle_png_rotated->h)/2.0)-1;
  airspeed_rotated_rect.w = airspeed_rect.w;
  airspeed_rotated_rect.h = airspeed_rect.h;
  SDL_BlitSurface(airspeed_needle_png_rotated, NULL, screen, &airspeed_rotated_rect); 
  SDL_FreeSurface(airspeed_needle_png_rotated);
  
  //Static Dial Mode
  //SDL_BlitSurface(airspeed_needle_png, NULL, screen, &airspeed_rect);
  
  
    
  SDL_BlitSurface(attitude_horizon_png, NULL, screen, &attitude_rect);  
  SDL_BlitSurface(attitude_dial_png, NULL, screen, &attitude_rect);
  SDL_BlitSurface(attitude_crosshair_png, NULL, screen, &attitude_rect);
  SDL_BlitSurface(specular_png, NULL, screen, &attitude_rect);

  // Altitude Gauge

  SDL_BlitSurface(altitude_dial_png, NULL, screen, &altitude_rect);
  
  // Rotate the hundreds needle sprite
  altitude_hundreds_needle_png_rotated = rotozoomSurface(altitude_hundreds_needle_png, ceil((-1)*altitude_hundreds_degrees), 1, rotozoom_aa_quality_mode); 
  
  altitude_hundreds_rotated_rect.x = ceil(altitude_rect.x + (altitude_hundreds_needle_png->w - altitude_hundreds_needle_png_rotated->w)/2.0);
  altitude_hundreds_rotated_rect.y = ceil(altitude_rect.y + (altitude_hundreds_needle_png->h - altitude_hundreds_needle_png_rotated->h)/2.0)-1;
  altitude_hundreds_rotated_rect.w = altitude_rect.w;
  altitude_hundreds_rotated_rect.h = altitude_rect.h;
  SDL_BlitSurface(altitude_hundreds_needle_png_rotated, NULL, screen, &altitude_hundreds_rotated_rect); 
  SDL_FreeSurface(altitude_hundreds_needle_png_rotated);
  
  //Static Dial Mode
  //SDL_BlitSurface(altitude_hundreds_needle_png, NULL, screen, &altitude_rect);
  
  // Rotate the thousands needle sprite
  altitude_thousands_needle_png_rotated = rotozoomSurface(altitude_thousands_needle_png, ceil((-1)*altitude_thousands_degrees), 1, rotozoom_aa_quality_mode); 
  
  altitude_thousands_rotated_rect.x = ceil(altitude_rect.x + (altitude_thousands_needle_png->w - altitude_thousands_needle_png_rotated->w)/2.0);
  altitude_thousands_rotated_rect.y = ceil(altitude_rect.y + (altitude_thousands_needle_png->h - altitude_thousands_needle_png_rotated->h)/2.0)-1;
  altitude_thousands_rotated_rect.w = altitude_rect.w;
  altitude_thousands_rotated_rect.h = altitude_rect.h;
  SDL_BlitSurface(altitude_thousands_needle_png_rotated, NULL, screen, &altitude_thousands_rotated_rect); 
  SDL_FreeSurface(altitude_thousands_needle_png_rotated);
  
  //Static Dial Mode
  //SDL_BlitSurface(altitude_thousands_needle_png, NULL, screen, &altitude_rect);
    
  SDL_Flip(screen);
}

//Draw the navputer text
void render_navputer(void){
  char GPSStatus[20] = " ";
  char GPSMode[50] = " ";
  char GPSCombined[255];
  
  
  char number_of_satellites_string[50]= "0";
  char bearing_string[100] = "0.0";
  char waypoint_string[100] = "0.0";
  char heading_combined[255];
  
  char lat_string[50] = "0.0";
  char long_string[50] = "0.0";
  char latlong_combined[255];
  
  char altitude_in_feet_string[50] = "0.0";
  char airspeed_in_km_string[50] = "0.0";
  char speed_alt_combined[255];

  // Navputer Background image
  SDL_BlitSurface(navputer_png, NULL, screen, &navputer_rect);
  
  // GPS Status Mode Text
	if(gprmc.status[0] == 'A') {
		strcpy(GPSStatus, "Active");
	}	else if(gprmc.status[0] == 'V')	{
		strcpy(GPSStatus, "Void");
	}	else {
		//strcpy(GPSStatus, "");
		strcpy(GPSStatus, "Awaiting Data...");
	}
	
	
	if(gprmc.mode[0] == 'A') {
		strcpy(GPSMode, "Autonomous Mode");
	}	else if(gprmc.mode[0] == 'S') {
		strcpy(GPSMode, "Simulator Mode");
	}	else if(gprmc.mode[0] == 'E')	{
		strcpy(GPSMode, "Estimated Mode");
	}	else if(gprmc.mode[0] == 'D')	{
		strcpy(GPSMode, "Differential Mode");
	}	else if(gprmc.mode[0] == 'N')	{
		strcpy(GPSMode, "Not Valid Mode");
	}	else {
		strcpy(GPSMode, "");
	}
	
	// Merge the GPS status strings 
	// Example String: "Status: Active - Differential Mode"
	strcpy(GPSCombined, "Status: ");
	strcat(GPSCombined, GPSStatus);
	strcat(GPSCombined, " - ");
	strcat(GPSCombined, GPSMode);
	
	#define zero_air_speed 16
	#define max_air_speed 73
	  
  // Bearing and Waypoint display
  
  // Debug heading
  //heading_degrees = 180;
  
  
  // Airspeed Gauge km to degrees rotation
	if(strlen(gprmc.speed_over_ground)){
		//convert knots to km / hr
		sscanf(gprmc.speed_over_ground, "%lf", &airspeed_in_km);
		airspeed_in_km *= 1.852;
	} else {
    airspeed_in_km = 0;
	}
	
	// Airspeed debugging
	//airspeed_in_km = 50;
	if(airspeed_in_km > 16){
	  // Show the regular airspeed
	  airspeed_degrees = 92.4+(airspeed_in_km*-5.7);
    if (airspeed_in_km > max_air_speed){
      // Limit the max airspeed to 73 km
      airspeed_degrees = 92.4+(max_air_speed*-5.7);
    }
	} else {
	  // Set the needle to "zero" km notch which is the 16 km position
	  airspeed_degrees = 92.4+(zero_air_speed*-5.7);
	}
	

	sscanf(gprmc.track_angle, "%lf", &heading_degrees);
	sscanf(gpgga.fix_quality, "%d", &fix_quality);
	sscanf(gpgga.number_of_satellites, "%d", &number_of_satellites);
	
	IntToStr(number_of_satellites, number_of_satellites_string);
	DoubleToStr(heading_degrees, bearing_string);
	DoubleToStr(heading_waypoint_degrees, waypoint_string);
	
	//Merge Strings
	strcpy(heading_combined, "Sats: ");
	strcat(heading_combined, number_of_satellites_string);
  strcat(heading_combined, "   Bearing: ");
  strcat(heading_combined, bearing_string);
  //strcat(heading_combined, "°   Waypt: ");
  strcat(heading_combined, "   Waypt: ");
  strcat(heading_combined, waypoint_string);
  //strcat(heading_combined, "°");


  // LatLong Display
  sprintf(lat_string,"%3.4lf", gpgga.latitude_degrees);
  sprintf(long_string,"%3.4lf", gpgga.longitude_degrees);

	//Merge Strings
  strcpy(latlong_combined, "Lat: ");
  strcat(latlong_combined, lat_string);
  strcat(latlong_combined, gpgga.northing);
  strcat(latlong_combined, "  Long: ");
  strcat(latlong_combined, long_string);
  strcat(latlong_combined, gpgga.easting);
                           
                          
  // Speed and Altitude Display  

  altitude_in_meters =  gpgga.altitude_double;
  
  //Debug testing
  //altitude_in_meters += ((rand() % 2) - 1);
  //altitude_in_meters += frame_counter * 30.0;
   
  altitude_in_feet = altitude_in_meters * 3.2808399;
  
  	
	// Variometer Gauge
	#define max_vario_change 2000
	#define min_vario_change -2000
	
	#define gps_update_rate 1.0
	
	//Variometer change rate
	//debug
	//variometer_altitude_change += ((rand() % 100) - 50);

	//Vario change per minute calculated per update cycle
	variometer_altitude_change = ((float)(altitude_in_feet - previous_altitude) / gps_update_rate) * 60;
	
	if(variometer_altitude_change>max_vario_change){
	  //Clamp max at 2000 ft
  	variometer_degrees = (max_vario_change * 8.6) * 0.01;
	} else if (variometer_altitude_change<min_vario_change){
		//Clamp min at -2000 ft
  	variometer_degrees = (min_vario_change * 8.6) * 0.01;
	} else{
	  //Convert the regular vario range of -2000 to 2000 to degrees
	  variometer_degrees = (variometer_altitude_change * 8.6) * 0.01;
	}
	
	
	//Altitude Gauge
	
	altitude_thousands_degrees = altitude_in_feet * 0.036;
  altitude_hundreds_degrees = ( (float)((int)altitude_in_feet % 1000) ) * 0.36;
  
  sprintf(airspeed_in_km_string,"%3.1lf", airspeed_in_km);
  sprintf(altitude_in_feet_string,"%3.1lf", altitude_in_feet);                   
  
  //Merge Strings
  strcpy(speed_alt_combined, "Speed: ");
  strcat(speed_alt_combined, airspeed_in_km_string);
  strcat(speed_alt_combined, " km/hr  Altitude: ");
  strcat(speed_alt_combined, altitude_in_feet_string);
  strcat(speed_alt_combined, " feet");                                  
                                                                                                                                    
  // Write the text
  // GPS Status
  TFT_Write_Text_Centered(GPSCombined, display_area_rect.y+(character_height*0) ); 
  
  // Bearing and Heading
  TFT_Write_Text_Centered(heading_combined, display_area_rect.y+(character_height*1)+1 );
  
  // Latitude and Longitude 
  TFT_Write_Text_Centered(latlong_combined, display_area_rect.y+(character_height*2)+1 );
  
  // Speed and Altitude
  TFT_Write_Text_Centered(speed_alt_combined, display_area_rect.y+(character_height*3)+1 ); 
  
}

void IntToStr(int input, char *output){
  sprintf(output,"%0d", input);
}

void DoubleToStr(double input, char *output){
  sprintf(output,"%3.1lf", input);
}

//Write text to the SDL screen
int TFT_Write_Text(char *textstring, int x_pos, int y_pos){
  SDL_Color text_color;
  
  // White text
  //text_color.r = 255;
  //text_color.g = 255;
  //text_color.b = 255;
  
  // Yellow text
  //text_color.r = 239;
  //text_color.g = 189;
  //text_color.b = 0;
  
  // Green text
  text_color.r = 0;
  text_color.g = 255;
  text_color.b = 0;
  
  SDL_Surface *textSurface = TTF_RenderText_Blended(fnt, textstring, text_color);
  
  SDL_Rect textDestRect;
  
  if (textSurface) {
    // Center the text horizontally and position it
    textDestRect.x = x_pos;
    textDestRect.y = y_pos;
    textDestRect.w = textSurface->w;
    textDestRect.h = textSurface->h;
    SDL_BlitSurface(textSurface, NULL, screen, &textDestRect);
    SDL_FreeSurface(textSurface);
  }
  //SDL_Flip(screen);	
  return textDestRect.w;
}

//Write text to the SDL screen
int TFT_Write_Text_Centered(char *textstring, int y_pos){
  const NAVPUTER_FRAME_WIDTH = 256;
  SDL_Color text_color;
  
  // White text
  //text_color.r = 255;
  //text_color.g = 255;
  //text_color.b = 255;
  
  // Yellow text
  //text_color.r = 239;
  //text_color.g = 189;
  //text_color.b = 0;
  
  // Check for a GPS Signal
  if(gprmc.status[0] == 'V'){  
    // Void GPS data is shown with Red text
    text_color.r = 255;
    text_color.g = 0;
    text_color.b = 0;
  } else {
    // Active GPS data is shown with Green text
    text_color.r = 0;
    text_color.g = 255;
    text_color.b = 0;
  }
  
  SDL_Surface *textSurface = TTF_RenderText_Blended(fnt, textstring, text_color);
  
  SDL_Rect textDestRect;
  
  if (textSurface) {
    // Center the text horizontally and position it
    textDestRect.x = (NAVPUTER_FRAME_WIDTH/2)-(textSurface->w/2);
    textDestRect.y = y_pos;
    textDestRect.w = textSurface->w;
    textDestRect.h = textSurface->h;
    SDL_BlitSurface(textSurface, NULL, screen, &textDestRect);
    SDL_FreeSurface(textSurface);
  }
  //SDL_Flip(screen);	
  return textDestRect.w;
}


void get_string(int fd, char *data){
	char buffer[255];
	
	char data_chunk;
	int i = 0;
	int s = 0;
	int need_data=0;	
	
	need_data = 1;
	
	while(need_data){
		read(fd,&data_chunk, sizeof(char));
		buffer[i] = data_chunk;
		
		// Null terminate
		buffer[254] = 0;
		
		// Check for end of line \n
		if(buffer[i] == '\n'){
			
			//printf("%s",buffer);
			// Check for $ starting character
			if(buffer[0] == '$'){
				//printf("Got string\n");
				strcpy(data,buffer);
				need_data = 0; //exit while loop when we have a string
				// Set to minus one because of the i++ at the bottom of the loop
				
				// Write a gps string to the nmea logfile
				if(gps_log_fp){
  				fprintf(gps_log_fp, data);
  		  }
			}
			i=-1;
			
			
			for(s=0;s<=254;s++){
				buffer[s] = 0;
			}
		}
		
		
		i++;
		if(i >=254){
			i=0;
		}
	}
}



void parseGPS(char *input_string){
	int i = 0;
	int p = 0;
	int start, length;
	char *starting_position = NULL;
	int comma_positions[MAX_COMMA_POSITIONS];
	
	char temp_position_string[15] = " ";
	
	double lat_degrees;
	double lat_minutes;
	
	double long_degrees;
	double long_minutes;
		
	int totalCommas;
	
	//---------------------
	//
	// Scan for commas in input_string
	//
	//---------------------
	
  totalCommas = 0;
    
	//clean comma positions array
	for(i=0;i<MAX_COMMA_POSITIONS;i++){
		comma_positions[i] = 0;
	}
	
	//scan for comma positions
	for(i=0;i<=strlen(input_string);i++){
		if (input_string[i]	== ','){
			//printf("[%d]:Comma at:%d\n",p, i);
			comma_positions[p] =  i;
			p++;
		}
	}
	
  totalCommas = p;
    
	if(strlen(input_string) < 80) {
		
		//---------------------
		//
		//Check for GPGGA Data
		//
		//---------------------
		
		//strstr checks for a match in a string, it returns the address of the first matching 
		// character, and NULL if no match was found. 
		if(strstr(input_string, "$GPGGA") != NULL){
			printf("$GPGGA string match\n");
			
			//there should be 12 commas in a valid gprmc string
			if (totalCommas == 14) {
				
				
				gpgga.calculated_checksum = 0;
				gpgga.converted_checksum = 0;
				
				//---------------------
				//
				// checksum
				//
				//---------------------
				// steps back from the end of string's /r/n
				
				//start = strlen(input_string)-4;
				start = strlen(input_string)-3;
				length = (strlen(input_string)-2)-(strlen(input_string)-4);
				//length = (strlen(input_string)-1)-(strlen(input_string)-4);
				
				if (length>=1){
					starting_position = &input_string[start];
					
					printf("Starting character:%d, string length:%d\n",start, length);
					
					strncpy(gpgga.checksum, starting_position,length);
					//terminate string and remove carriage return
					gpgga.checksum[length] = 0;
					
					printf("Checksum: %s\n",gpgga.checksum);
					
				} else {
					printf("Checksum length is less than 1\n");
				}
				
				//---------------------
				//
				//calculate checksum
				//
				//---------------------
				
				//run from just after the $ dollar sign to the last comma before the asterisk.
				//for(i=1;i<strlen(input_string)-7;i++){
				for(i=1;i<strlen(input_string)-6;i++){
					gpgga.calculated_checksum ^= input_string[i];
					//%x is the hex format specifier
					//printf("[%d]: %d\n",i,gpgga.calculated_checksum);
				}
				printf("Calculated checksum:%d hex:%X\n",gpgga.calculated_checksum,gpgga.calculated_checksum);
				
				//convert gpgga.checksum string to integer for comparison
				sscanf(gpgga.checksum, "%X", &gpgga.converted_checksum);
				printf("GPS checksum to int:%d\n", gpgga.converted_checksum);
				
				
				if(gpgga.calculated_checksum == gpgga.converted_checksum){
					printf("GPGGA Checksum match!\n\n");

					//---------------------
					//
					// fix time in utc
					//
					//---------------------
					
					start = comma_positions[0]+1;
					length = (comma_positions[1]-comma_positions[0])-1;
					
					if (length>=1){
						
						//UTC length must equal 9 to be valid
						if (length == 9){
							
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gpgga.fix_time_utc, starting_position,length);
							//terminate string
							gpgga.fix_time_utc[length] = 0;
							
							printf("Fix Time: %s\n",gpgga.fix_time_utc);
							
						} else {
							printf("UTC length is greater than 9\n");
						}
						
						printf("UTC string length:%d\n", length);
						
					} else {
						printf("UTC length is less than 1\n");
					}
					
					//---------------------
					//
					// latitude
					//
					//---------------------
					
					start = comma_positions[1]+1;
					length = (comma_positions[2]-comma_positions[1])-1;
					//start latitude length check
					if (length>=1){

						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.latitude, starting_position,length);
						//terminate string
						gpgga.latitude[length] = 0;
						
						printf("Latitude: %s\n",gpgga.latitude);
						
						//---------------------
						//
						// northing
						//
						//---------------------
						
						start = comma_positions[2]+1;
						length = (comma_positions[3]-comma_positions[2])-1;
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gpgga.northing, starting_position,length);
							//terminate string
							gpgga.northing[length] = 0;
							
							printf("Northing: %s\n",gpgga.northing);
							
							
						}	else {
							printf("Northing length is less than 1\n");
						}
						
						//---------------------
						//
						// convert latitude + northing to degrees
						//
						//---------------------
						
						//clean temporary positions array
						for(i=0;i<15;i++){
							temp_position_string[i] = 0;
						}
						
						//copy degrees section of latitude
						start = 0;
						length = 2;
						starting_position = &gpgga.latitude[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(temp_position_string, starting_position,length);
						//terminate string
						temp_position_string[length] = 0;
						
						printf("Latitude in Degrees string: %s\n",temp_position_string);
						
						sscanf(temp_position_string, "%lf", &lat_degrees);
						
						printf("Latitude in Degrees float: %lf\n",lat_degrees);
						
						//clean temporary positions array
						for(i=0;i<15;i++){
							temp_position_string[i] = 0;
						}
						
						//copy minutes section of latitude
						start = 2;
						length = strlen(gpgga.latitude)-2;
						starting_position = &gpgga.latitude[start];
						
						printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(temp_position_string, starting_position,length);
						//terminate string
						temp_position_string[length] = 0;
						
						printf("Latitude in Minutes string: %s\n",temp_position_string);
						
						sscanf(temp_position_string, "%lf", &lat_minutes);
						
						printf("Latitude in Minutes float: %lf\n",lat_minutes);
						
						//convert decimal minutes to decimal degrees
						lat_minutes /= 60.00;
						
						printf("Latitude Minutes in decimal degrees float: %lf\n",lat_minutes);
						
						lat_degrees += lat_minutes;
						
						//test forcing to south value
						//gpgga.northing[0] = 'S';
						
						// add minus sign for southern latitudes
						if (gpgga.northing[0] == 'S'){
							lat_degrees *= -1.0;
						}
						
						printf("Combined lat degrees + lat minutes in decimal degrees: %lf\n",lat_degrees);
						
						gpgga.latitude_degrees = lat_degrees;
						//end latitude length check
					} else{
						printf("Latitude length is less than 1\n");
					}
					
					//---------------------
					//
					// longitude
					//
					//---------------------
					
					start = comma_positions[3]+1;
					length = (comma_positions[4]-comma_positions[3])-1;
					
					//start latitude length check
					if (length>=1){
						
						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.longitude, starting_position,length);
						//terminate string
						gpgga.longitude[length] = 0;
						
						printf("Longitude: %s\n",gpgga.longitude);
						
						//---------------------
						//
						// easting
						//
						//---------------------
						
						start = comma_positions[4]+1;
						length = (comma_positions[5]-comma_positions[4])-1;
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gpgga.easting, starting_position,length);
							//terminate string
							gpgga.easting[length] = 0;
							
							printf("Easting: %s\n",gpgga.easting);
							
						}	else {
							printf("Easting length is less than 1\n");
						}
						
						//---------------------
						//
						// convert longitude + easting to degrees
						//
						//---------------------
						
						//clean temporary positions array
						for(i=0;i<15;i++){
							temp_position_string[i] = 0;
						}
						
						//copy degrees section of latitude
						start = 0;
						length = 3;
						starting_position = &gpgga.longitude[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(temp_position_string, starting_position,length);
						//terminate string
						temp_position_string[length] = 0;
						
						printf("Longitude in Degrees string: %s\n",temp_position_string);
						
						sscanf(temp_position_string, "%lf", &long_degrees);
						
						printf("Longitude in Degrees float: %lf\n",long_degrees);
						
						//clean temporary positions array
						for(i=0;i<15;i++){
							temp_position_string[i] = 0;
						}
						
						//copy minutes section of latitude
						start = 3;
						length = strlen(gpgga.longitude)-2;
						starting_position = &gpgga.longitude[start];
						
						printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(temp_position_string, starting_position,length);
						//terminate string
						temp_position_string[length] = 0;
						
						printf("Longitude in Minutes string: %s\n",temp_position_string);
						
						sscanf(temp_position_string, "%lf", &long_minutes);
						
						printf("Longitude in Minutes float: %lf\n",long_minutes);
						
						//convert decimal minutes to decimal degrees
						long_minutes /= 60.00;
						
						printf("Longitude Minutes in decimal degrees float: %lf\n",long_minutes);
						
						long_degrees += long_minutes;
						
						//test forcing to east value
						//gpgga.easting[0] = 'E';
						
						// add minus sign for western longitudes
						if (gpgga.easting[0] == 'W'){
							long_degrees *= -1.0;
						}
						
						printf("Combined long degrees + long minutes in decimal degrees: %lf\n",long_degrees);
						
						gpgga.longitude_degrees = long_degrees;
						//end latitude length check
					} else {
						printf("Longitude length is less than 1\n");
					}
					
					//---------------------
					//
					// fix_quality
					//
					//---------------------
					start = comma_positions[5]+1;
					length = (comma_positions[6]-comma_positions[5])-1;
					
					if (length>=1){
						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.fix_quality, starting_position,length);
						//terminate string
						gpgga.fix_quality[length] = 0;
						
						printf("Fix Quality: %s\n",gpgga.fix_quality);
					} else {
						printf("Fix Quality length is less than 1\n");
					}
					
					//---------------------
					//
					// Number of Satellites  
					//
					//---------------------
					start = comma_positions[6]+1;
					length = (comma_positions[7]-comma_positions[6])-1;
					if (length>=1){
						
						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.number_of_satellites, starting_position,length);
						//terminate string
						gpgga.number_of_satellites[length] = 0;
						
						printf("Number of Satellites: %s\n",gpgga.number_of_satellites);
					} else {
						printf("Number of Satellites length is less than 1\n");
					}
					
					//---------------------
					//
					// Horizontal Dilution of Precision (HDOP)
					//
					//---------------------
					start = comma_positions[7]+1;
					length = (comma_positions[8]-comma_positions[7])-1;
					if (length>=1){
						
						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.hdop, starting_position,length);
						//terminate string
						gpgga.hdop[length] = 0;
						
						printf("Horizontal Dilution of Precision (HDOP): %s\n",gpgga.hdop);
						
					} else {
						printf("HDOP length is less than 1\n");
					}
					
					//---------------------
					//
					// altitude
					//
					//---------------------
					
					start = comma_positions[8]+1;
					length = (comma_positions[9]-comma_positions[8])-1;
					if (length>=1){
						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.altitude, starting_position,length);
						//terminate string
						gpgga.altitude[length] = 0;
						
						printf("Altitude: %s\n",gpgga.altitude);
						
						
						sscanf(gpgga.altitude, "%lf", &gpgga.altitude_double);
						printf("Altitude as double:%lf\n", gpgga.altitude_double);
						
						// altitude units
						start = comma_positions[9]+1;
						length = (comma_positions[10]-comma_positions[9])-1;
						starting_position = &input_string[start];
						
						//printf("Starting character:%d, string length:%d\n",start, length);
						
						strncpy(gpgga.altitude_units, starting_position,length);
						//terminate string
						gpgga.altitude_units[length] = 0;
						
						printf("Altitude Units: %s\n",gpgga.altitude_units);

					} else {
						printf("Altitude length is less than 1\n");
					}
					
					
					//gpgga.height_of_geoid[0] = 0;
					//gpgga.height_of_geoid_units[0] = 0; //M
					//gpgga.last_dgps_update_time[0] = 0;
					//gpgga.dgps_id[0] = 0;
					
				} //end checsum match
				else{
					printf("\n\nGPGGA Checksum does not match!\n\n");
				}
				
			}//end total commas check
			else{
				printf("Wrong numbers of commas for $GPGGA string: %d\n", totalCommas);
			}    
		}// end of GPGGA match if
		
		
		//---------------------
		//
		//Check for GPRMC Data
		//
		//---------------------
		
		
		if(strstr(input_string, "$GPRMC") != NULL){
			printf("$GPRMC string match\n");
			
			//there should be 12 commas in a valid gprmc string
			if (totalCommas == 12) {
						
				//---------------------
				//
				// checksum
				//
				//---------------------
				gprmc.calculated_checksum = 0;
				gprmc.converted_checksum = 0;
				
				//start = strlen(input_string)-4;
				start = strlen(input_string)-3;
				//length = (strlen(input_string)-1)-(strlen(input_string)-4);
				length = (strlen(input_string)-2)-(strlen(input_string)-4);
				
				if (length>=1){
					starting_position = &input_string[start];
					
					//printf("Starting character:%d, string length:%d\n",start, length);
					
					strncpy(gprmc.checksum, starting_position,length);
					//terminate string and remove carriage return
					//gprmc.checksum[length-2] = 0;
					
					printf("Checksum:%s\n",gprmc.checksum);
					
					//calculate checksum
					//run from just after the $ dollar sign to the last comma before the asterisk.
					//for(i=1;i<strlen(input_string)-5;i++){
					for(i=1;i<strlen(input_string)-4;i++){
						gprmc.calculated_checksum ^= input_string[i];
						//%x is the hex format specifier
						//printf("[%d]: %d\n",i,gpgga.calculated_checksum);
					}
					printf("Calculated checksum:%d hex:%X\n",gprmc.calculated_checksum,gprmc.calculated_checksum);
					
					
					//convert gpgga.checksum string to integer for comparison
					sscanf(gprmc.checksum, "%X", &gprmc.converted_checksum);
					printf("GPS checksum to int:%d\n", gprmc.converted_checksum);
					
					// checksum match check
					if(gprmc.calculated_checksum == gprmc.converted_checksum){
						printf("GPRMC Checksum match!\n\n");
						

						//---------------------
						//
						// status
						//
						//---------------------
						
						start = comma_positions[1]+1;
						length = (comma_positions[2]-comma_positions[1])-1;
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gprmc.status, starting_position,length);
							//terminate string
							gprmc.status[length] = 0;
							
							printf("Status: %s = ",gprmc.status);
							if ( gprmc.status[0] == 'A'){
								printf("GPS data is active.\n");
							}
							
							if ( gprmc.status[0] == 'V'){
								printf("GPS data is void.\n");
							}
							
						} else {
							printf("GPS Status length is less than 1\n");
						}
						
						//---------------------
						//
						// speed over ground
						//
						//---------------------
						
						start = comma_positions[6]+1;
						length = (comma_positions[7]-comma_positions[6])-1;
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gprmc.speed_over_ground, starting_position,length);
							//terminate string
							gprmc.speed_over_ground[length] = 0;
							
							printf("Speed Over Ground:%s Knots\n",gprmc.speed_over_ground);
							
						} else {
							printf("Speed Over Ground length is less than 1\n");
						}
						
						//---------------------
						//
						// track angle
						//
						//---------------------
						start = comma_positions[7]+1;
						length = (comma_positions[8]-comma_positions[7])-1;
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gprmc.track_angle, starting_position, length);
							//terminate string
							gprmc.track_angle[length] = 0;
							
							printf("Track Angle:%s\n",gprmc.track_angle);
							
						} else {
							printf("Track Angle length is less than 1\n");
						}
						
						//---------------------
						//
						// date
						//
						//---------------------
						start = comma_positions[8]+1;
						length = (comma_positions[9]-comma_positions[8])-1;
						
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gprmc.date, starting_position,length);
							//terminate string
							gprmc.date[length] = 0;
							
							printf("Date:%s\n",gprmc.date);
							
						} else {
							printf("Date length is less than 1\n");
						}
						
						//---------------------
						//
						// mode
						//
						//---------------------
						start = comma_positions[11]+1;
						length = ((comma_positions[11]+2)-comma_positions[11])-1;
						
						if (length>=1){
							starting_position = &input_string[start];
							
							//printf("Starting character:%d, string length:%d\n",start, length);
							
							strncpy(gprmc.mode, starting_position,length);
							//terminate string
							gprmc.mode[length] = 0;
							
							printf("Mode:%s = ",gprmc.mode);
							
							if(gprmc.mode[0] == 'S'){
								printf("Simulator Mode\n");
							}
							else if(gprmc.mode[0] == 'A'){
								printf("Autonomous Mode\n");
							}
							else if(gprmc.mode[0] == 'E'){
								printf("Estimated Mode\n");
							}
							else if(gprmc.mode[0] == 'D'){
								printf("Differential Mode\n");
							}
							else if(gprmc.mode[0] == 'N'){
								printf("Not Valid Mode\n");
							}
							else{
								printf("Mode Error!\n");
							}
							
						} else {
							printf("GPS Mode length is less than 1\n");
						}
						
						//end checksum match
					} else {
						printf("\n\nGPRMC Checksum does not match!\n\n");
					}
				// end checksum length check	
				} else {
					printf("Checksum Length is less than 1\n");
				}
				
				
			//end total commas check	
			} else {
				printf("Wrong numbers of commas for $GPRMC string: %d\n", totalCommas);
			} 
		} // end of GPRMC match if
	//end string length check
	} else {
    printf("NMEA sentence too long!!!\n");
  }
    
} //end parseGPS()


void kml_update(void){

  new_time = time(NULL);
  uptime = new_time - begin_program;

  printf("The program has run for %d minutes and %d seconds\n", ((new_time - program_begin_time)/60), (new_time - program_begin_time) % 60);

	//paste in minutes
	sprintf(tempconversionstring, "%2.d",(new_time - program_begin_time) / 60);
	strcpy(uptime_string, tempconversionstring);
	// seconds divider
	strcat(uptime_string, ":");
	//paste in seconds
	sprintf(tempconversionstring, "%2.d",(new_time - program_begin_time) % 60);
	strcat(uptime_string, tempconversionstring);
	
	
	//start kml writing 5 seconds after program starts
	if(((new_time - program_begin_time) % 60) > 5)
	{
		int error_status = 0; 
		error_status = generate_kml();
		
		if (error_status == -1){
			printf("KML Writing error.\n");
		}
		else
		{
			printf("Wrote KML sucessfully.\n");
		}
	} //end uptime check
	
} //end kmlTimerFired


// Save a SDL viewport snapshot to a bmp image.
void kml_save_bmp_snapshot(void){
  SDL_RWops *rwop;

  rwop=SDL_RWFromFile(bmp_snapshot_filepath, "w");
  SDL_SaveBMP_RW(screen, rwop, 0);
   
  printf("Saving a snapshot to: %s\n", bmp_snapshot_filepath);
  
  //Create a KMZ file from the BMP and KML file
  printf("Packaging KMZ resource.\n");
  
  system("sh /opt/missioncontrol/make_kmz.sh");
  
}


int generate_kml(void){

	
	//---------------------
	//
	// Google KML Writing
	//
	//---------------------
	
	#define  KMLLOG "/opt/missioncontrol/gps_logs/tracklog.kml"
	
	FILE *kml_fp;
	int points;
	//int waypoint_folder_count = 0; 
	
	// Don't overflow thewaypoint array
	if(current_waypoint < MAX_WAYPOINTS){
		strcpy(waypoint_string[current_waypoint].waypoint_time_string, uptime_string);
		latitude_degrees[current_waypoint] = gpgga.latitude_degrees;
		longitude_degrees[current_waypoint] = gpgga.longitude_degrees;
		altitude_double[current_waypoint] = gpgga.altitude_double;
		
		
		// Don't increment the waypoint if there is not a valid fix
		if(gprmc.mode[0] != 'N'){
			current_waypoint++;
		}
	}
	

  // Open the kml log file for writing
	kml_fp = fopen(KMLLOG, "w");
	//kml_fp = fopen(kml_tracklog_filepath, "w");
	
	// Check for null kml_fp file pointer
	if(kml_fp == NULL){
		printf("Could not write the KML logfile.\n");
		return -1;	
	}
	
	fprintf(kml_fp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(kml_fp,"<kml xmlns=\"http://earth.google.com/kml/2.2\">\n");
	fprintf(kml_fp,"\t<Document>\n");
	
	fprintf(kml_fp,"\t\t<name>Mission Control - Track Log</name>\n");
	fprintf(kml_fp,"\t\t<open>1</open>\n");
	fprintf(kml_fp,"\t\t<description>Mission Control reports your GPS position in real time.</description>\n");
	
	fprintf(kml_fp,"\t\t<Style id=\"currentPlacemark\">\n");
	fprintf(kml_fp,"\t\t\t<IconStyle>\n");
	fprintf(kml_fp,"\t\t\t\t<Icon>\n");
	fprintf(kml_fp,"\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/shapes/airports.png</href>\n"); 
  fprintf(kml_fp,"\t\t\t\t</Icon>\n");
	fprintf(kml_fp,"\t\t\t</IconStyle>\n");
	fprintf(kml_fp,"\t\t</Style>\n");
	
	fprintf(kml_fp,"\t\t<Style id=\"startPlacemark\">\n");
	fprintf(kml_fp,"\t\t\t<IconStyle>\n");
	fprintf(kml_fp,"\t\t\t\t<Icon>\n");
	//fprintf(kml_fp,"\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/shapes/placemark_square.png</href>\n"); 
	fprintf(kml_fp,"\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/paddle/purple-stars.png</href>\n"); //blue marker with star
    fprintf(kml_fp,"\t\t\t\t</Icon>\n");
	fprintf(kml_fp,"\t\t\t</IconStyle>\n");
	fprintf(kml_fp,"\t\t</Style>\n");
	
	fprintf(kml_fp,"\t\t<Style id=\"knotPlacemark\">\n");
	fprintf(kml_fp,"\t\t\t<IconStyle>\n");
	fprintf(kml_fp,"\t\t\t\t<Icon>\n");
	//fprintf(kml_fp,"\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/shapes/placemark_circle.png</href>\n"); 
	fprintf(kml_fp,"\t\t\t\t\t<href>http://maps.google.com/mapfiles/kml/paddle/blu-blank.png</href>\n");  //plain blue marker
	
  fprintf(kml_fp,"\t\t\t\t</Icon>\n");
	fprintf(kml_fp,"\t\t\t</IconStyle>\n");
	fprintf(kml_fp,"\t\t</Style>\n");
	
	fprintf(kml_fp,"\t\t<Folder>\n");
	fprintf(kml_fp,"\t\t\t<name>Tracklog Data</name>\n");
	fprintf(kml_fp,"\t\t\t<description>This folder contains the tracklog data.</description>\n");
	
	//not valid fix
	if(gprmc.mode[0] != 'N'){
		
		fprintf(kml_fp,"\t\t<Placemark>\n");
		fprintf(kml_fp,"\t\t<styleUrl>#startPlacemark</styleUrl>\n");	
		fprintf(kml_fp,"\t\t\t<name>%s</name>\n", "Start");
		fprintf(kml_fp,"\t\t\t<description>%s</description>\n", "This is the start of the path.");
		fprintf(kml_fp,"\t\t\t<Point>\n");
    fprintf(kml_fp,"\t\t\t<altitudeMode>absolute</altitudeMode>\n");
		fprintf(kml_fp,"\t\t\t\t<coordinates>");
		fprintf(kml_fp, "%lf,%lf,%lf\n", longitude_degrees[0], latitude_degrees[0], altitude_double[0]);
		fprintf(kml_fp,"</coordinates>\n");	
		fprintf(kml_fp,"\t\t\t</Point>\n");
		fprintf(kml_fp,"\t\t</Placemark>\n");
		
		fprintf(kml_fp,"\t\t<Placemark>\n");
		fprintf(kml_fp,"\t\t<styleUrl>#currentPlacemark</styleUrl>\n");	
		fprintf(kml_fp,"\t\t\t<name>%s</name>\n", "You Are Here");
		fprintf(kml_fp,"\t\t\t<description>%s</description>\n", "This is the current GPS position.");
		
		/*
		 fprintf(kml_fp,"\t\t\t<LookAt>\n");
		 fprintf(kml_fp,"\t\t\t\t<longitude>%lf</longitude>\n", longitude_degrees[current_waypoint-1]);
		 fprintf(kml_fp,"\t\t\t\t<latitude>%lf</latitude>\n",latitude_degrees[current_waypoint-1]);
		 fprintf(kml_fp,"\t\t\t\t<altitude>%lf</altitude>\n", altitude_double[current_waypoint-1]);
		 fprintf(kml_fp,"\t\t\t\t<range>%lf</range>\n",  altitude_double[current_waypoint-1]*1.41);
		 fprintf(kml_fp,"\t\t\t\t<tilt>45</tilt>\n");
		 fprintf(kml_fp,"\t\t\t\t<heading>%s</heading>\n", gprmc.track_angle);
		 fprintf(kml_fp,"\t\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n");
		 fprintf(kml_fp,"\t\t\t</LookAt>\n");
		 */
		
		fprintf(kml_fp,"\t\t\t<Point>\n");
        fprintf(kml_fp,"\t\t\t<altitudeMode>absolute</altitudeMode>\n");
		fprintf(kml_fp,"\t\t\t\t<coordinates>");
		fprintf(kml_fp, "%lf,%lf,%lf\n", gpgga.longitude_degrees, gpgga.latitude_degrees, gpgga.altitude_double);
		fprintf(kml_fp,"</coordinates>\n");	
		fprintf(kml_fp,"\t\t\t</Point>\n");
		fprintf(kml_fp,"\t\t</Placemark>\n");
		
		/*
		 waypoint_folder_count = (current_waypoint/step_interval)-1;
		 if (waypoint_folder_count < 0){
		 waypoint_folder_count = 0;
		 }
		 
		 fprintf(kml_fp,"\t\t<Folder>\n");
		 fprintf(kml_fp,"\t\t\t<name>%s</name>\n", "Waypoint Knots");
		 fprintf(kml_fp,"\t\t\t<description>This folder contains the %d waypoints.</description>\n", waypoint_folder_count);
		 
		 //write 10 second waypoint knots.
		 for(points=step_interval; points < (current_waypoint-step_interval); points+= step_interval){
		 //sprintf(waypoint_number, "%d",points);
		 
		 fprintf(kml_fp,"\t\t<Placemark>\n");
		 fprintf(kml_fp,"\t\t<styleUrl>#knotPlacemark</styleUrl>\n");
		 fprintf(kml_fp,"\t\t\t<name>%s</name>\n", waypoint_string[points].waypoint_time_string);
		 fprintf(kml_fp,"\t\t\t<description>This is waypoint %d, and gps sample %d.</description>\n", points/step_interval, points);
		 fprintf(kml_fp,"\t\t\t<Point>\n");
		 fprintf(kml_fp,"\t\t\t<altitudeMode>absolute</altitudeMode>\n");
		 fprintf(kml_fp,"\t\t\t\t<coordinates>");
		 fprintf(kml_fp, "%lf,%lf,%lf\n", longitude_degrees[points], latitude_degrees[points], altitude_double[points]);
		 fprintf(kml_fp,"</coordinates>\n");
		 fprintf(kml_fp,"\t\t\t</Point>\n");
		 fprintf(kml_fp,"\t\t</Placemark>\n");
		 }
		 fprintf(kml_fp,"\t\t</Folder>\n");
		 */
		fprintf(kml_fp,"\t\t<StyleMap id=\"msn_ylw-pushpin\">\n");
		fprintf(kml_fp,"\t\t\t<Pair>\n");
		fprintf(kml_fp,"\t\t\t<key>normal</key>\n");
		fprintf(kml_fp,"\t\t\t<styleUrl>#sn_ylw-pushpin</styleUrl>\n");
		fprintf(kml_fp,"\t\t\t</Pair>\n");
		fprintf(kml_fp,"\t\t\t<Pair>\n");
		fprintf(kml_fp,"\t\t\t\t<key>highlight</key>\n");
		fprintf(kml_fp,"\t\t\t\t<styleUrl>#sh_ylw-pushpin</styleUrl>\n");
		fprintf(kml_fp,"\t\t\t</Pair>\n");
		fprintf(kml_fp,"\t\t</StyleMap>\n");
		
		
		fprintf(kml_fp,"\t\t<Style id=\"sn_ylw-pushpin\">\n");
		fprintf(kml_fp,"\t\t\t<LineStyle>\n");
		fprintf(kml_fp,"\t\t\t\t<color>ff0000ff</color>\n");
		fprintf(kml_fp,"\t\t\t\t<width>3</width>\n");
		fprintf(kml_fp,"\t\t\t</LineStyle>\n");
		fprintf(kml_fp,"\t\t</Style>\n");
		
		fprintf(kml_fp,"\t\t<Placemark>\n");
		fprintf(kml_fp,"\t\t\t<name>%s</name>\n", "Mobile Pi Path");
		fprintf(kml_fp,"\t\t\t<description>This is the path of the Mobile Pi Unit. It contains %d gps samples.</description>\n", current_waypoint);
		fprintf(kml_fp,"<styleUrl>#msn_ylw-pushpin</styleUrl>\n");
		fprintf(kml_fp,"\t\t\t<LineString>\n");
		//fprintf(kml_fp,"\t\t\t\t<extrude>1</extrude>\n");
		fprintf(kml_fp,"\t\t\t\t<tessellate>1</tessellate>\n");
        fprintf(kml_fp,"\t\t\t\t<altitudeMode>absolute</altitudeMode>\n");
		fprintf(kml_fp,"\t\t\t\t<coordinates>\n");
		
		for(points=0; points < current_waypoint; points++){
			
			//printf("[%d]Lat%10.10lf\tLong%10.10lf\n",points,latitude,longitude);
			fprintf(kml_fp,"\t\t\t\t%lf,%lf,%lf\n", longitude_degrees[points], latitude_degrees[points], altitude_double[points]);
		}
		
		fprintf(kml_fp,"\t\t\t\t</coordinates>\n");	
		
		
		fprintf(kml_fp,"\t\t\t</LineString>\n");
		fprintf(kml_fp,"\t\t</Placemark>\n");
		
		
		 //print dashboard snapshot
		 //if(uptime > 1){
		 fprintf(kml_fp,"\t\t<ScreenOverlay>\n");
		 fprintf(kml_fp,"\t\t\t<name>Mission Control Dashboard</name>\n");
		 fprintf(kml_fp,"\t\t\t<visibility>1</visibility>\n");
		 fprintf(kml_fp,"\t\t\t<Icon>\n");
		 fprintf(kml_fp,"\t\t\t  <href>dashboard.bmp</href>\n");
		 fprintf(kml_fp,"\t\t\t</Icon>\n");
         
		 fprintf(kml_fp,"\t\t\t<overlayXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		 fprintf(kml_fp,"\t\t\t<screenXY x=\"0\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>\n");
		 fprintf(kml_fp,"\t\t\t<rotationXY x=\"0\" y=\"0\" xunits=\"pixels\" yunits=\"pixels\"/>\n");
		 fprintf(kml_fp,"\t\t\t<size x=\"896\" y=\"128\" xunits=\"pixels\" yunits=\"pixels\"/>\n");
		 fprintf(kml_fp,"\t\t</ScreenOverlay>\n");
		 
		 //}
     
        
	} //end not valid fix check
	
	
	fprintf(kml_fp,"\t\t</Folder>\n");
	
	fprintf(kml_fp,"\t</Document>\n");
	fprintf(kml_fp,"</kml>\n");
	
	fclose(kml_fp);
	
  //printf("Saved File %s\n", kml_tracklog_filepath);
  printf("Saved File %s\n", KMLLOG);

	
	return 0;
}

