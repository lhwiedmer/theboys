#include <stdio.h>
#include <stdlib.h>
#include "libfila.h"

fila_t *cria_fila(){
    fila_t* fila = malloc(sizeof(fila_t));
    if(!fila)
        return NULL;
    fila -> tamanho = 0;
    fila -> ini = NULL;
    fila -> fim = NULL;
    return fila; 
}

void destroi_nodos_fila(nodo_f_t *nodo){
    if(!nodo)
        return;
    destroi_nodos_fila(nodo -> prox);
    free(nodo);
}

fila_t *destroi_fila(fila_t *f){
    if(!f)
        return NULL;
    destroi_nodos_fila(f -> ini);
    free(f);
    return NULL;
}

int fila_vazia(fila_t *f){
    if ((f -> tamanho) <= 0)
        return 1;
    return 0;
}

int tamanho_fila(fila_t *f){
    return f->tamanho;
}

int insere_fila(fila_t *f, int elem){
    nodo_f_t *nodo = malloc(sizeof(nodo_f_t));
    if(!nodo)
        return 0;
    nodo -> chave = elem;
    nodo -> prox = NULL;
    if(fila_vazia(f)){
        f -> ini = nodo;
        f -> fim = nodo;
    }
    else{
        (f -> fim) -> prox = nodo;
        f -> fim = nodo;
    }
    f -> tamanho++;
    return 1;
}

int retira_fila(fila_t *f, int *elem){
    nodo_f_t *pont;
    if(fila_vazia(f))
        return 0;   
    pont = f -> ini;
    f -> ini = pont -> prox;
    *elem = pont -> chave;
    f -> tamanho--;
    free(pont);  
    return 1;
}

void imprime_nodos(nodo_f_t *nodo){
    if(!nodo)
        return;
    printf("%d ", nodo -> chave);
    imprime_nodos(nodo -> prox);
}

void imprime_fila(fila_t *f){
    if(!f -> ini)
        return;
    imprime_nodos(f -> ini);
    printf("\n");
}