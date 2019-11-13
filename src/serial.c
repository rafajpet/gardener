//
// Created by rafik on 13. 11. 2019.
//

#include "serial.h"

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */

static int fd;

int init_serial_port() {

    fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY);

    if(fd == -1)						/* Error Checking */
    {
        printf("\n  Error! in Opening ttyUSB0  ");
        return  -1;
    } else {
        printf("\n  ttyUSB0 Opened Successfully ");
    }

    struct termios SerialPortSettings;	/* Create the structure                          */
    tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings,B115200); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B115200); /* Set Write Speed as 9600                       */

    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    {
        printf("\n  ERROR ! in Setting attributes");
        return -1;
    } else {
        printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
    }
    return 0;
}

int write_to_serial_port(const char data){
    int  bytes_written  = 0;
    bytes_written = write(fd,&data,sizeof(char));
    printf("\n  %d Bytes written to ttyUSB0", bytes_written);
    return  bytes_written;
}

int close_serial_port(){
    close(fd);
}

