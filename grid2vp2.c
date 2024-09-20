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
#define OURMAX 100

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

    if(hbarnums%2==0)
        cairo_rectangle(cr, pt[hbarnums*2-2].p.x-hbotiny, pt[hbarnums*2-2].p.y-hbotiny, hbotiny*2, hbotiny*2);
    else 
        cairo_rectangle(cr, pt[hbarnums*2-1].p.x-hbotiny, pt[hbarnums*2-1].p.y-hbotiny, hbotiny*2, hbotiny*2);
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
	if(argc!=3) {
		printf("Usage. Pls supply 2 argx: 1) integer - number of horizontal bars. 2) integer between 0 and 100 to find.\n");
		exit(EXIT_FAILURE);
	}
    int hbarnums=atoi(argv[1]);
    int myp=atoi(argv[2]);
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
            pt[i].d=pt[i-1].d + dwr2*OURMAX/tglen;
        } else if(i4==1) {
            pt[i].t=CURVER;
            pt[i].d=pt[i-1].d + M_PI*radi*OURMAX/tglen;
        } else if(i4==2) {
            pt[i].t=LINE2L;
            pt[i].d=pt[i-1].d + dwr2*OURMAX/tglen;
        } else if(i4==3) {
            pt[i].t=CURVEL;
            pt[i].d=pt[i-1].d + M_PI*radi*OURMAX/tglen;
        }
        printf("(%2.4f,%i)@ (%2.4f,%2.4f)\n", pt[i].d, pt[i].t, pt[i].p.x, pt[i].p.y);
    }

    // we use background ligh and darks grey arrows to emphasise direction of our grill.
    setbkarrows(cr, pt, hbarnums, hbo4);

    cairo_set_source_rgba(cr, 0.65, 0.8, 0.45, 1);
    setgrill(cr, pt, hbarnums, radi);
    // we put an arrow at start and square at end.
    // NOTE if your stroke line widith is big,the arrow head will be thinner and you will notice space.
    setstartend(cr, pt, hbarnums, hbo4);

    // find point
    for(i=0;i<2*hbarnums;i++) {
        if(myp>pt[i].d)
            continue;
        else
            break;
    }
    int pidx=i;
    printf("myp (%i) found btwn idx %i (type %i, dist %2.4f) and idx %i (type %i, dist %2.4f)\n", myp, pidx-1, pt[pidx-1].t, pt[pidx-1].d, pidx, pt[pidx].t, pt[pidx].d);
    // we're depnding oon final value of i
    // fracd; // is calculated in terms of distance, but must be converted to pixel distance via .x and .y
    pos_t mpt, mpt2; // the poistion of the point pixelwise.
    double fracd=((double)myp - pt[pidx-1].d) / (pt[pidx].d - pt[pidx-1].d); // fraction of distance, this can then be multiplid by pixel distance btwn nearest points
    printf("fracd=%2.6f\n", fracd); 
    cairo_set_source_rgba(cr, 0.65, 0.8, 0.45, 1);
    switch(pt[pidx].t) {
        case LINE2R:
            mpt.x=pt[pidx-1].p.x + fracd * (pt[pidx].p.x - pt[pidx-1].p.x);
            mpt.y=pt[pidx-1].p.y;
            cairo_move_to(cr, mpt.x, mpt.y-hbo4);
            cairo_line_to(cr, mpt.x, mpt.y);
            cairo_stroke(cr);
            break;
        case LINE2L:
            mpt.x=pt[pidx-1].p.x + (1-fracd) * (pt[pidx].p.x - pt[pidx-1].p.x); // reverse direction, therefore 1-fracp
            mpt.y=pt[pidx-1].p.y;
            cairo_move_to(cr, mpt.x, mpt.y-hbo4);
            cairo_line_to(cr, mpt.x, mpt.y);
            cairo_stroke(cr);
            break;
        case CURVER:
            printf("CurveR angle in rads=%2.6f in degrees=%2.6f\n", fracd*M_PI, fracd*180);
            mpt.x=pt[pidx-1].p.x + radi*sin(fracd*M_PI);
            mpt.y=pt[pidx-1].p.y + radi*(1-cos(fracd*M_PI));
            printf("sin:%2.6f\n", radi*sin(fracd*M_PI));
            printf("cos:%2.6f\n", radi*cos(fracd*M_PI));
            // mpt2.x=pt[pidx-1].p.x + hbo4*sin(fracd*M_PI);
            // mpt2.y=pt[pidx-1].p.y + hbo4*cos(fracd*M_PI);
            // mpt2.x=pt[pidx-1].p.x - hbo4*tan(fracd*M_PI);
            // mpt2.y=pt[pidx-1].p.y + hbo4;
            mpt2.x=pt[pidx-1].p.x;
            mpt2.y=pt[pidx-1].p.y;
            cairo_move_to(cr, mpt.x, mpt.y);
            cairo_rel_line_to(cr, mpt2.x, mpt2.y);
            cairo_stroke(cr);
            break;
        case CURVEL:
            printf("CurveL angle in rads=%2.6f in degrees=%2.6f\n", fracd*M_PI, fracd*180);
            mpt.x=pt[pidx].p.x - radi*sin(fracd*M_PI); // yes, know, tricky, p-1 is on the other side.
            mpt.y=pt[pidx-1].p.y + radi*(1-cos(fracd*M_PI));
            mpt2.x=pt[pidx-1].p.x - hbo4*sin(fracd*M_PI);
            mpt2.y=pt[pidx-1].p.y + hbo4*(1-cos(fracd*M_PI));
            cairo_move_to(cr, mpt.x, mpt.y);
            cairo_line_to(cr, mpt2.x, mpt2.y);
            cairo_stroke(cr);
            break;
    }
    printf("p-1.x=%2.4f, p-1.y=%2.4f p.x=%2.4f p.y=%2.4f\n", pt[pidx-1].p.x, pt[pidx-1].p.y, pt[pidx].p.x, pt[pidx].p.y);

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "grid2vp.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    free(pt);

    return 0;
}
