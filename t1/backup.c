void trata_chegada(mundo_t mundo, evento_t *evento, lef_t* l){
    int id_h = evento -> dado1;
    int id_l = evento -> dado2; 
    heroi_t heroi = (mundo.herois[id_h]);
    local_t local = (mundo.locais[id_l]); 
    int tpl;
    printf("%6d:CHEGA HEROI %2d Local %d ", mundo.tempo_atual, id_h, id_l);
    if (esta_lotado(local)){
        if (quer_ficar(local, heroi)){
            insere_fila(local.fila, id_h);
            printf("(%2d/%2d), FILA %2d\n", cardinalidade_cjt(local.equipe),
            local.lot_max,local.fila -> tamanho);
        }
        else{
            cria_saida(id_h, id_l, mundo.tempo_atual, l);
            printf("(%2d/%2d), DESISTE\n", cardinalidade_cjt(local.equipe),
            local.lot_max);
        }
    }
    else{
        tpl = tempo_de_permanencia(heroi.paciencia);
        insere_cjt(local.equipe, heroi.id);
        printf("(%2d/%2d), ENTRA\n", cardinalidade_cjt(local.equipe),
        local.lot_max);
        cria_saida(id_h, id_l, mundo.tempo_atual + tpl, l);
    } 
}