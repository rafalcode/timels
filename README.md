# timels: a set of timeline programs using cairo and pango.

This is about displaying timelines. Essentially it's a graphical output
where the passage of time is represented 

Aluding to the widely known grammar of graphics, a certain parameter is represented or mapped onto
a measure in the graph, which in this case is the x-axis. 

The y-axis may also be used for this, but he x-axis foolows the flow of the wirtten English language so it's convenient.
Time is a single dimension, so at a simple level one axis is fine.


SO wha is a time line. Well it usually relates to a phenomenon of some duration where there are at least two time points taken. A start and and an end.
Th isis enough for a time line. However, this makes for only one time interval and so is trivial when just considering one time line. Of course when comparing two opr more 
timelines, one time interval is often interesting, but for looking within one timeline, at least two time intervals would be needed, and that requires three timepoints.

# watch for print friendliness!
Simple black and white is alredy something. No colours thanks.

# grid2v.c
this taken cfrom cairobegs, A useful building block.
Note the struct d_t with the 1D distance paramerization
the positions of the points right to left will have the position index decrease.
So, with an even number of horibars, the final position will be hbarnums*2-2!

# grid2vs.c
includes the section ing, i./e. dividing into ltype.

# grid2vp.c
percetnage, includes normalising by percentage. Th is in fact very easy,
you divded the displacemnt by the total length (which is in unit of pixels rtraversed)
and multiply that by 100. Then you have a distance which is 1 to 100.

