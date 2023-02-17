#include <stdio.h>
#include <stdlib.h>
#include "libfila.h"

fila_t *cria_fila()
{
    fila_t *fila = malloc(sizeof(fila_t));
    if (!fila)
        return NULL;
    fila->tamanho = 0;
    fila->ini = NULL;
    fila->fim = NULL;
    return fila;
}

fila_t *destroi_fila(fila_t *f)
{
    nodo_f_t *nodo;
    if (vazia_fila(f))
    {
        free(f);
        return NULL;
    }
    nodo = f->ini;
    while (nodo->prox != NULL)
    {
        nodo = nodo->prox;
        free(f->ini);
        f->ini = nodo;
    }
    free(f->fim);
    free(f);
    return NULL;
}

int vazia_fila(fila_t *f)
{
    if (tamanho_fila(f) == 0)
        return 1;
    return 0;
}

int tamanho_fila(fila_t *f)
{
    return f->tamanho;
}

int insere_fila(fila_t *f, int elemento)
{
    nodo_f_t *nodo = malloc(sizeof(nodo_f_t));
    if (!nodo)
        return 0;
    nodo->elem = elemento;
    nodo->prox = NULL;
    if (vazia_fila(f))
    {
        f->ini = nodo;
        f->fim = nodo;
    }
    else
    {
        (f->fim)->prox = nodo;
        f->fim = nodo;
    }
    f->tamanho++;
    return 1;
}

int retira_fila(fila_t *f, int *elemento)
{
    nodo_f_t *nodo;
    if (vazia_fila(f))
        return 0;
    nodo = f->ini;
    f->ini = nodo->prox;
    *elemento = nodo->elem;
    f->tamanho--;
    free(nodo);
    return 1;
}
