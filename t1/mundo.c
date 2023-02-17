#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "liblef.h"
#include "libfila.h"
#include "libconjunto.h"

#define TEMPO_INICIAL 0
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS N_HABILIDADES * 5
#define N_LOCAIS N_HEROIS / 6
#define LOT_MAX_MIN 5
#define LOT_MAX_MAX 30
#define CHEGADA_MAX 96 * 7
#define MAX_HABILIDADES 6
#define MAX_LOT 30
#define FIM_DO_MUNDO 34944
#define N_MISSOES FIM_DO_MUNDO / 100
#define CHEGADA 1
#define SAIDA 2
#define MISSAO 3
#define FIM 4
#define IDADE_MIN 18
#define IDADE_MAX 100
#define PACIENCIA_MIN 0
#define PACIENCIA_MAX 100
#define MIN_MISSAO 3
#define MAX_MISSAO 6
#define ORDEM 1
#define INICIO 0

typedef struct coordenada
{
    int x;
    int y;
} coord_t;

typedef struct heroi
{
    int id;           /* Inteiro que representa o heroi. */
    int paciencia;    /* Inteiro que representa a paciência do heroi. */
    int idade;        /* Inteiro que representa a idade do heoi.*/
    int xp;           /* Numero de missões completas pelo heroi. */
    conjunto_t *habs; /* Conjunto de habilidades do heroi. */
} heroi_t;

typedef struct local
{
    int id;              /* Inteiro que representa o local. */
    int lot_max;         /* Número máximo de herois que podem estar ao mesmo tempo no local. */
    conjunto_t *equipe;  /* Conjunto de todos os herois do local. */
    fila_t *fila;        /* Fila de herois que estão esperando para entrar no local. */
    coord_t localizacao; /* Coordenas do local no plano cartesiano. */
} local_t;

typedef struct mundo
{
    int tempo_atual;
    conjunto_t *habilidades;  /* Conjunto com todas as habilidades do mundo */
    int n_herois;             /* Numero de herois do mundo. */
    int n_locais;             /* Numero de locais do mundo*/
    int n_tam_mundo;          /* Coordenadas maximas dos locais no plano cartesiano. */
    heroi_t herois[N_HEROIS]; /* Vetor com todos os herois do mundo. */
    local_t locais[N_LOCAIS]; /* Vetor com todos os locais do mundo. */
} mundo_t;

/* Retorna o maior inteiro entre a e b. */
int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

/* Gera um número aleatório entre 0 e b - a e
 * soma ele com a para alcançar a faixa desejada. */
int aleat(int a, int b)
{
    return (rand() % (b - a)) + a;
}

/* Retorna o tempo de permanencia de um heroi num local,
 * sendo esse tempo no minimo 1. */
int tempo_de_permanencia(int paciencia)
{
    int tpl = paciencia / 10 + aleat(-2, 6);
    tpl = max(1, tpl);
    return tpl;
}

/* Retorna a distancia entre dois pontos utilizando a equacao 
 * Dab = sqrt((xb-xa)²+(yb-ya)²). */
int distancia_pontos(coord_t a, coord_t b)
{
    coord_t aux;
    double distancia;
    aux.x = b.x - a.x;
    aux.y = b.y - a.y;
    distancia = sqrt((aux.x * aux.x) + (aux.y * aux.y));
    return ceil(distancia);
}

/* Retorna o tempo de deslocamento de um heroi entre um local de origem
 * e um local de destino. */
int tempo_de_deslocamento(heroi_t heroi, local_t local_orgn, local_t local_dstn)
{
    int v, d, aux, tdl;
    aux = heroi.idade - 40;
    aux = max(0, aux);
    v = 100 - aux;
    d = distancia_pontos(local_orgn.localizacao, local_dstn.localizacao);
    /* Tempo = d/v. aux nao pode ser menor do que 1. */
    tdl = d / v;
    return tdl / 15;
}

/* Cria e retorna um evento. */
evento_t cria_evento(int tempo, int tipo, int dado1, int dado2)
{
    evento_t evento;
    evento.tempo = tempo;
    evento.tipo = tipo;
    evento.dado1 = dado1;
    evento.dado2 = dado2;
    return evento;
}

/* Cria evento de chegada e coloca na lef em ordem caso n == ORDEM,
 * e no inicio caso n == INICIO. */
void cria_chegada(heroi_t heroi, local_t local, lef_t *l, int tempo, int n)
{
    if (n == ORDEM)
    {
        evento_t evento = cria_evento(tempo, CHEGADA, heroi.id, local.id);
        adiciona_ordem_lef(l, &evento);
    }
    else
    {
        evento_t evento = cria_evento(tempo, CHEGADA, heroi.id, local.id);
        adiciona_inicio_lef(l, &evento);
    }
}

/* Cria evento saida e o adiciona em ordem na lef. */
void cria_saida(int id_h, int id_l, int tempo, lef_t *l)
{
    evento_t evento = cria_evento(tempo, SAIDA, id_h, id_l);
    adiciona_ordem_lef(l, &evento);
}

/* Cria evento de missao e coloca na lef em ordem. */
void cria_missao(int id, int tempo, lef_t *l)
{
    evento_t evento = cria_evento(tempo, MISSAO, id, 0);
    adiciona_ordem_lef(l, &evento);
}

/* Cria o evento de fim do mundo e o coloca na lef em ordem. */
void cria_fim(mundo_t mundo, lef_t *l)
{
    evento_t evento = cria_evento(FIM_DO_MUNDO, FIM, 0, 0);
    adiciona_ordem_lef(l, &evento);
}

/* Cria e retorna um heroi.
 * Tambem cria um evento de chegada para esse heroi. */
heroi_t cria_heroi(mundo_t mundo, int id, lef_t *l)
{
    heroi_t heroi;
    heroi.id = id;
    heroi.idade = aleat(IDADE_MIN, IDADE_MAX);
    heroi.paciencia = aleat(PACIENCIA_MIN, PACIENCIA_MAX);
    heroi.habs = cria_subcjt_cjt(mundo.habilidades, MAX_HABILIDADES);
    heroi.xp = 0;
    int id_l = aleat(0, mundo.n_locais - 1);
    local_t local = mundo.locais[id_l];
    cria_chegada(heroi, local, l, aleat(mundo.tempo_atual, CHEGADA_MAX), ORDEM);
    return heroi;
}

/* Cria e retorna um local. */
local_t cria_local(mundo_t mundo, int id, lef_t *l)
{
    local_t local;
    local.id = id;
    local.localizacao.x = aleat(0, N_TAMANHO_MUNDO - 1);
    local.localizacao.y = aleat(0, N_TAMANHO_MUNDO - 1);
    local.lot_max = aleat(LOT_MAX_MIN, LOT_MAX_MAX);
    local.fila = cria_fila();
    local.equipe = cria_cjt(local.lot_max);
    return local;
}

/* Cria o mundo. */
mundo_t cria_mundo(mundo_t mundo, lef_t *l)
{
    int i;
    mundo.tempo_atual = TEMPO_INICIAL;
    mundo.n_tam_mundo = N_TAMANHO_MUNDO;
    mundo.n_locais = N_LOCAIS;
    mundo.n_herois = N_HEROIS;
    mundo.habilidades = cria_cjt(N_HABILIDADES);
    /* Cria as habilidades. */
    for (i = 0; i < N_HABILIDADES; i++)
        insere_cjt((mundo.habilidades), i);
    /* Cria os locais. */
    for (i = 0; i < N_LOCAIS; i++)
        (mundo.locais)[i] = cria_local(mundo, i, l);
    /* Cria os herois. */
    for (i = 0; i < N_HEROIS; i++)
        (mundo.herois)[i] = cria_heroi(mundo, i, l);
    /* Cria as missoes. */
    for (i = 0; i < N_MISSOES; i++)
        cria_missao(i, aleat(mundo.tempo_atual, FIM_DO_MUNDO), l);
    cria_fim(mundo, l);
    return mundo;
}

/* Retorna 1 caso o local esteja lotado, e 0 caso não esteja. */
int esta_lotado(local_t local)
{
    int card = cardinalidade_cjt(local.equipe);
    if (local.lot_max == card)
        return 1;
    return 0;
}

/* Retorna 1 se o herói quiser esperar na fila do local, e 0 caso não queira. */
int quer_ficar(local_t local, heroi_t heroi)
{
    if ((heroi.paciencia/4 - tamanho_fila(local.fila)) > 0)
        return 1;
    return 0;
}

/* Trata a saida de um heroi que nao quer esperar na fila do local que ele
 * acabou de chegar. */
void saida_desistente(mundo_t mundo, heroi_t heroi, local_t local, lef_t *l)
{
    int id_l = aleat(0, mundo.n_locais - 1);
    local_t local_dst = mundo.locais[id_l];
    int tdl = tempo_de_deslocamento(heroi, local, local_dst);
    cria_chegada(heroi, local_dst, l, mundo.tempo_atual + tdl, ORDEM);
}

/* Trata o evento de chegada. */
void trata_chegada(mundo_t mundo, evento_t *evento, lef_t *l)
{
    int id_h = evento->dado1;
    int id_l = evento->dado2;
    heroi_t heroi = (mundo.herois[id_h]);
    local_t local = (mundo.locais[id_l]);
    int tempo = mundo.tempo_atual;
    int card = cardinalidade_cjt(local.equipe);
    int lot = local.lot_max;
    int tpl;
    printf("%6d:CHEGA HEROI %2d Local %d (%2d/%2d),", tempo, id_h, id_l, card, lot);
    if (esta_lotado(local))
    {
        if (quer_ficar(local, heroi))
        {
            insere_fila(local.fila, id_h);
            printf(" FILA %2d\n", local.fila->tamanho);
        }
        else
        {
            saida_desistente(mundo, heroi, local, l);
            printf(" DESISTE\n");
        }
    }
    else
    {
        tpl = tempo_de_permanencia(heroi.paciencia);
        insere_cjt(local.equipe, heroi.id);
        printf(" ENTRA\n");
        cria_saida(id_h, id_l, tempo + tpl, l);
    }
}

/* Trata os processos que ocorrem após a saída de um heroi de um local. */
void pos_saida(mundo_t mundo, heroi_t heroi, local_t local, lef_t *l)
{
    int tdl;
    local_t local_dst = mundo.locais[aleat(0, mundo.n_locais - 1)];
    tdl = tempo_de_deslocamento(heroi, local, local_dst);
    cria_chegada(heroi, local, l, mundo.tempo_atual + tdl, ORDEM);
}

/* Trata o evento de saida. */
void trata_saida(mundo_t mundo, evento_t *evento, lef_t *l)
{
    int id_h = evento->dado1;
    int id_l = evento->dado2;
    heroi_t heroi = (mundo.herois[id_h]);
    local_t local = (mundo.locais[id_l]);
    int tempo = mundo.tempo_atual;
    int lot = local.lot_max;
    int id_h_etr;
    retira_cjt(local.equipe, id_h);
    int card = cardinalidade_cjt(local.equipe);
    printf("%6d:SAIDA HEROI %2d Local %d ", tempo, id_h, id_l);
    /* Se houver algum heroi na fila, coloca o heroi
     * que estiver na primeira posicao dela dentro do local. */
    if (!vazia_fila(local.fila))
    {
        retira_fila(local.fila, &id_h_etr);
        heroi_t heroi_etr = mundo.herois[id_h_etr];
        printf("(%2d/%2d), REMOVE FILA HEROI %2d", card, lot, id_h_etr);
        cria_chegada(heroi_etr, local, l, mundo.tempo_atual, INICIO);
    }
    else
        printf("(%2d/%2d)", card, lot);
    printf("\n");
    pos_saida(mundo, heroi, local, l);
}

/* Retorna 1 se a equipe estiver apta para realizar a missao,
 * e 0 se nao estiver. */
int esta_apta(conjunto_t *missao, conjunto_t *equipe, mundo_t mundo)
{
    conjunto_t *habilidades;
    heroi_t heroi;
    int i, j, aptidao, id_h;
    habilidades = cria_cjt(N_HABILIDADES);
    /* Passa por todos os herois da equipe. */
    for (i = 0; i < equipe->card; i++)
    {
        id_h = equipe->v[i];
        heroi = mundo.herois[id_h];
        /* Passa por todas habilidades do heroi. */
        for (j = 0; j < cardinalidade_cjt(heroi.habs); j++)
            insere_cjt(habilidades, heroi.habs->v[j]);
    }
    imprime_cjt(habilidades);
    aptidao = contido_cjt(missao, habilidades);
    destroi_cjt(habilidades);
    return aptidao;
}

/* Aumenta em 1 o xp de todos os herois que participaram da missao. */
void atualiza_xp(int id_l, mundo_t *mundo)
{
    int i, id_h;
    conjunto_t *equipe;
    equipe = mundo->locais[id_l].equipe;
    /* Passa por todos os herois da equipe. */
    for (i = 0; i < cardinalidade_cjt(equipe); i++)
    {
        id_h = equipe->v[i];
        mundo->herois[id_h].xp++;
    }
}

/* Decide se a equipe que esta sendo sendo testada eh melhor do que
 * a melhor equipe atual. Retorna 1 se for, e 0 se nao for. */
int eh_melhor(conjunto_t *missao, mundo_t mundo, int i, int tam, int card)
{
    if (esta_apta(missao, mundo.locais[i].equipe, mundo) && (card < tam))
        return 1;
    return 0;
}

/* Retorna o id da menor equipe apta, e o tamanho dela pelo parâmetro *tam. */
int encontra_menor(conjunto_t *missao, mundo_t mundo, int *passou, evento_t *evento)
{
    int menor, i, card, tam;
    tam = mundo.n_herois;
    printf("%6d:MISSAO %3d HAB_REQ  ", mundo.tempo_atual, evento->dado1);
    imprime_cjt(missao);
    /* Passa por todos os locais do mundo. */
    for (i = 0; i < mundo.n_locais; i++)
    {
        printf("%6d:MISSAO %3d HAB_EQL %2d:", mundo.tempo_atual, evento->dado1, i);
        card = cardinalidade_cjt(mundo.locais[i].equipe);
        if (eh_melhor(missao, mundo, i, tam, card))
        {
            tam = card;
            menor = i;
            *passou = 1;
        }
    }
    return menor;
}

/* Caso a missao tenha sido completa, atualiza o xp dos herois
 * que participaram dela, se nao foi, cria uma nova missao. */
void pos_missao(mundo_t *mundo, evento_t *evento, int menor, int passou, lef_t *l)
{
    printf("%6d:MISSAO %3d ", mundo->tempo_atual, evento->dado1);
    if (passou)
    {
        atualiza_xp(menor, mundo);
        printf("HER_EQS %2d:", menor);
        imprime_cjt(mundo->locais[menor].equipe);
        return;
    }
    cria_missao(aleat(mundo->tempo_atual, FIM_DO_MUNDO), evento->dado1, l);
    printf("IMPOSSIVEL\n");
}

/* Trata o evento de missao. */
void trata_missao(evento_t *evento, mundo_t *mundo, lef_t *l)
{
    conjunto_t *missao;
    int passou, menor;
    passou = 0;
    /* Cria o conjunto de habilidades da missao. */
    missao = cria_subcjt_cjt(mundo->habilidades, aleat(MIN_MISSAO, MAX_MISSAO));
    menor = encontra_menor(missao, *mundo, &passou, evento);
    pos_missao(mundo, evento, menor, passou, l);
    destroi_cjt(missao);
}

/* Destroi o mundo liberando toda a memoria alocada dinamicamente. */
void fim_do_mundo(mundo_t *mundo)
{
    int i;
    local_t local;
    heroi_t heroi;
    /* Destroi os locais. */
    for (i = 0; i < mundo->n_locais; i++)
    {
        local = mundo->locais[i];
        destroi_fila(local.fila);
        destroi_cjt(local.equipe);
    }
    /* Destroi os herois. */
    for (i = 0; i < mundo->n_herois; i++)
    {
        heroi = mundo->herois[i];
        destroi_cjt(heroi.habs);
    }
    destroi_cjt(mundo->habilidades);
}

/* Imprime o xp e todos os herois do mundo. */
void imprime_xp(mundo_t mundo)
{
    int i, xp;
    heroi_t heroi;
    /* Passa por todos os herois do mundo. */
    for (i = 0; i < mundo.n_herois; i++)
    {
        heroi = mundo.herois[i];
        xp = heroi.xp;
        printf("HEROI %2d EXPERIENCIA %3d\n", i, xp);
    }
}

int main()
{
    mundo_t mundo;
    lef_t *l_eve;
    evento_t *evento;
    int nao_acabou = 1;
    l_eve = cria_lef();
    srand(0);
    mundo = cria_mundo(mundo, l_eve);
    /* Retira eventos da l_eve enquanto o mundo nao acabar. */
    while (nao_acabou)
    {
        evento = obtem_primeiro_lef(l_eve);
        mundo.tempo_atual = evento->tempo;
        switch (evento->tipo)
        {
        case CHEGADA:
            trata_chegada(mundo, evento, l_eve);
            break;
        case SAIDA:
            trata_saida(mundo, evento, l_eve);
            break;
        case MISSAO:
            trata_missao(evento, &mundo, l_eve);
            break;
        case FIM:
            printf("%6d: FIM\n", evento->tempo);
            nao_acabou = 0;
        default:
            break;
        }
        free(evento);
    }
    imprime_xp(mundo);
    fim_do_mundo(&mundo);
    destroi_lef(l_eve);
    return 0;
}