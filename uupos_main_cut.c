#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int main (int argc, char *argv[1]) {
	int i;
	int fd; /* file descriptor */
	char buffer[128];

	struct tm  strctime;
	time_t     realtime;

	fd = open("/dev/ttyS0",O_WRONLY);
	if(fd == -1) {
		fprintf(stderr,"Cannot open /dev/ttyS0\n");
		return 1;
	}
	
	time(&realtime);
	strctime = *localtime(&realtime);
	strftime(buffer, 128, "Am %Y-%m-%d um %H:%M Uhr\n", &strctime);

	write(fd,"Ruebezahl Naturkost\n",strlen("Ruebezahl Naturkost\n"));
	write(fd,buffer,strlen(buffer));

	for(i=0; i<40; i++) {
		sprintf(buffer, "Produkt blubber ...               %i EUR\n",i);
		write(fd,buffer,strlen(buffer));
	}
	write(fd, "\nusw. usf.\n\n",strlen("\nusw. usf.\n\n"));
	write(fd, "... fortlaufender text\n", strlen("... fortlaufender text\n"));
	
	write(fd,"\x1D\x56\x42\xFB",4);
	close(fd);
	
	return 0;
}
