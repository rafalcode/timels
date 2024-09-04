/* uses gri0.c from cairims to repupose grid code trndot.c ibut to do to do a simple grid */
#include <cairo/cairo.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CW 800 // canvas width
#define CH 600 // canvas height
#define TMAR 50 // top maring ... usually this will be biggest
#define BMAR 20
#define LMAR 20
#define RMAR 20
#define LINEWID 4

typedef enum {
    NOWT, LINE2R, LINE2L, CURVER, CURVEL, // line rightwards, lineleftwards, curve at right(most), curve at left. First one is nowt, as in nothing. only refers to first
} ltype;

typedef struct /* pos_t */
{
    double x, y;
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

void setstartend(cairo_t *cr, d_t *pt, int hbarnums, double hbo4)
{
    double hbotiny=hbo4;

    cairo_move_to(cr,pt[0].p.x-hbotiny, pt[0].p.y-hbotiny);
    cairo_line_to(cr,pt[0].p.x, pt[0].p.y);
    cairo_line_to(cr,pt[0].p.x-hbotiny, pt[0].p.y+hbotiny);
    cairo_close_path(cr);
    cairo_fill(cr);

    cairo_rectangle(cr, pt[hbarnums*2-2].p.x-hbotiny, pt[hbarnums*2-2].p.y-hbotiny, hbotiny*2, hbotiny*2);
    cairo_fill(cr);
}

void setgrill(cairo_t *cr, d_t *pt, int hbarnums, double radi)
{
    int i, i2;

    cairo_move_to(cr,pt[0].p.x, pt[0].p.y);
    for(i=0;i<hbarnums;i++) {
        i2=i%2;
        if(i2==0) {
            cairo_line_to(cr,pt[2*i+1].p.x, pt[2*i+1].p.y);
            if(i!=hbarnums-1)
                cairo_arc(cr, pt[2*i+1].p.x, pt[2*i+1].p.y+radi, radi, -M_PI_2, M_PI_2);
        } else {
            cairo_line_to(cr,pt[2*i].p.x, pt[2*i].p.y);
            if(i!=hbarnums-1)
                cairo_arc_negative(cr, pt[2*i].p.x, pt[2*i].p.y+radi, radi, -M_PI_2, -3*M_PI_2);
        }
    }
    cairo_set_line_width (cr, LINEWID);
    cairo_stroke(cr);
}

int main (int argc, char *argv[])
{
	if(argc!=2) {
		printf("Usage. Pls supply 1 argument: integer - number of horizontal bars.\n");
		exit(EXIT_FAILURE);
	}
    int hbarnums=atoi(argv[1]);
    int i, j;

    // we start with the canvas. Set up surface, set bg etc.
    cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, CW, CH);
    cairo_t *cr = cairo_create (surface);
    cairo_rectangle (cr, 0, 0, CW, CH);
    cairo_set_source_rgba (cr, 0, 0, 0, 0.95); /*  final number is alpha, 1.0 is opaque */
    cairo_fill (cr);

    // now the drawable
    pos_t dtl;
    dtl.x=LMAR; // dtl: drawable top left
    dtl.y=TMAR;
    double dw=CW-LMAR-RMAR; // drawable width extent
    double dh=CH-TMAR-BMAR; // drawable height extent
    // just for show
    cairo_rectangle (cr, dtl.x, dtl.y, dw, dh); /* arg explan: topleftcorner and size of shape  */
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
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
    // assign displacement and ltype: first one is already zero because of calloc, which is what we want it to be
    int i4;
    for(i=1;i<hbarnums*2;i++) {
        i4=(i-1)%4;
        if(i4==0) {
            pt[i].t=LINE2R;
            pt[i].d=pt[i-1].d + dwr2;
        } else if(i4==1) {
            pt[i].t=CURVER;
            pt[i].d=pt[i-1].d + M_PI*radi;
        } else if(i4==2) {
            pt[i].t=LINE2L;
            pt[i].d=pt[i-1].d + dwr2;
        } else if(i4==3) {
            pt[i].t=CURVEL;
            pt[i].d=pt[i-1].d + M_PI*radi;
        }
        printf("(%2.4f,%i)\n", pt[i].d, pt[i].t); 
    }

    // we use background ligh and darks grey arrows to emphasise direction of our grill.
    setbkarrows(cr, pt, hbarnums, hbo4);

    cairo_set_source_rgba(cr, 0.65, 0.8, 0.45, 1);
    setgrill(cr, pt, hbarnums, radi);
    // we put an arrow at start and square at end.
    setstartend(cr, pt, hbarnums, hbo4);

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "grid2vs.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    free(pt);

    return 0;
}
