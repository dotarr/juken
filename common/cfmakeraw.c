/* strsignal.c -- implement cfmakeraw() for architectures without it
*/

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <termios.h>

/*

NAME

	cfmakeraw -- set raw mode

SYNOPSIS

    #include <termios.h>
    int cfmakeraw (struct termios *termios_p)

DESCRIPTION

    This function sets the structure specified by termios_p to raw mode.
    There is no effect on the hardware until a subsequent successful call
    to tcsetattr(). 

PARAMETERS 

    termios_p  Points to a termios structure. 

RETURN VALUES 

    None. 


*/

void
cfmakeraw(termios_p)
  struct termios* termios_p;
{
    termios_p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                            |INLCR|IGNCR|ICRNL|IXON);
    termios_p->c_oflag &= ~OPOST;
    termios_p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    termios_p->c_cflag &= ~(CSIZE|PARENB);
    termios_p->c_cflag |= CS8;
}


