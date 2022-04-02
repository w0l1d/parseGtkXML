//
// Created by W0L1D on 3/28/2022.
//
#include <gtk/gtk.h>

#ifndef PARSEGTKXML_MACROS_SUPPLIMENTAIRES_H
#define PARSEGTKXML_MACROS_SUPPLIMENTAIRES_H

void
macro_signal_dialogResponse(
        GtkDialog *self,
        gint response_id,
        gpointer user_data) {
    if (response_id == GTK_RESPONSE_YES)
        printf("the user accepted");
    else if (response_id == GTK_RESPONSE_NO)
        printf("the user refused");
    else
        printf("the user did not respond");
}


// Function to open a dialog box with a message
void
macro_quickMessageDialog(GtkWindow *parent, gchar *title, gchar *message, gboolean block_screen) {
    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;

    // Create the widgets
    flags = GTK_DIALOG_MODAL;
    dialog = gtk_dialog_new_with_buttons(title,
                                         parent,
                                         flags,
                                         "_Oui",
                                         GTK_RESPONSE_YES,
                                         "_Non",
                                         GTK_RESPONSE_NO,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG (dialog));
    label = gtk_label_new(message);


    // Ensure that the dialog box is destroyed when the user responds
    g_signal_connect_swapped (dialog,
                              "response",
                              G_CALLBACK(gtk_widget_destroy),
                              dialog);

    // gets the user response to dialog box
    g_signal_connect_swapped (dialog,
                              "response",
                              G_CALLBACK(macro_signal_dialogResponse),
                              NULL);

    // Add the label, and show everything we’ve added
    gtk_container_add(GTK_CONTAINER(content_area),label);

    block_screen ?
    gtk_dialog_run(GTK_DIALOG(dialog))
                 :
    gtk_widget_show_all(dialog);
}


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


#endif //PARSEGTKXML_MACROS_SUPPLIMENTAIRES_H
