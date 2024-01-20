#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRLEN  80
#define HEAD 200

void  drawshoppingend();
char* boundstr(char *dest, const char *src, XftFont *font, int width);

Display *dpy;
XftDraw *draw;
XftFont *xftnorm, *xfthead, *xfthuge;
XftColor xcblack,xcblue;
XGlyphInfo giprice,gidesc;

char totalstr[8],pricestr[8],cashstr[8];
char string[MAXSTRLEN] = "";
char *euro = "€";

int main(void) {
	Window win;
	Colormap cmap;
	XEvent event;
	XWindowAttributes attributes;
	
	KeySym keysym;
	char buffer[128],buf[128];
	int len = 0;
	int noc;

	XRenderColor cblack = { 0x0000, 0x0000, 0x0000, 0xffff};
	XRenderColor cblue  = { 0x3300, 0x4d00, 0x6600, 0xffff};
	XColor cborder,cdback,clback,chead;
	GC gcborder,gcdback,gclback,gchead;
	int scr;
	
	unsigned int i,j;
	unsigned int cartno = 0;
	unsigned int end = 0;
	unsigned int lines = 0;
	unsigned int bill[2][512];

	FILE *db;
	char csv[] = ";\n";
	char c;
	char tmpline[512];
	char *token;
	char **code;
	char **typ;
	char **desc;
	float *price,total=0.,bar=0.;

	for(i=0; i<512; i++)
		bill[1][i] =  1;
	cashstr[0] = '\0';

	/* import database */
	db = fopen("database.csv","r");
	if(db == NULL) {
		fprintf(stderr,"Could not open database! exiting ...\n");
		fflush(stderr);
		exit(1);
	}
	while((c = getc(db)) != EOF) {
		if(c == '\n')
			lines++;
	}
	fclose(db);
	code = (char **)malloc(lines * sizeof(char *));
	typ = (char **)malloc(lines * sizeof(char *));
	desc = (char **)malloc(lines * sizeof(char *));
	price = (float *)malloc(lines * sizeof(float *));
	db = fopen("database.csv","r");
	for(j=0; j<lines; j++) {
		fgets(tmpline,512,db);
		
		token = strtok(tmpline, csv);
		code[j] = (char *)malloc((strlen(token) + 1) * sizeof(char));
		strcpy(code[j], token);

		token = strtok(NULL, csv);
		price[j] = atof(token);
		
		token = strtok(NULL, csv);
		typ[j] = (char *)malloc((strlen(token) + 1) * sizeof(char));
		strcpy(typ[j], token);

		token = strtok(NULL, csv);
		desc[j] = (char *)malloc((strlen(token) + 1) * sizeof(char));
		strcpy(desc[j], token);
	}
	fclose(db);

	/* open connection with the server */
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		fprintf(stderr, "Cannot open display\n");
		fflush(stderr);
		exit(1);
	}

	scr    = DefaultScreen(dpy);
	cmap   = DefaultColormap(dpy, scr);
	
	if(!XAllocNamedColor(dpy, cmap, "#334d66", &cborder, &cborder)) {
		fprintf(stderr, "Cannot allocate border color\n");
		fflush(stderr);
		exit(1);
	}
	if(!XAllocNamedColor(dpy, cmap, "#e0e0e0", &cdback, &cdback)) {
		fprintf(stderr, "Cannot allocate backgroundground color\n");
		fflush(stderr);
		exit(1);
	}
	if(!XAllocNamedColor(dpy, cmap, "#efefef", &clback, &clback)) {
		fprintf(stderr, "Cannot allocate midthground color\n");
		fflush(stderr);
		exit(1);
	}
	if(!XAllocNamedColor(dpy, cmap, "#eeffff", &chead, &chead)) {
		fprintf(stderr, "Cannot allocate lightground color\n");
		fflush(stderr);
		exit(1);
	}

	/* create window */
	win = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 100, 100, 900, 600, 1,
			cdback.pixel, clback.pixel);

	/* give window title */
	XStoreName(dpy, win, "X Cash Register");

	/* create grafical context */
	gcborder = XCreateGC(dpy, win, 0, NULL);
	XSetForeground(dpy, gcborder, cborder.pixel);

	gcdback = XCreateGC(dpy, win, 0, NULL);
	XSetForeground(dpy, gcdback, cdback.pixel);

	gclback = XCreateGC(dpy, win, 0, NULL);
	XSetForeground(dpy, gclback, clback.pixel);

	gchead = XCreateGC(dpy, win, 0, NULL);
	XSetForeground(dpy, gchead, chead.pixel);

	/* select kind of events we are interested in */
	XSelectInput(dpy, win, ExposureMask | KeyPressMask );

	/* map (show) the window */
	XMapWindow(dpy, win);

	xfthuge = XftFontOpen(dpy, scr,
		XFT_FAMILY, XftTypeString, "dejavuserifcondensed",
		XFT_SIZE, XftTypeDouble, 100.0,
		NULL);

	xfthead = XftFontOpen(dpy, scr,
		XFT_FAMILY, XftTypeString, "dejavusans",
		XFT_SIZE, XftTypeDouble, 24.0,
		NULL);
	
	xftnorm = XftFontOpen(dpy, scr,
		XFT_FAMILY, XftTypeString, "dejavusans",
		XFT_SIZE, XftTypeDouble, 13.0,
		NULL);

	XftColorAllocValue(dpy, DefaultVisual(dpy, scr), cmap, &cblack, &xcblack);
	XftColorAllocValue(dpy, DefaultVisual(dpy, scr), cmap, &cblue, &xcblue);

	draw = XftDrawCreate(dpy, win, DefaultVisual(dpy, scr), cmap);

	/* event loop */
	while(1) {
		XNextEvent(dpy, &event);
		
		switch(event.type) {
			case Expose:
				/* Get the actual window dimensions */
				XGetWindowAttributes(dpy, win, &attributes);
				
				/* draw or redraw the window */
				XClearWindow(dpy, win);
				XFillRectangle(dpy, win, gchead, 5, 5, attributes.width-10, HEAD);
				XftTextExtentsUtf8(dpy, xftnorm, (FcChar8*)"99.99", strlen("99.99"), &giprice);
				for(i=0; i<20*cartno; i+=40)
					XFillRectangle(dpy, win, gcdback, 5, HEAD+10+i, attributes.width-10, 20);
				for(i=0; i<cartno; i++) {
					sprintf(buf, "%.2f", price[bill[0][i]]);
					XftTextExtentsUtf8(dpy, xftnorm, (FcChar8*)buf, strlen(buf), &giprice);
					XftDrawStringUtf8(draw, &xcblack, xftnorm, attributes.width-giprice.width-20, HEAD+26+i*20, (FcChar8*)buf, strlen(buf));
					boundstr(buf, desc[bill[0][i]], xftnorm, attributes.width-60);
					XftDrawStringUtf8(draw, &xcblack, xftnorm, 10, HEAD+26+i*20, (FcChar8*)buf, strlen(buf));
				}
				XftDrawStringUtf8(draw, &xcblue, xfthuge, 20, HEAD-30, (FcChar8*)euro, strlen(euro));
				if(price[j] != 0) {
					XftDrawStringUtf8(draw, &xcblue, xfthuge, 100, HEAD-30, (FcChar8*)totalstr, strlen(totalstr));
					XftTextExtentsUtf8(dpy, xfthead, (FcChar8*)pricestr, strlen(pricestr), &giprice);
					XftDrawStringUtf8(draw, &xcblue, xfthead, attributes.width-giprice.width-20, 40, (FcChar8*)pricestr, strlen(pricestr));
					boundstr(buf, desc[j], xfthead, attributes.width-60);
					XftDrawStringUtf8(draw, &xcblue, xfthead, 20, 40, (FcChar8*)buf, strlen(buf));
				} else
					XftDrawStringUtf8(draw, &xcblue, xfthead, 20, 40, (FcChar8*)"Rübezahl Naturkost", strlen("Rübezahl Naturkost"));
				break;
		
			case KeyPress:
				noc = XLookupString(&event.xkey, buffer, 15, &keysym, NULL);
				buffer[noc] = '\0';
				
				/* refer to returned KeySym */
				switch(keysym) {
					case XK_Escape:
						printf("quiting ...\n");
						for(j=0; j<lines; j++) {
							free(code[j]);
							free(typ[j]);
							free(desc[j]);
						}
						free(code);
						free(typ);
						free(desc);
						free(price);
						XftColorFree(dpy, DefaultVisual(dpy, scr), cmap, &xcblack);
						XftColorFree(dpy, DefaultVisual(dpy, scr), cmap, &xcblue);
						XftDrawDestroy(draw);

						/* close connection to server */
						XCloseDisplay(dpy);
						exit(0);
						break;
					
					case XK_Home:
						end = 1;
						len = 0;
						string[0] = '\0';
						sprintf(totalstr, "%.2f", total);
						XFillRectangle(dpy, win, gchead, 5, 5, attributes.width-10, HEAD);
						drawshoppingend();
						break;
					
					case XK_Return:
					
					case XK_Linefeed:
						if(string[0] == '\0')
							break;
						printf("Entered string: %s\n", string);
						if(end == 1) {
							bar = atof(string);
							sprintf(string, "%.2f", bar);
							sprintf(cashstr, "%.2f", bar - total);
							XFillRectangle(dpy, win, gchead, 5, 5, attributes.width-10, HEAD);
							drawshoppingend();
							total = 0;
							sprintf(totalstr, "%.2f", total);
							cartno = 0;
							end = 0;
							len = 0;
							string[0] = '\0';
							cashstr[0] = '\0';
							break;
						}
						
						/* main put to cart loop part */
						for(j=0; j<lines; j++) {
							/* try to find product code in database */
							if(strcmp(string,code[j]) == 0) {
								bill[0][cartno] = j;
								cartno++;
								break;
							}
						}
						/* if product found in database */
						if(price[j] != 0) {
							XClearWindow(dpy, win);
							XFillRectangle(dpy, win, gchead, 5, 5, attributes.width-10, HEAD);
							total += price[j];
							sprintf(pricestr, "%.2f", price[j]);
							sprintf(totalstr, "%.2f", total);
							for(i=0; i<9; i++) {
								if(totalstr[i] == '.') {
									totalstr[i] = ',';
									break;
								}
							}
							XftDrawStringUtf8(draw, &xcblue, xfthuge, 20, HEAD-30, (FcChar8*)euro, strlen(euro));
							XftDrawStringUtf8(draw, &xcblue, xfthuge, 100, HEAD-30, (FcChar8*)totalstr, strlen(totalstr));

							XftTextExtentsUtf8(dpy, xfthead, (FcChar8*)pricestr, strlen(pricestr), &giprice);
							XftDrawStringUtf8(draw, &xcblue, xfthead, attributes.width-giprice.width-20, 40, (FcChar8*)pricestr, strlen(pricestr));

							boundstr(buf, desc[j], xfthead, attributes.width-60);
							XftDrawStringUtf8(draw, &xcblue, xfthead, 20, 40, (FcChar8*)buf, strlen(buf));
							
							for(i=0; i<20*cartno; i+=40)
								XFillRectangle(dpy, win, gcdback, 5, HEAD+10+i, attributes.width-10, 20);
							for(i=0; i<cartno; i++) {
								sprintf(buf, "%.2f", price[bill[0][i]]);
								XftTextExtentsUtf8(dpy, xftnorm, (FcChar8*)buf, strlen(buf), &giprice);
								XftDrawStringUtf8(draw, &xcblack, xftnorm, attributes.width-giprice.width-20, HEAD+26+i*20, (FcChar8*)buf, strlen(buf));
								boundstr(buf, desc[bill[0][i]], xftnorm, attributes.width-60);
								XftDrawStringUtf8(draw, &xcblack, xftnorm, 10, HEAD+26+i*20, (FcChar8*)buf, strlen(buf));
							}
						} else { 
							/* product code not found */
							strcat(string, " <- are you sure that this is valid?!");
							XftDrawStringUtf8(draw, &xcblue, xfthead, 20, 40, (FcChar8*)string, strlen(string));
						}
						len = 0;
						string[0] = '\0';
						break;
					
					case XK_BackSpace:
					
					case XK_Delete:
						if(len) {
							len--;
							string[len] = '\0';
							if(end == 0) {
								XFillRectangle(dpy, win, gchead, 20, 10, attributes.width-30, 40);
								XftDrawStringUtf8(draw, &xcblue, xfthead, 20, 40, (FcChar8*)string, strlen(string));
							} else {
								XFillRectangle(dpy, win, gchead, 195, 40, attributes.width-200, 40);
								XftDrawStringUtf8(draw, &xcblue, xfthead, 195,  70, (FcChar8*)string, strlen(string));
							}
						}
						break;

					default:
						/* refer to returned ASCII-value */
						if((len + noc + 1) < MAXSTRLEN) {
							strcat(string, buffer);
							len++;
							if(end == 0) {
								XFillRectangle(dpy, win, gchead, 5, 5, attributes.width-10, HEAD);
								XftDrawStringUtf8(draw, &xcblue, xfthead, 20, 40, (FcChar8*)string, strlen(string));
							} else {
								/* shopping end */
								XFillRectangle(dpy, win, gchead, 5, 5, attributes.width-10, HEAD);
								drawshoppingend();
							}
						}
						break;
				}
				break;

			case MappingNotify:
				XRefreshKeyboardMapping(&event.xmapping);
				break;

		}
	}

	return 1;
}

void drawshoppingend() {
	XftDrawStringUtf8(draw, &xcblue, xfthead,  20,  40, (FcChar8*)"Summe", strlen("Summe"));
	XftDrawStringUtf8(draw, &xcblue, xfthead, 170,  40, (FcChar8*)euro, strlen(euro));
	XftDrawStringUtf8(draw, &xcblue, xfthead, 195,  40, (FcChar8*)totalstr, strlen(totalstr));
	XftDrawStringUtf8(draw, &xcblue, xfthead,  20,  70, (FcChar8*)"Bar", strlen("Bar"));
	XftDrawStringUtf8(draw, &xcblue, xfthead, 170,  70, (FcChar8*)euro, strlen(euro));
	XftDrawStringUtf8(draw, &xcblue, xfthead, 195,  70, (FcChar8*)string, strlen(string));
	XftDrawStringUtf8(draw, &xcblue, xfthead,  20, 100, (FcChar8*)"Zurück", strlen("Zurück"));
	XftDrawStringUtf8(draw, &xcblue, xfthead, 170, 100, (FcChar8*)euro, strlen(euro));
	XftDrawStringUtf8(draw, &xcblue, xfthead, 195, 100, (FcChar8*)cashstr, strlen(cashstr));
}

char* boundstr(char *dest, const char *src, XftFont *font, int width) {
	int i;

	XftTextExtentsUtf8(dpy, font, (FcChar8*)src, strlen(src), &gidesc);
	if(giprice.width+gidesc.width < width+20)
		strcpy(dest, src);
	else {
		for(i=1; i<500; i++) {
			strncpy(dest, src, strlen(src)-i-1);
			dest[strlen(src)-i-1] = '\0';
			strcat(dest, "»");
			XftTextExtentsUtf8(dpy, font, (FcChar8*)dest, strlen(dest), &gidesc);
			if(giprice.width+gidesc.width < width)
				break;
		}
	}

	return dest;
}

