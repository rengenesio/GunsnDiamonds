#include "jogo.h"

void NovoJogo(void)
{
	int coord_clique[2][2],
		pontos = 0,
		pontos_bonus = 0,
		clique = 0,
		free_to_go = 0,
		iTemp,
		ultimo_clicado = 0,
		fase = 1,
		i;
	BITMAP *Joias[8];
	BITMAP *fundo = load_bitmap(PATH_FUNDO, NULL);
	BITMAP *buffer = create_bitmap(800,600);
	BITMAP *buttons[3];
	BITMAP *numeros[10];
	BITMAP *mira = load_bitmap(PATH_MIRA_MARCA, NULL);
	JOIA matriz_grade[10][10];
	SAMPLE * dica;
	SAMPLE * pause;
	SAMPLE * troca;

	CarregaSom(&dica, "../resources/sounds/dica.wav");
	CarregaSom(&pause, "../resources/sounds/pause.wav");
	CarregaSom(&troca, "../resources/sounds/troca.wav");
	SeparaFrames(PATH_NUMEROS, numeros, 10);

	if(LoadButtons(buttons))
	{
		printf("Erro ao carregar os botoes.\n");
		DeloadAll(Joias, fundo, buffer, buttons, numeros, mira);
		exit(1);
	}

	clear_to_color(buffer, 0);

	show_mouse(NULL);
	DrawBackground(fundo, screen);
	show_mouse(screen);

	ZeraVetorJoias(matriz_grade);
	CarregaJoias(Joias, matriz_grade);

	DrawBackground(fundo, buffer);
	DesenhaJoias(Joias, matriz_grade, buffer);

	/* Iniciando os indicadores gr�ficos */
    show_mouse(NULL);
	AtualizaPontuacao(fundo, buffer, pontos, numeros);
	AtualizaPontuacaoBonus(fundo, buffer, pontos_bonus, numeros);
	AtualizaFase(fundo, buffer, fase, numeros);
	show_mouse(screen);

	PreCacheMoves(matriz_grade);

	/*Temporizador*/
	IniciaTempoFase(fase);
	LOCK_VARIABLE(tempo_restante);
	LOCK_FUNCTION(TimerTempoContagem);
	install_int(TimerTempoContagem, 1000);

	while(ExisteJogadaPossivel(matriz_grade) && tempo_restante > 0) /* Loop Principal do jogo */
	{
		while((mouse_b & 1)); /* Espera o jogador soltar o mouse...*/

        while(!(mouse_b & 1)) /* Espera o jogador clicar */
        {
            if(ClicouBotaoDica() && ultimo_clicado != 1) /* Clicou no bot�o de dica */
            {
                ResetButton(fundo, buffer);
                draw_sprite(buffer, buttons[0], BT_DICA_X, BT_DICA_Y);
                show_mouse(NULL);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                ultimo_clicado = 1;
            }
            else if(ClicouBotaoPause() && ultimo_clicado != 2) /* Clicou no bot�o de pause */
            {
                ResetButton(fundo, buffer);
                draw_sprite(buffer, buttons[1], BT_PAUSE_X, BT_PAUSE_Y);
                show_mouse(NULL);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                ultimo_clicado = 2;
            }
            else if(ClicouBotaoSair() && ultimo_clicado != 3) /* Clicou no bot�o p/ sair do jogo */
            {
                ResetButton(fundo, buffer);
                draw_sprite(buffer, buttons[2], BT_SAIR_X, BT_SAIR_Y);
                show_mouse(NULL);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                ultimo_clicado = 3;
            }
            else if(!ClicouBotaoDica() && !ClicouBotaoPause() && !ClicouBotaoSair() && ultimo_clicado != 0)
            {
                ResetButton(fundo, buffer);
                show_mouse(NULL);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                ultimo_clicado = 0;
            }
			if(redraw_tempo)
            {
                show_mouse(NULL);
                RedrawTempo(fundo, buffer, numeros);
                BufferToScreen(buffer, screen);
				show_mouse(screen);
				redraw_tempo = 0;
			}
			rest(1); /* Aliviar um pouco o processador xD */
		}

        /* Pega onde o jogador clicou, e faz a devida acao */
		if(MouseDentroDaGrade()) /* Clicou na grade */
		{
			PegaQuadradoPelaPosMouse(&coord_clique[clique][0], &coord_clique[clique][1]);
            show_mouse(NULL);
			MarcaJoia(mira, buffer, coord_clique[clique][0], coord_clique[clique][1]);
			BufferToScreen(buffer, screen);
			show_mouse(screen);
			clique++;

			if(clique == 2) /* Hora de, tentar, remover as joias */
			{
				/*Pausa o timer */
				game_paused = 1;

				show_mouse(NULL);
				DesmarcaJoia(buffer, fundo, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
					coord_clique[0][0], coord_clique[0][1]);
                DesmarcaJoia(buffer, fundo, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
					coord_clique[1][0], coord_clique[1][1]);
                BufferToScreen(buffer, screen);
                show_mouse(screen);

                /* Checa se s�o adjacentes */
				if(!QuadradoAdjacente(coord_clique[0][0], coord_clique[0][1], coord_clique[1][0], coord_clique[1][1]))
                {
                    /* n�o sao adjacentes */
                    clique = 0;
                    /*Despausa o timer */
                    game_paused = 0;
                    continue;
                }

                /* Checa se trocando uma pela outra, fecha uma cadeia */
                switch(GetDirection(coord_clique[0][0], coord_clique[0][1], coord_clique[1][0], coord_clique[1][1]))
                {
                    case 0:
                        if(!(matriz_grade[coord_clique[0][0]][coord_clique[0][1]].troca_cima ||
                            matriz_grade[coord_clique[1][0]][coord_clique[1][1]].troca_baixo))
                        {
                            clique =0;
                        }
                        break;
                    case 1:
                        if(!(matriz_grade[coord_clique[0][0]][coord_clique[0][1]].troca_direita ||
                            matriz_grade[coord_clique[1][0]][coord_clique[1][1]].troca_esquerda))
                        {
                            clique =0;
                        }
                        break;
                    case 2:
                        if(!(matriz_grade[coord_clique[0][0]][coord_clique[0][1]].troca_baixo ||
                            matriz_grade[coord_clique[1][0]][coord_clique[1][1]].troca_cima))
                        {
                            clique =0;
                        }
                        break;
                    case 3:
                        if(!(matriz_grade[coord_clique[0][0]][coord_clique[0][1]].troca_esquerda ||
                            matriz_grade[coord_clique[1][0]][coord_clique[1][1]].troca_direita))
                        {
                            clique =0;
                        }
					    break;
				}

				if(!clique) /* A troca n�o � valida */
				{
					play_sample(troca, volume, 128, 1000, 0);
					show_mouse(NULL);
					Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
					BufferToScreen(buffer, screen);
					Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
					BufferToScreen(buffer, screen);
					show_mouse(screen);
					/*Despausa o timer */
					game_paused = 0;
					continue;
				}

                /* Troca as joias de lugar */
				show_mouse(NULL);
				Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
				BufferToScreen(buffer, screen);
				show_mouse(screen);

                /* Remove as combinacoes formadas */
				show_mouse(NULL);
				pontos = pontos + (DestroiCombinacoes(coord_clique[0][0], coord_clique[0][1], buffer, fundo, matriz_grade) * 5);
				pontos = pontos + (DestroiCombinacoes(coord_clique[1][0], coord_clique[1][1], buffer, fundo, matriz_grade) * 5);
				BufferToScreen(buffer, screen);
				show_mouse(screen);

                /* Cai as outras joias */
                while(!free_to_go)
                {
                    while(!free_to_go)
                    {
                        free_to_go = 1;
                        /* Cai as pedras */
                        show_mouse(NULL);
                        while(CairPedras(fundo, matriz_grade, Joias, buffer)); /*Cai todas as pedras possiveis */
                        show_mouse(screen);
                        /* preenche a linha 0 com pedras */
                        show_mouse(NULL);
                        if(PreencheLinha0(matriz_grade, Joias, buffer)) /* Se n�o h� mais espa�os em branco, free_to_go = 1 */
                        {
                            free_to_go = 0;
                            BufferToScreen(buffer, screen);
                        }
                        show_mouse(screen);
                    }

                    show_mouse(NULL);
                    iTemp = SearchAndDestroy(matriz_grade, buffer, fundo);
                    if(iTemp)
                    {
                        free_to_go = 0;
                        BufferToScreen(buffer, screen);
                        pontos = pontos + (iTemp * 5);
                    }
                    show_mouse(screen);
                }
                free_to_go = 0;

                /* Refaz o cache dos movimentos possivels */
                ZeraVetorJoias(matriz_grade);
                PreCacheMoves(matriz_grade);
                clique = 0;

                show_mouse(NULL);
                AtualizaPontuacao(fundo, buffer, pontos, numeros);
                show_mouse(screen);

                if(PassouDeFase(fase, pontos))
                {
                    pontos_bonus += tempo_restante * fase;
                    fase++;
                    show_mouse(NULL);
                    AtualizaFase(fundo, buffer, fase, numeros);
                    AtualizaPontuacaoBonus(fundo, buffer, pontos_bonus, numeros);
                    show_mouse(screen);
                    IniciaTempoFase(fase);
                }

                /*Despausa o timer */
                game_paused = 0;
            }
        }
        else if(ClicouBotaoPause()) /* Clicou no bot�o de pausa */
        {
            /*Pausa o timer */
            game_paused = 1;
            play_sample(pause, volume, 128, 1000, 0);
            GamePause(buffer, fundo);
            /*Despausa o timer */
            game_paused = 0;
        }
        else if(ClicouBotaoDica()) /* Clicou no bot�o de dica */
        {
            /*Pausa o timer */
            game_paused = 1;
            if(pontos >= 10)
            {
                play_sample(dica, volume, 128, 1000, 0);
                ProcuraDica(matriz_grade, buffer, fundo, Joias);
                pontos -= 10;
                show_mouse(NULL);
                AtualizaPontuacao(fundo, buffer, pontos, numeros);
                show_mouse(screen);
            }
            /*Despausa o timer */
            game_paused = 0;
        }
        else if(ClicouBotaoSair()) /* Clicou no bot�o p/ sair do jogo */
        {
            break;
        }
    }

	stop_sample(dica);
	destroy_sample(dica);
	stop_sample(troca);
	destroy_sample(troca);
	stop_sample(pause);
	destroy_sample(pause);

    DeloadAll(Joias, fundo, buffer, buttons, numeros, mira);
    remove_int(TimerTempoContagem);
    clear_keybuf();
	pontos += pontos_bonus;
    GameOver(pontos);
}
/* OK */
void CarregaJoias (BITMAP *joias[8], JOIA matriz_grade[10][10])
{
	int linha, coluna;

	joias[0] = create_bitmap(TAM_JOIA,TAM_JOIA);
	clear_to_color(joias[0], makecol(255,0,255));
	joias[1] = load_bitmap(PATH_JOIA_1, NULL);
	joias[2] = load_bitmap(PATH_JOIA_2, NULL);
	joias[3] = load_bitmap(PATH_JOIA_3, NULL);
	joias[4] = load_bitmap(PATH_JOIA_4, NULL);
	joias[5] = load_bitmap(PATH_JOIA_5, NULL);
	joias[6] = load_bitmap(PATH_JOIA_6, NULL);
	joias[7] = load_bitmap(PATH_JOIA_7, NULL);

	for(linha=0;linha<8;linha++)
	{
		if(!joias[linha])
		{
			printf("Erro ao carregar o bitmap joia%d.tga\n", linha);
			exit(1);
		}
	}

	for (linha=0; linha<10 ;linha++)
	{
		for (coluna=0; coluna<10 ;coluna++)
		{
			while(1)
			{
				matriz_grade[linha][coluna].joia = rand() % 7 + 1;

				if((linha < 2) && (coluna < 2)) /* Nao e necessario fazer checagem aqui, pois nao a possibilidade de ter 3 iguais... sao os 4 do canto superior esquerdo*/
					break;
				else if((linha < 2) && !((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-1].joia) && (matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-2].joia))) /* Checagem para as 2 primeiras linhas... logo n�o precisamos fazer verificacao vertical, so horizontal */
					break;
				else if((coluna < 2) && !((matriz_grade[linha][coluna].joia == matriz_grade[linha-1][coluna].joia) && (matriz_grade[linha][coluna].joia == matriz_grade[linha-2][coluna].joia))) /* Checagem para as 2 primeiras colunas... logo n�o precisamos fazer verificacao horizontal, so vertical */
					break;
                else if(!(matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-1].joia && matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-2].joia) &&
                        !(matriz_grade[linha][coluna].joia == matriz_grade[linha-1][coluna].joia && matriz_grade[linha][coluna].joia == matriz_grade[linha-2][coluna].joia))
                    break;
		    }
		}
	}
}
void DesenhaJoias (BITMAP *joias[8], JOIA matriz_grade[10][10], BITMAP *buffer)
{
	int linha, coluna;

	show_mouse(NULL);
	for (linha=0; linha<10 ;linha++)
	{
		for (coluna=0; coluna<10 ;coluna++)
		{
            DrawJoia(joias[matriz_grade[linha][coluna].joia], buffer, linha, coluna);
		}
	}
    BufferToScreen(buffer, screen);
	show_mouse(screen);
}
void DrawJoia(BITMAP *joia, BITMAP *target, int linha, int coluna)
{
    draw_sprite(target, joia,
                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coluna + 1)) + ((TAM_QUADRADO) * (coluna)) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)),
                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (linha + 1)) + ((TAM_QUADRADO) * (linha)) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)));
}
void DrawBackground(BITMAP *fundo, BITMAP *target)
{
    blit(fundo, target, 0, 0, 0, 0, 800, 600);
}
int MouseDentroDaGrade(void)
{
	if( (mouse_x >= INICIO_GRADE_X) &&
		(mouse_x < FINAL_GRADE_X) &&
		(mouse_y >= INICIO_GRADE_Y) &&
		(mouse_y < FINAL_GRADE_Y))
		return 1;

	return 0;
}
int ClicouBotaoPause(void)
{
    if( (mouse_x >= BT_PAUSE_X) &&
        (mouse_x < (BT_PAUSE_X + BT_PAUSE_LARG)) &&
        (mouse_y >= BT_PAUSE_Y) &&
        (mouse_y < (BT_PAUSE_Y + BT_PAUSE_ALT)))
        return 1;

	return 0;
}
int ClicouBotaoDica(void)
{
    if( (mouse_x >= BT_DICA_X) &&
        (mouse_x < (BT_DICA_X + BT_DICA_LARG)) &&
        (mouse_y >= BT_DICA_Y) &&
        (mouse_y < (BT_DICA_Y + BT_DICA_ALT)))
        return 1;

	return 0;
}
int ClicouBotaoSair(void)
{
    if( (mouse_x >= BT_SAIR_X) &&
        (mouse_x < (BT_SAIR_X + BT_SAIR_LARG)) &&
        (mouse_y >= BT_SAIR_Y) &&
        (mouse_y < (BT_SAIR_Y + BT_SAIR_ALT)))
        return 1;

	return 0;
}
void PegaQuadradoPelaPosMouse(int *linha, int *coluna)
{
    *coluna = (mouse_x - (INICIO_GRADE_X + TAM_LINHA_GRADE/2))/46;
    *linha = (mouse_y - (INICIO_GRADE_Y + TAM_LINHA_GRADE/2))/46;

    if(*coluna > 9)
        *coluna = 9;
    if(*linha > 9)
        *linha = 9;
}
int QuadradoAdjacente(int quad1_x, int quad1_y, int quad2_x, int quad2_y)
{
	if( (((quad2_x == (quad1_x + 1)) || (quad2_x == (quad1_x - 1))) && (quad2_y == quad1_y)) ||
		(((quad2_y == (quad1_y + 1)) || (quad2_y == (quad1_y - 1))) && (quad2_x == quad1_x)) )
		return 1;

	return 0;
}
int GetDirection(int quad1_linha, int quad1_coluna, int quad2_linha, int quad2_coluna)
{

    if(quad1_linha > quad2_linha)
        return 0; /* Quad 2 esta acima do quad1 */
    else if(quad1_coluna < quad2_coluna)
        return 1; /* Quad 2 esta a direita do quad1 */
    else if(quad1_linha < quad2_linha)
        return 2; /* Quad 2 esta abaixo do quad1 */
    else if(quad1_coluna > quad2_coluna)
        return 3; /* Quad 2 esta a esquerda do quad1 */

    return -1; /* ERRO!!! Nunca deveria chegar at� aqui */
}
void ApagaQuadrado(int linha, int coluna, BITMAP *target, BITMAP *fundo)
{
    int x, y;
    x = INICIO_GRADE_X + (TAM_LINHA_GRADE * (coluna + 1)) + ((TAM_QUADRADO) * (coluna));
    y = INICIO_GRADE_Y + (TAM_LINHA_GRADE * (linha + 1)) + ((TAM_QUADRADO) * (linha));
	blit(fundo, target, x, y, x, y, TAM_QUADRADO, TAM_QUADRADO);
}
void Troca(JOIA matriz_grade[10][10], int coord_clique[2][2], BITMAP *buffer, BITMAP *fundo, BITMAP *Joias[8])
{
	int temp, dist_maior = TAM_LINHA_GRADE + TAM_QUADRADO;
	BITMAP *ori_buffer = create_bitmap(800,600);

    /* Apaga os dois quadrados */
    ApagaQuadrado(coord_clique[0][0], coord_clique[0][1], buffer, fundo);
    ApagaQuadrado(coord_clique[1][0], coord_clique[1][1], buffer, fundo);

    /*Anima��o de troca */
    blit(buffer, ori_buffer, 0, 0, 0, 0, 800, 600);

    switch(GetDirection(coord_clique[0][0], coord_clique[0][1], coord_clique[1][0], coord_clique[1][1]))
    {
        case 0:
            /*baixo -> cima */
            for(temp=0;temp<10;temp++)
            {
                blit(ori_buffer, buffer, 0, 0, 0, 0, 800, 600);
                if(temp<=2)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2))  + 3 * (temp + 1),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2))  - 3 * (temp + 1),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)));
                }
                else if(temp <= 6)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 9,
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - 9,
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)));
                }
                else
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 3 * ((9 - temp + 1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - (3 * (9 - temp + 1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)));
                }
                BufferToScreen(buffer, screen);
                rest(1000/100);
            }
            break;
        case 1:
            /*esquerda -> direita */
            for(temp=0;temp<10;temp++)
            {
                blit(ori_buffer, buffer, 0, 0, 0, 0, 800, 600);
                if(temp<=2)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 3 * ((temp + 1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - (3 * (temp + 1)));
                }
                else if(temp <= 6)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 9);
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - 9);
                }
                else
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 3 * ((9 - temp + 1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - (3 * (9 - temp + 1)));
                }
                BufferToScreen(buffer, screen);
                rest(1000/100);
            }

            break;
        case 2:
            /*cima -> baixo */
            for(temp=0;temp<10;temp++)
            {
                blit(ori_buffer, buffer, 0, 0, 0, 0, 800, 600);
                if(temp<=2)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2))  + 3 * (temp + 1),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2))  - 3 * (temp + 1),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)));
                }
                else if(temp <= 6)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 9,
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - 9,
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)));
                }
                else
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 3 * ((9 - temp + 1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - (3 * (9 - temp + 1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)));
                }
                BufferToScreen(buffer, screen);
                rest(1000/100);
            }
            break;
        case 3:
            /*esquerda <- direita */
            for(temp=0;temp<10;temp++)
            {
                blit(ori_buffer, buffer, 0, 0, 0, 0, 800, 600);
                if(temp<=2)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 3 * ((temp + 1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - (3 * (temp + 1)));
                }
                else if(temp <= 6)
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 9);
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - 9);
                }
                else
                {
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[0][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[0][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[0][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + 3 * ((9 - temp + 1)));
                    draw_sprite(buffer, Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (coord_clique[1][1] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][1])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + ((dist_maior/10) * (temp+1)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (coord_clique[1][0] + 1)) + ((TAM_QUADRADO) * (coord_clique[1][0])) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) - (3 * (9 - temp + 1)));
                }
                BufferToScreen(buffer, screen);
                rest(1000/100);
            }
            break;
    }
    blit(ori_buffer, buffer, 0, 0, 0, 0, 800, 600);

    /* Troca os valores de suas joias */
    temp = matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia;
    matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia = matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia;
    matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia = temp;

    /* Redesenha-os*/
    DrawJoia(Joias[matriz_grade[coord_clique[0][0]][coord_clique[0][1]].joia], buffer, coord_clique[0][0], coord_clique[0][1]);
    DrawJoia(Joias[matriz_grade[coord_clique[1][0]][coord_clique[1][1]].joia], buffer, coord_clique[1][0], coord_clique[1][1]);

    destroy_bitmap(ori_buffer);
}
int DestroiCombinacoes(int linha, int coluna, BITMAP *buffer, BITMAP *fundo, JOIA matriz_grade[10][10])
{
	SAMPLE * vidro;
	int bin=1, cont=0, count_g = 0, destroeQuad=0;
	/* A variavel bin sera utilizada para compara��o binaria mais a frente:
        Seus bits servem para indicar quais quadrados terao que ser destruidos mais a frente... se tiver 1, destroe quadrado, 0 n�o.
        Contando os bits da direita para a esquerda (1� = bit 0, 2� = bit 1, 3� = bit 2)...
        1� - bit sempre ligado...
        2� - quadrado adjacente a esquerda / quadrado adjacente acima
        3� - segundo quadrado a esquerda / segundo quadrado acima
        4� - quadrado adjacente a direita / quadrado adjacente abaixo
        5� - segundo quadrado a direita / segundo quadrado abaixo
    */

    CarregaSom(&vidro, "../resources/sounds/vidro.wav");

	/* Verificacoes verticais: */
	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha-1][coluna].joia) && (linha-1>=0))
	{
		bin+=2;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha-2][coluna].joia) && (linha-2>=0))
		{
			bin+=4;
			cont++;
		}
	}

	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha+1][coluna].joia) && (linha+1<10))
	{
		bin+=8;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha+2][coluna].joia) && (linha+2<10))
		{
			bin+=16;
			cont++;
		}
	}

	/*Remocao dos quadrados da horizontal: */
	if(cont>=2)
	{
		destroeQuad = 1;
		if(bin & 2)
		{
            ApagaQuadrado(linha - 1, coluna, buffer, fundo);
            matriz_grade[linha - 1][coluna].joia = 0;
		}
		if(bin & 4)
		{
			ApagaQuadrado(linha - 2, coluna, buffer, fundo);
			matriz_grade[linha - 2][coluna].joia = 0;
		}
		if(bin & 8)
        {
			ApagaQuadrado(linha + 1, coluna, buffer, fundo);
			matriz_grade[linha + 1][coluna].joia = 0;
        }
		if(bin & 16)
		{
			ApagaQuadrado(linha + 2, coluna, buffer, fundo);
			matriz_grade[linha + 2][coluna].joia = 0;
		}
		count_g += cont;
	}

	cont=0;
	bin=1;

	/* Verificacoes horizontais: */
	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-1].joia) && (coluna-1>=0))
	{
		bin+=2;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-2].joia) && (coluna-2>=0))
		{
			bin+=4;
			cont++;
		}
	}

	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna+1].joia) && (coluna+1<10))
	{
		bin+=8;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna+2].joia) && (coluna+2<10))
		{
			bin+=16;
			cont++;
		}
	}

	if(cont>=2)
	{
		destroeQuad = 1;
		if(bin & 2)
		{
			ApagaQuadrado(linha, coluna - 1, buffer, fundo);
			matriz_grade[linha][coluna - 1].joia = 0;
		}
		if(bin & 4)
		{
			ApagaQuadrado(linha, coluna - 2, buffer, fundo);
			matriz_grade[linha][coluna - 2].joia = 0;
		}
		if(bin & 8)
		{
			ApagaQuadrado(linha, coluna + 1, buffer, fundo);
			matriz_grade[linha][coluna + 1].joia = 0;
		}
		if(bin & 16)
		{
			ApagaQuadrado(linha, coluna + 2, buffer, fundo);
			matriz_grade[linha][coluna + 2].joia = 0;
		}
		count_g += cont;
	}

	if(destroeQuad)
	{
		ApagaQuadrado(linha, coluna, buffer, fundo);
		matriz_grade[linha][coluna].joia = 0;
		play_sample(vidro, volume, 128, 1000, 0);
		count_g += 1;
	}

	rest(350);
	stop_sample(vidro);
	destroy_sample(vidro);
	return count_g;
}
void BufferToScreen(BITMAP *buffer, BITMAP *screen)
{
    blit(buffer, screen, 0, 0, 0, 0, RESOLUCAO_X, RESOLUCAO_Y);
}

void ScreenToBuffer(BITMAP *buffer)
{
	blit(screen, buffer, 0, 0, 0, 0, 800, 600);
}

void ZeraVetorJoias(JOIA matriz_grade[10][10])
{
		int i, j;

		for(i =0; i<10;i++)
			for(j=0;j<10;j++)
			{
				matriz_grade[i][j].troca_cima = 0;
				matriz_grade[i][j].troca_esquerda = 0;
				matriz_grade[i][j].troca_direita = 0;
				matriz_grade[i][j].troca_baixo = 0;
			}
}

int PreCacheMoves(JOIA matriz_grade[10][10])
{
    int i, j, linha, coluna, count = 0, temp;


    for(i =0; i<10;i++)
        for(j=0;j<10;j++)
        {
            linha = i;
            coluna = j;

            if(matriz_grade[linha][coluna].joia == 0)
                continue;

            if(i > 0) /* Fecha combina��o a cima */
            {
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha-1][coluna].joia;
                matriz_grade[linha-1][coluna].joia = temp;
                if(VerificaMovimento(matriz_grade, i - 1, j))
                {
                    matriz_grade[i][j].troca_cima = 1;
                    count++;
                }
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha-1][coluna].joia;
                matriz_grade[linha-1][coluna].joia = temp;
            }
            if(i < 9) /* Fecha combina��o a baixo */
            {
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha+1][coluna].joia;
                matriz_grade[linha+1][coluna].joia = temp;
                if(VerificaMovimento(matriz_grade, i + 1, j))
                {
                    matriz_grade[i][j].troca_baixo = 1;
                    count++;
                }
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha+1][coluna].joia;
                matriz_grade[linha+1][coluna].joia = temp;
            }
            if(j > 0) /* Fecha combina��o a esquerda */
            {
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha][coluna-1].joia;
                matriz_grade[linha][coluna-1].joia = temp;
                if(VerificaMovimento(matriz_grade, i, j - 1))
                {
                    matriz_grade[i][j].troca_esquerda = 1;
                    count++;
                }
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha][coluna-1].joia;
                matriz_grade[linha][coluna-1].joia = temp;
            }
            if(j < 9) /* Fecha combina��o a direita */
            {
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha][coluna+1].joia;
                matriz_grade[linha][coluna+1].joia = temp;
                if(VerificaMovimento(matriz_grade, i, j + 1))
                {
                    matriz_grade[i][j].troca_direita = 1;
                    count++;
                }
                temp = matriz_grade[linha][coluna].joia;
                matriz_grade[linha][coluna].joia = matriz_grade[linha][coluna+1].joia;
                matriz_grade[linha][coluna+1].joia = temp;
            }
        }

    return count;
}
int VerificaMovimento(JOIA matriz_grade[10][10], int linha, int coluna)
{
    int bin=1, cont=0;
	/* A variavel bin sera utilizada para compara��o binaria mais a frente:
        Seus bits servem para indicar quais quadrados terao que ser destruidos mais a frente... se tiver 1, destroe quadrado, 0 n�o.
        Contando os bits da direita para a esquerda (1� = bit 0, 2� = bit 1, 3� = bit 2)...
        1� - bit sempre ligado...
        2� - quadrado adjacente a esquerda / quadrado adjacente acima
        3� - segundo quadrado a esquerda / segundo quadrado acima
        4� - quadrado adjacente a direita / quadrado adjacente abaixo
        5� - segundo quadrado a direita / segundo quadrado abaixo
    */

	/* Verificacoes verticais: */
	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha-1][coluna].joia) && (linha-1>=0))
	{
		bin+=2;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha-2][coluna].joia) && (linha-2>=0))
		{
			bin+=4;
			cont++;
		}
	}

	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha+1][coluna].joia) && (linha+1<10))
	{
		bin+=8;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha+2][coluna].joia) && (linha+2<10))
		{
			bin+=16;
			cont++;
		}
	}

	/*Remocao dos quadrados da horizontal: */
	if(cont>=2)
	{
        return 1;
	}

	cont=0;
	bin=1;

	/* Verificacoes horizontais: */
	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-1].joia) && (coluna-1>=0))
	{
		bin+=2;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna-2].joia) && (coluna-2>=0))
		{
			bin+=4;
			cont++;
		}
	}

	if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna+1].joia) && (coluna+1<10))
	{
		bin+=8;
		cont++;
		if ((matriz_grade[linha][coluna].joia == matriz_grade[linha][coluna+2].joia) && (coluna+2<10))
		{
			bin+=16;
			cont++;
		}
	}

	if(cont>=2)
	{
        return 1;
	}

	return 0;
}
int CairPedras(BITMAP *fundo, JOIA matriz_grade[10][10], BITMAP *joias[8], BITMAP *buffer)
{
    BITMAP *ori_buffer = create_bitmap(800, 600);
    int vai_cair[10][10];
    int i, j, k = 0, set_all = 0;

    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            vai_cair[i][j] = 0;

    if(!ori_buffer)
    {
        printf("Erro ao carregar criar o buffer CairPedras():ori_buffer.\n");
        exit(1);
    }
    if(!buffer)
    {
        printf("Erro ao carregar criar o buffer CairPedras():buffer.\n");
        exit(1);
    }


    /* Prepara o ori_buffer... deixando em branco todos os quadrados que ir�o cair, isto �, onde linha + 1 = 0 */
    ScreenToBuffer(ori_buffer);
    /* i=8;i>=0;i-- */
    /* i=0;i<9;i++ */

    for(j=0;j<10;j++)
    {
        for(i=8;i>=0;i--)
        {
            if(set_all)
            {
                ApagaQuadrado(i, j, ori_buffer, fundo);
                k++;
                vai_cair[i][j] = 1;
                continue;
            }

            if(matriz_grade[i+1][j].joia == 0 && matriz_grade[i][j].joia != 0)
            {
                ApagaQuadrado(i, j, ori_buffer, fundo);
                k++;
                set_all = 1;
                vai_cair[i][j] = 1;
            }
        }
        set_all = 0;
    }

    if(k == 0)
    {
        destroy_bitmap(ori_buffer);
        return 0;
    }

    /* Cai os quadrados de 3px em 3px... 15 vezes durante 1 segundo */
    for(k=1;k<=15;k++)
    {
        blit(ori_buffer, buffer, 0, 0, 0, 0, 800, 600);

        for(i=8;i>=0;i--)
            for(j=0;j<10;j++)
            {
                if(vai_cair[i][j])
                {
                    draw_sprite(buffer, joias[matriz_grade[i][j].joia],
                                INICIO_GRADE_X + (TAM_LINHA_GRADE * (j + 1)) + ((TAM_QUADRADO) * (j)) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)),
                                INICIO_GRADE_Y + (TAM_LINHA_GRADE * (i + 1)) + ((TAM_QUADRADO) * (i)) + ((TAM_QUADRADO/2) - (TAM_JOIA/2)) + k * 3);

                    if(k == 15)
                    {
                        matriz_grade[i+1][j].joia = matriz_grade[i][j].joia;
                        if(i == 0)
                        {
                            matriz_grade[i][j].joia = 0;
                        }
                    }
                }
            }

        BufferToScreen(buffer, screen);
        //rest(1);
    }

    destroy_bitmap(ori_buffer);
    return 1;
}
int PreencheLinha0(JOIA matriz_grade[10][10], BITMAP *joias[8], BITMAP *buffer)
{
    int i, j, ret = 0;

    for(i=0;i<10;i++)
        if(matriz_grade[0][i].joia == 0)
        {
            ret = 1;
            matriz_grade[0][i].joia  = rand()%7 + 1;

            for(j=0; (matriz_grade[j][i].joia != 0) ;j++);

            while(matriz_grade[0][i].joia == matriz_grade[j][i].joia)
            {
                matriz_grade[0][i].joia = rand()%7 + 1;
            }

            DrawJoia(joias[matriz_grade[0][i].joia], buffer, 0, i);
        }

    return ret;
}
int SearchAndDestroy(JOIA matriz_grade[10][10], BITMAP *buffer, BITMAP* fundo)
{
	SAMPLE * vidro;
    int i, j, count = 0, sd[10][10];

    CarregaSom(&vidro, "../resources/sounds/vidro.wav");

    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            sd[i][j] = 0;

    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            if(VerificaMovimento(matriz_grade, i, j))
                sd[i][j] = 1;

    /*DumpGrid(matriz_grade); */

    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            if(sd[i][j])
            {
                /*printf("linha: %d, coluna: %d, joia: %d\n", i, j, matriz_grade[i][j].joia); */
                ApagaQuadrado(i, j, buffer, fundo);
                matriz_grade[i][j].joia = 0;
                count++;
            }

    if(count)
        play_sample(vidro, volume, 128, 1000, 0);
    /*printf("\n\n");*/
    /*DumpGrid(matriz_grade); */
	rest(350);
	stop_sample(vidro);
	destroy_sample(vidro);
    return count;
}
void DumpGrid(JOIA matriz_grade[10][10])
{
    int i, j;

    for(i=0;i<10;i++)
    {
        for(j=0;j<10;j++)
            printf("%d ", matriz_grade[i][j].joia);
        printf("\n");
    }
    printf("\n");
}
int LoadButtons(BITMAP *buttons[3])
{
    buttons[0] = load_bitmap(PATH_BOTAO_DICA, NULL);
    buttons[1] = load_bitmap(PATH_BOTAO_PAUSE, NULL);
    buttons[2] = load_bitmap(PATH_BOTAO_SAIR, NULL);

    if(!buttons[0] || !buttons[1] || !buttons[2])
        return 1;

    return 0;
}
void ResetButton(BITMAP *fundo, BITMAP *buffer)
{
    blit(fundo, buffer, BT_PAUSE_X, BT_PAUSE_Y, BT_PAUSE_X, BT_PAUSE_Y, BT_PAUSE_LARG, BT_PAUSE_ALT);
    blit(fundo, buffer, BT_DICA_X, BT_DICA_Y, BT_DICA_X, BT_DICA_Y, BT_DICA_LARG, BT_DICA_ALT);
    blit(fundo, buffer, BT_SAIR_X, BT_SAIR_Y, BT_SAIR_X, BT_SAIR_Y, BT_SAIR_LARG, BT_SAIR_ALT);
}
void RedrawTempo(BITMAP *fundo, BITMAP *buffer, BITMAP *numeros[10])
{
    int minutos = tempo_restante/60,
        segundos = tempo_restante%60,
        min_nums[1], seg_nums[2];

    /* Apaga os numeros antigos */
    blit(fundo, buffer, QUADRINHO_TEMPO_X_1, QUADRINHO_TEMPO_Y,
                        QUADRINHO_TEMPO_X_1, QUADRINHO_TEMPO_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_TEMPO_X_2, QUADRINHO_TEMPO_Y,
                        QUADRINHO_TEMPO_X_2, QUADRINHO_TEMPO_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_TEMPO_X_3, QUADRINHO_TEMPO_Y,
                        QUADRINHO_TEMPO_X_3, QUADRINHO_TEMPO_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);

    PegaNumeroNaCasa(minutos, 1, min_nums);
    PegaNumeroNaCasa(segundos, 2, seg_nums);

    if(min_nums[0] >= 1)
    {
        draw_sprite(buffer, numeros[min_nums[0] - 1], QUADRINHO_TEMPO_X_1, QUADRINHO_TEMPO_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_TEMPO_X_1, QUADRINHO_TEMPO_Y);
    }

    if(seg_nums[1] >= 1)
    {
        draw_sprite(buffer, numeros[seg_nums[1] - 1], QUADRINHO_TEMPO_X_2, QUADRINHO_TEMPO_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_TEMPO_X_2, QUADRINHO_TEMPO_Y);
    }

    if(seg_nums[0] >= 1)
    {
        draw_sprite(buffer, numeros[seg_nums[0] - 1], QUADRINHO_TEMPO_X_3, QUADRINHO_TEMPO_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_TEMPO_X_3, QUADRINHO_TEMPO_Y);
    }
}
void PegaNumeroNaCasa(int number, int casas, int numeros[])
{
    int i, exp = 1;

    for(i=0;i<(casas - 1);i++)
        exp *= 10;

    /*printf("numero = %d exp = %d\n", number, exp);*/

    for(i=0;i<casas;i++)
    {
        numeros[casas - 1 - i] = number/exp;
        number = number - (numeros[casas - 1 - i] * exp);
        exp /= 10;
        /*printf("numeros[casas - 1 - %d] = %d\n", i, numeros[casas - 1 - i]);
        printf("numero = %d exp = %d\n", number, exp);*/
    }
}
void AtualizaPontuacao(BITMAP *fundo, BITMAP *buffer, int pontos,  BITMAP *numeros[10])
{
    int nums[6];

    /* Apaga os numeros antigos */
    blit(fundo, buffer, QUADRINHO_X_1, QUADRINHO_PONTOS_Y,
                        QUADRINHO_X_1, QUADRINHO_PONTOS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_2, QUADRINHO_PONTOS_Y,
                        QUADRINHO_X_2, QUADRINHO_PONTOS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_3, QUADRINHO_PONTOS_Y,
                        QUADRINHO_X_3, QUADRINHO_PONTOS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_4, QUADRINHO_PONTOS_Y,
                        QUADRINHO_X_4, QUADRINHO_PONTOS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_5, QUADRINHO_PONTOS_Y,
                        QUADRINHO_X_5, QUADRINHO_PONTOS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_6, QUADRINHO_PONTOS_Y,
                        QUADRINHO_X_6, QUADRINHO_PONTOS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);

    PegaNumeroNaCasa(pontos, 6, nums);

    if(nums[5] >= 1)
    {
        draw_sprite(buffer, numeros[nums[5] - 1], QUADRINHO_X_1, QUADRINHO_PONTOS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_1, QUADRINHO_PONTOS_Y);
    }
    if(nums[4] >= 1)
    {
        draw_sprite(buffer, numeros[nums[4] - 1], QUADRINHO_X_2, QUADRINHO_PONTOS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_2, QUADRINHO_PONTOS_Y);
    }
    if(nums[3] >= 1)
    {
        draw_sprite(buffer, numeros[nums[3] - 1], QUADRINHO_X_3, QUADRINHO_PONTOS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_3, QUADRINHO_PONTOS_Y);
    }
    if(nums[2] >= 1)
    {
        draw_sprite(buffer, numeros[nums[2] - 1], QUADRINHO_X_4, QUADRINHO_PONTOS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_4, QUADRINHO_PONTOS_Y);
    }
    if(nums[1] >= 1)
    {
        draw_sprite(buffer, numeros[nums[1] - 1], QUADRINHO_X_5, QUADRINHO_PONTOS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_5, QUADRINHO_PONTOS_Y);
    }
    if(nums[0] >= 1)
    {
        draw_sprite(buffer, numeros[nums[0] - 1], QUADRINHO_X_6, QUADRINHO_PONTOS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_6, QUADRINHO_PONTOS_Y);
    }

}
void AtualizaPontuacaoBonus(BITMAP *fundo, BITMAP *buffer, int pontos,  BITMAP *numeros[10])
{
    int nums[6];

    /* Apaga os numeros antigos */
    blit(fundo, buffer, QUADRINHO_X_1, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_X_1, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_2, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_X_2, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_3, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_X_3, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_4, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_X_4, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_5, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_X_5, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_6, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_X_6, QUADRINHO_PONTOSBONUS_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);

    PegaNumeroNaCasa(pontos, 6, nums);

    if(nums[5] >= 1)
    {
        draw_sprite(buffer, numeros[nums[5] - 1], QUADRINHO_X_1, QUADRINHO_PONTOSBONUS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_1, QUADRINHO_PONTOSBONUS_Y);
    }
    if(nums[4] >= 1)
    {
        draw_sprite(buffer, numeros[nums[4] - 1], QUADRINHO_X_2, QUADRINHO_PONTOSBONUS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_2, QUADRINHO_PONTOSBONUS_Y);
    }
    if(nums[3] >= 1)
    {
        draw_sprite(buffer, numeros[nums[3] - 1], QUADRINHO_X_3, QUADRINHO_PONTOSBONUS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_3, QUADRINHO_PONTOSBONUS_Y);
    }
    if(nums[2] >= 1)
    {
        draw_sprite(buffer, numeros[nums[2] - 1], QUADRINHO_X_4, QUADRINHO_PONTOSBONUS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_4, QUADRINHO_PONTOSBONUS_Y);
    }
    if(nums[1] >= 1)
    {
        draw_sprite(buffer, numeros[nums[1] - 1], QUADRINHO_X_5, QUADRINHO_PONTOSBONUS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_5, QUADRINHO_PONTOSBONUS_Y);
    }
    if(nums[0] >= 1)
    {
        draw_sprite(buffer, numeros[nums[0] - 1], QUADRINHO_X_6, QUADRINHO_PONTOSBONUS_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_6, QUADRINHO_PONTOSBONUS_Y);
    }

}
int ExisteJogadaPossivel(JOIA matriz_grade[10][10])
{
    int i,j;
    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            if(matriz_grade[i][j].troca_cima ||
                matriz_grade[i][j].troca_esquerda ||
                matriz_grade[i][j].troca_direita ||
                matriz_grade[i][j].troca_baixo)
                    return 1;

    return 0;
}
void ProcuraDica(JOIA matriz_grade[10][10], BITMAP *buffer, BITMAP *fundo, BITMAP *Joias[8])
{
    int coord_clique[2][2];
    int i,j;


    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
        {
            if(matriz_grade[i][j].troca_cima)
            {
                coord_clique[0][0] = i;
                coord_clique[0][1] = j;
                coord_clique[1][0] = i - 1;
                coord_clique[1][1] = j;
                show_mouse(NULL);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                return;
            }
            else if(matriz_grade[i][j].troca_baixo)
            {
                coord_clique[0][0] = i;
                coord_clique[0][1] = j;
                coord_clique[1][0] = i + 1;
                coord_clique[1][1] = j;
                show_mouse(NULL);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                return;
            }
            else if(matriz_grade[i][j].troca_esquerda)
            {
                coord_clique[0][0] = i;
                coord_clique[0][1] = j;
                coord_clique[1][0] = i;
                coord_clique[1][1] = j - 1;
                show_mouse(NULL);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                return;
            }
            else if(matriz_grade[i][j].troca_direita)
            {
                coord_clique[0][0] = i;
                coord_clique[0][1] = j;
                coord_clique[1][0] = i;
                coord_clique[1][1] = j + 1;
                show_mouse(NULL);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                Troca(matriz_grade, coord_clique, buffer, fundo, Joias);
                BufferToScreen(buffer, screen);
                show_mouse(screen);
                return;
            }
        }

}
int PassouDeFase(int fase, int pontos)
{
    switch(fase)
    {
        case 1:
            if(pontos >= FASE_2_PONTOS)
                return 1;
            break;
        case 2:
            if(pontos >= FASE_3_PONTOS)
                return 1;
            break;
        case 3:
            if(pontos >= FASE_4_PONTOS)
                return 1;
            break;
        case 4:
            if(pontos >= FASE_5_PONTOS)
                return 1;
            break;
        case 5:
            if(pontos >= FASE_6_PONTOS)
                return 1;
            break;
    }

    return 0;
}
void AtualizaFase(BITMAP *fundo, BITMAP *buffer, int fase,  BITMAP *numeros[10])
{
    int nums[2];

    /* Apaga os numeros antigos */
    blit(fundo, buffer, QUADRINHO_X_1, QUADRINHO_FASE_Y,
                        QUADRINHO_X_1, QUADRINHO_FASE_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);
    blit(fundo, buffer, QUADRINHO_X_2, QUADRINHO_FASE_Y,
                        QUADRINHO_X_2, QUADRINHO_FASE_Y,
                        QUADRINHO_LARG,
                        QUADRINHO_ALT);

    PegaNumeroNaCasa(fase, 2, nums);

    if(nums[1] >= 1)
    {
        draw_sprite(buffer, numeros[nums[1] - 1], QUADRINHO_X_1, QUADRINHO_FASE_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_1, QUADRINHO_FASE_Y);
    }
    if(nums[0] >= 1)
    {
        draw_sprite(buffer, numeros[nums[0] - 1], QUADRINHO_X_2, QUADRINHO_FASE_Y);
    }
    else
    {
        draw_sprite(buffer, numeros[9], QUADRINHO_X_2, QUADRINHO_FASE_Y);
    }
}
void IniciaTempoFase(int fase)
{
    switch(fase)
    {
        case 1:
            tempo_restante = FASE_1_TEMPO;
            break;
        case 2:
            tempo_restante = FASE_2_TEMPO;
            break;
        case 3:
            tempo_restante = FASE_3_TEMPO;
            break;
        case 4:
            tempo_restante = FASE_4_TEMPO;
            break;
        case 5:
            tempo_restante = FASE_5_TEMPO;
            break;
        case 6:
            tempo_restante = FASE_6_TEMPO;
            break;
    }
}
void DeloadAll(BITMAP *Joias[8], BITMAP *fundo, BITMAP *buffer, BITMAP *buttons[3], BITMAP *numeros[10], BITMAP *mira)
{
    int i;

    for(i=0;i<8;i++)
        destroy_bitmap(Joias[i]);

    destroy_bitmap(fundo);
    destroy_bitmap(buffer);

    for(i=0;i<3;i++)
        destroy_bitmap(buttons[i]);

    for(i=0;i<10;i++)
        destroy_bitmap(numeros[i]);

    destroy_bitmap(mira);
}
void GamePause(BITMAP *buffer, BITMAP *fundo)
{
    BITMAP *tapajoias = load_bitmap(PATH_JOIA_TAPA, NULL);
    BITMAP *pause_buffer = create_bitmap(800,600);
    int i, j;

    if(!tapajoias)
    {
        printf("Erro ao carregar o bitmap tapajoias.tga\n");
        exit(1);
    }
    if(!pause_buffer)
    {
        printf("Erro ao carregar o bitmap *pause_buffer\n");
        exit(1);
    }

    blit(buffer, pause_buffer, 0, 0, 0, 0, 800, 600);
    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
        {
            ApagaQuadrado(i, j, pause_buffer, fundo);
            DrawJoia(tapajoias, pause_buffer, i, j);
        }

    show_mouse(NULL);
    blit(pause_buffer, screen, 0, 0, 0, 0, 800, 600);
    show_mouse(screen);

    while(1)
    {
        while((mouse_b & 1)) rest(1);
        while(!(mouse_b & 1)) rest(1);

        if(ClicouBotaoPause())
        {
            break;
        }
    }

    show_mouse(NULL);
    blit(buffer, screen, 0, 0, 0, 0, 800, 600);
    show_mouse(screen);

	destroy_bitmap(tapajoias);
	destroy_bitmap(pause_buffer);
}

void MarcaJoia(BITMAP *mira, BITMAP *buffer, int linha, int coluna)
{
	DrawJoia(mira, buffer, linha, coluna);
}

void DesmarcaJoia(BITMAP *buffer, BITMAP *fundo, BITMAP *joia, int linha, int coluna)
{
	ApagaQuadrado(linha, coluna, buffer, fundo);
	DrawJoia(joia, buffer, linha, coluna);
}

void GameOver(int pontos)
{
	BITMAP *buffer = create_bitmap(RESOLUCAO_X, RESOLUCAO_Y);
	BITMAP *botao;
	BITMAP *fundo;
	BITMAP *recorde;
	BITMAP *letras[26];

	CarregaBitmap(&botao, PATH_GAMEOVER_OK);
	CarregaBitmap(&fundo, PATH_GAMEOVER_SCREEN);
	CarregaBitmap(&recorde, PATH_GAMEOVER_RECORDE);
	SeparaFrames(PATH_LETRAS, letras, 26);

	int i,j;
	int botao_ok = 0;

	char nome[21];

	int rec_pontos[10];
	
	char c;
	char rec_nomes[10][21];

	while ((mouse_b & 1) || keypressed());

	botao = load_bitmap(PATH_GAMEOVER_OK, NULL);

	for(i=0;i<21;i++)
		nome[i] = 0;

	for(i=0;i<10;i++)
		rec_pontos[i] = 0;

	for(i=0;i<10;i++)
		for(j=0;j<21;j++)
			rec_nomes[i][j] = 0;

	CarregaRecordes(rec_nomes, rec_pontos);
	SeparaFrames(PATH_LETRAS, letras, 26);
	show_mouse(NULL);
	DrawBackground(fundo, buffer);
	show_mouse(screen);

	if(!EntrouNosRecordes(pontos, rec_nomes, rec_pontos))
	{
		show_mouse(NULL);
		BufferToScreen(buffer, screen);
		show_mouse(screen);
		clear_keybuf();
		while(!key[KEY_ESC] && !mouse_b);
		while (key[KEY_ESC] || mouse_b);
		clear_keybuf();
		return;
	}

	blit(recorde, buffer, 0, 0, GAMEOVER_RECORDE_X, GAMEOVER_RECORDE_Y, recorde->w, recorde->h);
	show_mouse(NULL);
	BufferToScreen(buffer, screen);
	show_mouse(screen);
	clear_keybuf();

	while(1)
	{
		if (GameOverEstaNoOk() && botao_ok == 0)
		{
			blit(recorde, buffer, 0, 0, GAMEOVER_RECORDE_X, GAMEOVER_RECORDE_Y, recorde->w, recorde->h);
			AtualizaNome(nome, buffer, recorde, letras);
			blit(botao, buffer, 0, 0, GAMEOVER_BOTAO_OK_X, GAMEOVER_BOTAO_OK_Y, GAMEOVER_BOTAO_OK_LARG, GAMEOVER_BOTAO_OK_ALT);
			show_mouse(NULL);
			BufferToScreen(buffer, screen);
			show_mouse(screen);
			botao_ok = 1;
		}

		if ((!GameOverEstaNoOk()) && botao_ok == 1)
		{
			blit(recorde, buffer, 0, 0, GAMEOVER_RECORDE_X, GAMEOVER_RECORDE_Y, recorde->w, recorde->h);
			AtualizaNome(nome, buffer, recorde, letras);
			show_mouse(NULL);
			BufferToScreen(buffer, screen);
			show_mouse(screen);
			botao_ok = 0;
		}

		if (GameOverEstaNoOk() && mouse_b & 1)
			break;

		if (keypressed())
		{
			if (key[KEY_ESC] || key[KEY_ENTER])
				break;
			else if (key[KEY_BACKSPACE])
			{
				RemoveUltimaLetraDoNome(nome);
				AtualizaNome(nome, buffer, recorde, letras);
				show_mouse(NULL);
				BufferToScreen(buffer, screen);
				show_mouse(screen);
		        clear_keybuf();
			}
			else
			{
				c = (readkey() & 0xFF);
				if (c >= 'a' && c <= 'z')
					c = c - 'a' + 'A';
				if (c >= 'A' && c <= 'Z')
				{
					AdicionaLetraAoNome(nome, c);
					show_mouse(NULL);
					AtualizaNome(nome, buffer, recorde, letras);
					BufferToScreen(buffer,screen);
			        show_mouse(screen);
			        clear_keybuf();
				}
			}
		}
	}

    while(mouse_b & 1);

    destroy_bitmap(buffer);
    destroy_bitmap(fundo);
    destroy_bitmap(recorde);
    destroy_bitmap(botao);

    for(i=0;i<26;i++)
    {
        destroy_bitmap(letras[i]);
    }

    InsereRecorde(nome, pontos, rec_nomes, rec_pontos);
    GravaOrdenado(rec_nomes, rec_pontos);
}

int EntrouNosRecordes(int pontos, char nomes[10][21], int rec_pontos[10]) /**/
{
	int i;
	CarregaRecordes(nomes, rec_pontos);
	for(i=0;i<10;i++)
		if(pontos > rec_pontos[i])
			return 1;

	return 0;
}

void AdicionaLetraAoNome(char nome[21], char letra) /**/
{
	if(strlen(nome) > 19)
		return;

	nome[strlen(nome)] = letra;
	nome[strlen(nome) + 1] = '\0';
}

void RemoveUltimaLetraDoNome(char nome[21]) /**/
{
	if((strlen(nome) - 1) < 0)
		return;

	nome[strlen(nome)] = 0;
	nome[strlen(nome) - 1] = '\0';
}

void AtualizaNome(char nome[21], BITMAP *buffer, BITMAP *recorde, BITMAP *letras[26])
{
	int i;
	blit(recorde, buffer, 0, 0, GAMEOVER_RECORDE_X, GAMEOVER_RECORDE_Y, recorde->w, recorde->h);
	for(i=0 ; nome[i] != 0 ; i++)
		draw_sprite(buffer, letras[nome[i] - 'A'], GAMEOVER_RECORDE_NOME_X + LETRAS_LARG * i, GAMEOVER_RECORDE_NOME_Y);
}

void CarregaRecordes(char nomes[10][21], int rec_pontos[10])
{
	FILE *recordes;
	int i;

	recordes = fopen("./recordes.rec", "r");

	for(i=0;i<10;i++)
	{
		nomes[i][0] = 0;
		rec_pontos[i] = 0;
	}

	fseek(recordes, 0, SEEK_SET);
	for(i=0;i<10;i++)
	{
		if((fread(&nomes[i][0], sizeof(char), 21, recordes) != 21) || (fread(&rec_pontos[i], sizeof(int), 1, recordes) != 1))
		{
			break;
		}
	}

	fclose(recordes);
}

void GravaOrdenado(char nomes[10][21], int rec_pontos[10])
{
	FILE *recordes;
	int i, j, maior = 0, maior_ind, gravado[10], total = 0;

	recordes = fopen("./recordes.rec", "w");

	fseek(recordes, 0, SEEK_SET);

	for(i=0;i<10;i++)
		gravado[i] = 0;

	while(total < 10)
	{
		maior = 0;
		maior_ind = 0;
		for(i=0;i<10;i++)
		{
			if(rec_pontos[i] >= maior && gravado[i] == 0)
			{
				maior = rec_pontos[i];
				maior_ind = i;
			}
		}

        gravado[maior_ind] = 1;

        for(j=0;j<21;j++)
            fwrite(&nomes[maior_ind][j], sizeof(char), 1, recordes);

        fwrite(&rec_pontos[maior_ind], sizeof(int), 1, recordes);

        /*if(fwrite(&nomes[maior_ind][0], sizeof(char), 21, recordes) != 21 || fwrite(&rec_pontos[maior_ind], sizeof(int), 1, recordes) != 1)
        {
            printf("Problema ao escrever o arquivo recordes.rec\n");
            break;
        }*/
        total++;
    }

    fclose(recordes);


}
void InsereRecorde(char nome[21], int pontos, char rec_nomes[10][21], int rec_pontos[10])
{
    int i, menor = rec_pontos[0], ind_menor = 0;

    for(i=1;i<10;i++)
        if(rec_pontos[i] < menor)
            ind_menor = i;

    rec_pontos[ind_menor] = pontos;
    for(i=0;i<21;i++)
    {
        rec_nomes[ind_menor][i] = nome[i];
    }
}
int GameOverEstaNoOk()
{
    if(mouse_x >= GAMEOVER_BOTAO_OK_X &&
        mouse_x <= (GAMEOVER_BOTAO_OK_X + GAMEOVER_BOTAO_OK_LARG) &&
        mouse_y >= GAMEOVER_BOTAO_OK_Y &&
        mouse_y <= (GAMEOVER_BOTAO_OK_Y + GAMEOVER_BOTAO_OK_ALT))
            return 1;

    return 0;
}


void TimerTempoContagem()
{
    if(!game_paused)
    {
		tempo_restante--;
		redraw_tempo = 1;
    }
}
END_OF_FUNCTION(TimerTempoContagem)
