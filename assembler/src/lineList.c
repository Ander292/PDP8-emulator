#include "instr.h"
#include "lineList.h"
#include "assembler.h"

#include <stdio.h>
#include <stdlib.h>

lineList listCreate(){
    lineList result;
    result.head = NULL;
    result.size = 0;
    result.tail = NULL;

    return result;
}

pNode getnode(){
    pNode result = malloc(sizeof(node));
    result->next = NULL;
    result->prev = NULL;

    return result;
}

void freenode(pNode p){
    free(p);
}

void insertEndOg(lineList *l, lineOg og){
    pNode q = getnode();
    q->type = TYPE_OG;
    l->size++;
    q->orig = og;
    if(l->head == NULL || l->tail == NULL){
        l->head = q;
        l->tail = q;
    }
    else{
        l->tail->next = q;
        q->prev = l->tail;
        l->tail = q;
    }
}

void insertEndT(lineList *l, lineT lt){
    pNode q = getnode();
    q->type = TYPE_T;
    q->transl = lt;
    l->size++;
    if(l->head == NULL || l->tail == NULL){
        l->head = q;
        l->tail = q;
    }
    else{
        l->tail->next = q;
        q->prev = l->tail;
        l->tail = q;
    }
}

lineOg deleteFrontOg(lineList *l){
    if(l->head == NULL) return (lineOg){0};
    pNode p = l->head;
    l->size--;

    if((void *)(l->head->next) == NULL){
        l->head = NULL;
        l->tail = NULL;
    } else{
        l->head->next->prev = NULL;
        l->head = l->head->next;
    }

    lineOg result = p->orig;
    freenode(p);
    return result;
}

lineT deleteFrontT(lineList *l){
    if(l->head == NULL) return (lineT){.type = TRANSLATION_SKIP, 0};
    pNode p = l->head;
    l->size--;

    if(l->head->next == NULL){
        l->head = NULL;
        l->tail = NULL;
    }
    else{
        l->head->next->prev = NULL;
        l->head = l->head->next;
    }

    lineT result = p->transl;
    freenode(p);
    return result;
}

void printList(lineList *l){
    unsigned int i;
    pNode p;
    for(i = 0, p = l->head; p != NULL; p = p->next, i++){
        switch(p->type){
            case TYPE_OG:
                printf("lineOg [%u]: %4u | %4s | %4u\n", i, p->orig.address, p->orig.name, p->orig.operand);
                break;
            case TYPE_T:
                printf("lineT  [%u]: %4u | %4u | %5u | %5u\n", i, p->transl.type, p->transl.address, p->transl.opCode, p->transl.operand);
                break;
        }
    }
}

void freeList(lineList *l){
    for(pNode p = l->head; p != NULL;){
        pNode q = p;
        p = p->next;
        freenode(q);
    }
    l->head = NULL;
    l->tail = NULL;
}