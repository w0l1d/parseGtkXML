//
// Created by W0L1D on 3/19/2022.
//

#ifndef PARSEGTKXML_UTILS_H
#define PARSEGTKXML_UTILS_H
#include <gtk/gtk.h>


typedef struct _MyInterfaceObject {
    gchar *id;
    GObject *obj;
} MyInterObj;

typedef struct {
    GList *list;
} MyInterface;

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

gboolean macro_enumFromString (GType type,
                      const gchar  *string,
                      gint *enum_value) {
    GEnumClass *eclass;
    GEnumValue *ev;
    gchar *endptr;
    gint value;
    gboolean ret;
    g_return_val_if_fail (G_TYPE_IS_ENUM (type), FALSE);
    g_return_val_if_fail (string != NULL, FALSE);
    ret = TRUE;
    endptr = NULL;
    errno = 0;
    value = (gint)g_ascii_strtoull (string, &endptr, 0);
    if (errno == 0 && endptr != string) /* parsed a number */
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

void macro_valueFromStringType(GType         type,
                               const gchar  *strValue,
                               GValue       *value) {
    if (!strValue || !value){
        g_printerr("value from string type : Param Null");
        exit(-1);
    }



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



//get GType from name

typedef GType (*GTypeGetFunc) (void);

static gchar *
type_name_mangle (const gchar *name)
{
    GString *symbol_name = g_string_new ("");
    gint i;
    for (i = 0; name[i] != '\0'; i++)
    {
        /* skip if uppercase, first or previous is uppercase */
        if ((name[i] == g_ascii_toupper (name[i]) &&
             i > 0 && name[i-1] != g_ascii_toupper (name[i-1])) ||
            (i > 2 && name[i]   == g_ascii_toupper (name[i]) &&
             name[i-1] == g_ascii_toupper (name[i-1]) &&
             name[i-2] == g_ascii_toupper (name[i-2])))
            g_string_append_c (symbol_name, '_');
        g_string_append_c (symbol_name, g_ascii_tolower (name[i]));
    }
    g_string_append (symbol_name, "_get_type");
    return g_string_free (symbol_name, FALSE);
}

static GType
macro_resolveTypeLazily (const gchar *name)
{
    static GModule *module = NULL;
    GTypeGetFunc func;
    gchar *symbol;
    GType gtype = G_TYPE_INVALID;
//    if (!module)
//        module = g_module_open (NULL, 0);
//    symbol = type_name_mangle (name);
//    if (g_module_symbol (module, symbol, (gpointer)&func))
//        gtype = func ();
//    g_free (symbol);
    return gtype;
}

static GType
macro_RealGetTypeFromName (const gchar *type_name)
{
    GType gtype;
    gtype = g_type_from_name (type_name);
    if (gtype != G_TYPE_INVALID)
        return gtype;
//    gtype = macro_resolveTypeLazily (type_name);
//    if (gtype != G_TYPE_INVALID)
//        return gtype;
    gtk_test_register_all_types ();
    return g_type_from_name (type_name);
}

GType macro_getTypeFromName(const gchar *type_name)
{
    return macro_RealGetTypeFromName(type_name);
}

gint compObjectsIDs(gpointer obj, gpointer id)
{
    const gchar  *b = id;
    MyInterObj *a = obj;

    /* Compared by title */
    return g_ascii_strcasecmp(a->id, b);
}

GObject *macro_findWidget(MyInterface *inteface, const gchar *id) {
    GtkWidget *widget;

    GList *elem = g_list_find_custom(inteface->list, id, (GCompareFunc)compObjectsIDs);

    return elem?((MyInterObj*)elem->data)->obj:NULL;
}


#endif //PARSEGTKXML_UTILS_H
