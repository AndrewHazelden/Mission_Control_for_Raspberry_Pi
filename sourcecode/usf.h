#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Universal Settings Format - USF Library
// 2013-12-29 - V1.0 Build 3
// by Andrew Hazelden andrew@andrewhazelden.com

// Data Types supported: int, long, float, double, string

// Standard USF Error states:
// USR_error 0 means everything worked normally
// USF_error -1 means setting the .usf file could not be loaded
// USF_error -2 means a requested usf_setting could not be found in the .usf file

// Todo - have read functions skip line starting with a #

FILE *usf_file_pointer;
int USF_error = 0;
char USF_encoding[10];

void usf_read_init(char* usf_filepath);
void usf_read_encoding(void);
int usf_exit(void);
void usf_debug(FILE *usf_file_pointer);
void usf_read_int(char* usf_setting, int* usf_value);
void usf_read_long(char* usf_setting, long int* usf_value);
void usf_read_float(char* usf_setting, float* usf_value);
void usf_read_double(char* usf_setting, double* usf_value);
void usf_read_string(char* usf_setting, char* usf_value);


//Load the USF file pointer
void usf_read_init(char* usf_filepath){

  //printf("Universal Settings Format Library\n");
  //printf("---------------------------------\n");

  usf_file_pointer = fopen(usf_filepath, "r");
  if(usf_file_pointer != NULL){
    printf("The settings file %s is open.\n", usf_filepath); 
    USF_error = 0;
  } else {
    printf("The settings file %s could not be opened.\n", usf_filepath); 
    USF_error = 1;
  }

  //Read the document encoding value
  if(USF_error == 0){
    usf_read_encoding();
  }  

}

//closes settings file on exit
int usf_exit(void){
  fclose(usf_file_pointer);
  
  //printf("USF Exit State: %d\n", USF_error);
  
  switch ( USF_error ) {
    case 0:
      return 0;
      break;
    case 1:
      //printf("USF Warning: There was a .usf file read error.\n");
      return 1;
      break;
    case 2:
      //printf("USF Warning: The requested settings entry was not found in the .usf file.\n");
      return 2;
      break;
    default:
      return 0;
      break;
  }

}

//Debug the current settings file
void usf_debug(FILE *usf_file_pointer){
  char input_line[255];
  
  rewind(usf_file_pointer);
  
  printf("\n\nSettings File:\n");
  printf("---------------------------------\n");
  
  //Scan through the current file
  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    printf("%s", input_line); 
  }
  
  printf("---------------------------------\n");
  printf("End of File\n\n");
}


//USF_read_int("launch_time", &launch_time);
void usf_read_int(char* usf_setting, int* usf_value){
  char input_line[255];
  char *start = NULL;
  char usf_pattern_match[255];
  char space_char = '\0';
  int temp_value;
  int found_usf_setting = 0;
  
  rewind(usf_file_pointer); 
  
  strcpy(usf_pattern_match, "@");
  strcat(usf_pattern_match, usf_setting);

  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    if( strstr(input_line, usf_pattern_match) ){
      //printf("Found USF integer: %s", input_line);
      start = input_line + strlen(usf_pattern_match);
      
      //Check for the space between characters
      space_char = *start;
      if(isspace(space_char)){
        sscanf(start, "%d", &temp_value );
       printf("[%s] %d\n", usf_setting, temp_value);
        *usf_value = temp_value;
        
        //A usf setting was located
        found_usf_setting=1;
      }
    }
  }
  
  if(!found_usf_setting){
    *usf_value = 0;
    USF_error = 2;
  }
}

//USF_read_long("launch_time", &launch_time);
void usf_read_long(char* usf_setting, long int* usf_value){
  char input_line[255];
  char *start = NULL;
  char usf_pattern_match[255];
  char space_char = '\0';
  long int temp_value;
  int found_usf_setting = 0;
  
  rewind(usf_file_pointer);
  
  strcpy(usf_pattern_match, "@");
  strcat(usf_pattern_match, usf_setting);
  
  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    if( strstr(input_line, usf_pattern_match) ){
      //printf("Found USF integer: %s", input_line);
      start = input_line + strlen(usf_pattern_match);
      
      //Check for the space between characters
      space_char = *start;
      if(isspace(space_char)){
        sscanf(start, "%ld", &temp_value );
        printf("[%s] %ld\n", usf_setting, temp_value);
        *usf_value = temp_value;
        
        //A usf setting was located
        found_usf_setting=1;
      }
    }
  }
  
  if(!found_usf_setting){
    *usf_value = 0;
    USF_error = 2;
  }
}




//USF_read_float("launch_time", &launch_time);
void usf_read_float(char* usf_setting, float* usf_value){
  char input_line[255];
  char *start = NULL;
  char usf_pattern_match[255];
  char space_char = '\0';
  float temp_value;
  int found_usf_setting = 0;
  
  rewind(usf_file_pointer);
  
  strcpy(usf_pattern_match, "@");
  strcat(usf_pattern_match, usf_setting);
  
  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    if( strstr(input_line, usf_pattern_match) ){
      //printf("Found USF float: %s", input_line);
      start = input_line + strlen(usf_pattern_match);
      
      //Check for the space between characters
      space_char = *start;
      if(isspace(space_char)){
        sscanf(start, "%f", &temp_value );
        printf("[%s] %f\n", usf_setting, temp_value);
        *usf_value = temp_value;
        
        //A usf setting was located
        found_usf_setting=1;
      }
    }
  }
  
  if(!found_usf_setting){
    *usf_value = 0;
    USF_error = 2;
  }
}


//USF_read_double("launch_time", &launch_time);
void usf_read_double(char* usf_setting, double* usf_value){
  char input_line[255];
  char *start = NULL;
  char usf_pattern_match[255];
  char space_char = '\0';
  double temp_value;
  int found_usf_setting = 0;
  
  rewind(usf_file_pointer);
  
  strcpy(usf_pattern_match, "@");
  strcat(usf_pattern_match, usf_setting);
  
  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    if( strstr(input_line, usf_pattern_match) ){
      //printf("Found USF double: %s", input_line);
      start = input_line + strlen(usf_pattern_match);
      
      //Check for the space between characters
      space_char = *start;
      if(isspace(space_char)){
        sscanf(start, "%lf", &temp_value );
        printf("[%s] %lf\n", usf_setting, temp_value);
        *usf_value = temp_value;
        
        //A usf setting was located
        found_usf_setting=1;
      }
    }
  }
  
  if(!found_usf_setting){
    *usf_value = 0;
    USF_error = 2;
  }
}



//USF_read_string("rocket_name", &rocket_name);
void usf_read_string(char* usf_setting, char* usf_value){
  char input_line[255];
  char *start = NULL;
  char usf_pattern_match[255];
  char space_char = '\0';
  int found_usf_setting = 0;
  
  rewind(usf_file_pointer);
  
  strcpy(usf_pattern_match, "$");
  strcat(usf_pattern_match, usf_setting);
  
  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    if( strstr(input_line, usf_pattern_match) ){
      //printf("Found USF string: %s", input_line);
      start = input_line + strlen(usf_pattern_match);
      
      //Check for the space between characters
      space_char = *start;
      if(isspace(space_char)){
        strncpy(usf_value, start+2, strlen(start+2)-2);
        //strncat(usf_value, start+2, strlen(start+2)-2);
        printf("[%s] %s\n", usf_setting, usf_value);
        
        //A usf setting was located
        found_usf_setting=1;
      }
    }
  }
  
  if(!found_usf_setting){
    //strcat(usf_value, "");
    strcpy(usf_value, "");
    USF_error = 2;
  }
}


//USF_read_encoding();
void usf_read_encoding(){
  char input_line[255];
  char *start = NULL;
  char usf_pattern_match[255];
  char space_char = '\0';
  int found_usf_setting = 0;

  char usf_setting[255] = "encoding";
  char usf_value[255] = "";

  rewind(usf_file_pointer);
  
  strcpy(usf_pattern_match, "$");
  strcat(usf_pattern_match, usf_setting);
  
  while(!feof(usf_file_pointer) ){
    fgets (input_line , 255 , usf_file_pointer);
    if( strstr(input_line, usf_pattern_match) ){
      //printf("Found USF string: %s", input_line);
      start = input_line + strlen(usf_pattern_match);
      
      //Check for the space between characters
      space_char = *start;
      if(isspace(space_char)){
        strncpy(usf_value, start+2, strlen(start+2)-2);
        strcpy(USF_encoding, usf_value);
        //printf("[%s] %s\n", usf_setting, USF_encoding);
        
        //A usf setting was located
        found_usf_setting=1;
      }
    }
  }
  
  if( (!found_usf_setting)){
    strcpy(USF_encoding, "ASCII");
    printf("No encoding format specified. Using ASCII as the default encoding.\n\n");
 } else {
   printf("%s encoding specified.\n\n", USF_encoding);
 }
}
