#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

int set_video_mode(void)
{
   /* request auto detection */
   int gdriver = DETECT, gmode, errorcode;
   int xmax, ymax;

   /* initialize graphics and local variables */
   initgraph(&gdriver, &gmode, "c:\\bc31\\bgi");

   /* read result of initialization */
   errorcode = graphresult();
   /* an error occurred */
   if (errorcode != grOk)
   {
      printf("Graphics error: %s\n", grapherrormsg(errorcode));
      printf("Press any key to halt:");
      getch();
      exit(1);
   }

   setcolor(getmaxcolor());
   xmax = getmaxx();
   ymax = getmaxy();

   return 0;
}


void main()
{
   /* draw a diagonal line */
   line(20,20, 50, 50);

   /* clean up */
   getch();

   closegraph();

}