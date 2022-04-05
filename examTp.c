#include <stdio.h>
#include <string.h>
#include "parsers.h"



/*
GtkWidget *getMenuExample() {
    GtkWidget *item_menu;
    item_menu = gtk_menu_new();
    for (int i = 0; i < 4; ++i) {

        GtkWidget *subitem;
        subitem = gtk_menu_item_new_with_label("Menu item");
        // g_signal_connect(GTK_WIDGET(subitem), "activate", G_CALLBACK(button_click), menulist[i].ssmenu[loop]);
        gtk_menu_shell_append(GTK_MENU_SHELL(item_menu), subitem);
    }
    return item_menu;
}

void macro_transMenuHoriz(GtkMenu *menu) {
    GList *items = gtk_container_get_children(GTK_CONTAINER(menu));

    GtkWidget *item;


    guint i = 0;
    while (items) {
        item = items->data;
        g_object_ref(item);
        gtk_container_remove(GTK_CONTAINER(menu), item);
        gtk_menu_attach(menu, item, i, i+1, 0, 1);
        g_object_unref(item);
        i++;
        items = items->next;
    }
}
*/


/*******************************************************************
 * begin                      EVENTS
 * *****************************************************************
 */
void button_clicked(GtkWidget *widget, gpointer data) {

    g_print("clicked\n");
}

void button_quit(GtkWidget *widget, gpointer data) {
    gtk_window_close(GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}



/*******************************************************************
 * end                           EVENTS
 * *****************************************************************
 */















static void
activate(GtkApplication *app,
         gpointer user_data) {


    MyInterface *myInterface = macro_getWidgets("examTP.glade");
    GtkWidget *xmlWindow = GTK_WIDGET(macro_findWidget(myInterface, "mainWindow"));
    GtkWidget *xmlWindowSec = GTK_WIDGET(macro_findWidget(myInterface, "windowSec"));

    gtk_application_add_window(app, GTK_WINDOW(xmlWindow));
    //    gtk_application_add_window(app, GTK_WINDOW(xmlWindowSec));


    /// Load css styling file
    macro_loadCSS(xmlWindow, "style.css");

    /// main window action buttons
    GtkWidget *buttonOk = GTK_WIDGET(macro_findWidget(myInterface, "okbutton"));
    GtkWidget *buttonQt = GTK_WIDGET(macro_findWidget(myInterface, "quitterButton"));


    /*GtkWidget *menu = GTK_WIDGET(macro_findWidget(myInterface, "azerty"));
    macro_transMenuHoriz(menu);*/



    g_signal_connect(G_OBJECT(buttonOk), "clicked", G_CALLBACK(button_clicked), NULL);
    g_signal_connect(G_OBJECT(buttonQt), "clicked", G_CALLBACK(button_quit), NULL);

    gtk_widget_show_all(xmlWindow);
    gtk_widget_show_all(xmlWindowSec);


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
