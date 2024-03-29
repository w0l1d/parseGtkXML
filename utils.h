//
// Created by W0L1D on 3/19/2022.
//

#ifndef PARSEGTKXML_UTILS_H
#define PARSEGTKXML_UTILS_H
#include <gtk/gtk.h>


/*****
 * structure of an object and it id
 */
typedef struct _MyInterfaceObject {
    gchar *id; /// object id
    GObject *obj; /// direct pointer to the gobject
} MyInterObj;

/*****
 * this structure contains only one attribute
 * but left like that in case we had to add new ones
 * in future versions
 */
typedef struct {
    GList *list; /// list of objects
} MyInterface;


/***
 * return the value of a given string as boolean
 * @param string boolean as string
 * @param value returned value
 * @return if the operation is successful or not
 */
gboolean
macro_trans_boolean_from_string (const gchar  *string,
                                  gboolean     *value)
{
    if (string[0] == '\0')
        goto error;
    else if (string[1] == '\0')
    {
        gchar c;
        c = string[0];
        if (c == '1' ||
            c == 'y' || c == 't' ||
            c == 'Y' || c == 'T')
            *value = TRUE;
        else if (c == '0' ||
                 c == 'n' || c == 'f' ||
                 c == 'N' || c == 'F')
            *value = FALSE;
        else
            goto error;
    }
    else
    {
        if (g_ascii_strcasecmp (string, "true") == 0 ||
            g_ascii_strcasecmp (string, "yes") == 0)
            *value = TRUE;
        else if (g_ascii_strcasecmp (string, "false") == 0 ||
                 g_ascii_strcasecmp (string, "no") == 0)
            *value = FALSE;
        else
            goto error;
    }
    return TRUE;
    error:
    g_printerr("Could not parse boolean '%s'", string);
    return FALSE;
}


/******
 * return the value of a given string as enumaration
 * @param type the type of te gvalue
 * @param string property name
 * @param enum_value translated value
 * @return if the operation fails or succeeds
 */
gboolean macro_enumFromString (GType type,
                      const gchar  *string,
                      gint *enum_value) {
    GEnumClass *eclass;
    GEnumValue *ev;
    gchar *endptr;
    gint value;
    gboolean ret;
    /// if the given type is enum
    g_return_val_if_fail (G_TYPE_IS_ENUM (type), FALSE);
    /// if the string is not empty
    g_return_val_if_fail (string != NULL, FALSE);
    ret = TRUE;
    endptr = NULL;
    /// gtk global error contant function
    errno = 0;
    // Converts a string to a #guint64 value
    value = (gint)g_ascii_strtoull (string, &endptr, 0);

    /// parsed a number
    if (errno == 0 && endptr != string)
        *enum_value = value;
    else
    {

        eclass = g_type_class_ref (type);
        ev = g_enum_get_value_by_name (eclass, string);
        if (!ev)
            ev = g_enum_get_value_by_nick (eclass, string);
        if (ev)
            *enum_value = ev->value;
        else {
            g_printerr("Could not parse enum '%s'", string);
            ret = FALSE;
        }
        g_type_class_unref (eclass);
    }
    return ret;
}


/******
 * traslate a property as string to a gvalue
 * @param type the type of the property
 * @param strValue it string value
 * @param value returned transformed value (initially default value)
 */
void macro_valueFromStringType(GType         type,
                               const gchar  *strValue,
                               GValue       *value) {
    if (!strValue || !value){
        g_printerr("value from string type : Param Null");
        exit(-1);
    }



    /// trasform by type
    switch (G_TYPE_FUNDAMENTAL(type)) {
        case G_TYPE_CHAR:
            g_value_set_schar (value, strValue[0]);
            break;
        case G_TYPE_UCHAR:
            g_value_set_uchar (value, (guchar)strValue[0]);
            break;
        case G_TYPE_BOOLEAN: {
            gboolean b;
            if (macro_trans_boolean_from_string(strValue, &b))
                g_value_set_boolean(value, b);
            break;
        }
        case G_TYPE_INT:
        case G_TYPE_LONG:
        case G_TYPE_INT64: {
            gint64 l;
            gchar *endptr = NULL;
            l = g_ascii_strtoll (strValue, &endptr, 0);
            if (endptr == strValue)
            {
                g_printerr("Could not parse integer '%s'", strValue);
                break;
            }
            if (G_VALUE_HOLDS_INT (value))
                g_value_set_int (value, (gint)l);
            else if (G_VALUE_HOLDS_LONG (value))
                g_value_set_long (value, (glong)l);
            else
                g_value_set_int64 (value, (gint64)l);
            break;
        }
        case G_TYPE_UINT:
        case G_TYPE_ULONG:
        case G_TYPE_UINT64:
        {
            guint64 ul;
            gchar *endptr = NULL;
            ul = g_ascii_strtoull (strValue, &endptr, 0);
            if (endptr == strValue)
            {
                g_printerr("Could not parse unsigned integer '%s'", strValue);
                break;
            }
            if (G_VALUE_HOLDS_UINT (value))
                g_value_set_uint (value, ul);
            else if (G_VALUE_HOLDS_ULONG (value))
                g_value_set_ulong (value, ul);
            else
                g_value_set_uint64 (value, ul);
            break;
        }


        case G_TYPE_FLOAT:
        case G_TYPE_DOUBLE:
        {
            gdouble d;
            gchar *endptr = NULL;
            d = g_ascii_strtod (strValue, &endptr);
            if (endptr == strValue)
            {
                g_printerr("Could not parse double '%s'", strValue);
                break;
            }
            if (G_VALUE_HOLDS_FLOAT (value))
                g_value_set_float (value, (gfloat)d);
            else
                g_value_set_double (value, d);
            break;
        }

        case G_TYPE_STRING:
            g_value_set_string (value, strValue);
            break;

        case G_TYPE_ENUM: {
            gint enumValue;
            if (macro_enumFromString(type, strValue, &enumValue))
                g_value_set_enum(value, enumValue);
            break;
        }
    }
}







/******
 * get the property type from it type name
 * @param type_name type name
 * @return the type corresponding to the name
 */
static GType
macro_RealGetTypeFromName (const gchar *type_name)
{
    GType gtype;
    gtype = g_type_from_name (type_name);
    if (gtype != G_TYPE_INVALID)
        return gtype;

    /// Force registration of all core Gtk+ and Gdk object types.
    /// This allows to refer to any of those object types
    gtk_test_register_all_types ();
    return g_type_from_name (type_name);
}

/***
 * hanlde the call to macro_RealGetTypeFromName static method
 * @param type_name the type name
 * @return type by the name
 */
GType macro_getTypeFromName(const gchar *type_name)
{
    return macro_RealGetTypeFromName(type_name);
}

/****
 * used for the curtom search on glist of objects
 * @param obj current object in filter
 * @param id id used to compare with
 * @return the comparison result
 * 0 if equal
 */
gint compObjectsIDs(gpointer obj, gpointer id)
{
    const gchar  *b = id;
    MyInterObj *a = obj;

    /// Compared by id
    return g_ascii_strcasecmp(a->id, b);
}


/*******
 * search for an object by it id
 * @param inteface list of objects
 * @param id id to look for
 * @return found object
 */
GObject *macro_findWidget(MyInterface *inteface, const gchar *id) {

    GList *elem = g_list_find_custom(inteface->list, id, (GCompareFunc)compObjectsIDs);

    return elem?((MyInterObj*)elem->data)->obj:NULL;
}

/****************
 * transforms a vertical GtkMenu into horizontal
 * @param menu used GtkMenu
 */
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




void macro_loadCSS(GtkWidget *window, const gchar *file) {
    GFile *css_gFile = g_file_new_for_path(file);
    GtkCssProvider *cssProvider = gtk_css_provider_new();

    gtk_css_provider_load_from_file(cssProvider, css_gFile, 0);

    gtk_style_context_add_provider_for_screen(gtk_widget_get_screen(window),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
}


#endif //PARSEGTKXML_UTILS_H
