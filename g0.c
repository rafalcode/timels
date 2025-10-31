/* I'm remodelling ht epositional styling as I call it */
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
#define LINEWID 3

#define HBARNUMS 3

typedef enum {
    NOWT, LINE2R, LINE2L, CURVER, CURVEL, // line rightwards, lineleftwards, curve at right(most), curve at left. First one is nowt, as in nothing. only refers to first
} ltype;

typedef struct /* pos_t */
{
    float x, y;
} pos_t;

typedef struct /* d_t, displacement type. this is a parmetrization */
{
    float cd; // cumulative displacment aka distance
    pos_t p;
    ltype t;
    char tt[7];
} d_t;

int main (int argc, char *argv[])
{
    int i, i2, j;

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
    float dw=CW-LMAR-RMAR; // drawable width extent
    float dh=CH-TMAR-BMAR; // drawable height extent
    // just for show
    cairo_rectangle (cr, dtl.x, dtl.y, dw, dh); /* arg explan: topleftcorner and size of shape  */
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
    cairo_fill (cr);

    /* vertical divider how ar ewe going to section off the screen vertically */
    float hbarsz=dh/HBARNUMS; // y direction.
    printf("hbarsz=%2.4f\n", hbarsz); 
    float radi=hbarsz/2.;
    float hbo4=hbarsz/4.; // hbarsz over 4.
    float dwr2=dw-radi*2;
    int totpoints = HBARNUMS*2;
    d_t *dpt=calloc(totpoints, sizeof(d_t)); /* ori: origin, lb, last bar, */

    // first point, top left
    dpt[0].p.x= dtl.x + radi;
    dpt[0].p.y = dtl.y + 3*hbo4;
    dpt[0].cd = 0;
    dpt[0].t = NOWT;
    sprintf(dpt[0].tt, "%s", "NOWT");
    int mi; // mod i
    // first row, only x changes
    float onedd=0; // one d distance, I sometimes call it pixeldistance, number of pixels the timeline traverses.
    for(i=1;i<totpoints;i+=2) {
        mi=i%4;
        if(mi==1) {
            dpt[i].p.x= dpt[i-1].p.x + dwr2;
            dpt[i].cd= dpt[i-1].cd + dwr2;
            dpt[i].p.y= dpt[i-1].p.y;
            dpt[i].t = LINE2R;
            sprintf(dpt[i].tt, "%s", "LINE2R");
            // point that follows: and arc
            if(i<totpoints-2) {
                dpt[i+1].p.x= dpt[i].p.x;
                dpt[i+1].p.y= dpt[i].p.y +hbarsz; 
                dpt[i+1].cd= dpt[i].cd + radi+M_PI;
                dpt[i+1].t=CURVER;
                sprintf(dpt[i+1].tt, "%s", "CURVER");
            }
        } else if(mi==3) {
            dpt[i].p.x= dpt[i-1].p.x - dwr2;
            dpt[i].p.y= dpt[i-1].p.y;
            dpt[i].cd= dpt[i-1].cd + dwr2;
            dpt[i].t = LINE2L;
            sprintf(dpt[i].tt, "%s", "LINE2L");
            // point that follows: and negarc
            if(i<totpoints-2) {
                dpt[i+1].p.x= dpt[i].p.x;
                dpt[i+1].p.y= dpt[i].p.y +hbarsz; 
                dpt[i+1].cd= dpt[i].cd + radi+M_PI;
                dpt[i+1].t=CURVEL;
                sprintf(dpt[i+1].tt, "%s", "CURVEL");
            }
        }
    }
    for(i=0;i<totpoints;i++)
        printf("pt%i:%2.4f,%2.4f,%s,cd=%2.4f\n", i, dpt[i].p.x, dpt[i].p.y, dpt[i].tt, dpt[i].cd); 

    // we use background ligh and darks grey arrows to emphasise direction of our grill.
    // setbkarrows(cr, lb, HBARNUMS, hbo4);

    cairo_set_source_rgb(cr, 0.25, 0.7, 0.35);
    cairo_set_line_width (cr, LINEWID);
    for(i=1;i<totpoints;i+=2) {
        mi=i%4;
        cairo_move_to(cr,dpt[i-1].p.x, dpt[i-1].p.y);
        cairo_line_to(cr,dpt[i].p.x, dpt[i].p.y);
        if((mi==3) & (i!=totpoints-1))
            cairo_arc_negative(cr, dpt[i].p.x, dpt[i].p.y+radi, radi, -M_PI_2, -3*M_PI_2);
        else if(i!=totpoints-1)
            cairo_arc(cr, dpt[i].p.x, dpt[i].p.y+radi, radi, -M_PI_2, M_PI_2);
        cairo_stroke(cr);
    }

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "g0.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    free(dpt);

    return 0;
}
