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



#ifndef PARSEGTKXML_PARSERS_H
#define PARSEGTKXML_PARSERS_H






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
print_element_names(xmlNode * a_node)
{

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


    xmlChar *type = xmlGetProp(node, (const xmlChar *)ATTR_CLASS);

    printf("\n object name : %s\n", type);
    widget = g_object_new(g_type_from_name((const gchar*)type), NULL);

    /*** TODO :: PARSE THE REST OF ATTRIBUTES IF EXISTS
     xmlAttr *curAttr = node->properties;

     while(curAttr && !xmlStrcasecmp(curAttr->name, (const xmlChar *)ATTR_CLASS)) {


         curAttr = curAttr->next;
     }*/
    return widget;
}

void macro_ApplyObjProp(GObject *object, xmlNode *node) {
    GValue value = G_VALUE_INIT;

    const gchar *property = (gchar *)xmlGetProp(node,(const xmlChar *)ATTR_NAME);


    const gchar *content = (gchar *)xmlNodeGetContent(node);
    g_object_get_property(object,property,&value);
    macro_valueFromStringType(value.g_type, content, &value);
    g_object_set_property(object, property,  &value);
}

void macro_parseChildrenTags(GObject *object, xmlNode *node) {
    g_assert(object != NULL);


    xmlNode *curNode = node->children;
    while (curNode) {
        //TODO :: COMPLETE CODE HERE
        // SET PROPERTY DYNAMICALLY WITHOUT KNOWING THE GTYPE

        if (!xmlStrcasecmp(curNode->name, (xmlChar *)TAG_PROPERTY))
            macro_ApplyObjProp(object, node);
        else if (!xmlStrcasecmp(curNode->name, (xmlChar *)TAGCH))



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
