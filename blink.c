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

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define LED     0

void process(){
    digitalWrite (LED, HIGH) ;  // On
    syslog(LOG_INFO, "Led 0 is %d\n",digitalRead(LED)) ; // On
    delay (500) ;               // mS
    digitalWrite (LED, LOW) ;   // Off
    syslog(LOG_INFO,"Led 0 is %d\n",digitalRead(LED)) ; // On
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
config_setting_t *setting;
const char *str1, *str2;
int tmp;
char *config_file_name = "/root/test/blink.conf";
/*Initialization*/
config_init(&cfg);
/* Read the file. If there is an error, report it and exit. */
if (!config_read_file(&cfg, config_file_name)){
  //printf("\n%s:%d - %s", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
  syslog(LOG_INFO,"\n%s:%d - %s", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
  config_destroy(&cfg);
  return -1;}
/* Get the configuration file name. */
if (config_lookup_string(&cfg, "filename", &str1))
  //printf("\nFile Type: %s", str1);
  syslog(LOG_INFO,"\nFile Type: %s", str1);
  else
    printf("\nNo 'filename' setting in configuration file.");
/*Read the parameter group*/
  setting = config_lookup(&cfg, "params");
  if (setting != NULL){
    /*Read the string*/
    if (config_setting_lookup_string(setting, "param1", &str2)){
     // printf("\nParam1: %s", str2);}
      syslog(LOG_INFO,"\nParam1: %s", str2);}
      else
        //printf("\nNo 'param1' setting in configuration file.");
        syslog(LOG_INFO,"\nNo 'param1' setting in configuration file.");
        
        //printf("\n");
    }

//    config_destroy(&cfg);
//}

/* Pi stuff */
  printf ("Raspberry Pi blink\n") ;

  wiringPiSetup () ;
  pinMode (LED, OUTPUT) ;

  while(1){
    process();
  }
}
