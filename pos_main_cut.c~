#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>



int main(int argc, char *argv[1]) {
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
	strftime(buffer, 128, "Am %Y-%m-%d um %H:%M Uhr\n\n", &strctime);

	/* initalize printer */
	write(fd,"\x1b\x40",2);     // esc @
	write(fd,"\x1b\x32",2);     // esc 2 default linespace
	write(fd,"\x1b\x3C",2);     // esc < return home
	write(fd,"\x1b\x52\x02",3); // esc R 2 set german character set

	/* start printing */
	write(fd,"R\x5d", 2);
	write(fd,"bezahl Naturkost\n",strlen("bezahl Naturkost\n"));
	write(fd,buffer,strlen(buffer));

	for(i=0; i<20; i++) {
		sprintf(buffer, "Produkt blubber ...              %i EUR\n",i);
		write(fd,buffer,strlen(buffer));
		usleep(300000);
	}
	for(i=0; i<20; i++) {
		sprintf(buffer, "Produkt blubber ...              %i EUR\n",i);
		write(fd,buffer,strlen(buffer));
		usleep(300000);
	}
	write(fd, "\nusw. usf.\n\n",strlen("\nusw. usf.\n\n"));
	write(fd, "... fortlaufender text\n", strlen("... fortlaufender text\n"));
	
	write(fd,"\x1d\x56\x42\x02",4);
	close(fd);
	
	return 0;
}
