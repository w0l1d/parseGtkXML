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


#endif //PARSEGTKXML_MACROS_SUPPLIMENTAIRES_H
