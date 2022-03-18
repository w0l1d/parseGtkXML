#include <stdio.h>
#include "parsers.h"

static void
activate(GtkApplication *app,
         gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;

    window = gtk_application_window_new(app);


    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);


    gtk_container_add(GTK_CONTAINER (window), box);
    gtk_widget_show_all(window);

}

int
main(int argc,
     char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);


    return status;
}