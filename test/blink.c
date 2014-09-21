#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <libconfig.h>
#include <time.h>
#include <string.h>
#include <wiringPi.h>
#include "scribe_utils.h"
// gpio1 Pin - wiringPi pin 0 is BCM_GPIO 17.

#define	LED	0
int gpio1, gpio2, gpio3, gpio4, gpio5, gpio6, gpio7, gpio8, gpio9, gpio10, gpio11, gpio12, gpio13, port_scr;
const char *gpio1_m, *gpio2_m, *gpio3_m, *gpio4_m, *gpio5_m, *gpio6_m, *gpio7_m, *gpio8_m, *gpio9_m, *gpio10_m, *gpio11_m, *gpio12_m, *gpio13_m;
const char *str1, *host, *user, *pass, *db, *port;
const char  *host_scr, *cat_scr;
const char *eol = "\n";
const char *msg = "Led has value: ";
char *host_scr1, *cat_scr1;
char *config_file_name = "/root/Pi/test/blink.conf";
char buff_time[128], buff[32];
int val = 0 ;
char val_str[32];
scribe_t *scribe;

void process(){
/*Write and Read the RPi pins*/
    digitalWrite (gpio1, HIGH) ;  // On
    val = digitalRead(gpio1);
/*Get the time of day and initialize scribe logging*/
    struct tm *sTm;
    time_t now = time (NULL);
    sTm = localtime(&now);
    strftime(buff_time, sizeof(buff_time), "%z %Y-%m-%d %A %H:%M:%S", sTm);//Format the time as wanted
/*Prepare the logs to be sent*/
//    char *scribe_logs = malloc(strlen(buff_time) + strlen(msg) + sizeof(val) + 2);//Allocate memory
    char *scribe_logs = malloc(strlen(msg) + sizeof(val) + 2);//Allocate memory
//    strcpy(scribe_logs, buff_time);//Add time
//    strcat(scribe_logs, " ");//Put space after time
    strcat(scribe_logs, msg);//Add the mesage 
    sprintf(val_str, "%d", val);//Copy int to string
    strcat(scribe_logs, val_str);//Add the value of the pin
    strcat(scribe_logs, eol);//Add EndOfLine char to the end
    cat_scr1 = strdup(cat_scr);//Copy const char to char
/*Send the logs to scribe*/
    int status = scribe_write(scribe,cat_scr1,scribe_logs);
    if (status != 0) { syslog(LOG_ERR, "Problem while sending logs to scribe");
       exit(EXIT_FAILURE);
     }
  }

int main (void){
  syslog(LOG_NOTICE, "Entering Daemon");
  syslog (LOG_INFO, "Program started by User %d", getuid ());
  pid_t pid, sid;
  pid = fork(); //Fork the Parent Process
  if (pid < 0) { syslog(LOG_ERR, "Can not create a new PID for our child process");}
  if (pid > 0) { exit(EXIT_SUCCESS); } //We got a good pid, Close the Parent Process
  umask(0); //Change File Mask
  sid = setsid(); //Create a new Signature Id for our child
  if (sid < 0) { syslog(LOG_ERR, "Can not create a new SID on child process");}
  if ((chdir("/")) < 0) { syslog(LOG_ERR, "Can not change directory on child process");}
  //Close Standard File Descriptors:
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

/* Config file  */
config_t cfg;
config_setting_t *database_c, *scribe_c;
/*Initialization*/
config_init(&cfg);
/* Read the file. If there is an error, report it and exit. */
if (!config_read_file(&cfg, config_file_name)){
  syslog(LOG_INFO,"%s:%d - %s", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
  config_destroy(&cfg);
  return -1;}
/* Get the configuration file name. */
if (config_lookup_string(&cfg, "filename", &str1))
  syslog(LOG_INFO,"File Type: %s", str1);
  else
    printf("No 'filename' setting in configuration file.");
/*Scribe conf file*/
scribe_c = config_lookup(&cfg, "Scribe");  
if (scribe_c != NULL){
   syslog(LOG_INFO, "Using Scribe");
   if (config_setting_lookup_string(scribe_c, "host", &host_scr)){
     syslog(LOG_INFO, "host: %s", host_scr);}
     else {syslog(LOG_INFO, "No host defined");return -1;}
   if (config_setting_lookup_int(scribe_c, "port", &port_scr)){
     syslog(LOG_INFO, "port: %d", port_scr);}
     else {syslog(LOG_INFO, "No port defined");return -1;}
   if (config_setting_lookup_string(scribe_c, "cat", &cat_scr)){
     syslog(LOG_INFO, "cat: %s", cat_scr);}
     else {syslog(LOG_INFO, "No category(cat) defined");return -1;}
  }
/*Read MySQL parameter group*/
database_c = config_lookup(&cfg, "MySQL");
  if (database_c != NULL){
    syslog(LOG_INFO, "Using MySQL");
    if (config_setting_lookup_string(database_c, "host", &host)){
      syslog(LOG_INFO, "host: %s", host);}
      else {syslog(LOG_INFO, "No host defined");return -1;}
    if (config_setting_lookup_string(database_c, "user", &user)){
      syslog(LOG_INFO, "user: %s", user);}
      else {syslog(LOG_INFO, "No user defined");return -1;}
    if (config_setting_lookup_string(database_c, "pass", &pass)){
      syslog(LOG_INFO, "pass: %s", pass);}
      else {syslog(LOG_INFO, "No pass defined");return -1;}
    if (config_setting_lookup_string(database_c, "db", &db)){
      syslog(LOG_INFO, "db: %s", db);}
      else {syslog(LOG_INFO, "No db defined");return -1;}
    if (config_setting_lookup_string(database_c, "port", &port)){
      syslog(LOG_INFO, "port: %s", port);}
      else {syslog(LOG_INFO, "No port defined");return -1;}  
   }
/*GPIO's conf file*/
if (config_lookup_int(&cfg, "GPIO1", &gpio1))
  syslog(LOG_INFO, "Read: %d", gpio1);
  else {syslog(LOG_INFO, "You haven't defined any GPIO to read");return -1;}
if (config_lookup_string(&cfg, "GPIO1_M", &gpio1_m))
  syslog(LOG_INFO, "Read: %s", gpio1_m);
  else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}
    
if (config_lookup_int(&cfg, "GPIO2", &gpio2)){
  syslog(LOG_INFO, "Read: %d", gpio2);
  if (config_lookup_string(&cfg, "GPIO2_M", &gpio2_m))
    syslog(LOG_INFO, "Read: %s", gpio2_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO3", &gpio3)){
  syslog(LOG_INFO, "Read: %d", gpio3);
  if (config_lookup_string(&cfg, "GPIO3_M", &gpio3_m))
    syslog(LOG_INFO, "Read: %s", gpio3_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO4", &gpio4)){
  syslog(LOG_INFO, "Read: %d", gpio4);
  if (config_lookup_string(&cfg, "GPIO4_M", &gpio4_m))
    syslog(LOG_INFO, "Read: %s", gpio4_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO5", &gpio5)){
  syslog(LOG_INFO, "Read: %d", gpio5);
  if (config_lookup_string(&cfg, "GPIO5_M", &gpio5_m))
    syslog(LOG_INFO, "Read: %s", gpio5_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO6", &gpio6)){
  syslog(LOG_INFO, "Read: %d", gpio6);
  if (config_lookup_string(&cfg, "GPIO6_M", &gpio6_m))
    syslog(LOG_INFO, "Read: %s", gpio6_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO7", &gpio7)){
  syslog(LOG_INFO, "Read: %d", gpio7);
  if (config_lookup_string(&cfg, "GPIO7_M", &gpio7_m))
    syslog(LOG_INFO, "Read: %s", gpio7_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO8", &gpio8)){
  syslog(LOG_INFO, "Read: %d", gpio8);
  if (config_lookup_string(&cfg, "GPIO8_M", &gpio8_m))
    syslog(LOG_INFO, "Read: %s", gpio8_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO9", &gpio9)){
  syslog(LOG_INFO, "Read: %d", gpio9);
  if (config_lookup_string(&cfg, "GPIO9_M", &gpio9_m))
    syslog(LOG_INFO, "Read: %s", gpio9_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO10", &gpio10)){
  syslog(LOG_INFO, "Read: %d", gpio10);
  if (config_lookup_string(&cfg, "GPIO10_M", &gpio10_m))
    syslog(LOG_INFO, "Read: %s", gpio10_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO11", &gpio11)){
  syslog(LOG_INFO, "Read: %d", gpio11);
  if (config_lookup_string(&cfg, "GPIO11_M", &gpio11_m))
    syslog(LOG_INFO, "Read: %s", gpio11_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO12", &gpio12)){
  syslog(LOG_INFO, "Read: %d", gpio12);
  if (config_lookup_string(&cfg, "GPIO12_M", &gpio12_m))
    syslog(LOG_INFO, "Read: %s", gpio12_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
if (config_lookup_int(&cfg, "GPIO13", &gpio13)){
  syslog(LOG_INFO, "Read: %d", gpio13);
  if (config_lookup_string(&cfg, "GPIO13_M", &gpio13_m))
    syslog(LOG_INFO, "Read: %s", gpio13_m);
    else {syslog(LOG_INFO, "You haven't defined the GPIO mode");return -1;}}
    
//config_destroy(&cfg);
/* Pi stuff */

  wiringPiSetup () ;
  pinMode (gpio1, OUTPUT) ;

host_scr1 = strdup(host_scr);//Copy const char to char
scribe = calloc(1, sizeof(scribe_t));//Allocate memory for scribe
scribe_open(scribe, host_scr1, port_scr);//Open connection to scribe

  while(1){
    process();
   usleep(125000);
  }
}
