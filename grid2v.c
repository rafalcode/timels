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

#define HBARNUMS 12

typedef struct /* pos_t */
{
    float x, y;
} pos_t;

void setbkarrows(cairo_t *cr, pos_t *lb, int hbarnums, float hbo4)
{
    int i, i2;
    for(i=0;i<hbarnums;i++) {
        i2=i%2;
        if(i2==0) {
            cairo_move_to(cr,lb[2*i].x-hbo4, lb[2*i].y-hbo4);
            cairo_line_to(cr,lb[2*i+1].x, lb[2*i+1].y-hbo4);
            cairo_line_to(cr,lb[2*i+1].x+hbo4, lb[2*i+1].y);
            cairo_line_to(cr,lb[2*i+1].x, lb[2*i+1].y+hbo4);
            cairo_line_to(cr,lb[2*i].x-hbo4, lb[2*i].y+hbo4);
            cairo_line_to(cr,lb[2*i].x, lb[2*i].y);
            cairo_close_path(cr);
            cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 1.);
        } else {
            cairo_move_to(cr,lb[2*i].x, lb[2*i].y-hbo4);
            cairo_line_to(cr,lb[2*i+1].x+hbo4, lb[2*i+1].y-hbo4);
            cairo_line_to(cr,lb[2*i+1].x, lb[2*i+1].y);
            cairo_line_to(cr,lb[2*i+1].x+hbo4, lb[2*i+1].y+hbo4);
            cairo_line_to(cr,lb[2*i].x, lb[2*i].y+hbo4);
            cairo_line_to(cr,lb[2*i].x-hbo4, lb[2*i].y);
            cairo_close_path(cr);
            cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 1.);
        }
        cairo_fill(cr);
    }
}

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
    float radi=hbarsz/2.;
    float hbo4=hbarsz/4.; // hbarsz over 4.
    float dwr2=dw-radi*2;
    int totpoints = HBARNUMS*2;
    pos_t *lb=calloc(totpoints, sizeof(pos_t)); /* ori: origin, lb, last bar, */

    // first point, top left
    lb[0].x= dtl.x + radi;
    lb[0].y = dtl.y + 3*hbo4;
    // first row, only x changes
    for(i=1;i<2;i++) {
         lb[i].x= lb[i-1].x + dwr2;
         lb[i].y= lb[i-1].y;
    }
    // first column
    for(i=1;i<HBARNUMS;i++) {
        lb[i*2].x= lb[(i-1)*2].x;
        lb[i*2].y= lb[(i-1)*2].y + hbarsz;
    }

    for(i=1;i<HBARNUMS;i++)
        for(j=1;j<2;j++) {
            lb[i*2+j].x= lb[i*2+j-1].x + dwr2;
            lb[i*2+j].y= lb[i*2+j-1].y;
        }

    // we use background ligh and darks grey arrows to emphasise direction of our grill.
    setbkarrows(cr, lb, HBARNUMS, hbo4);

    cairo_set_source_rgba(cr, 0.65, 0.8, 0.45, 1);
    cairo_move_to(cr,lb[0].x, lb[0].y);
    for(i=0;i<HBARNUMS;i++) {
        i2=i%2;
        // for(j=0;j<2;++j) {
        //     cairo_arc(cr, lb[2*i+j].x, lb[2*i+j].y, 2, 0, 2 * M_PI);
        //     cairo_fill(cr);
        // }
        if(i2==0) {
            cairo_line_to(cr,lb[2*i+1].x, lb[2*i+1].y);
            if(i!=HBARNUMS-1)
                cairo_arc(cr, lb[2*i+1].x, lb[2*i+1].y+radi, radi, -M_PI_2, M_PI_2);
        } else {
            cairo_line_to(cr,lb[2*i].x, lb[2*i].y);
            if(i!=HBARNUMS-1)
                cairo_arc_negative(cr, lb[2*i].x, lb[2*i].y+radi, radi, -M_PI_2, -3*M_PI_2);
        }
    }
    cairo_set_line_width (cr, LINEWID);
    cairo_stroke(cr);

    /* Write output and clean up */
    cairo_surface_write_to_png (surface, "grid2v.png");
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
    free(lb);

    return 0;
}
