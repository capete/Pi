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

// gpio1 Pin - wiringPi pin 0 is BCM_GPIO 17.

#define	LED	0
int gpio1;

void process(){
    digitalWrite (gpio1, HIGH) ;  // On
    syslog(LOG_INFO, "Led 0 is %d\n",digitalRead(gpio1)) ; // On
    delay (500) ;               // mS
    digitalWrite (gpio1, LOW) ;   // Off
    syslog(LOG_INFO,"Led 0 is %d\n",digitalRead(gpio1)) ; // On
    delay (500) ;
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
config_setting_t *setting, *database;
const char *str1, *str2, *host, *user, *pass, *db, *port;
int tmp;
char *config_file_name = "/root/test/blink.conf";

/*Initialization*/
config_init(&cfg);
/* Read the file. If there is an error, report it and exit. */
if (!config_read_file(&cfg, config_file_name)){
  syslog(LOG_INFO,"\n%s:%d - %s", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
  config_destroy(&cfg);
  return -1;}
/* Get the configuration file name. */
if (config_lookup_string(&cfg, "filename", &str1))
  syslog(LOG_INFO,"\nFile Type: %s", str1);
  else
    printf("\nNo 'filename' setting in configuration file.");
  
/*Read MySQL parameter group*/
database = config_lookup(&cfg, "MySQL");
  if (database != NULL){
    syslog(LOG_INFO, "Using database:");
    if (config_setting_lookup_string(database, "host", &host)){
      syslog(LOG_INFO, "host: %s", host);}
      else {syslog(LOG_INFO, "No host defined");return -1;}
    if (config_setting_lookup_string(database, "user", &user)){
      syslog(LOG_INFO, "user: %s", user);}
      else {syslog(LOG_INFO, "No user defined");return -1;}
    if (config_setting_lookup_string(database, "pass", &pass)){
      syslog(LOG_INFO, "pass: %s", pass);}
      else {syslog(LOG_INFO, "No pass defined");return -1;}
    if (config_setting_lookup_string(database, "db", &db)){
      syslog(LOG_INFO, "db: %s", db);}
      else {syslog(LOG_INFO, "No db defined");return -1;}
    if (config_setting_lookup_string(database, "port", &port)){
      syslog(LOG_INFO, "port: %s", port);}
      else {syslog(LOG_INFO, "No port defined");return -1;}  
   }
if (config_lookup_int(&cfg, "GPIO1", &gpio1))
  syslog(LOG_INFO, "Read: %d", gpio1);
  else {syslog(LOG_INFO, "You haven't defined any GPIO to read");return -1;}
/*Read the parameter group*/
  setting = config_lookup(&cfg, "params");
  if (setting != NULL){
    /*Read the string*/
    if (config_setting_lookup_string(setting, "param1", &str2)){
      syslog(LOG_INFO,"\nParam1: %s", str2);}
      else
        syslog(LOG_INFO,"\nNo 'param1' setting in configuration file.");
        /*Read the integer*/
    if (config_setting_lookup_int(setting, "param2", &tmp)){
      syslog(LOG_INFO,"\nParam2: %d", tmp);}
      else
        syslog(LOG_INFO,"\nNo 'param2' setting in configuration file.");
    }
    config_destroy(&cfg);

/* Pi stuff */
  printf ("Raspberry Pi blink\n") ;

  wiringPiSetup () ;
  pinMode (gpio1, OUTPUT) ;

  while(1){
    process();
  }
}
