#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <time.h>

#define WINDOW_WIDTH  300
#define WINDOW_HEIGHT 300

#define WHITE 255,255,255
#define BLUE    0,  0,255
#define RED  0.77, 0.16, 0.13
#define BLACK   0,  0, 0
#define WIDTH 3

static char buffer[256];
static time_t curtime;
static struct tm *loctime;
static int seconds;
static int minutes;
static int hours;
static int radius;
static int movingSecondsEffect = FALSE;

static void toggleMovingSeconds()
{
 movingSecondsEffect = !movingSecondsEffect;
}

//static void quit_application(GtkApplication *app)
static void quit_application(gpointer app)
{
  //g_print ("You clicked \"Quit\"\n");

  g_application_quit (G_APPLICATION (app));
}

static void clicked(GtkWindow *window, GdkEventButton *event, gpointer user_data)
{
    /* toggle window manager frames */
    gtk_window_set_decorated(window, !gtk_window_get_decorated(window));
}

static void show_hide_frame(GtkWindow *window)
{
    /* toggle window manager frames */
    gtk_window_set_decorated(window, !gtk_window_get_decorated(window));
}

int show_popup(GtkWidget *widget, GdkEvent *event) {
  
  const int RIGHT_CLICK = 3;
    
  if (event->type == GDK_BUTTON_PRESS) {
      
      GdkEventButton *bevent = (GdkEventButton *) event;
      
      if (bevent->button == RIGHT_CLICK) {      
          
          gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL,
              bevent->button, bevent->time);
      }
          
      return TRUE;
  }

  return FALSE;
}



void DrawTickAt (GtkWidget *widget, cairo_t *cr, int nHour, int cx, int cy)
{
   /* --- Convert time to radians. --- */
   double dRadians = nHour * 3.14 / 6.0;

   /* --- Draw line from .95 * rad to 1.0 * rad --- */
   int x1 = cx+(int) ((0.95 * radius * sin (dRadians)));
   int y1 = cy-(int) ((0.95 * radius * cos (dRadians)));
   int x2 = cx+(int) ((1.0 * radius * sin (dRadians)));
   int y2 = cy-(int) ((1.0 * radius * cos (dRadians)));

   //cairo_set_source_rgb(cr, BLUE);
   cairo_set_source_rgb(cr, WHITE);
   cairo_set_line_width(cr, 2);
   cairo_move_to(cr, x1, y1);
   cairo_line_to(cr, x2, y2);
   cairo_stroke(cr);
}


void DrawSeconds (GtkWidget *widget, cairo_t *cr, int cx, int cy)
{
   /* --- Get radians from seconds --- */
   float dRadians = seconds * 3.14 / 30.0;
    
   /* --- Draw seconds --- */
   cairo_set_source_rgb(cr, WHITE);
   cairo_set_line_width(cr, WIDTH);
   cairo_move_to(cr, cx, cy);
   cairo_line_to(cr, 
                   cx + (0.9 * radius * sin (dRadians)),
                   cy - (0.9 * radius * cos (dRadians)));
   cairo_stroke(cr);
}

void DrawMinutes (GtkWidget *widget, cairo_t *cr, int cx, int cy)
{
   /* --- Get radians from seconds --- */
   float dRadians = (minutes * 3.14 / 30.0) +
                    (seconds * 3.14 / 1800.0);
    
   /* --- Draw seconds --- */
   cairo_set_source_rgb(cr, WHITE);
   cairo_set_line_width(cr, WIDTH * 2);
   cairo_move_to(cr, cx, cy);
   cairo_line_to(cr, 
                   cx + (0.7 * radius * sin (dRadians)),
                   cy - (0.7 * radius * cos (dRadians)));
   cairo_stroke(cr);
}

void DrawHours (GtkWidget *widget, cairo_t *cr, int cx, int cy)
{
   /* --- Get radians from seconds --- */
   float dRadians = ( (hours % 12) * 3.14 / 6.0) +
                    (  minutes * 3.14 / 360.0);
    
   /* --- Draw seconds --- */
   cairo_set_source_rgb(cr, WHITE);
   cairo_set_line_width(cr, WIDTH * 3);
   cairo_move_to(cr, cx, cy);
   cairo_line_to(cr, 
                   cx + (0.5 * radius * sin (dRadians)),
                   cy - (0.5 * radius * cos (dRadians)));
   cairo_stroke(cr);
}

static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
   int midx = gtk_widget_get_allocated_width(widget)/2;
   int midy = gtk_widget_get_allocated_height(widget)/2;
   int nHour;

   /* --- Draw Tickmarks at each hour --- */
   for (nHour = 1; nHour <= 12; nHour++)
   {
        DrawTickAt (widget, cr, nHour, midx, midy);
   }

   /* draw horizontal line */
   //cairo_set_source_rgb(cr, BLUE);
   //cairo_set_line_width(cr, WIDTH);
   //cairo_move_to(cr, midx-60, 160);
   //cairo_line_to(cr, midx+60, 160);
   //cairo_stroke(cr);

   //cairo_set_source_rgb(cr, RED);
   cairo_set_source_rgb(cr, WHITE);
   /*cairo_select_font_face (cr, "Georgia",
            CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD); */
   cairo_set_font_size (cr, 30);
   cairo_move_to(cr, midx-(midx/2),midy/2);
   cairo_show_text(cr, buffer);

   //cairo_set_source_rgb(cr, BLUE);
   cairo_set_source_rgb(cr, WHITE);
   radius = MIN (midx, midy) -2 ;
   cairo_move_to(cr, midx , 0);

   if(movingSecondsEffect)
   {
     if (seconds == 0) { seconds = 60; }
     double angle0hour = ( 2.0 * M_PI ) * ( 3.0 / 4.0 ) ;
     double angle2 = angle0hour + ( 2.0 * M_PI ) * ( seconds / 60.0 ) ;
     cairo_arc (cr, midx, midy, radius, angle0hour, angle2);
     cairo_stroke(cr);
   }

   DrawSeconds (widget, cr, midx, midy);
   DrawMinutes (widget, cr, midx, midy);
   DrawHours (widget, cr, midx, midy);

   return FALSE;
}


static gint time_handler (GtkWidget *widget)
{

   curtime = time(NULL);
   loctime = localtime(&curtime);
   seconds = loctime->tm_sec;
   minutes = loctime->tm_min;
   hours   = loctime->tm_hour;

   strftime(buffer, 256, "%T", loctime);

   gtk_widget_queue_draw(widget);

   return TRUE;
}

static void activate (GtkApplication* app, gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *drawing_area;
  GtkWidget *ebox;
  GtkWidget *pmenu;
  GtkWidget *secondsMenuItem;
  GtkWidget *frameMenuItem;
  GtkWidget *hideMenuItem;
  GtkWidget *quitMenuItem;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Gtk3Clock");
  gtk_window_set_default_size (GTK_WINDOW (window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  gtk_widget_set_app_paintable(window, TRUE);

  GdkScreen *screen = gtk_widget_get_screen(window);
  GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
  gtk_widget_set_visual(window, visual);

  //frame = gtk_frame_new (NULL);
  //gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);

  //frame = gtk_fixed_new ();
  //gtk_container_add (GTK_CONTAINER (window), frame);

  ebox = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(window), ebox);

  drawing_area = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawing_area, WINDOW_WIDTH - 20 , WINDOW_HEIGHT - 20);

  //gtk_container_add (GTK_CONTAINER (frame), drawing_area);
  gtk_container_add (GTK_CONTAINER (ebox), drawing_area);

  g_signal_connect (drawing_area, "draw", G_CALLBACK (draw_cb), NULL);
  g_signal_connect (drawing_area, "delete-event", G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
  g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);

  pmenu = gtk_menu_new();
  
  secondsMenuItem = gtk_menu_item_new_with_label("Show/Hide Moving Seconds");
  gtk_widget_show(secondsMenuItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), secondsMenuItem);
  
  frameMenuItem = gtk_menu_item_new_with_label("Show/Hide Frame");
  gtk_widget_show(frameMenuItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), frameMenuItem);
  
  hideMenuItem = gtk_menu_item_new_with_label("Minimize");
  gtk_widget_show(hideMenuItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), hideMenuItem);
  
  quitMenuItem = gtk_menu_item_new_with_label("Quit");
  gtk_widget_show(quitMenuItem);
  gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), quitMenuItem);
  
  g_signal_connect_swapped(G_OBJECT(secondsMenuItem), "activate", 
      G_CALLBACK(toggleMovingSeconds), NULL);    
  
  g_signal_connect_swapped(G_OBJECT(frameMenuItem), "activate", 
      G_CALLBACK(show_hide_frame), GTK_WINDOW(window));    
  
  g_signal_connect_swapped(G_OBJECT(hideMenuItem), "activate", 
      G_CALLBACK(gtk_window_iconify), GTK_WINDOW(window));    
  
  g_signal_connect_swapped(G_OBJECT(quitMenuItem), "activate", 
      G_CALLBACK(quit_application), app);  
      //G_CALLBACK(gtk_main_quit), NULL);  

  g_signal_connect_swapped(G_OBJECT(window), "destroy",
      G_CALLBACK(quit_application), app);
      //G_CALLBACK(gtk_main_quit), NULL);
        
  g_signal_connect_swapped(G_OBJECT(ebox), "button-press-event", 
      G_CALLBACK(show_popup), pmenu);  


  g_timeout_add (1000, (GSourceFunc) time_handler, drawing_area);

  gtk_widget_show_all (window);
}

int main (int    argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.clock", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
 
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
