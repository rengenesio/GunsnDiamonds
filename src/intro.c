#include "intro.h"

int main (void)
{
	int i=1;
	char a;

	srand(time(NULL));

	if (!start())
	{
		end();
		puts("Press enter...");
		scanf("%c", &a);
		a++;
		return -1;
	}

	IniciaJogo();
	Intro();

	while (i)
	{
		switch (Menu())
		{
			case 0:
				Jogo();
				break;
			case 1:
				Recordes();
				break;
			case 2:
				Opcoes();
				break;
			case 3:
				Ajuda();
				break;
			case 4:
				Creditos();
				break;
			case 5:
				Sair();
				i=0;
				break;
		}
	}


	end();

	return 0;
}
END_OF_MAIN()

/* ---------------------------------------- */

int start(void) {
	if (!testFiles()) {
		puts("Please reinstall the app.");
		return (FALSE);
	}

	allegro_init();

	if (install_keyboard()) {
		puts("App not started: keyboard error.");
		return (FALSE);
	}

	if (install_mouse() == -1){
		puts("App not started: mouse error.");
		return (FALSE);
	}
	install_timer();

	if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) < 0) {
		puts("App not started: sound error.");
		return (FALSE);
	}

	set_color_depth(COLOR_DEPTH);
	if(set_gfx_mode(GFX_AUTODETECT, RESOLUCAO_X, RESOLUCAO_Y, 0, 0) < 0) {
		puts("App not started: graphics error.");
		return (FALSE);
	}

	return (TRUE);
}

/* ---------------------------------------- */

int Menu (void)
{
	int i;
	int linha, ultimo_desenhado = -1;
	BITMAP * menu[6];
	SAMPLE * tiro;

	clear_keybuf();

	CarregaSom(&tiro, PATH_SOM_TIRO);
	CarregaBitmap(&fundomenu, PATH_FUNDOMENU);

	SeparaFrames(PATH_BOTOESMENU, menu, 6);

	if (!menu_ja_foi_aberto)
	{
		EfeitoAbrir(fundomenu,0,0, 1,1,0,0, 2);
		menu_ja_foi_aberto=1;
	}	
	else
	{
		show_mouse(NULL);
		draw_sprite(screen, fundomenu, 0,0);
		show_mouse(screen);
	}

	altura_botao = menu[0]->h;
	largura_botao = menu[0]->w;

	while (mouse_b & 1);
	while (!(mouse_b & 1) || (ultimo_desenhado == -1))
	{
		linha = (mouse_y - Y_INICIAL_BOTOES)/menu[0]->h;
		if(MouseEstaEmAlgumBotao() && (ultimo_desenhado != linha))
		{
			show_mouse(NULL);
			draw_sprite(screen, fundomenu, 0, 0);
			if(linha == 6)
				linha = 5; /* Workaround para bug de truncamento de float -> inteiro... =/ */

			draw_sprite(screen, menu[linha], X_INICIAL_BOTOES, linha*menu[0]->h + Y_INICIAL_BOTOES);
			show_mouse(screen);
			ultimo_desenhado = linha;
		}
		else if(!MouseEstaEmAlgumBotao() && (ultimo_desenhado != -1))
        {
            show_mouse(NULL);
            draw_sprite(screen, fundomenu, 0, 0);
            show_mouse(screen);
            ultimo_desenhado = -1;
        }
	}

	play_sample(tiro, 255, 128, 1000, 0);

	for (i=0; i<6 ; i++)
		destroy_bitmap(menu[i]);

	clear_keybuf();
	while (mouse_b&1);

	return linha;
}

/* ---------------------------------------- */

int MouseEstaEmAlgumBotao(void)
{
    if((mouse_x < X_INICIAL_BOTOES) || (mouse_x > X_INICIAL_BOTOES + largura_botao) || (mouse_y < Y_INICIAL_BOTOES) || (mouse_y >= Y_INICIAL_BOTOES + (altura_botao) * 6))
        return 0;

    return 1;
}

/* ---------------------------------------- */

int testFiles(void) {
	int i = TRUE;
	char temp[30];
	FILE *arquivo;
	FILE *tempor;

	if (!(arquivo = fopen(PATH_FILES, "r"))) 	{
		printf("Error! File not found: %s\n", PATH_FILES);
		return (FALSE);
	}

	while(fscanf(arquivo, "%s\n", temp) == EOF) {
		if (!(tempor = fopen(temp, "r"))) {
			printf("File not found: %s\n", temp);
			i = (FALSE);
		}
		else
			fclose(tempor);
	}

	fclose(arquivo);

	if (!(tempor = fopen("./recordes.rec", "r"))) {
		if (!(tempor = fopen("./recordes.rec", "w"))) {
			puts("File not found: recordes.rec");
			puts("Unable to create file recordes.rec");

			i = (FALSE);
		}
		else
			fclose(tempor);
	}
	else
		fclose(tempor);

	return i;
}

/* ---------------------------------------- */

void Ajuda (void)
{
	int estasobre=0;
	BITMAP * ajuda;
	BITMAP * botaomenu;

	CarregaBitmap(&ajuda, PATH_AJUDA);
	CarregaBitmap(&botaomenu, PATH_BOTAOMENU);

	EfeitoAbrir(ajuda,1, fundomenu, 1,1,0,0,20);
	while (!key[KEY_ESC])
	{
		if (mouse_x>=X_MENU && mouse_x<=X_MENU + botaomenu->w && mouse_y>=Y_MENU && mouse_y<=Y_MENU + botaomenu->h)
		{
			if (estasobre==0)
			{
				show_mouse(NULL);
				draw_sprite(screen, botaomenu,X_MENU, Y_MENU);
				show_mouse(screen);
				estasobre=1;
			}
			if (mouse_b & 1)
			{
				while (mouse_b & 1);
				break;
			}
		}
		if ((mouse_x<X_MENU || mouse_x>X_MENU + botaomenu->w || mouse_y<Y_MENU || mouse_y>Y_MENU + botaomenu->h) && estasobre==1)
		{
			estasobre=0;
			show_mouse(NULL);
			draw_sprite(buffer, ajuda,0, 0);
			draw_sprite(screen, buffer,0,0);
			show_mouse(screen);
		}
	}
	destroy_bitmap(ajuda);
	destroy_bitmap(botaomenu);
}

/* ---------------------------------------- */

void CarregaBitmap (BITMAP ** bitmap, char nome[])
{
	if (!(*bitmap = load_bitmap(nome, NULL)))
	{
		printf("Erro no arquivo %s\nPor favor reinstale o aplicativo.\n", nome);
		end();
	}
}


/* ---------------------------------------- */

void CarregaSom (SAMPLE ** som, char nome[])
{
	if (!(*som = load_sample(nome)))
	{
		printf("Erro no arquivo %s\nPor favor reinstale o aplicativo.\n", nome);
		end();
	}
}

/* ---------------------------------------- */

void Creditos (void)
{
	int i, aplauso = 0;

	BITMAP * atira[4];
	BITMAP * creditos;
	BITMAP * explode[20];
	BITMAP * granada;
	BITMAP * mendigo[12];
	BITMAP * recarrega[19];
	BITMAP * rocketeiro[11];
	BITMAP * tanque[12];

	SAMPLE * aplausos;
	SAMPLE * explosao;
	SAMPLE * recarrega1;
	SAMPLE * tiro;

	SeparaFrames(PATH_ATIRA, atira, 4);
	CarregaBitmap(&creditos, PATH_CREDITOS);
	SeparaFrames(PATH_EXPLODE, explode, 20);
	CarregaBitmap(&granada, PATH_GRANADA);
	SeparaFrames(PATH_MENDIGOANDA, mendigo, 12);
	SeparaFrames(PATH_RECARREGA, recarrega, 19);
	SeparaFrames(PATH_ROCKETEIRO, rocketeiro, 11);
	SeparaFrames(PATH_TANQUE, tanque, 12);

	CarregaSom (&aplausos, PATH_SOM_APLAUSOS);
	CarregaSom(&explosao, PATH_SOM_EXPLOSAO);
	CarregaSom (&recarrega1, PATH_SOM_RECARREGA);
	CarregaSom (&tiro, PATH_SOM_TIRO);
	
	clear_keybuf();

	for (i=0; i<1048 && !keypressed() && !(mouse_b & 1) ; i++)
	{
		show_mouse(NULL);
		clear_to_color(buffer,0);
		blit(creditos, buffer, 0,0, (RESOLUCAO_X - creditos->w)/2,RESOLUCAO_Y-i,creditos->w, i );
	
		/* Tacow Berry */
		if (i>=Y_TACOWBERRY && i<Y_FINAL_TACOWBERRY)
		{
			if (i>=Y_RECARREGA && i<Y_FINAL_RECARREGA)
			{
				draw_sprite(buffer,recarrega[(i/4)%19], X_CREDITOS, RESOLUCAO_Y - i + Y_TACOWBERRY);
				if (i==Y_RECARREGA+48)
					play_sample(recarrega1, 255, 128, 1000, 0);
			}

			else
			{
				draw_sprite(buffer,atira[(i/4)%4], X_CREDITOS, RESOLUCAO_Y - i + Y_TACOWBERRY);
				if ((i/4)%4 == 0)
				{
					stop_sample(tiro);
					play_sample(tiro, 255/2, 128, 900, 0);
				}
			}
		}

		/* Granada */
		if (i>=Y_GRANADA && i<Y_FINAL_GRANADA)
			rotate_sprite(buffer, granada, X_CREDITOS+(i-Y_GRANADA)/2, RESOLUCAO_Y - i + Y_GRANADA, itofix(i*4));
		if (i>=Y_FINAL_GRANADA && i<Y_FINAL_GRANADA+40)
		{
			draw_sprite(buffer,explode[(i-Y_FINAL_GRANADA)/2], X_CREDITOS + 183, RESOLUCAO_Y - i + Y_GRANADA - (explode[0]->h));
			if (i==Y_FINAL_GRANADA)
				play_sample(explosao, 255, 128, 1000, 0);
 		}
		
		/* Rocketeiro */
		if (i>= Y_ROCKETEIRO && i<Y_ROCKETEIRO+300)
			draw_sprite(buffer, rocketeiro[0], X_CREDITOS-20, RESOLUCAO_Y - i + Y_ROCKETEIRO);
		if (i>=Y_ROCKETEIRO+300 && i<608)
			draw_sprite(buffer, rocketeiro[((i-Y_ROCKETEIRO-300)/3)], X_CREDITOS-20, RESOLUCAO_Y - i + Y_ROCKETEIRO);
		if (i>=608 && i<648)
			draw_sprite(buffer,explode[(i-608)/2], X_CREDITOS, RESOLUCAO_Y - i +Y_ROCKETEIRO+25 - (explode[0]->h));

		/* Serjipe */
		if (i>= Y_TANQUE && i<Y_FINAL_TANQUE)
			rotate_sprite_v_flip(buffer, tanque[(i/3)%5 +7], X_CREDITOS - 10, RESOLUCAO_Y - i + Y_TANQUE , itofix(128));

		/* Mendigo */
		if (i>=Y_MENDIGO && i<Y_FINAL_MENDIGO)
			draw_sprite(buffer,mendigo[(i/3)%12], X_CREDITOS, RESOLUCAO_Y - i + Y_MENDIGO);

		/* Aplausos */		
		if (i==900)
		{
			play_sample(aplausos, 255, 128,1000,0);
			aplauso = 1;
		}
		if (i!=1047)
			show_mouse(buffer);

		draw_sprite(screen, buffer, 0, 0);
		rest(10);
	}
	clear_keybuf();
	while (mouse_b & 1 || keypressed());
	clear_keybuf();
	show_mouse(NULL);
	clear(buffer);
	if (!aplauso)
		play_sample(aplausos, 255, 128,1000,0);
	blit(creditos, buffer, 0,0, (RESOLUCAO_X - creditos->w)/2,RESOLUCAO_Y-1048,creditos->w, 1048 );
	draw_sprite(screen, buffer, 0, 0);
	show_mouse(screen);
	while (!keypressed() && !(mouse_b & 1));
	clear_keybuf();

	stop_sample(aplausos);

	for (i=0; i<20 ; i++)
	{
		if (i==0)
		{
			destroy_bitmap(granada);
			destroy_bitmap(creditos);
			destroy_sample(aplausos);
			destroy_sample(explosao);
			destroy_sample(recarrega1);
			destroy_sample(tiro);
		}
		if (i<4)
			destroy_bitmap(atira[i]);
		if (i<11)
			destroy_bitmap(rocketeiro[i]);
		if (i<12)
		{
			destroy_bitmap(mendigo[i]);
			destroy_bitmap(tanque[i]);
		}
		if (i<19)
			destroy_bitmap(recarrega[i]);
		destroy_bitmap(explode[i]);
	}
}

/* ---------------------------------------- */

void EfeitoAbrir (BITMAP * bitmap, int comfundo, BITMAP * fundooo, int horizontalmente, int verticalmente, int pos_x, int pos_y, int velocidade)
{
	BITMAP * fundo;
	int efeitovertical;
	float efeitohorizontal;
	fundo = create_bitmap(RESOLUCAO_X, RESOLUCAO_Y);
	if (comfundo==0)
	{
		BITMAP *fundoo;
		fundoo = create_bitmap(RESOLUCAO_X, RESOLUCAO_Y);
		clear_to_color(fundoo,0);
		draw_sprite(fundo, fundoo,0,0);	
	}
	else
		draw_sprite(fundo, fundooo,0,0);
	clear(buffer);
	clear_keybuf();
	while (mouse_b & 1);
	if (horizontalmente)
	{
		if (verticalmente)
		{
			for (efeitohorizontal=0, efeitovertical=0; efeitovertical<=RESOLUCAO_Y/2 && !keypressed() && !(mouse_b & 1); efeitohorizontal+=velocidade*((float)bitmap->w/(float)bitmap->h), efeitovertical+=velocidade)
			{
				show_mouse(NULL);
				draw_sprite(buffer,fundo,0,0);
				blit (bitmap,buffer, bitmap->w/2 - efeitohorizontal, bitmap->h/2-efeitovertical, bitmap->w/2 - efeitohorizontal + pos_x , (bitmap->h)/2 -efeitovertical + pos_y, efeitohorizontal*2,efeitovertical*2);
				if (efeitovertical+velocidade<=RESOLUCAO_Y/2)
					show_mouse(buffer);
				draw_sprite(screen, buffer,0,0);
				rest(1);
			}
		}
		else
		{
			for (efeitohorizontal=0; efeitohorizontal<=RESOLUCAO_X/2 && !keypressed() && !(mouse_b & 1); efeitohorizontal+=velocidade)
			{
				show_mouse(NULL);
				draw_sprite(buffer,fundo,0,0);
				blit (bitmap,buffer, bitmap->w/2 - efeitohorizontal, 0, (bitmap->w)/2 - efeitohorizontal + pos_x , pos_y, efeitohorizontal*2,bitmap->h);
				if (efeitohorizontal+velocidade<=RESOLUCAO_X/2)
					show_mouse(buffer);
				draw_sprite(screen, buffer,0,0);
				rest(1);
			}
		}
	}
	else
	{
		for (efeitovertical=0; efeitovertical<=RESOLUCAO_Y/2 && !keypressed() && !(mouse_b & 1); efeitovertical+=velocidade)
		{
			show_mouse(NULL);
			if (fundo)
				draw_sprite(buffer,fundo,0,0);
			draw_sprite(buffer,fundo,0,0);
			blit (bitmap,buffer, 0, bitmap->h/2 - efeitovertical, pos_x, (bitmap->h)/2 - efeitovertical + pos_y, bitmap->w, efeitovertical*2);
			if (efeitovertical+velocidade<=RESOLUCAO_Y/2)
			{
				show_mouse(buffer);
			}
			draw_sprite(screen, buffer,0,0);
			rest(1);
		}
	}
	show_mouse(NULL);
	draw_sprite(screen, bitmap, pos_x,pos_y);
	show_mouse(screen);
	while (mouse_b &1);
}

/* ---------------------------------------- */

void Explosao (BITMAP*bitmap, float tamanho, int pos_x, int pos_y)
{
	int vertices[32] = {35*tamanho ,1*tamanho , 58*tamanho ,22*tamanho , 71*tamanho  ,1*tamanho,66*tamanho ,26*tamanho ,90*tamanho  ,15*tamanho  ,73*tamanho  ,42*tamanho  ,107*tamanho  ,64*tamanho  ,72*tamanho  ,68*tamanho  ,90*tamanho  ,104*tamanho  ,63*tamanho  ,85*tamanho  ,49*tamanho  ,102*tamanho  ,46*tamanho  ,63*tamanho  ,14*tamanho  ,72*tamanho  ,34*tamanho  ,49*tamanho  ,4*tamanho  ,39*tamanho  ,41*tamanho  ,31*tamanho };
	int cont;
	for (cont=0; cont<32 ; cont++)
	{
		if (cont%2)
			vertices[cont]+=pos_y;
		else
			vertices[cont]+=pos_x;
	}
	polygon (bitmap, 16, vertices, makecol(255,0,255));
}

/* ---------------------------------------- */

void FadeIn(BITMAP *bitmap, int velocidade)
{
	int a;
	clear(buffer);
	for (a=0; a<256; a+=velocidade)
	{
		clear(buffer);
		set_trans_blender(0,0,0,a);
		draw_trans_sprite(buffer, bitmap, 0, 0);
		vsync();
		blit(buffer, screen, 0,0, 0,0, buffer->w, buffer->h);
		rest(10);
	}
	blit(bitmap, screen, 0,0, 0,0, bitmap->w, bitmap->h);
}

/* ---------------------------------------- */

void FadeOut(int velocidade)
{
    BITMAP *bmp_orig;
	int i;

    if ((bmp_orig = create_bitmap(RESOLUCAO_X, RESOLUCAO_Y)))
    {
		clear(buffer);
		blit(screen, bmp_orig, 0,0, 0,0, RESOLUCAO_X, RESOLUCAO_Y);
		if (velocidade <= 0)
			velocidade = 16;

		for (i = 255-velocidade; i > 0; i-=velocidade)
		{
			clear(buffer);
			set_trans_blender(0,0,0,i);
			draw_trans_sprite(buffer, bmp_orig, 0, 0);
			vsync();
			blit(buffer, screen, 0,0, 0,0, RESOLUCAO_X, RESOLUCAO_Y);
		}
		destroy_bitmap(bmp_orig);
	}
	rectfill(screen, 0,0, SCREEN_W,SCREEN_H, makecol(0,0,0));
}

/* ---------------------------------------- */

void end (void)
{
	allegro_exit();
}

/* ---------------------------------------- */

void IniciaJogo (void)
{
	buffer = create_bitmap (RESOLUCAO_X, RESOLUCAO_Y);
	
	CarregaBitmap(&mouse, PATH_MIRA);
	set_mouse_sprite(mouse);
	set_mouse_sprite_focus((mouse->w)/2, (mouse->h)/2);

	if (!(arqopcoes = fopen("opcoes", "r")))
	{
		arqopcoes = fopen("opcoes", "w");
		fprintf(arqopcoes, "%d", volume = 128);
		fclose(arqopcoes);
	}
	else
	{
		fscanf(arqopcoes, "%d", &volume);
		fclose(arqopcoes);
	}

	if (volume>255)
		volume = 255;
	if (volume<0)
		volume = 0;
	set_volume(volume,volume);
}

/* ---------------------------------------- */

void Intro (void)
{
	int i, j=230, k=144;

	BITMAP * abaixa[4];
	BITMAP * anda[9];
	BITMAP * apresenta;
	BITMAP * atira[4];
	BITMAP * bomba;
	BITMAP * explode[20];
	BITMAP * explode2[20];
	BITMAP * fundo;
	BITMAP * granada;
	BITMAP * logotipo;
	BITMAP * mendigo[8];
	BITMAP * mendigoanda[12];
	BITMAP * missel;
	BITMAP * parado[3];
	BITMAP * pula[7];
	BITMAP * queima[20];
	BITMAP * recarrega[19];
	BITMAP * rocket[3];
	BITMAP * tacabomba[5];
	BITMAP * tanque[12];
	BITMAP * vira[2];

	SAMPLE * explosao;
	SAMPLE * grito;
	SAMPLE * missile;
	SAMPLE * passos;
	SAMPLE * recarrega1;
	SAMPLE * tank;
	SAMPLE * tiro;

	TocaMp3(&musicaintro, PATH_SOM_MUSICAINTRO);
	install_int_ex(MusicaIntro, 10000);

	SeparaFrames(PATH_ABAIXA, abaixa,4);
	SeparaFrames(PATH_ANDA, anda, 9);
	CarregaBitmap(&apresenta, PATH_APRESENTA);
	SeparaFrames(PATH_ATIRA, atira, 4);
	CarregaBitmap(&bomba, PATH_BOMBA);
	SeparaFrames(PATH_EXPLODE, explode, 20);
	SeparaFrames(PATH_EXPLODE2, explode2, 20);
	CarregaBitmap(&fundo, PATH_MURO);	
	CarregaBitmap(&granada, PATH_GRANADA);
	CarregaBitmap(&logotipo, PATH_LOGOTIPO);
	SeparaFrames(PATH_MENDIGO, mendigo, 8);
	SeparaFrames(PATH_MENDIGOANDA, mendigoanda, 12);
	CarregaBitmap(&missel, PATH_MISSEL);
	SeparaFrames(PATH_PARADO, parado, 3);
	SeparaFrames(PATH_PULA, pula, 7);
	SeparaFrames(PATH_QUEIMA, queima, 20);
	SeparaFrames(PATH_RECARREGA, recarrega, 19);
	SeparaFrames(PATH_ROCKET,rocket, 3);
	SeparaFrames(PATH_TACABOMBA, tacabomba, 5);
	SeparaFrames(PATH_TANQUE, tanque, 12);
	SeparaFrames(PATH_VIRA, vira, 2);

	CarregaSom (&explosao, PATH_SOM_EXPLOSAO);
	CarregaSom (&grito, PATH_SOM_GRITO);
	CarregaSom (&missile, PATH_SOM_MISSILE);
	CarregaSom (&passos, PATH_SOM_PASSOS);
	CarregaSom (&recarrega1, PATH_SOM_RECARREGA);
	CarregaSom (&tank, PATH_SOM_TANK);
	CarregaSom (&tiro, PATH_SOM_TIRO);

	FadeIn(fundo, 15);

	for (i=0; i<730 && !keypressed() && !(mouse_b & 1) ;i++)
	{
		clear(buffer);
		draw_sprite(buffer, logotipo,0,0);
		draw_sprite(buffer, fundo,0,0);

		if (i<200) /* Mendigo correndo pra cima do Tacow Berry */
			rotate_sprite_v_flip(buffer, mendigo[i%8], 800-i*8, 200, itofix(128));

		if (i==1)
			play_sample(passos, 255, 128,1000,1);

		if (i<40) /* Tacow Berry correndo pra tacar a bomba */
			draw_sprite(buffer,anda[i%9], i*6, 200);

		if (i>=40 && i<45) /* Tacow Berry tacando bomba */
		{
			draw_sprite(buffer,tacabomba[(i-40)%5], 240, 200);
			stop_sample(passos);
		}

		if (i>=45 && i<47) /* Tacow Berry virando */
			draw_sprite(buffer, vira[i-45],240, 200);

		if (i>=40 && j<319) /* Bomba girando e caindo */
		{
			rotate_sprite(buffer, granada, j+20, (j-319)*(j-319)*0.002 + 180, itofix(j*3));
			j+=5;
		}

		if (i>=57 && i<77) /* Bomba explodindo */
		{
			draw_sprite(buffer, explode[i-57],j-(explode[0]->w)/2+20,(j-319)*(j-319)*0.002 + 188 - explode[0]->h);
			if (i==57)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i==58) /* Parede explodindo */
			Explosao (fundo, 1, 280, 118);

		if (i>=47 && i<97) /* Tacow Berry saindo para pegar tanque */
		{
			rotate_sprite_v_flip(buffer, anda[(i-47)%9], 240-(i-47)*6, 200, itofix(128));
			if (i==47)
				play_sample(passos, 255, 128,1000,1);
		}
		if (i==92)
			stop_sample(passos);

		if (i>=124 && i<283) /* Mendigo fugindo do tanque */
			draw_sprite(buffer, mendigo[(i-124)%8], (i-140)*6, 200);

		if (i>=160 && i<=228) /* Tanque andando até enguiçar */
		{
			draw_sprite(buffer, tanque[(i-160)%12], (i-170)*6, 180);
			if (i == 160 || i == 185)
				play_sample(tank, 255/2, 128, 1000, 0);
			if (i==228)
				stop_sample(tank);
		}

		if (i>190 && i<207) /* Bomba do tanque*/
		{
			rotate_sprite(buffer, bomba, (i-190)*20+215, 200-(i-190)*2, itofix(i*4));
			if (i==206)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=207 && i<227) /* Bomba do tanque explodindo */
		{
			j = 538;
			draw_sprite(buffer, explode[i-207],j-(explode[0]->w)/2,(j-319)*(j-319)*0.002 + 121 - explode[0]->h);
			Explosao (fundo, 2, 430, 100);
		}

		if (i==238) /* Tacow Berry saindo do tanque */
			draw_sprite(buffer, pula[0],398, 170-(i-238)*7);

		if (i==239) /* Tacow Berry saindo do tanque */
			draw_sprite(buffer, pula[0],398, 170-(i-238)*5);

		if (i==240) /* Tacow Berry saindo do tanque */
			draw_sprite(buffer, pula[0],398, 170-(i-238));

		if (i>=228 && i<356) /* Tanque enguiçado */
			draw_sprite(buffer, tanque[10], 348, 180);

		if (i>=241 && i<247) /* Tacow Berry saindo do tanque */
			draw_sprite(buffer, pula[1],398, 170+(i-238)*4 -6);

		if (i>=247 && i<252) /* Tacow Berry saindo do tanque */
			draw_sprite(buffer, pula[i-245],398, 197);

		if (i==252)
			play_sample(passos, 255, 128, 1000, 1);

		if (i>=252 && i<300) /* Tacow Berry andando pra explodir o tanque */
			draw_sprite(buffer,anda[i%9], 398+(i-252)*6, 200);

		if (i==300)
			stop_sample(passos);

		if (i>=300 && i<303) /* Tacow Berry virando pra atirar no tanque */
			draw_sprite(buffer, vira[(i-300)/2], 690, 200);

		if (i>=303 && i<341) /* Tacow Berry recarregando */
		{
			rotate_sprite_v_flip(buffer, recarrega[(i-303)/2], 685, 200, itofix(128));
			if (i==335)
				play_sample(recarrega1, 255, 128, 1000, 0);
		}

		if (i>=341 && i<354) /* Tacow Berry atirando */
		{
			rotate_sprite_v_flip(buffer, atira[(i-341)%4], 676, 201, itofix(128));
			if (i==341 || i==347 || i==353)
				play_sample(tiro, 255, 128, 1000, 0);
		}

		if (i>=354 && i<374) /* Tanque explodindo */
		{
			draw_sprite(buffer, explode2[i-354], 388, 140);
			if (i==354)
				play_sample(explosao, 255, 128, 1000, 0);
			Explosao (fundo, 2.5, 290, 90);
			rectfill(fundo, 433, 177, 513, 237, makecol(255,0,255));
			rectfill(fundo, 489, 141, 517, 168, makecol(255,0,255));
			rectfill (fundo, 338, 194, 377, 248, makecol(255,0,255));
		}
			
		if (i>=308 && i<365) /* Rocketeiro subindo */
			draw_sprite(buffer,rocket[0], 300, RESOLUCAO_Y-i+308);

		if (i>=365 && i<409) /* Rocketeiro lançando missel */
		{
			rotate_sprite(buffer, missel, 320+(i-308-rocket[0]->h)*10, 532-(i-308-rocket[0]->h)*10, itofix(-32));
			draw_sprite(buffer,rocket[0], 300, RESOLUCAO_Y-rocket[0]->h);
			if (i==365)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=354 && i<395) /* Tacow Berry descansando */
			rotate_sprite_v_flip(buffer, parado[((i-353)/6)%3], 695, 198, itofix(128));
		
		if (i>=395 && i<411) /* Tacow Berry abaixando */
			rotate_sprite_v_flip(buffer, abaixa[(i-395)/4], 695, 198, itofix(128));
		
		if (i>=409 && i<429) /* Missel explodindo */
		{
			draw_sprite(buffer, explode2[i-409], 690, 70);
			Explosao(fundo, 1.6, 640,70);
			if (i==409)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=411 && i<416) /* Tacow Berry abaixado */
			rotate_sprite_v_flip(buffer, abaixa[3], 695, 198, itofix(128));

		if (i>=416 && i<428) /* Tacow Berry levantando */
			rotate_sprite_v_flip(buffer, abaixa[(431-i)/4], 695, 198, itofix(128));

		if (i>=428 && i<549) /* Tacow Berry correndo */
		{
			rotate_sprite_v_flip(buffer, anda[(i-428)%9], 690-(i-428)*6, 198, itofix(128));
			if (i==428)
				play_sample(passos, 255, 128, 1000, 1);
		}

		if (i>=409 && i<443) /* Rocketeiro lançando segundo missel */
		{
			rotate_sprite(buffer, missel, 320+(i-409)*9, 532-(i-409)*10, itofix(-38));
			draw_sprite(buffer,rocket[0], 300, RESOLUCAO_Y-rocket[0]->h);
			if (i==409)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=443 && i<463) /* Missel 2 explodindo */
		{
			draw_sprite(buffer, explode2[i-443], 601, 122);
			Explosao(fundo, 1.6, 541,120);
			rectfill(fundo, 573, 168, 638, 220, makecol(255,0,255));
			rectfill(fundo, 632, 113, 727, 201, makecol(255,0,255));
			rectfill(fundo, 673, 193, 721, 224, makecol(255,0,255));
			rectfill(fundo, 577, 148, 629, 179, makecol(255,0,255));
			if (i==443)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=443 && i<489) /* Rocketeiro lançando terceiro missel */
		{
			rotate_sprite(buffer, missel, 300+(i-443)*3.5, 532-(i-443)*10, itofix(-48));
			draw_sprite(buffer,rocket[1], 300, RESOLUCAO_Y-rocket[1]->h);
			if (i==443)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=476 && i<510) /* Rocketeiro lançando quarto missel */
		{
			if (i<480)
				draw_sprite(buffer,rocket[1], 300, RESOLUCAO_Y-rocket[1]->h);
			rotate_sprite(buffer, missel, 300+(i-476)*3.3, 532-(i-476)*10, itofix(-50));
			if (i==476)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=489 && i<509) /* Terceiro missel explodindo */
		{
			draw_sprite(buffer, explode2[i-489], 410, 0);
			draw_sprite(buffer, explode2[i-489], 430, 0);
			draw_sprite(buffer, explode2[i-489], 440, 0);
			Explosao(fundo, 4, 271,-100);
			rectfill(fundo, 573, 168, 638, 220, makecol(255,0,255));
			rectfill(fundo, 539, 146, 623, 279, makecol(255,0,255));
			rectfill(fundo, 500, 0, 556, 12, makecol(255,0,255));
			if (i==489)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=509 && i<529) /* Quarto missel explodindo */
		{
			draw_sprite(buffer, explode2[i-509], 410, 100);
			Explosao(fundo, 2, 271,50);
			rectfill(fundo, 371, 70, 424, 111, makecol(255,0,255));
			if (i==509)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=489 && i<521) /* Rocketeiro lancando 5 missel */
		{
			if (i<492)
				draw_sprite(buffer,rocket[2], 300, RESOLUCAO_Y-rocket[1]->h);
			rotate_sprite(buffer, missel, 290, 532-(i-489)*10, itofix(-64));
			if (i==489)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=521 && i<541) /* Missel 5 explodindo */
		{
			draw_sprite(buffer, explode2[i-521], 270, 120);
			Explosao(fundo, 4, 111,00);
			rectfill(fundo, 384, 280, 429, 303, makecol(255,0,255));
			rectfill(fundo, 304, 59, 370, 207, makecol(255,0,255));
			rectfill(fundo, 384, 255, 418, 286, makecol(255,0,255));
			if (i==521)
				play_sample(explosao, 255, 128, 1000, 0);
		}
		if (i>=492 && i<512)
			rotate_sprite_v_flip(buffer, rocket[1], 300, RESOLUCAO_Y-rocket[1]->h, itofix(128));

		if (i>=495 && i<527) /* Missel 6 lançado */
		{
			rotate_sprite(buffer, missel, 290-(i-500)*2.7, 532-(i-495)*10, itofix(-85));
			if (i==495)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=527 && i<547) /* Missel 6 explodindo */
		{
			draw_sprite(buffer, explode2[i-527], 170, 120);
			Explosao(fundo, 2, 100,80);
			rectfill(fundo, 200, 20, 280, 130, makecol(255,0,255));
			rectfill(fundo, 170, 195, 198, 272, makecol(255,0,255));
			if (i==527)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=512 && i<619)
			rotate_sprite_v_flip(buffer, rocket[0], 300, RESOLUCAO_Y-rocket[0]->h, itofix(128));
		
		if (i>=515 && i<550)/* Missel 7 lançado (eu juro que esse é o ultimo missel xD) */
		{
			rotate_sprite(buffer, missel, 290-(i-515)*6.5, 532-(i-515)*10, itofix(-96));
			if (i==515)
				play_sample(missile, 255/2, 128, 1000, 0);
		}

		if (i>=550 && i<570) /* Missel 7 explodindo */
		{
			draw_sprite(buffer, explode2[i-550], 20, 90);
			draw_sprite(buffer, explode2[i-550], 30, 95);
			draw_sprite(buffer, explode2[i-550], 40, 90);
			draw_sprite(buffer, explode2[i-550], 50, 95);
			Explosao(fundo, 4, -100,-50);
			rectfill(fundo, 0, 0, 73, 100, makecol(255,0,255));
			if (i==550)
				play_sample(explosao, 255, 128, 1000, 0);
		}

		if (i>=565 && i<=584)
			draw_sprite(buffer,anda[(i-565)%9], (i-565)*6, 500);
		
		if (i==584)
			stop_sample(passos);

		if (i>=584 && i<589) /* Tacow Berry tacando a bomba */
			draw_sprite(buffer,tacabomba[i-584], 114, 500);

		if (i>=584 && i<619) /* Bomba girando e caindo */
		{
			rotate_sprite(buffer, granada, k, (k-144)*(k-144)*0.002 + 500, itofix(k*3));
			k+=5;
		}

		if (i==619)
			play_sample(explosao, 255, 128, 1000, 0);
		if (i>=619 && i<639) /* ultima bomba explodindo */
			draw_sprite(buffer,explode[i-619], 300, RESOLUCAO_Y-explode[0]->h);

		if (i==625)
			play_sample(grito, 255, 128, 1000,0);

		if (i>=625 && i<645) /* Rocketeiro queimando */
			draw_sprite(buffer,queima[i-625], 285, RESOLUCAO_Y-queima[0]->h);
		
		if (i>=645) /* Esqueleto Roqueteiro */
			draw_sprite(buffer,queima[19], 285, RESOLUCAO_Y-queima[0]->h);
		
		if (i>=589 && i<610) /* Tacow Berry parado */
		{
			stop_sample(passos);
			draw_sprite(buffer,parado[((i-589)/4)%3], 114, 500);
		}

		if (i>=623) /* Aparecendo o "Orgulhosamente Apresenta" */
			blit(apresenta,buffer, 0,0, (RESOLUCAO_X-apresenta->w)/2, 510, (i-623)*6, apresenta->h);

		if (i==610)
			play_sample(passos, 255, 128, 1000, 1);

		if (i>=610) /* Tacow berry saindo de cena */
			draw_sprite(buffer,anda[(i-609)%9], (i-610)*6+114, 500);
			
		if (i>=610 && i<656) /* Mendigo passeando feliz e contente */
			rotate_sprite_v_flip(buffer,mendigoanda[((i-610)/2)%12], RESOLUCAO_X - (i-610)*2, 293, itofix(128));
		
		if (i>=625 && i<656) /* Bomba caindo na cabeca do mendigo */
			rotate_sprite(buffer, granada, 700, (i-625)*10, itofix((i-625)*3));
		
		if (i==656) /* Bomba explodindo */
			play_sample(explosao, 255, 128, 1000, 0);

		if (i>=656 && i<676)
		{
			draw_sprite(buffer,explode2[i-656], 670, 340-explode2[0]->h);
			Explosao(fundo, 3, 500, 130);
		}

		if (i==660)
			play_sample(grito, 255, 128, 1000,0);
		if (i>=660 && i<680)
			draw_sprite(buffer,queima[i-660], 670, 271);
		
		if (i>=680)
			draw_sprite(buffer,queima[19], 670, 271);
		draw_sprite(screen,buffer,0,0);
		if (i==725)
			stop_sample(passos);

		rest(37);
	}
	FadeOut(15);

	for (i=0; i<20 ;i++)
	{
		if (i==0)
		{
			destroy_bitmap(apresenta);
			destroy_bitmap(bomba);
			destroy_bitmap(fundo);
			destroy_bitmap(granada);
			destroy_bitmap(logotipo);
			destroy_bitmap(missel);
			destroy_sample(explosao);
			destroy_sample(grito);
			destroy_sample(missile);
			destroy_sample(passos);
			destroy_sample(recarrega1);
			destroy_sample(tank);
			destroy_sample(tiro);
		}

		if (i<2)
			destroy_bitmap(vira[i]);

		if (i<3)
		{
			destroy_bitmap(parado[i]);
			destroy_bitmap(rocket[i]);
		}

		if (i<4)
		{
			destroy_bitmap(abaixa[i]);
			destroy_bitmap(atira[i]);
		}
		if (i<5)
			destroy_bitmap(tacabomba[i]);

		if (i<7)
			destroy_bitmap(pula[i]);

		if (i<8)
			destroy_bitmap(mendigo[i]);

		if (i<9)
			destroy_bitmap(anda[i]);

		if (i<12)
		{
			destroy_bitmap(mendigoanda[i]);
			destroy_bitmap(tanque[i]);
		}

		if (i<19)
			destroy_bitmap(recarrega[i]);

		destroy_bitmap(explode[i]);
		destroy_bitmap(explode2[i]);
		destroy_bitmap(queima[i]);
	}
}

/* ---------------------------------------- */

void Jogo (void)
{
	SAMPLE * musicajogo;
	ParaMp3(musicaintro);
	remove_int(MusicaIntro);
	CarregaSom(&musicajogo, PATH_SOM_MUSICAJOGO);
	play_sample(musicajogo, 255, 128, 1000, 1);
	NovoJogo();
	stop_sample(musicajogo);
	destroy_sample(musicajogo);
	TocaMp3(&musicaintro, PATH_SOM_MUSICAINTRO);
	install_int_ex(MusicaIntro,10000);
}

/* ---------------------------------------- */

void MusicaIntro(void)
{
	if (poll_mp3_file(musicaintro) != ALMP3_OK)
	{
		musicaintro = open_mp3_file(PATH_SOM_MUSICAINTRO);
		play_mp3_file(musicaintro, BUFSZ, 255, 128);
	}
}
END_OF_FUNCTION(MusicaIntro)

/* ---------------------------------------- */

void Opcoes (void)
{
	int i;
	int estasobre=0;
	FILE * arqopcoes;
	BITMAP * opcoes;
	BITMAP * botaomenu;
	BITMAP * botaovolume;
	BITMAP * explode[20];
	SAMPLE * explosao;

	CarregaBitmap(&opcoes, PATH_OPCOES);
	CarregaBitmap(&botaomenu, PATH_BOTAOMENU);
	CarregaBitmap(&botaovolume, PATH_BOTAOVOLUME);
	SeparaFrames(PATH_EXPLODE, explode, 20);
	CarregaSom (&explosao, PATH_SOM_EXPLOSAO);
	EfeitoAbrir(opcoes,1, fundomenu, 1, 1, 0, 0, 20);
	clear_keybuf();
	show_mouse(NULL);
	draw_sprite(screen, botaovolume, X_INICIAL_VOLUME+volume*(X_FINAL_VOLUME-X_INICIAL_VOLUME)/255 - botaovolume->w/2, Y_VOLUME);
	show_mouse(screen);
	while (!key[KEY_ESC])
	{
		if (mouse_x>=X_MENU && mouse_x<=X_MENU + botaomenu->w && mouse_y>=Y_MENU && mouse_y<=Y_MENU + botaomenu->h)
		{
			if (estasobre==0)
			{
				show_mouse(NULL);
				draw_sprite(screen, botaomenu,X_MENU, Y_MENU);
				show_mouse(screen);
				estasobre=1;
			}
			if (mouse_b & 1)
			{
				while (mouse_b & 1);
				break;
			}
		}
		if ((mouse_x<X_MENU || mouse_x>X_MENU + botaomenu->w || mouse_y<Y_MENU || mouse_y>Y_MENU + botaomenu->h) && estasobre==1)
		{
			estasobre=0;
			show_mouse(NULL);
			draw_sprite(buffer, opcoes,0, 0);
			draw_sprite(buffer, botaovolume, X_INICIAL_VOLUME+volume*(X_FINAL_VOLUME-X_INICIAL_VOLUME)/255 - botaovolume->w/2, Y_VOLUME);
			draw_sprite(screen, buffer,0,0);
			show_mouse(screen);
		}
		if (mouse_y>=Y_VOLUME && mouse_y<=Y_VOLUME + botaovolume->h && mouse_x>=X_INICIAL_VOLUME && mouse_x<X_FINAL_VOLUME && mouse_b&1)
		{
			while (mouse_b & 1)
			{
				if (mouse_x>X_INICIAL_VOLUME  && mouse_x <X_FINAL_VOLUME)
					volume=(mouse_x-X_INICIAL_VOLUME)*2;
				else
				{
					if (mouse_x<=X_INICIAL_VOLUME)
						volume=1;
					if (mouse_x>=X_FINAL_VOLUME)
						volume=255;
				}
				draw_sprite(buffer,opcoes,0,0);
				show_mouse(buffer);
				draw_sprite(buffer, botaovolume, X_INICIAL_VOLUME+volume*(X_FINAL_VOLUME-X_INICIAL_VOLUME)/255 - botaovolume->w/2 , Y_VOLUME);
				draw_sprite(screen, buffer,0,0);
				set_volume(volume,volume);
			}
			stop_sample(explosao);
			set_volume(volume,volume);
			play_sample(explosao, 255, 128, 1000, 0);
			for (i=0; i<20 && !(mouse_b&1) && !keypressed() ; i++)
			{
				draw_sprite(buffer,opcoes,0,0);
				draw_sprite(buffer, explode[i], X_INICIAL_VOLUME+volume*(X_FINAL_VOLUME-X_INICIAL_VOLUME)/255 - explode[0]->w/2 , Y_VOLUME-60 );
				show_mouse(NULL);
				draw_sprite(screen,buffer,0,0);
				show_mouse(screen);
				rest(30);
			}
			draw_sprite(buffer,opcoes,0,0);
			draw_sprite(buffer, botaovolume, X_INICIAL_VOLUME+volume*(X_FINAL_VOLUME-X_INICIAL_VOLUME)/255 - botaovolume->w/2 , Y_VOLUME);
			show_mouse(NULL);
			draw_sprite(screen,buffer,0,0);
			show_mouse(screen);
		}
	}
	arqopcoes = fopen("opcoes", "w");
	fprintf(arqopcoes, "%d", volume);
	fclose(arqopcoes);
	destroy_bitmap(opcoes);
	destroy_bitmap(botaovolume);
	destroy_bitmap(botaomenu);
	for (i=0; i<20 ; i++)
		destroy_bitmap(explode[i]);
	destroy_sample(explosao);
}

/* ---------------------------------------- */

void ParaMp3 (MP3FILE *mp3)
{
	close_mp3_file(mp3);
	//free(mp3);
}

/* ---------------------------------------- */

void Recordes (void)
{
	int estasobre=0;
	int rec_pontos[10], temp[10];
	int i, j, k;
	int pontos[6];
	char nomes[10][21];
	BITMAP *botaomenu;
	BITMAP *letras[26];
	BITMAP *numeros[10];
	BITMAP *recordes;

	CarregaBitmap(&botaomenu, PATH_BOTAOMENU);
	SeparaFrames(PATH_LETRAS, letras, 26);
	SeparaFrames(PATH_NUMEROS, numeros, 10);
	CarregaBitmap(&recordes, PATH_TELARECORDES);

	for(i=0 ; i<10 ; i++)
	{
		rec_pontos[i] = 0;
		for(j=0;j<21;j++)
			nomes[i][j] = 0;
	}

	CarregaRecordes(nomes, rec_pontos);

	for (i=0; i<10 ; i++)
		temp[i] = rec_pontos[i];

	for(i=0 ; (rec_pontos[i] != 0 && i<10) ; i++)
	{
		for (j=0 ; j<strlen(nomes[i]) ; j++)
			draw_sprite(recordes, letras[nomes[i][j] - 'A'], MENU_RECORDES_N_X + j*letras[0]->w, MENU_RECORDES_N_Y + i*letras[0]->h);

		j = 100000;
		k = 0;
		while (j>=1)
		{
			draw_sprite(recordes, numeros[temp[i]/j], MENU_RECORDES_P_X + k*numeros[0]->w, MENU_RECORDES_P_Y + i*letras[0]->h);
			temp[i] = temp[i]%j;
			j = j/10;
			k++;
		}
    }

	EfeitoAbrir(recordes,1, fundomenu, 1, 1, 0, 0, 20);

    show_mouse(NULL);
    blit(buffer, screen, 0,0,0,0, 800, 600);
    show_mouse(screen);

	while (!key[KEY_ESC])
	{
		if (mouse_x>=X_MENU && mouse_x<=X_MENU + botaomenu->w && mouse_y>=Y_MENU && mouse_y<=Y_MENU + botaomenu->h)
		{
			if (estasobre==0)
			{
				show_mouse(NULL);
				draw_sprite(screen, botaomenu,X_MENU, Y_MENU);
				show_mouse(screen);
				estasobre=1;
			}
			if (mouse_b & 1)
			{
				while (mouse_b & 1);
				break;
			}
		}
		if ((mouse_x<X_MENU || mouse_x>X_MENU + botaomenu->w || mouse_y<Y_MENU || mouse_y>Y_MENU + botaomenu->h) && estasobre==1)
		{
			estasobre=0;
			show_mouse(NULL);
			draw_sprite(buffer, recordes,0, 0);
			draw_sprite(screen, buffer,0,0);
			show_mouse(screen);
		}
	}

	destroy_bitmap(recordes);
	destroy_bitmap(botaomenu);

	for (i=0; i<26 ;i++)
	{
		if (i<10)
			destroy_bitmap(numeros[i]);
		destroy_bitmap(letras[i]);
	}
}

/* ---------------------------------------- */

void Sair (void)
{
	BITMAP * logotipo;
	BITMAP * sair;
	sair = create_bitmap(RESOLUCAO_X, RESOLUCAO_Y);
	CarregaBitmap(&logotipo, PATH_LOGOTIPO);
	clear_to_color(sair, 0);
	draw_sprite(sair, logotipo,13,0);
	textprintf_centre_ex(sair, font, RESOLUCAO_X/2, 500, makecol(255,255,255),0,  "Agradece por usar nossos programas");
	EfeitoAbrir(sair, 0,0, 1, 0,0,0, 3);
	rest(1500);
}

/* ---------------------------------------- */

void SeparaFrames(char nome[], BITMAP * bitmap[], int frames)
{
	int i;
	BITMAP * temp;
	CarregaBitmap(&temp, nome);
	for (i=0; i<frames ; i++)
	{
		bitmap[i] = create_bitmap( temp->w/frames , temp->h );
		blit(temp, bitmap[i], i*bitmap[0]->w, 0, 0,0, bitmap[0]->w, bitmap[0]->h);
	}
	destroy_bitmap(temp);
}

/* ---------------------------------------- */

void TocaMp3 (MP3FILE **mp3, char caminho[])
{
	if ((*mp3 = open_mp3_file(caminho)) == NULL)
	{
		printf("Erro ao abrir o arquivo %s\nPor favor reinstale o aplicativo\n", caminho);
		end();
	}
	play_mp3_file(*mp3, BUFSZ, 255, 128);
}
