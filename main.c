#include <stdio.h>
#include "parsers.h"
#include "macros_supplimentaires.h"

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

static void
activate(GtkApplication *app,
         gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *menuBtn;




    MyInterface *myInterface = macro_getWidgets("simplest interface.glade");

    GtkWidget *xmlWindow = GTK_WIDGET(macro_findWidget(myInterface, "window"));

    /// transform th menu to horizontal
/*
    GtkWidget *xmlMenu = GTK_WIDGET(macro_findWidget(myInterface, "test555"));

    if (!xmlMenu)
        g_printerr("menu is not found");
    else
        macro_transMenuHoriz(GTK_MENU(xmlMenu));
*/

    gtk_application_add_window(app, GTK_WINDOW(xmlWindow));

    gtk_widget_show_all(xmlWindow);

    //    macro_quickMessageDialog(GTK_WINDOW(xmlWindow), "My custom dialog", "hello can you accept ?", TRUE);


    /*

    window = gtk_application_window_new(app);




    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget  *btn = gtk_button_new_with_label("Test button");

    g_object_set_data(btn, "key", "hello world");
    g_object_set_data(box, "key", "hello world 222");

    gtk_container_add(GTK_CONTAINER (box), btn);

    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_HALF);




    gtk_container_add(GTK_CONTAINER (window), box);
    gtk_widget_show_all(window);
*/

}

int
main(int argc,
     char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);

    g_object_unref(app);



    return status;
}