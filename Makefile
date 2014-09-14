DEBUG   = -O3
CC      = gcc
INCLUDE = -I/usr/local/include
CFLAGS  = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS = -L/usr/local/lib
LDLIBS    = -lwiringPi -lwiringPiDev -lpthread -lm -lconfig

# Should not alter anything below this line
###############################################################################

SRC     =       blink.c blink8.c blink12.c                                      \
                blink12drcs.c                                                   \
                pwm.c                                                           \
                speed.c wfi.c isr.c isr-osc.c                                   \
                lcd.c lcd-adafruit.c clock.c                                    \
                nes.c                                                           \
                softPwm.c softTone.c                                            \
                delayTest.c serialRead.c serialTest.c okLed.c ds1302.c          \
                rht03.c piglow.c

OBJ     =       $(SRC:.c=.o)

BINS    =       $(SRC:.c=)

blink:  blink.o
        @echo [link]
        @$(CC) -o $@ blink.o $(LDFLAGS) $(LDLIBS)
