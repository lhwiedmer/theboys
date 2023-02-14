#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liblef.h"

lef_t *cria_lef(){
    lef_t *l = malloc(sizeof(lef_t));
    if (!l)
        return NULL;
    l -> Primeiro = NULL;
    return l;
}

void destroi_nodos_lef(nodo_lef_t *nodo){
    if (nodo -> prox != NULL){
        destroi_nodos_lef(nodo -> prox);
    }
    free(nodo -> evento);
    free(nodo);
}

lef_t *destroi_lef (lef_t *l){
    if (!l)
        return NULL;
    if (l -> Primeiro != NULL){
        destroi_nodos_lef(l -> Primeiro);
    }
    free(l); 
    return NULL;
}

int adiciona_inicio_lef(lef_t *l, evento_t *evento){
    evento_t *novo_evento = malloc(sizeof(evento_t));
    if(!novo_evento)
        return 0;
    nodo_lef_t *nodo = malloc(sizeof(nodo_lef_t));
    if (!nodo){
        free(novo_evento);
        return 0;
    }
    nodo -> evento = novo_evento;
    memcpy(novo_evento, evento, sizeof(evento_t));
    if (!l -> Primeiro){
        l -> Primeiro = nodo;
        nodo -> prox = NULL;
        return 1;
    }
    nodo -> prox = l -> Primeiro;
    l -> Primeiro = nodo;
    return 1;
}

nodo_lef_t *encontra_posicao(lef_t *l, int tempo){
    nodo_lef_t *anterior;
    anterior = l -> Primeiro;
    while ((anterior -> prox != NULL) && (anterior -> prox -> evento -> tempo < tempo)){
        anterior = anterior -> prox;
    }
    return anterior;
}

int adiciona_ordem_lef(lef_t *l, evento_t *evento){
    evento_t *novo_evento = malloc(sizeof(evento_t));
    nodo_lef_t *nodo = malloc(sizeof(nodo_lef_t));
    nodo_lef_t *aux;
    if(!novo_evento)
        return 0;
    if (!nodo){
        free(novo_evento);
        return 0;
    }
    if ((l -> Primeiro == NULL)||(evento -> tempo < l -> Primeiro -> evento -> tempo)){
        nodo -> prox = l -> Primeiro;
        l -> Primeiro = nodo;
    }
    else{
        aux = encontra_posicao(l, evento -> tempo);
        nodo -> prox = aux -> prox;
        aux -> prox = nodo;
    }
    nodo -> evento = novo_evento;
    memcpy(nodo -> evento, evento, sizeof(evento_t));
    return 1;
}


evento_t *obtem_primeiro_lef(lef_t *l){
    if (l -> Primeiro == NULL)
        return NULL;
    evento_t *aux = l -> Primeiro->evento;
    nodo_lef_t *nodo = l -> Primeiro;
    l -> Primeiro = nodo -> prox;
    free(nodo);
    return aux;
}