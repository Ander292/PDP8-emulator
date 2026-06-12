#ifndef LINE_LIST_H
#define LINE_LIST_H

    #include "shared.h"
    #include "assembler.h"

    #define TYPE_OG 1
    #define TYPE_T 2

    typedef struct node 
        node, *pNode;
    struct node{
        int type;
        union{
            lineOg orig;
            lineT transl;
        };
        pNode next;
        pNode prev;
    };

    typedef struct lineList{
        pNode head;
        pNode tail;
        size_t size;
    } lineList;

    lineT deleteFrontT(lineList *l);
    lineOg deleteFrontOg(lineList *l);
    void insertEndT(lineList *l, lineT lt);
    void insertEndOg(lineList *l, lineOg og);
    lineList listCreate();
    void printList(lineList *l);
    void freeList(lineList *l);
    
#endif