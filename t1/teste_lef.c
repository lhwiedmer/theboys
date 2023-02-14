#include <stdio.h>
#include <stdlib.h>
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
    evento = novo_evento;
    if (!l -> Primeiro){
        l -> Primeiro = nodo;
        nodo -> prox = NULL;
        return 1;
    }
    nodo -> prox = l -> Primeiro;
    l -> Primeiro = nodo;
    return 1;
}

void posiciona_ordem(lef_t *l, nodo_lef_t *nodo){
    nodo_lef_t *aux = l -> Primeiro;
    if (aux -> evento -> tempo > nodo -> evento -> tempo){
        nodo -> prox = l -> Primeiro;
        l -> Primeiro = nodo;
        return;
    }
    while (aux -> prox != NULL && aux -> prox -> evento -> tempo < nodo -> evento -> tempo)
        aux = aux -> prox;
    nodo -> prox = aux -> prox;
    aux -> prox = nodo;
}

int adiciona_ordem_lef(lef_t *l, evento_t *evento){
    evento_t *novo_evento = malloc(sizeof(evento_t));
    if(!novo_evento)
        return 0;
    nodo_lef_t *nodo = malloc(sizeof(nodo_lef_t));
    if (!nodo){
        free(novo_evento);
        return 0;
    }
    nodo -> evento = novo_evento;
    evento = novo_evento;
    if (!l -> Primeiro){
        l -> Primeiro = nodo;
        nodo -> prox = NULL;
        return 1;
    }
    posiciona_ordem(l, nodo);
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

void imprime_nos(nodo_lef_t *no){
    if(!no)
        return;

    printf("%d ", no->evento->tempo);
    imprime_nos(no->prox);
}

void imprime_lef(lef_t *l){
    if(!l)
        return;

    imprime_nos(l->Primeiro);
    printf("\n");
}

int main(){
    int n;
    lef_t *l = cria_lef();
    evento_t evento;
    evento_t *ev;

    evento.dado1 = 0;
    evento.dado2 = 0;
    evento.tipo = 0;
    printf("Opcoes:\n 1-Evento Ordem.\n 2-Evento inicio.\n 3-Tira primeiro no.\n 4-imprime\n 0-Sair\n");
    scanf("%d", &n);

    while (1){
        printf("Tempo do evento:\n");
        switch (n){
            case 1:
                scanf("%d", &evento.tempo);
                adiciona_ordem_lef(l, &evento);
                imprime_lef(l);
                break;
            case 2:
                scanf("%d", &evento.tempo);
                adiciona_inicio_lef(l, &evento);
                imprime_lef(l);
                break;
            case 3:
                ev = obtem_primeiro_lef(l);
                if (ev != NULL){
                    printf ("%d\n", ev->tempo);
                    free(ev);
                }
                break;
            case 4:
                printf ("\n");
                imprime_lef(l);
                printf ("\n");
                break;
            case 0:
                destroi_lef(l);
                return 0;
                break;
            default:
                break;
        }
        printf("Opcoes:\n 1-Evento Ordem.\n 2-Evento inicio.\n 3-Tira primeiro no.\n 4-imprime\n 0-Sair\n");
        scanf("%d", &n);
    }
    destroi_lef(l);
    return 0;
}
