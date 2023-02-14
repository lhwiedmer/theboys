#include <stdio.h>
#include <stdlib.h>
#include "libconjunto.h"

conjunto_t *cria_cjt(int max){
    conjunto_t	*c = malloc(sizeof(conjunto_t));
    if (c == NULL)
        return NULL;
    c->v = malloc(max * sizeof(int));
    c->max = max;
    c->card = 0;
    return c;
}

conjunto_t *destroi_cjt(conjunto_t *c){
    free(c->v);
    free(c);
    return NULL;
}

int busca_binaria(conjunto_t *c, int elemento){
    int ini = 0;
    int fim = c -> card - 1;
    int meio = (ini + fim) / 2;
    while ((ini <= fim) && (elemento != *(c->v + meio))){
        if (elemento < *(c->v + meio))
            fim = meio - 1;
        else
            ini = meio + 1;
        meio = (ini + fim) / 2;
    }
    if (fim < ini)
        return -1;
    return meio;
}

int vazio_cjt(conjunto_t *c){
    if ((c -> card) == 0)
        return 1;
    return 0;
}

int cardinalidade_cjt(conjunto_t *c){
    return c -> card;
}

int insere_cjt(conjunto_t *c, int elemento){
    if(c -> card + 1 <= c->max){
		if(!(pertence_cjt(c, elemento))){
			c -> card++;
			int  i;
			i = c -> card - 1;
			while((i > 0) && (elemento < *(c-> v + i - 1))){
				*(c->v+i) = *(c->v+i-1);
				i = i - 1;
			}
			*(c-> v + i) = elemento;
		}
		return 1;
	}
	return 0;
}


int retira_cjt(conjunto_t * c, int elemento){
    int i, indice;
    indice = busca_binaria(c, elemento);
    if (indice != -1){
        for (i = indice; i <= (c -> card) - 2; i++)
            *(c->v + i) = *(c->v + i + 1);
        c -> card = c -> card - 1;
        return 1;
    }
    return 0;
}

int pertence_cjt(conjunto_t * c, int elemento){
    if ((busca_binaria(c, elemento) != -1))
        return 1;
    return 0;
}

int contido_cjt(conjunto_t * c1, conjunto_t * c2){
    int i = 0;
    while (i < c1->card) {
        if (!pertence_cjt(c2, c1->v[i]))
            return 0;
        i++;
    }
    return 1;
}

int sao_iguais_cjt(conjunto_t * c1, conjunto_t * c2){
    int i;
    if ((c1 -> card) != (c2 -> card))
        return 0;
    i = 0;
    while ((i <= c1 -> card - 1) && (*(c1->v + i) = *(c2->v + i)))
        i = i + 1;
    if (i <= c1 -> card - 1)
        return 0;
    return 1;
}

conjunto_t *diferenca_cjt(conjunto_t * c1, conjunto_t * c2){
    conjunto_t *dif;
    int i;
    dif = copia_cjt(c1);
    for (i = 0; i <= (c2 -> card - 1); i++)
        retira_cjt(dif, (*(c2 -> v + i)));
    return dif;
}
        

conjunto_t *interseccao_cjt(conjunto_t * c1, conjunto_t * c2){
    int i, j, max;
    conjunto_t *inter;
    max = (c1 -> card) + (c2 -> card);
    if (!(inter = cria_cjt(max)))
        return NULL;
    for (i = 0; i <= c1 -> card -1; i++){
        for (j = 0; j <= c2 -> card -1; j++){
            if ((*(c1 -> v + i)) == (*(c2 -> v + j)))
                insere_cjt(inter, (*(c1 -> v + i)));
        }
    }
    return inter;
}

conjunto_t *uniao_cjt(conjunto_t * c1, conjunto_t * c2){
    int i;
    conjunto_t *uni;
    int max = 400;
    if (!(uni = cria_cjt(max)))
        return NULL;
    for (i = 0; i <= c1 -> card - 1; i++)
        insere_cjt(uni, (*(c1 -> v + i)));
    for (i = 0; i <= 7; i++)
        insere_cjt(uni, (*(c2 -> v + i)));
    return uni;
}

conjunto_t *copia_cjt(conjunto_t * c){
    conjunto_t *cop;
    int max, i;
    max = c->max;
    cop = cria_cjt(max);
    if ((cop) == NULL)
    {
        return NULL;
    }
    cop -> card = c -> card;
    for (i = 0; i <= c -> card - 1; i++)
    {
        *(cop->v + i) = *(c->v + i);
    }
    return cop;
}

conjunto_t *cria_subcjt_cjt (conjunto_t *c, int n) {
    if (n >= c->card)
        return copia_cjt(c);

    int i = 0;
    int indice;
    conjunto_t *sub_cjt = cria_cjt(c->max);
    if (!sub_cjt)
        return NULL;

    while (i < n) {
        indice = rand()% c->card;
        if ( !pertence_cjt(sub_cjt, c->v[indice]) ){
            insere_cjt(sub_cjt, c->v[indice]);
            i++;
        }
    }

    return sub_cjt;
}

void imprime_cjt(conjunto_t * c){
    int i;
    if (c -> card > 0){
        for (i = 0; i < c -> card - 1; i++)
            printf("%d ", *(c->v + i));
        printf("%d\n", *(c-> v + i));
    }
    else printf("conjunto vazio\n");
}

void inicia_iterador_cjt(conjunto_t * c){
    c->ptr = 0;
}

int incrementa_iterador_cjt(conjunto_t * c, int *ret_iterador){
    if (c -> ptr < c -> card){
        *ret_iterador = *(c -> v + c -> ptr);
        c -> ptr++;
        return 1;
    }
    return 0;
}

int retira_um_elemento_cjt(conjunto_t * c){
    int retirado = *(c->v + (c -> card - 1));
    c -> card = c -> card - 1;
    return retirado;
}