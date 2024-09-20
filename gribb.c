/* boxb, box border */
#include <cairo/cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CW 640 // canvas width
#define CH 480 // canvas height

// there's an outer border ...
#define TMAR 50 // top maring ... usually this will be biggest
#define BMAR 20
#define LMAR 20
#define RMAR 20

// then the box itself has a Topmarg and leftmarg
#define BM 10 // box margin
#define BSZ 80 // essential box size without its borders.

// and an inner one!
#define ITM TMAR+BM+BSZ+BM
#define ILM LMAR+BM+BSZ+BM
#define LINEWID 4

#define ILMAR 15 // inner left margn .. for our curling timeline! 
#define ITMAR 10 // inner top margin 
                  //
#define BMAR 20

#define HBARNUMS 4

typedef enum {
    NOWT, LINE2R, LINE2L, CURVER, CURVEL, // line rightwards, lineleftwards, curve at right(most), curve at left. First one is nowt, as in nothing. only refers to first
} ltype;
typedef struct /* pos_t */
{
    float x, y;
} pos_t;

typedef struct /* d_t, dsplacement type. this is a parmetrization */
{
    double d; // displacment aka distance
    pos_t p;
    ltype t;
} d_t;

void setbkarrows(cairo_t *cr, d_t *pt, int hbarnums, double hbo4)
{
    int i, i2;
    for(i=0;i<hbarnums;i++) {
        i2=i%2;
        if(i2==0) {
            cairo_move_to(cr,pt[2*i].p.x-hbo4, pt[2*i].p.y-hbo4);
            cairo_line_to(cr,pt[2*i+1].p.x, pt[2*i+1].p.y-hbo4);
            cairo_line_to(cr,pt[2*i+1].p.x+hbo4, pt[2*i+1].p.y);
            cairo_line_to(cr,pt[2*i+1].p.x, pt[2*i+1].p.y+hbo4);
            cairo_line_to(cr,pt[2*i].p.x-hbo4, pt[2*i].p.y+hbo4);
            cairo_line_to(cr,pt[2*i].p.x, pt[2*i].p.y);
            cairo_close_path(cr);
            cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 1.);
        } else {
            cairo_move_to(cr,pt[2*i].p.x, pt[2*i].p.y-hbo4);
            cairo_line_to(cr,pt[2*i+1].p.x+hbo4, pt[2*i+1].p.y-hbo4);
            cairo_line_to(cr,pt[2*i+1].p.x, pt[2*i+1].p.y);
            cairo_line_to(cr,pt[2*i+1].p.x+hbo4, pt[2*i+1].p.y+hbo4);
            cairo_line_to(cr,pt[2*i].p.x, pt[2*i].p.y+hbo4);
            cairo_line_to(cr,pt[2*i].p.x-hbo4, pt[2*i].p.y);
            cairo_close_path(cr);
            cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 1.);
        }
        cairo_fill(cr);
    }
}


int main (int argc, char *argv[])
{
	if(argc!=3) {
		printf("Usage. Pls supply 2 argx: 1) integer - number of horizontal bars. 2) integer between 0 and 100 to find.\n");
		exit(EXIT_FAILURE);
	}
    int i, j;
    int hbarnums=atoi(argv[1]);
    int myp=atoi(argv[2]);

    // we start with the canvas. Set up surface, set bg etc.
    cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, CW, CH);
    cairo_t *cr = cairo_create (surface);
    cairo_rectangle (cr, 0, 0, CW, CH);
    cairo_set_source_rgba (cr, 0, 0, 0, 0.95); /*  final number is alpha, 1.0 is opaque */
    cairo_fill (cr);

    //our boxes
    int tx = (CW-LMAR-RMAR-BM) / (BSZ+BM); // times xwise
    int txr = (CW-LMAR-RMAR-BM) % (BSZ+BM); // times xwise remainder
    printf("timesx %i timesremx %i\n", tx, txr); 
    int ty = (CH-TMAR-BMAR-BM) / (BSZ+BM); // times ywise
    int tyr = (CW-TMAR-RMAR-BM) % (BSZ+BM); // times xwise remainder
    printf("timesy %i timesremy %i\n", ty, tyr); 
    printf("So number of full boxes around perimter is actually %i\n", 2*tx+2*ty-4);

    int icw=tx*(BSZ+BM)+BM;
    int ich=ty*(BSZ+BM)+BM;

    // cairo_rectangle (cr, LMAR+txr/2, TMAR+tyr/2, CW-LMAR-RMAR-txr, CH-TMAR-BMAR-tyr-BM-BM);
    // cairo_rectangle (cr, LMAR+txr/2, TMAR+tyr/2, CW-LMAR-RMAR-txr, CH-TMAR-BMAR-tyr);
    cairo_rectangle (cr, LMAR+txr/2, TMAR+tyr/2, icw, ich);
    cairo_set_source_rgb(cr, 0.1, .1, .4);
    cairo_fill (cr);

    // and the inner one which will remain clear (drawable).
    pos_t dtl;
    dtl.x=ILM + txr/2 +ILMAR;
    dtl.y=ITM + tyr/2 +ITMAR;
    double dw= (tx-2)*(BSZ+BM)-BM-2*ILMAR;
    double dh= (ty-2)*(BSZ+BM)-BM-2*ITMAR; 
    cairo_rectangle(cr, dtl.x, dtl.y, dw, dh);
    cairo_set_source_rgb(cr, 0.2, .2, .2);
    cairo_fill (cr);

    /* vertical divider how ar ewe going to section off the screen vertically */
    double hbarsz=dh/hbarnums; // y direction.
    double radi=hbarsz/2.;
    double hbo4=hbarsz/4.; // hbarsz over 4.
    double dwr2=dw-radi*2;
    double tglen=hbarnums*dwr2 + M_PI*radi*(hbarnums-1); // total grill length: pi*radi is a semicircles circum, and in total there's one less than horizontal bars
    printf("tglen(px)=%2.6f\n", tglen); 

    d_t *pt=calloc(hbarnums*2, sizeof(d_t)); /* ori: origin, lb, last bar, */
    // first point, top left
    pt[0].p.x= dtl.x + radi;
    pt[0].p.y = dtl.y + 3*hbo4;
    // first row, only x changes
    for(i=1;i<2;i++) {
         pt[i].p.x= pt[i-1].p.x + dwr2;
         pt[i].p.y= pt[i-1].p.y;
    }
    // first column
    for(i=1;i<hbarnums;i++) {
        pt[i*2].p.x= pt[(i-1)*2].p.x;
        pt[i*2].p.y= pt[(i-1)*2].p.y + hbarsz;
    }
    // the rest
    for(i=1;i<hbarnums;i++)
        for(j=1;j<2;j++) {
            pt[i*2+j].p.x= pt[i*2+j-1].p.x + dwr2;
            pt[i*2+j].p.y= pt[i*2+j-1].p.y;
        }
    setbkarrows(cr, pt, hbarnums, hbo4);





    // onto drawing surrounding boxes
    pos_t *st=malloc((2*tx+2*ty-4)*sizeof(pos_t));
    st[0].x=LMAR+txr/2+BM;
    st[0].y=TMAR+tyr/2+BM;
    // from top left to top right
    for(i=1;i<tx;++i) {
        st[i].x=st[i-1].x + BSZ + BM;
        st[i].y=st[0].y;
    }
    // down from top right to bottom right
    for(i=tx;i<tx+ty-1;++i) {
        st[i].x=st[i-1].x;
        st[i].y=st[i-1].y + BSZ + BM;
    }
    // leftwise from bottom right
    for(i=tx+ty-1;i<2*tx+ty-2;++i) {
        st[i].x=st[i-1].x - BSZ - BM;
        st[i].y=st[tx+ty-2].y;
    }
    // from bottom left to top left.
    for(i=2*tx+ty-2;i<2*tx+2*ty-4;++i) {
        st[i].x=st[2*tx+ty-3].x;
        st[i].y=st[i-1].y - BSZ - BM;
    }

    for(i=0;i<2*tx+2*ty-4;++i) 
        cairo_rectangle (cr, st[i].x, st[i].y, BSZ, BSZ);
    cairo_set_source_rgb(cr, .6, .6, 0);
    cairo_fill (cr);

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "gribb.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    free(st);

    return 0;
}
