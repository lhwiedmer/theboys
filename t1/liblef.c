#include <stdlib.h>
#include <string.h>
#include "liblef.h"

lef_t *cria_lef()
{
    lef_t *lef = malloc(sizeof(lef_t));
    if (!lef)
        return NULL;
    lef->Primeiro = NULL;
    return lef;
}

lef_t *destroi_lef(lef_t *l)
{
    nodo_lef_t *aux;
    while (l->Primeiro != NULL)
    {
        aux = l->Primeiro;
        l->Primeiro = l->Primeiro->prox;
        free(aux->evento);
        free(aux);
    }
    free(l);
    return NULL;
}

int adiciona_inicio_lef(lef_t *l, evento_t *evento)
{
    evento_t *evento_novo = malloc(sizeof(evento_t));
    if (!evento_novo)
        return 0;
    nodo_lef_t *nodo = malloc(sizeof(nodo_lef_t));
    if (!nodo)
    {
        free(evento_novo);
        return 0;
    }
    nodo->evento = evento_novo;
    memcpy(evento_novo, evento, sizeof(evento_t));
    if (!l->Primeiro)
    {
        l->Primeiro = nodo;
        nodo->prox = NULL;
        return 1;
    }
    nodo->prox = l->Primeiro;
    l->Primeiro = nodo;
    return 1;
}

nodo_lef_t *encontra_ordem(lef_t *l, int tempo)
{
    nodo_lef_t *aux;
    aux = l->Primeiro;
    while ((aux->prox != NULL) && (aux->prox->evento->tempo < tempo))
        aux = aux->prox;
    return aux;
}

int adiciona_ordem_lef(lef_t *l, evento_t *evento)
{
    nodo_lef_t *novo, *nodo;
    evento_t *eve;
    novo = malloc(sizeof(nodo_lef_t));
    if (!novo)
        return 0;
    eve = malloc(sizeof(evento_t));
    if (!eve)
        return 0;
    if ((l->Primeiro == NULL) || (evento->tempo < l->Primeiro->evento->tempo))
    {
        novo->prox = l->Primeiro;
        l->Primeiro = novo;
    }
    else
    {
        nodo = encontra_ordem(l, evento->tempo);
        novo->prox = nodo->prox;
        nodo->prox = novo;
    }
    novo->evento = eve;
    memcpy(novo->evento, evento, sizeof(evento_t));
    return 1;
}

evento_t *obtem_primeiro_lef(lef_t *l)
{
    nodo_lef_t *aux;
    evento_t *evento;
    if (l->Primeiro == NULL)
        return NULL;
    aux = l->Primeiro;
    l->Primeiro = l->Primeiro->prox;
    evento = aux->evento;
    free(aux);
    return evento;
}
