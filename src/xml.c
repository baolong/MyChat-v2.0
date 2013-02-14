#include <stdio.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


xmlNodePtr xmlParseTostring(xmlDocPtr doc, xmlNodePtr cur, 

/***
 *
 * 获取cur节点中名字为name的子节点的值
 *
 * **/
void ParseSibling(xmlDocPtr doc, xmlNodePtr cur, char *name, char *value)
{
    if (cur == NULL)
        return;
    cur = cur->xmlChildrenNode;
    while(cur != NULL){
        if ((!xmlStrcmp(cur->name, (const xmlChar*)name))){
            value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 2);
            return;
        }
        cur = cur->next;
    }
    return;
}

/***
 *
 * 往cur节点插入名字为name、值为value的子节点
 *
 * **/
void InsertNode(xmlDocPtr doc, xmlNodePtr cur, char* name, char* value)
{
    xmlNewTextChild(cur, NULL, name, value);
    return;
}


/***
 *
 * 寻找node节点下一个名字为name的节点
 *
 * **/
xmlNodePtr FindNextNode(xmlNodePtr node, const char* name){
    if (!node){
        return NULL;
    }
    while(NULL != node){
        if (!xmlStrcmp(node->name,(const xmlChar *)name)){
            return node;
        }
        node = node->next;
    }
    return NULL;
}

/***
 *
 * 寻找root节点名字为name的子节点
 *
 * **/
xmlNodePtr FindChildNode(xmlNodePtr root, const char* name){
    if (!root){
        return NULL;
    }
    xmlNodePtr cur = root->children;
    while(NULL != cur){
        if (!xmlStrcmp(cur->name,(const xmlChar *)name)){
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}





