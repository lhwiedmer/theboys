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

typedef struct coordenada
{
    int x;
    int y;
} coord_t;

typedef struct heroi_aux
{
    int id;        /*Inteiro que representa o heroi_aux.*/
    int paciencia; /*Inteiro que representa a paciência do heroi_aux.*/
    int idade;
    int xp; /*Numero de missões completas pelo heroi_aux.*/
    conjunto_t *habs;
} heroi_t;

typedef struct local
{
    int id;              /*Inteiro que representa o local.*/
    int lot_max;         /*Número máximo de herois que podem estar ao mesmo tempo no local.*/
    conjunto_t *equipe;  /*Conjunto de todos os herois do local.*/
    fila_t *fila;        /*Fila de herois que estão esperando para entrar no local.*/
    coord_t localizacao; /*Coordenas do local no plano cartesiano.*/
} local_t;

typedef struct mundo
{
    int tempo_atual;
    conjunto_t *habilidades;
    int n_herois;
    int n_locais;
    int n_tam_mundo; /*Coordenadas maximas dos locais no plano cartesiano.*/
    heroi_t herois[N_HEROIS];
    local_t locais[N_LOCAIS];
} mundo_t;

/*Gera um número aleatório entre 0 e b + 1 - a e
 *soma ele com a para alcançar a faixa desejada.*/
int aleat(int a, int b)
{
    return rand() % (b + 1 - a) + a;
}

/*Retorna o tempo de permanencia de um heroi_aux num local
 *sendo esse tempo no minimo 1.*/
int tempo_de_permanencia(int paciencia)
{
    int tpl = paciencia / 10 + aleat(-2, 6);
    if (tpl < 1)
        return 1;
    return tpl;
}

/*Retorna a distancia entre dois pontos no plano cartesiano.*/
int distancia_pontos(coord_t a, coord_t b)
{
    coord_t cat;
    double distancia;
    cat.x = b.x - a.x;
    cat.y = b.y - a.y;
    distancia = sqrt(pow(cat.x, 2) + pow(cat.y, 2));
    return ceil(distancia);
}

/*Retorna o tempo de deslocamento de um heroi_aux de um local a até um local b.*/
int tempo_de_deslocamento(heroi_t heroi_aux, local_t local_a, local_t local_b)
{
    int v, d, tdl;
    d = distancia_pontos(local_a.localizacao, local_b.localizacao);
    v = heroi_aux.idade - 40;
    if (v < 0)
        v = 0;
    v = 100 - v;
    tdl = d / v;
    return tdl / 15;
}

/*Cria e retorna um evento.*/
evento_t cria_evento(int tempo, int tipo, int dado1, int dado2)
{
    evento_t evento;
    evento.tempo = tempo;
    evento.tipo = tipo;
    evento.dado1 = dado1;
    evento.dado2 = dado2;
    return evento;
}

/*Cria evento de chegada e coloca na lef em ordem caso n == 1,
 *e no inicio caso n == 0.*/
void cria_chegada(heroi_t heroi, local_t local, lef_t *l, int tempo, int n)
{
    if (n == 1)
    {
        evento_t evento = cria_evento(tempo,
                                      CHEGADA, heroi.id, local.id);
        adiciona_ordem_lef(l, &evento);
    }
    else
    {
        evento_t evento = cria_evento(tempo,
                                      CHEGADA, heroi.id, local.id);
        adiciona_inicio_lef(l, &evento);
    }
}

/*Cria evento saida e o adiciona em ordem na lef.*/
void cria_saida(int id_h, int id_l, int tempo, lef_t *l)
{
    evento_t evento = cria_evento(tempo, SAIDA, id_h, id_l);
    adiciona_ordem_lef(l, &evento);
}

/*Cria evento de missao e coloca na lef em ordem.*/
void cria_missao(int id, int tempo, lef_t *l)
{
    evento_t evento = cria_evento(tempo, MISSAO, id, 0);
    adiciona_ordem_lef(l, &evento);
}

/*Cria o evento de fim do mundo e o coloca na lef em ordem.*/
void cria_fim(mundo_t mundo, lef_t *l)
{
    evento_t evento = cria_evento(FIM_DO_MUNDO,
                                  FIM, 0, 0);
    adiciona_ordem_lef(l, &evento);
}

/*Cria e retorna um heroi.*/
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
    cria_chegada(heroi, local, l, aleat(mundo.tempo_atual, CHEGADA_MAX), 1);
    return heroi;
}

/*Cria e retorna um local.*/
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

/*Cria o mundo.*/
mundo_t cria_mundo(mundo_t mundo, lef_t *l)
{
    int i;
    mundo.tempo_atual = TEMPO_INICIAL;
    mundo.n_tam_mundo = N_TAMANHO_MUNDO;
    mundo.n_locais = N_LOCAIS;
    mundo.n_herois = N_HEROIS;
    mundo.habilidades = cria_cjt(N_HABILIDADES);
    for (i = 0; i < N_HABILIDADES; i++)
        insere_cjt((mundo.habilidades), i);
    for (i = 0; i < N_LOCAIS; i++)
        (mundo.locais)[i] = cria_local(mundo, i, l);
    for (i = 0; i < N_HEROIS; i++)
        (mundo.herois)[i] = cria_heroi(mundo, i, l);
    for (i = 0; i < N_MISSOES; i++)
        cria_missao(i, aleat(mundo.tempo_atual, FIM_DO_MUNDO), l);
    cria_fim(mundo, l);
    return mundo;
}

/*Retorna 1 caso o local esteja lotado, e 0 caso não esteja.*/
int esta_lotado(local_t local)
{
    int card = cardinalidade_cjt(local.equipe);
    if (local.lot_max == card)
        return 1;
    return 0;
}

/*Retorna 1 se o herói quiser esperar na fila do local, e 0 caso não queira.*/
int quer_ficar(local_t local, heroi_t heroi)
{
    if ((heroi.paciencia / 4 - tamanho_fila(local.fila)) > 0)
        return 1;
    return 0;
}

/*Trata a saida de um heroi que nao quer esperar na fila do local que ele
 *acabou de chegar.*/
void saida_desistente(mundo_t mundo, heroi_t heroi, local_t local, lef_t *l)
{
    int id_l = aleat(0, mundo.n_locais - 1);
    local_t local_dst = mundo.locais[id_l];
    int tdl = tempo_de_deslocamento(heroi, local, local_dst);
    cria_chegada(heroi, local_dst, l, mundo.tempo_atual + tdl, 1);
}

/*Trata o evento de chegada.*/
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
    printf("%6d:CHEGA HEROI %2d Local %d ", tempo, id_h, id_l);
    if (esta_lotado(local))
    {
        if (quer_ficar(local, heroi))
        {
            insere_fila(local.fila, id_h);
            printf("(%2d/%2d), FILA %2d\n", card, lot, local.fila->tamanho);
        }
        else
        {
            saida_desistente(mundo, heroi, local, l);
            printf("(%2d/%2d), DESISTE\n", card, lot);
        }
    }
    else
    {
        tpl = tempo_de_permanencia(heroi.paciencia);
        insere_cjt(local.equipe, heroi.id);
        card = cardinalidade_cjt(local.equipe);
        printf("(%2d/%2d), ENTRA\n", card, lot);
        cria_saida(id_h, id_l, tempo + tpl, l);
    }
}

/*Trata o evento de saida.*/
void trata_saida(mundo_t mundo, evento_t *evento, lef_t *l)
{
    int id_h = evento->dado1;
    int id_l = evento->dado2;
    heroi_t heroi = (mundo.herois[id_h]);
    local_t local = (mundo.locais[id_l]);
    int tempo = mundo.tempo_atual;
    int tdl;
    int lot = local.lot_max;
    int id_h_etr;

    retira_cjt(local.equipe, id_h);
    int card = cardinalidade_cjt(local.equipe);
    printf("%6d:SAIDA HEROI %2d Local %d ", tempo, id_h, id_l);

    if (!fila_vazia(local.fila))
    {
        retira_fila(local.fila, &id_h_etr);
        heroi_t heroi_etr = mundo.herois[id_h_etr];
        printf("(%2d/%2d), REMOVE FILA HEROI %2d", card, lot, id_h_etr);
        cria_chegada(heroi_etr, local, l, mundo.tempo_atual, 0);
    }
    else
        printf("(%2d/%2d)", card, lot);
    printf("\n");

    local_t local_dst = mundo.locais[aleat(0, mundo.n_locais - 1)];
    tdl = tempo_de_deslocamento(heroi, local, local_dst);
    cria_chegada(heroi, local, l, mundo.tempo_atual + tdl, 1);
}

int satisfaz_missao(conjunto_t *missao, conjunto_t *equipe, mundo_t *planeta)
{
    conjunto_t *habilidades;
    heroi_t heroi_da_vez;
    int i, j, contido, id_h;

    habilidades = cria_cjt(MAX_LOT * MAX_HABILIDADES);
    /*for para passar por cada heroi da equipe*/
    for (i = 0; i < equipe->card; i++)
    {
        id_h = equipe->v[i];
        heroi_da_vez = planeta->herois[id_h];
        /*forparainserircadaabilidadedoheroi*/
        for (j = 0; j < cardinalidade_cjt(heroi_da_vez.habs); j++)
            insere_cjt(habilidades, heroi_da_vez.habs->v[j]);
    }

    imprime_cjt(habilidades);
    contido = contido_cjt(missao, habilidades);
    destroi_cjt(habilidades);

    return contido;
}

/*
 * Atualiza a esperiencia dos herois apos uma missao.
 */
void atualiza_xp(int endereco, mundo_t *planeta)
{
    int i;
    conjunto_t *herois_local;

    herois_local = planeta->locais[endereco].equipe;
    for (i = 0; i < cardinalidade_cjt(herois_local); i++)
        planeta->herois[herois_local->v[i]].xp++;

    return;
}

/*
 * Executa uma missao, verifica todas as equipes de todos os locais e se for possivel diz
 * a menor equipe que satisfaz a missao.
 */
void executa_missao(evento_t *atual, mundo_t *planeta, lef_t *event)
{
    conjunto_t *missao;
    int i;
    int menor_tam, menor_lugar;

    menor_tam = planeta->n_locais;
    missao = cria_subcjt_cjt(planeta->habilidades, aleat(3, 6));
    printf("%6d:MISSAO %3d HAB_REQ  ", planeta->tempo_atual, atual->dado1);
    imprime_cjt(missao);

    for (i = 0; i < planeta->n_locais; i++)
    {
        printf("%6d:MISSAO %3d HAB_EQL %2d:", planeta->tempo_atual, atual->dado1, i);
        /* Passa por cada local, verifica se satisfaz a missao e se eh menor que o menor*/
        if (satisfaz_missao(missao, planeta->locais[i].equipe, planeta) && (cardinalidade_cjt(planeta->locais[i].equipe) < menor_tam))
        {
            menor_tam = cardinalidade_cjt(planeta->locais[i].equipe);
            menor_lugar = i;
        }
    }

    printf("%6d:MISSAO %3d ", planeta->tempo_atual, atual->dado1);
    if (menor_tam != planeta->n_locais)
    {
        atualiza_xp(menor_lugar, planeta);
        printf("HER_EQS %2d:", menor_lugar);
        imprime_cjt(planeta->locais[menor_lugar].equipe);
    }
    else
    {
        cria_missao(aleat(planeta->tempo_atual, FIM_DO_MUNDO), atual->dado1, event);
        printf("IMPOSSIVEL\n");
    }
    destroi_cjt(missao);

    return;
}

/*Destroi o mundo liberando toda a memoria alocada dinamicamente*/
void fim_do_mundo(mundo_t *mundo)
{
    int i;
    local_t local;
    heroi_t heroi;
    for (i = 0; i < mundo -> n_locais; i++)
    {
        local = mundo -> locais[i];
        destroi_fila(local.fila);
        destroi_cjt(local.equipe);
    }
    for (i = 0; i < mundo->n_herois; i++)
    {
        heroi = mundo->herois[i];
        destroi_cjt(heroi.habs);
    }
    destroi_cjt(mundo->habilidades);
}

void imprime_xp(mundo_t mundo)
{
    int i, xp;
    heroi_t heroi;
    xp = 0;
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
    evento_t *eve_atual;
    int nao_terminou = 1;
    l_eve = cria_lef();
    srand(0);
    mundo = cria_mundo(mundo, l_eve);
    while (nao_terminou == 1)
    {
        eve_atual = obtem_primeiro_lef(l_eve);
        mundo.tempo_atual = eve_atual->tempo;
        switch (eve_atual->tipo)
        {
        case CHEGADA:
            trata_chegada(mundo, eve_atual, l_eve);
            break;
        case SAIDA:
            trata_saida(mundo, eve_atual, l_eve);
            break;
        case MISSAO:
            executa_missao(eve_atual, &mundo, l_eve);
            break;
        case FIM:
            printf("FIM\n");
            nao_terminou = 0;
        default:
            break;
        }
    }
    imprime_xp(mundo);
    fim_do_mundo(&mundo);
    return 0;
}
