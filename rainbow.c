#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

enum {
	Kdel = 0x7f,
	Kup = 14,
	Kdown = 0,
	Kleft = 17,
	Kright = 18,
};


int color = 0xFF;
int rgbMask = 0xFFFFFFFF;

int center = 128;
int width = 127;

double f = 0;

Image *image, *colorSRC;

int getRGB(double f){
	
	int r,g,b;
	int rgb = 0xFF;
	f*=2.4;

	r = sin(f)*(width)+center;
	g = sin(f+2)*(width)+center;
	b = sin(f+4)*(width)+center;

	rgb = r<<24 | g<<16 | b<<8 | rgb;
	
	return rgb;
}

Image *initImage(double *f){

	int color = getRGB(*f);
	double df = 0.01;

	colorSRC = allocimage(display, Rect(0,0,1,1), RGB24, 1, color);

	image = allocimage(display, screen->r, RGB24, 1, color);


	//loop over every X in image, draw 1px lines
	for(int i=0; i<image->r.max.x; i++){
		*f += df;
		color = getRGB(*f);
		if(colorSRC) freeimage(colorSRC);
		colorSRC = allocimage(display, Rect(0,0,1,1), RGB24, 1, color);
		line(image, Pt(i,0), Pt(i,image->r.max.y), Endsquare, Endsquare, 1, colorSRC, ZP);

	}

	return image;
}

void
main(){
	
	initdraw(0,0, "Rainbow");
	einit(Ekeyboard|Emouse);

	double df = 0.01;
	
	char buf[55];

	char key;

	int step = 1;

	image = initImage(&f);

	for(;;){
	
		
		while(ecanmouse()) emouse();	/* don't get resize events without this! */
		color = getRGB(f);
		f+= df;

		if(colorSRC) freeimage(colorSRC);
		colorSRC = allocimage(display, Rect(0,0,1,1), RGB24, 1, color);
		
		draw(image, Rect(0, 0, image->r.max.x, image->r.max.y), image, nil, Pt(step,0));

		line(image, Pt(image->r.max.x,0), Pt(image->r.max.x,image->r.max.y), Endsquare, Endsquare, step, colorSRC, ZP);
		
		draw(screen, screen->r, image, nil, image->r.min);
		

		if(ecankbd()){
			key = ekbd();
			switch(key){
				case 'q': case Kdel: exits(0); break;
				case 'u': df -= 0.01; break;
				case 'i': df += 0.01; break;
				case 'n':
					if(step>1)
						step-=1;
					break;
				case 'm':
					step+=1;
					break;
				case 's': sleep(5000); break;
				case Kup: center++; break;
				case Kdown: center--; break;
				case Kleft: width--; break;
				case Kright: width++; break;
				default: 
					sprint(buf,"key %d", key);
					string(screen,screen->r.min, display->black,ZP, font, buf);
					break;
			}
		}
		
		sprint(buf,"df %f - step %d - w/c %d/%d", df, step, width, center);
		string(screen,addpt(screen->r.min, Pt(10,10)), display->black,ZP, font, buf);

		flushimage(display, 1);
		//sleep(1);

	}

}

void
eresized(int new)
{
	if(getwindow(display, Refnone) < 0)
		sysfatal("resize failed: %r");

	if(image) freeimage(image);

	image = allocimage(display, screen->r, RGB24, 1, color);
	image = initImage(&f);

}
