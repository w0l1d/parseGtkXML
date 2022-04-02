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

gboolean btn_press_callback(GtkWidget *btn, GdkEventButton *event, gpointer userdata)
{
    g_print("I am clicked a single time\n");

    if (event->type == GDK_2BUTTON_PRESS  /// double click event
    &&  event->button == 1) // on left mouse button
    {
        g_print("I am double clicked \n");
    }
    if (event->type == GDK_DOUBLE_BUTTON_PRESS  /// double click event
        &&  event->button == 1) // on left mouse button
    {
        g_print("I am double clicked 2\n");
    }
    if (event->type == GDK_3BUTTON_PRESS  /// double click event
        &&  event->button == 1) // on left mouse button
    {
        g_print("I am triple clicked\n");
    }
    if (event->type == GDK_TRIPLE_BUTTON_PRESS  /// double click event
        &&  event->button == 1) // on left mouse button
    {
        g_print("I am triple clicked\n");
    }
    return TRUE;//or false
}

static void first_dialog(void)
{
    // This creates (but does not yet display) a message dialog with
    // the given text as the title.
    GtkWidget* hello = gtk_message_dialog_new(
            NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_YES_NO,
            "Hi, I'm a message dialog!");

    // The (optional) secondary text shows up in the "body" of the
    // dialog. Note that printf-style formatting is available.
    gtk_message_dialog_format_secondary_text(
            GTK_MESSAGE_DIALOG(hello),
            "This is secondary text with printf-style formatting: %d",
            99);

    // This displays our message dialog as a modal dialog, waiting for
    // the user to click a button before moving on. The return value
    // comes from the :response signal emitted by the dialog. By
    // default, the dialog only has an OK button, so we'll get a
    // GTK_RESPONSE_OK if the user clicked the button. But if the user
    // destroys the window, we'll get a GTK_RESPONSE_DELETE_EVENT.
    int response = gtk_dialog_run(GTK_DIALOG(hello));

    printf("response was %d (OK=%d, DELETE_EVENT=%d)\n",
           response, GTK_RESPONSE_OK, GTK_RESPONSE_DELETE_EVENT);

    // If we don't destroy the dialog here, it will still be displayed
    // (in back) when the second dialog below is run.
    gtk_widget_destroy(hello);
}

static void second_dialog(void)
{
    // In the second dialog, we use markup (just bold and italics in
    // this case).
    GtkWidget* hello_markup = gtk_message_dialog_new_with_markup(
            NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "Hi, I'm a <b>message dialog</b> with <i>markup</i>!");

    // We *could* add secondary text, either plain text or with
    // markup, but we haven't done it here, just to show what the end
    // result looks like. Either way, printf-style formatting is
    // available.

    // gtk_message_dialog_format_secondary_markup(
    //     GTK_MESSAGE_DIALOG(hello_markup),
    //     "This is <i>secondary</i> markup.");

    // Again, this displays the second dialog as a modal dialog.
    gtk_dialog_run(GTK_DIALOG(hello_markup));

    gtk_widget_destroy(hello_markup);
}

static void
activate(GtkApplication *app,
         gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *menuBtn;


    GdkEventButton eventButton;

    first_dialog();
    second_dialog();

    MyInterface *myInterface = macro_getWidgets("simplest interface.glade");

    GtkWidget *xmlWindow = GTK_WIDGET(macro_findWidget(myInterface, "window"));

    GtkWidget *button = GTK_WIDGET(macro_findWidget(myInterface, "mybutton"));



    g_signal_connect(G_OBJECT(button),
                     "button-press-event",
                     G_CALLBACK(btn_press_callback),
                     NULL
    );



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