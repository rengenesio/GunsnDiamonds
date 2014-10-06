#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <ALMP3.H>
#include "mp3.h"

/* Definições do vídeo */
#define RESOLUCAO_X			800
#define RESOLUCAO_Y			600
#define COLOR_DEPTH			16

/* Definições do menu opções */
#define X_INICIAL_BOTOES		260
#define Y_INICIAL_BOTOES		197
#define X_MENU				364
#define Y_MENU				526
#define Y_VOLUME			317
#define X_INICIAL_VOLUME		400
#define X_FINAL_VOLUME			528

/* Definições dos créditos */
#define X_CREDITOS			410
#define Y_TACOWBERRY			165
#define Y_FINAL_TACOWBERRY		(Y_TACOWBERRY+RESOLUCAO_Y+37)
#define Y_RECARREGA			(Y_TACOWBERRY+300)
#define Y_FINAL_RECARREGA		(Y_TACOWBERRY+376)
#define Y_GRANADA			233
#define Y_FINAL_GRANADA			600
#define Y_ROCKETEIRO			276
#define Y_FINAL_ROCKETEIRO		(Y_ROCKETEIRO+RESOLUCAO_Y)
#define Y_TANQUE			325
#define Y_FINAL_TANQUE			(Y_TANQUE+RESOLUCAO_Y+65)
#define Y_MENDIGO			404
#define Y_FINAL_MENDIGO			(Y_MENDIGO+RESOLUCAO_Y+41)

/* Definições dos recordes */
#define MENU_RECORDES_N_X		144
#define MENU_RECORDES_N_Y		240
#define MENU_RECORDES_P_X		589
#define MENU_RECORDES_P_Y		240


/* Definição do caminho do arquivo com os caminhos das imagens e dos sons */
#define PATH_FILES			"../resources/files.bin"

/* Definições dos caminhos dos arquivos de imagens */
#define PATH_ABAIXA			"../resources/images/frames/abaixa.tga"
#define PATH_ANDA			"../resources/images/frames/anda.tga"
#define PATH_AJUDA			"../resources/images/ajuda.tga"
#define PATH_APRESENTA			"../resources/images/apresenta.tga"
#define PATH_ATIRA			"../resources/images/frames/atira.tga"
#define PATH_BOMBA			"../resources/images/frames/bomba.tga"
#define PATH_BOTAOMENU			"../resources/images/botaomenu.tga"
#define PATH_BOTAOVOLUME		"../resources/images/opcoes/botaovolume.tga"
#define PATH_BOTOESMENU			"../resources/images/menu/botoesmenu.tga"
#define PATH_CREDITOS			"../resources/images/creditos/creditos.tga"
#define PATH_EXPLODE			"../resources/images/frames/explode.tga"
#define PATH_EXPLODE2			"../resources/images/frames/explode2.tga"
#define PATH_FUNDOMENU			"../resources/images/menu/menu.tga"
#define PATH_GRANADA			"../resources/images/frames/granada.tga"
#define PATH_LETRAS			"../resources/images/frames/letras.tga"
#define PATH_LOGOTIPO			"../resources/images/logotipo.tga"
#define PATH_MENDIGO			"../resources/images/frames/mendigo.tga"
#define PATH_MENDIGOANDA		"../resources/images/frames/mendigoanda.tga"
#define PATH_MIRA			"../resources/images/mouse/mira.tga"
#define PATH_MISSEL			"../resources/images/frames/missel.tga"
#define PATH_MURO			"../resources/images/muro.tga"
#define PATH_NUMEROS			"../resources/images/frames/numeros_recordes.tga"
#define PATH_OPCOES			"../resources/images/opcoes/opcoes.tga"
#define PATH_PARADO			"../resources/images/frames/parado.tga"
#define PATH_PULA			"../resources/images/frames/pula.tga"
#define PATH_QUEIMA			"../resources/images/frames/queima.tga"
#define PATH_RECARREGA			"../resources/images/frames/recarrega.tga"
#define PATH_ROCKET			"../resources/images/frames/rocket.tga"
#define PATH_ROCKETEIRO			"../resources/images/frames/rocketeiro.tga"
#define PATH_TACABOMBA			"../resources/images/frames/tacabomba.tga"
#define PATH_TANQUE			"../resources/images/frames/tanque.tga"
#define PATH_TELARECORDES		"../resources/images/recordes.tga"
#define PATH_VIRA			"../resources/images/frames/vira.tga"

/* Definições dos caminhos dos arquivos de som */
#define PATH_SOM_APLAUSOS		"../resources/sounds/aplausos.wav"
#define PATH_SOM_EXPLOSAO		"../resources/sounds/explosao.wav"
#define PATH_SOM_GRITO			"../resources/sounds/grito.wav"
#define PATH_SOM_MISSILE		"../resources/sounds/missile.wav"
#define PATH_SOM_MUSICAINTRO		"../resources/sounds/musicaintro.mp3"
#define PATH_SOM_MUSICAJOGO		"../resources/sounds/musicajogo.wav"
#define PATH_SOM_PASSOS			"../resources/sounds/passos.wav"
#define PATH_SOM_RECARREGA		"../resources/sounds/recarrega.wav"
#define PATH_SOM_TANK			"../resources/sounds/tank.wav"
#define PATH_SOM_TIRO			"../resources/sounds/tiro.wav"

/* Declarações das funções */
int start(void); /* Starts Allegro */
int Menu (void); /* Menu principal do jogo */
int MouseEstaEmAlgumBotao(); /* Indica se o mouse está em algum botão no menu principal */
int testFiles(void); /* Tests if all resources files exist */
void Ajuda (void); /* Tela de ajuda */
void CarregaBitmap (BITMAP ** bitmap, char nome[]); /* Carrega um bitmap */
void CarregaSom (SAMPLE ** som, char nome[]); /* Carrega um som */
void Creditos (void); /* Tela dos créditos */
void EfeitoAbrir (BITMAP * bitmap, int comfundo, BITMAP * fundooo, int horizontalmente, int verticalmente, int pos_x, int pos_y, int velocidade); /* Efeito de abertura de bitmaps */
void Explosao (BITMAP*bitmap, float tamanho, int pos_x, int pos_y); /* Faz buraco na parede na introdução */
void FadeIn(BITMAP *bitmap, int velocidade); /* Fade In na tela */
void FadeOut(int velocidade); /* Fade Out na tela */
void end (void); /* Ends Allegro */
void IniciaJogo (void); /* Inicia as definições do jogo */
void Intro (void); /* Introdução */
void Jogo (void); /* Inicia um novo jogo */
void MusicaIntro(void); /* Carrega a mp3 da introdução */
void Opcoes (void); /* Tela de opções */
void ParaMp3 (MP3FILE *mp3); /* Para de tocar a mp3 */
void Recordes (void); /* Tela de recordes */
void Sair (void); /* Tela de saída do jogo */
void SeparaFrames(char nome[], BITMAP * bitmap[], int frames); /* Separa figuras de animação em frames */
void TocaMp3 (MP3FILE **mp3, char caminho[]); /* Fica tocando a mp3 */
extern void CarregaRecordes(char nomes[10][21], int rec_pontos[10]);
extern void NovoJogo(void);

/* Declarações das variáveis globais */
BITMAP * buffer;
BITMAP * fundomenu;
BITMAP * mouse;
FILE * arqopcoes;
MP3FILE * musicaintro;
int altura_botao, largura_botao, menu_ja_foi_aberto=0, volume;
int musicaintro_carregada=0;
