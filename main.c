#include <stdio.h>
#include "parsers.h"

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
    gtk_application_add_window(app, GTK_WINDOW(xmlWindow));


    window = gtk_application_window_new(app);

    label = gtk_label_new("My Menu");

    menuBtn = gtk_menu_button_new();
    gtk_menu_button_set_direction(GTK_MENU_BUTTON(menuBtn), GTK_ARROW_RIGHT);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(menuBtn), getMenuExample());

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), menuBtn, TRUE, TRUE, 0);


    button = gtk_flow_box_new();

    gtk_container_add(GTK_CONTAINER (button), box);




    gtk_container_add(GTK_CONTAINER (window), button);
    gtk_widget_show_all(window);
    gtk_widget_show_all(xmlWindow);

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