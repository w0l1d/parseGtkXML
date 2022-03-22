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
#define TAG_PACKING "packing"

#define ATTR_CLASS "class"
#define ATTR_NAME "name"


#define NODE_IS(node, tag) (!xmlStrcasecmp((node), (xmlChar *)(tag)))


#ifndef PARSEGTKXML_PARSERS_H
#define PARSEGTKXML_PARSERS_H



GtkWidget *macro_TransWidget(xmlNode *node);


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

GObject *macro_TransObjAttrs(xmlNode *node) {
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

    return widget;
}

void macro_ApplyObjProp(GObject *object, xmlNode *node) {
    GValue value = G_VALUE_INIT;

    //get property name from xml
    const gchar *property = (gchar *) xmlGetProp(node, (const xmlChar *) ATTR_NAME);
    //get property value
    const gchar *content = (gchar *) xmlNodeGetContent(node);
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

    GValue value = G_VALUE_INIT;
    GParamSpec *pspec;
    pspec = gtk_container_class_find_child_property (G_OBJECT_GET_CLASS (object),
                                                     property);


//    gtk_container_class_install_child_property(GTK_CONTAINER_GET_CLASS(object), pspec->param_id, pspec);
//    g_value_init (&value, pspec->value_type);

    g_value_set_gtype(&value, pspec->value_type);

    //get property default value from object
//    gtk_container_child_get_property(GTK_CONTAINER(object), GTK_WIDGET(child),property,&value);

    g_print("\nchild property %s, content %s, type %s, default %s, new\n", property, content, g_type_name(pspec->value_type), g_value_get_string(&value));
    //translate xml value from string to correct value
    macro_valueFromStringType(pspec->value_type, content, &value);


    g_print(" %s ", g_value_get_string(&value));

    //set value to the object
    g_object_set_property(child,property,&value);
}

void macro_applyChildProps(GObject *object, GObject *child, xmlNode *node) {
    xmlNode *curNode;
    for (curNode = node->children; curNode; curNode = curNode->next) {
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_PROPERTY)) {
            macro_ApplyObjChildProp(object, child, curNode);
        }
    }
}

void macro_addChild(GObject *object, xmlNode *node) {
    xmlNode *curNode;
    GObject *child;
    for (curNode = node->children; curNode; curNode = curNode->next) {
        //looking for child
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_OBJECT)) {
            child = (GObject *) macro_TransWidget(curNode);
            gtk_container_add(GTK_CONTAINER(object), GTK_WIDGET(child));

        }
        //looking for child properties
        if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_PACKING) && child) {
            macro_applyChildProps(object, child, curNode);
            child = NULL;
        }
    }
}

    void macro_parseChildrenTags(GObject *object, xmlNode *node) {
        g_assert(object != NULL);


        xmlNode *curNode = node->children;
        while (curNode) {
            //TODO :: COMPLETE CODE HERE
            // handle all tags

            if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_PROPERTY))
                macro_ApplyObjProp(object, curNode);
            else if (!xmlStrcasecmp(curNode->name, (xmlChar *) TAG_CHILD))
                macro_addChild(object, curNode);

            curNode = curNode->next;
        }

    }


    GtkWidget *macro_TransWidget(xmlNode *node) {
        xmlNode *curNode;
        curNode = node;
        GObject *widget = NULL;

        //NULL NODE
        if (!curNode) {
            fprintf(stderr, "NULL NODE\n");
            exit(-1);
        }

        //ignoring none object tags
        while (curNode && xmlStrcasecmp(curNode->name, (const xmlChar *) TAG_OBJECT)) {
            curNode = curNode->next;
        }
        //NO OBJECT NODE FOUNT
        if (!curNode) {
            fprintf(stderr, "NO OBJECT TAG FOUND\n");
            exit(-1);
        }

        widget = macro_TransObjAttrs(curNode);

        macro_parseChildrenTags(widget, curNode);


        return GTK_WIDGET(widget);
    }


    GtkWidget *macro_getWindow(gchar *filename) {
        xmlNode *root;
        root = macro_getRootElem(filename);



        /**
         * BUILD Window from xml
         */
        GtkWidget *window;
        window = macro_TransWidget(root->children);


        macro_cleanupXML(root->doc);
        return window;
    }




/**
 * Simple example to parse a file called "file.xml",
 * walk down the DOM, and print the name of the
 * xml elements nodes.
 */
//int
//main(int argc, char **argv)
//{
//    xmlDoc *doc = NULL;
//    xmlNode *root_element = NULL;
//
//    gchar *filename = "assets/main project.glade";
//
//
//
//    /*parse the file and get the DOM */
//    doc = xmlReadFile(filename, NULL, 0);
//
//    if (doc == NULL) {
//        printf("error: could not parse file %s\n", filename);
//    }
//
//    /*Get the root element node */
//    root_element = xmlDocGetRootElement(doc);
//
//
//
//    //TODO :: Traitement HERE
//    print_element_names(root_element);
//
//
//
//
//    /*free the document */
//    xmlFreeDoc(doc);
//
//    /*
//     *Free the global variables that may
//     *have been allocated by the parser.
//     */
//    xmlCleanupParser();
//
//    return 0;
//}


#endif //PARSEGTKXML_PARSERS_H
