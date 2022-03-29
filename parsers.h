//
// Created by W0L1D on 3/18/2022.
//
#include <gtk/gtk.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>
#include <stdio.h>
#include "utils.h"


#define TAG_OBJECT "object"
#define TAG_PROPERTY "property"
#define TAG_CHILD "child"
#define TAG_ITEMS "items"
#define TAG_PACKING "packing"

#define ATTR_CLASS "class"
#define ATTR_ID "id"
#define ATTR_NAME "name"
#define ATTR_TYPE "type"
#define ATTR_TYPE_SUBMENU "submenu"
#define ATTR_PROPERTY_GROUP "group"
#define ATTR_PROPERTY_IMAGE "image"
#define ATTR_PROPERTY_MODAL "modal"


#define NODE_IS(node, tag) (!xmlStrcasecmp((node), (xmlChar *)(tag)))


#ifndef PARSEGTKXML_PARSERS_H
#define PARSEGTKXML_PARSERS_H


GObject *macro_TransWidget(MyInterface *inteface, xmlNode *node);


/*
 *To compile this file using gcc you can type
 *gcc `xml2-config --cflags --libs` -o xmlexample libxml2-example.c
 */

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static void
print_element_names(xmlNode *a_node) {

    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        print_element_names(cur_node->children);
    }
}

xmlNode *macro_getRootElem(gchar *filename) {

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*parse the file and get the DOM */
    doc = xmlReadFile(filename, NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", filename);
        exit(-1);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    return root_element;
}

void macro_cleanupXML(xmlDoc *doc) {
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}

GObject *macro_TransObjAttrs(MyInterface *inteface, xmlNode *node) {
    GObject *widget;


    //NULL NODE
    if (!node) {
        fprintf(stderr, "NULL NODE\n");
        exit(-1);
    }


    xmlChar *typeName = xmlGetProp(node, (const xmlChar *) ATTR_CLASS);

    printf("\n object name : %s\n", typeName);
    GType gType = macro_getTypeFromName((const gchar *) typeName);


    g_assert(gType != 0);

    widget = g_object_new(gType, NULL);

    g_assert(widget != NULL);
    /// TODO :: PARSE THE REST OF ATTRIBUTES IF EXISTS

    //parsing the object id
    xmlChar *id = xmlGetProp(node, (const xmlChar *) ATTR_ID);

    if (id) {
        MyInterObj *interObj = g_malloc(sizeof(MyInterObj));
        interObj->id = g_strdup((const gchar *) id);
        interObj->obj = widget;
        inteface->list = g_list_append(inteface->list, interObj);

        g_free(id);
    }

    g_free(typeName);
    return widget;
}

void macro_ApplyObjProp(GObject *object,const gchar *property,const gchar *content) {

    GValue value = G_VALUE_INIT;
    //get property default value from object
    g_object_get_property(object, property, &value);
    //translate xml value from string to correct value
    macro_valueFromStringType(value.g_type, content, &value);
    //set value to the object
    g_object_set_property(object, property, &value);
}

void macro_ApplyObjChildProp(GObject *object, GObject *child, xmlNode *node) {
    //get property name from xml
    const gchar *property = (gchar *) xmlGetProp(node, (const xmlChar *) ATTR_NAME);
    //get property value
    const gchar *content = (gchar *) xmlNodeGetContent(node);
    //init value
    GValue value = G_VALUE_INIT;
    GParamSpec *pspec;
    //get property from object
    pspec = gtk_container_class_find_child_property(G_OBJECT_GET_CLASS (object), property);
    //init value type
    g_value_init(&value, pspec->value_type);
    //get property default value from object
    gtk_container_child_get_property(GTK_CONTAINER(object), GTK_WIDGET(child), property, &value);
    //translate xml value from string to correct value
    macro_valueFromStringType(pspec->value_type, content, &value);
    //set value to the object
    gtk_container_child_set_property(GTK_CONTAINER(object), GTK_WIDGET(child), property, &value);

}


void macro_applyChildProps(GObject *object, GObject *child, xmlNode *node) {
    xmlNode *curNode;
    for (curNode = node->children; curNode; curNode = curNode->next)
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_PROPERTY))
            macro_ApplyObjChildProp(object, child, curNode);
}

void macro_addChild(MyInterface *inteface, GObject *object, xmlNode *node) {
    xmlNode *curNode;
    GObject *child;
    for (curNode = node->children; curNode; curNode = curNode->next) {
        //looking for child
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_OBJECT)) {
            child = (GObject *) macro_TransWidget(inteface, curNode);
            if (!xmlStrcasecmp(
                    xmlGetProp(node, (const xmlChar *) ATTR_TYPE),
                    (const xmlChar *) ATTR_TYPE_SUBMENU))
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(object), GTK_WIDGET(child));
            else
                gtk_container_add(GTK_CONTAINER(object), GTK_WIDGET(child));
        }
        //looking for child properties
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_PACKING) && child) {
            macro_applyChildProps(object, child, curNode);
            child = NULL;
        }
    }
}

void macro_addComboBoxTextItems(GObject *object, xmlNode *node) {
    xmlNode *curNode;
    for (curNode = node->children; curNode; curNode = curNode->next) {
        //get property name from xml
        const gchar *id = (gchar *) xmlGetProp(curNode, (const xmlChar *) ATTR_ID);
        //get property value
        const gchar *content = (gchar *) xmlNodeGetContent(curNode);

        if (id)
            gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(object), id, content);
        else
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(object), content);
    }
}

void macro_parseChildrenTags(MyInterface *inteface, GObject *object, xmlNode *node) {
    g_assert(object != NULL);


    xmlNode *curNode = node->children;
    while (curNode) {
        //TODO :: COMPLETE CODE HERE
        // handle all tags
        /// Parse Property TAG
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_PROPERTY)) {
            //get property name from xml
            const gchar *property = (gchar *) xmlGetProp(curNode, (const xmlChar *) ATTR_NAME);
            //get property value
            const gchar *content = (gchar *) xmlNodeGetContent(curNode);

            //si l'attribut est group du radio button ou radio menu item
            if (!strcmpi(property, ATTR_PROPERTY_GROUP)) {
                GObject *group = macro_findWidget(inteface, content);
                if (!group)
                    g_printerr("\nGroup not found %s.\n", content);
                else if (GTK_IS_RADIO_BUTTON(object))
                    gtk_radio_button_join_group(GTK_RADIO_BUTTON(object),
                                                GTK_RADIO_BUTTON(group));
                else if (GTK_IS_RADIO_MENU_ITEM(object))
                    gtk_radio_menu_item_join_group(GTK_RADIO_MENU_ITEM(object),
                                                GTK_RADIO_MENU_ITEM(group));
            }
            //si l'attribut est image
            else if (!strcmpi(property, ATTR_PROPERTY_IMAGE)) {
                GObject *image = macro_findWidget(inteface, content);
                if (!image)
                    g_printerr("\nImage not found %s.\n", content);
                else if (GTK_IS_IMAGE_MENU_ITEM(object))
                    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(object), image);
                else if (GTK_IS_BUTTON(object))
                    gtk_button_set_image(GTK_BUTTON(object), GTK_WIDGET(image));
            }
            else
                macro_ApplyObjProp(object, property, content);
        }
        /// Parse Child TAG
        else if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_CHILD))
            macro_addChild(inteface, object, curNode);
        /// Parse Items TAG
        else if (GTK_IS_COMBO_BOX_TEXT(object) && !xmlStrcasecmp(curNode->name, (xmlChar *) TAG_ITEMS))
            macro_addComboBoxTextItems(object, curNode);

        curNode = curNode->next;
    }

}


GObject *macro_TransWidget(MyInterface *inteface, xmlNode *node) {
    xmlNode *curNode;
    curNode = node;
    GObject *widget = NULL;

    //NULL NODE
    if (!curNode) {
        fprintf(stderr, "NULL NODE\n");
        return ((GObject*)NULL);
    }

    //ignoring none object tags
    while (xmlStrcasecmp(curNode->name, (const xmlChar *) TAG_OBJECT)) {
        curNode = curNode->next;
        if (!curNode) {
            fprintf(stderr, "NO OBJECT TAG FOUND\n");
            return ((GObject *) NULL);
        }
    }


    widget = macro_TransObjAttrs(inteface, curNode);

    macro_parseChildrenTags(inteface, widget, curNode);

    return widget;
}




MyInterface *macro_getWidgets(gchar *filename) {
    xmlNode *root;
    root = macro_getRootElem(filename);
    xmlNode *curNode = root->children;
    MyInterface *inteface = g_malloc(sizeof(MyInterface));
    inteface->list = NULL;

    /**
     * BUILD Window from xml
     */

    while (curNode) {
        if (xmlStrcasecmp(curNode->name, (const xmlChar *) TAG_OBJECT))
            macro_TransWidget(inteface, curNode);
        curNode = curNode->next;
    }


    macro_cleanupXML(root->doc);
    return inteface;
}


#endif //PARSEGTKXML_PARSERS_H
