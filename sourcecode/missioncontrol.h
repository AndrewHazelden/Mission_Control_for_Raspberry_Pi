// Mission Control for RPi + GPS Click v1.4.2 2014-02-09
// By Andrew Hazelden 
//
// email: andrew@andrewhazelden.com
// blog: http://www.andrewhazelden.com/
//
// License: GPL v3

// Serial io headers

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <time.h>

/*
* Required Graphics Libraries:
* SDL "Simple DirectMedia Layer"
* http://www.libsdl.org/
*  
* SDL_TTF
* http://www.libsdl.org/projects/SDL_ttf/
*  
* SDL_gfx
* http://www.ferzkopp.net/joomla/content/view/19/14/  
* 
* SDL_mixer
* http://www.libsdl.org/projects/SDL_mixer/
*
* SDL_image
* http://libsdl.org/projects/SDL_image/
*/


#ifdef _WIN32
#include <SDL/SDL.h>      /* Adds graphics support */
#include <SDL/SDL_ttf.h>            /* Adds font support */
#include <SDL/SDL_rotozoom.h>       /* SDL_gfx Rotozoom  */
#include <SDL/SDL_gfxPrimitives.h>  /* SDL_gfx Primitives */
#include <SDL/SDL_framerate.h>  /* SDL_gfx Framerate Manager */
#include <SDL/SDL_image.h>  /* SDL_image graphics */
#include "SDL_mixer.h"
#else
#include <SDL.h>
#include <SDL_ttf.h>            /* Adds font support */
#include <SDL_rotozoom.h>       /* SDL_gfx Rotozoom  */
#include <SDL_gfxPrimitives.h>  /* SDL_gfx Primitives */
#include <SDL_framerate.h>  /* SDL_gfx Framerate Manager */
#include <SDL_image.h>  /* SDL_image graphics */
#include "SDL_mixer.h" 
#endif

// This is the location for the Mission Control preference file
#define USF_PREFERENCE_FILE_NAME "/opt/missioncontrol/gps_prefs.usf"

#define MAXDATASIZE 300 // max number of bytes we can get at once 
#define MAX_COMMA_POSITIONS 16
#define MAX_WAYPOINTS 10000
#define DIAL_WIDTH 128

void kml_update(void);
void kml_save_bmp_snapshot(void);
int generate_kml(void);
void parseGPS(char *input_string);
void startup(void);
int open_port(char* port_name, int baud_rate);
void close_port(int fd);
void set_baud_rate(int fd, int baud_rate);
void get_string(int fd, char *data);
void init_gfx(void);
void swap_layout(void);
void save_previous(void);
void load_sprites(void);
void free_sprites(void);
void render_screen(void);
void render_navputer(void);
void SwitchFullscreen(void);
void ToggleFullscreen(void);
int TFT_Write_Text(char *textstring, int x_pos, int y_pos);
int TFT_Write_Text_Centered(char *textstring, int y_pos);
void IntToStr(int input, char *output);
void DoubleToStr(double input, char *output);
void get_input(void);

//Global Variables
//int fontsize = 12;

// Tall mode - 2 wide x 3 tall gauges
int screen_width = DIAL_WIDTH*2;  // (Number of dials visible-1) * 128
int screen_height = DIAL_WIDTH*3;   //Display is one dial high * 128

//View Layout0=Vertical Block Layout, 1=Horizontal Layout 5up, 2=Horizontal Layout 6up
int view_layout;

//int screen_width = 800;
//int screen_height = 600;
int enable_fullscreen = 0;

//Control the guague rotation anti-aliasing quality 1=AA ON, 0=AA Off
int rotozoom_aa_quality_mode = 1;

// SDL done flag
int done;

// Set the default frame rate
int fps = 10;  //10 Hz GPS Speed updates

long int frame_counter=0;
//---------------------------------
// GPS Vars

double heading_degrees;
double heading_waypoint_degrees = 0.0;
double altitude_hundreds_degrees;
double altitude_thousands_degrees;

double altitude_in_feet;
double airspeed_in_km;  
double airspeed_degrees;
double fuel_percentage;
double attitude_roll;
double attitude_pitch;
double variometer_altitude_change;
double variometer_degrees;

// Navputer data display
double ping_ms;
double rssi;
int in_kbps, out_kbps;
double latitude, longitude;
double speed_over_ground, altitude_in_meters;
double previous_altitude;
int fix_quality;
int number_of_satellites;

char buf[MAXDATASIZE];

// NMEA parsing block
char one_second_buffer[MAXDATASIZE];
char buffer_tail[MAXDATASIZE];
char one_nmea_line[MAXDATASIZE];

char gpgga_string[MAXDATASIZE];
char gprmc_string[MAXDATASIZE];

double altitude_thousands_in_feet;
double altitude_hundreds_in_feet;

int counter;
int got_gpgga;

// End gps data

int previous_fix_time_utc;
int fix_time_utc;
int elapsed_update_interval;


//---------------------
//
// gpgga = fix information
//
//--------------------- 

struct gpgga_data{
  char fix_time_utc[25];
  char latitude[25];
  char northing[2];
  char longitude[25];
  char easting[2];
  char fix_quality[2];
  char number_of_satellites[3];
  char hdop[4];
  char altitude[25];
  char altitude_units[2];  //M
  //char height_of_geoid[10];
  //char height_of_geoid_units[2]; //M
  //char last_dgps_update_time[10];
  //char dgps_id[5];
  double latitude_degrees;
  double longitude_degrees;
  double altitude_double;
  char converted_checksum;
  char checksum[6]; // always starts with an * (asterisk)
  char calculated_checksum;
  

} gpgga;


//---------------------
//
// gprmc = recomended minimum data
//
//--------------------- 


struct gprmc_data{
  //char fix_time_utc[25];
  char status[2]; //status A=active, V=void.
  //char latitude[25];
  //char northing[2];
  //char longitude[25];
  //char easting[2];
  char speed_over_ground[25]; //speed in knots
  char track_angle[25]; //track angle in degrees true
  char date[10]; //date in 2 digit pairs day month year 120308
  //char magnetic_variation[25];
  //char magnetic_variation_directon[2]; // W
  char mode[2]; //S=simulator, A=autonomous, E=estimated, D=differential, N=not valid
  char checksum[6]; // always starts with an * (asterisk)
  char calculated_checksum;
  char converted_checksum; 
} gprmc;


//returns if it has a gpgga string
int got_gpgga;
int increment_raw;
char raw_gps[10000];
int begin_program;
int uptime;
int new_time;

time_t program_begin_time;

int current_waypoint;
struct wpt_str{
    char waypoint_time_string[10];
} waypoint_string[MAX_WAYPOINTS];

double latitude_degrees[MAX_WAYPOINTS];
double longitude_degrees[MAX_WAYPOINTS];
double altitude_double[MAX_WAYPOINTS];


//used for program uptime calculations
char tempconversionstring[10] = " ";

char uptime_string[10] = " ";

char buffer[1200];

//kml waypont step interval
int step_interval = 10;

//------------------------------------
// SDL vars
//------------------------------------

// Program uptime counter=
long int start_time = 0;

typedef struct {
  int r;
  int g;
  int b;
} rgb_colors;


// drawing colors
rgb_colors foreground_color;
rgb_colors background_color;

// drawing location
int previous_x = 0;
int previous_y = 0;

// The screen surface
SDL_Surface *screen = NULL;
SDL_Surface *navputer_png = NULL;
SDL_Surface *variometer_dial_png = NULL;
SDL_Surface *variometer_needle_png = NULL;
SDL_Surface *variometer_needle_png_rotated = NULL;
SDL_Surface *attitude_dial_png = NULL;
SDL_Surface *attitude_horizon_png = NULL;
SDL_Surface *attitude_crosshair_png = NULL;
SDL_Surface *specular_png = NULL;
SDL_Surface *heading_bg_png = NULL;
SDL_Surface *heading_dial_png = NULL;
SDL_Surface *heading_dial_png_rotated = NULL;
SDL_Surface *heading_arrow_png = NULL;
SDL_Surface *heading_waypoint_png = NULL;
SDL_Surface *airspeed_dial_png = NULL;
SDL_Surface *airspeed_needle_png = NULL;
SDL_Surface *airspeed_needle_png_rotated = NULL;
SDL_Surface *altitude_dial_png = NULL;
SDL_Surface *altitude_hundreds_needle_png = NULL;
SDL_Surface *altitude_hundreds_needle_png_rotated = NULL;
SDL_Surface *altitude_thousands_needle_png = NULL;
SDL_Surface *altitude_thousands_needle_png_rotated = NULL;

SDL_Rect navputer_rect;
SDL_Rect variometer_rect;
SDL_Rect variometer_rotated_rect;
SDL_Rect heading_rect;
SDL_Rect heading_dial_rotated_rect;
SDL_Rect attitude_rect;
SDL_Rect airspeed_rect;
SDL_Rect airspeed_rotated_rect;
SDL_Rect altitude_rect;
SDL_Rect altitude_hundreds_rotated_rect;
SDL_Rect altitude_thousands_rotated_rect;

SDL_Rect screen_rect;
SDL_Rect display_area_rect;
int flags=0;


// User input
int isbutton_down = 0; // read if the mouse button was pressed
int mousex = 0; // read the mousex position
int mousey = 0; // read the mousey position

int upButton=0; //Up
int downButton=0; //Down
int rightButton=0; //Right
int leftButton=0; //Left

SDL_Surface *program_icon;
SDL_Surface* lives_bmp = NULL;
SDL_Rect lives_rect;

Uint32 bgColor = 0;
Uint32 clickColor = 0;
Uint32 screen_color;

int character_with = 0;
int character_height = 0;

int big_character_with = 0;
int big_character_height = 0;

int total_text_rows = 0;
int total_text_cols = 0;
TTF_Font *fnt; // Font file
int text_row = 0; // current text row

//Temp debug value printing string
char temp_txt[12];

//GPS log file
char gps_tracklog_filepath[500];
char kml_tracklog_filepath[500];
char bmp_snapshot_filepath[500];

//NMEA log file saving filepointer
FILE *gps_log_fp;



