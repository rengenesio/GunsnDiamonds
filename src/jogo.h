#include <stdio.h>
#include <allegro.h>

#define RESOLUCAO_X					800
#define RESOLUCAO_Y					600

/* Valores constantes do Programa */
#define TAM_JOIA                    28
#define TAM_QUADRADO                34
#define TAM_LINHA_GRADE             11

/* TAM_QUADRADO + TAM_LINHA_GRADE = 44 */
#define COORD_PRIMEIRO_QUADRADO_X   323
#define COORD_PRIMEIRO_QUADRADO_Y   125
#define INICIO_GRADE_X              314
#define FINAL_GRADE_X               INICIO_GRADE_X + 10 * (TAM_QUADRADO + TAM_LINHA_GRADE) + TAM_LINHA_GRADE /* 774 */
#define INICIO_GRADE_Y              116
#define FINAL_GRADE_Y               INICIO_GRADE_Y + 10 * (TAM_QUADRADO + TAM_LINHA_GRADE) + TAM_LINHA_GRADE /* 576 */

#define BT_PAUSE_X                  140
#define BT_PAUSE_Y                  428
#define BT_PAUSE_LARG               67
#define BT_PAUSE_ALT                15
#define BT_DICA_X                   141
#define BT_DICA_Y                   396
#define BT_DICA_LARG                44
#define BT_DICA_ALT                 15
#define BT_SAIR_X                   142
#define BT_SAIR_Y                   519
#define BT_SAIR_LARG                120
#define BT_SAIR_ALT                 37

#define QUADRINHO_ALT               24
#define QUADRINHO_LARG              19
#define QUADRINHO_TEMPO_Y           163
#define QUADRINHO_TEMPO_X_1         139
#define QUADRINHO_TEMPO_X_2         167
#define QUADRINHO_TEMPO_X_3         189

#define QUADRINHO_X_1               139
#define QUADRINHO_X_2               161
#define QUADRINHO_X_3               183
#define QUADRINHO_X_4               205
#define QUADRINHO_X_5               227
#define QUADRINHO_X_6               249

#define QUADRINHO_PONTOS_Y          223
#define QUADRINHO_PONTOSBONUS_Y     300
#define QUADRINHO_FASE_Y            359

#define FASES                       6
#define FASE_2_PONTOS               1000
#define FASE_3_PONTOS               2000
#define FASE_4_PONTOS               3000
#define FASE_5_PONTOS               4500
#define FASE_6_PONTOS               6000
#define FASE_1_TEMPO                300
#define FASE_2_TEMPO                250
#define FASE_3_TEMPO                200
#define FASE_4_TEMPO                150
#define FASE_5_TEMPO                100
#define FASE_6_TEMPO                50

#define GAMEOVER_RECORDE_X          132
#define GAMEOVER_RECORDE_Y          321
#define GAMEOVER_RECORDE_NOME_X     191
#define GAMEOVER_RECORDE_NOME_Y     472
#define GAMEOVER_BOTAO_OK_X         383
#define GAMEOVER_BOTAO_OK_Y         529
#define GAMEOVER_BOTAO_OK_LARG      36
#define GAMEOVER_BOTAO_OK_ALT       27

#define LETRAS_ALT                  27
#define LETRAS_LARG                 21

#define PATH_JOIA_1                 "../resources/images/joias/joia1.tga"
#define PATH_JOIA_2                 "../resources/images/joias/joia2.tga"
#define PATH_JOIA_3                 "../resources/images/joias/joia3.tga"
#define PATH_JOIA_4                 "../resources/images/joias/joia4.tga"
#define PATH_JOIA_5                 "../resources/images/joias/joia5.tga"
#define PATH_JOIA_6                 "../resources/images/joias/joia6.tga"
#define PATH_JOIA_7                 "../resources/images/joias/joia7.tga"
#define PATH_JOIA_TAPA              "../resources/images/joias/tapajoias.tga"
#define PATH_BOTAO_DICA             "../resources/images/jogo/botaodica.tga"
#define PATH_BOTAO_PAUSE            "../resources/images/jogo/botaopausa.tga"
#define PATH_BOTAO_SAIR             "../resources/images/jogo/botaovoltamenu.tga"
#define PATH_FUNDO                  "../resources/images/jogo/fundo.tga"
#define PATH_MIRA_MARCA             "../resources/images/jogo/mira_marca.tga"
#define PATH_NUMEROS                "../resources/images/frames/numeros.tga"
#define PATH_LETRAS                 "../resources/images/frames/letras.tga"
#define PATH_GAMEOVER_SCREEN        "../resources/images/gameover/gameover.tga"
#define PATH_GAMEOVER_RECORDE       "../resources/images/gameover/recorde.tga"
#define PATH_GAMEOVER_OK            "../resources/images/gameover/botaook.tga"

/* Structs */
typedef struct _JOIA
{
	int joia;
	int troca_cima, troca_esquerda, troca_direita, troca_baixo;
} JOIA;

/* Prototipos das funções */
int CairPedras(BITMAP *fundo, JOIA matriz_grade[10][10], BITMAP *joias[8], BITMAP *buffer);
int ClicouBotaoPause(void);
int ClicouBotaoDica(void);
int ClicouBotaoSair(void);
int DestroiCombinacoes(int linha, int coluna, BITMAP *buffer, BITMAP *fundo, JOIA matriz_grade[10][10]);
int ExisteJogadaPossivel(JOIA matriz_grade[10][10]);
int GetDirection(int quad1_linha, int quad1_coluna, int quad2_linha, int quad2_coluna); /* Retorna se o quadrado 2 esta acima, a direita, embaixo ou a esquerda */
int LoadButtons(BITMAP *buttons[3]);
int ModuloDiferenca(int x, int y); /* Função para calcular o módulo da diferenca entre 2 números */
int MouseDentroDaGrade(void); /* Retorna 1 caso o mouse esteja dentro da area da grade... 0 se não) */
int PassouDeFase(int fase, int pontos);
int PreCacheMoves(JOIA matriz_grade[10][10]);
int QuadradoAdjacente(int quad1_x, int quad1_y, int quad2_x, int quad2_y);
int PreencheLinha0(JOIA matriz_grade[10][10], BITMAP *joias[8], BITMAP *buffer);
int SearchAndDestroy(JOIA matriz_grade[10][10], BITMAP *buffer, BITMAP* fundo);
int VerificaMovimento(JOIA matriz_grade[10][10], int linha, int coluna);

void ApagaQuadrado(int linha, int coluna, BITMAP *target, BITMAP *fundo);
void AtualizaFase(BITMAP *fundo, BITMAP *buffer, int fase,  BITMAP *numeros[10]);
void AtualizaPontuacao(BITMAP *fundo, BITMAP *buffer, int pontos, BITMAP *numeros[10]);
void AtualizaPontuacaoBonus(BITMAP *fundo, BITMAP *buffer, int pontos,  BITMAP *numeros[10]);
void BufferToScreen(BITMAP *buffer, BITMAP *screen);
void CarregaJoias(BITMAP *joias[8], JOIA matriz_grade[10][10]); /* Carrega as imagens das joias */
void DeloadAll(BITMAP *Joias[8], BITMAP *fundo, BITMAP *buffer, BITMAP *buttons[3], BITMAP *numeros[10], BITMAP *mira);
void DesenhaJoias(BITMAP *joias[8], JOIA matriz_grade[10][10], BITMAP *buffer);
void DesmarcaJoia(BITMAP *buffer, BITMAP *fundo, BITMAP *joia, int linha, int coluna);
void DrawBackground(BITMAP *fundo, BITMAP *target);
void DrawJoia(BITMAP *joia, BITMAP *target, int linha, int coluna);
void DumpGrid(JOIA matriz_grade[10][10]);
void GamePause(BITMAP *buffer, BITMAP *fundo);
void IniciaTempoFase(int fase);
void MarcaJoia(BITMAP *mira, BITMAP *buffer, int linha, int coluna);
void NovoJogo(void);
void PegaNumeroNaCasa(int number, int casas, int numeros[]);
void PegaQuadradoPelaPosMouse(int *linha, int *coluna);
void ProcuraDica(JOIA matriz_grade[10][10], BITMAP *buffer, BITMAP *fundo, BITMAP *Joias[8]);
void RedrawTempo(BITMAP *fundo, BITMAP *buffer, BITMAP *numeros[10]);
void ResetButton(BITMAP *fundo, BITMAP *buffer);
void ScreenToBuffer(BITMAP *buffer);
void TimerTempoContagem();
void Troca(JOIA matriz_grade[10][10], int coord_clique[2][2], BITMAP *buffer, BITMAP *fundo, BITMAP *Joias[8]); /* Troca os círculos selecionados */
void Verifica(int linha, int coluna); /* Faz a verificacao para destruir 3 ou mais joias iguais... na horizontal ou vertical */
void ZeraVetorJoias(JOIA matriz_grade[10][10]);

int EntrouNosRecordes(int pontos, char nomes[10][21], int rec_pontos[10]);
void AdicionaLetraAoNome(char nome[21], char letra);
void RemoveUltimaLetraDoNome(char nome[21]);
void AtualizaNome(char nome[21], BITMAP *buffer, BITMAP *recorde, BITMAP *letras[26]);
void CarregaRecordes(char nomes[10][21], int rec_pontos[10]);
void GravaOrdenado(char nomes[10][21], int rec_pontos[10]);
void InsereRecorde(char nome[21], int pontos, char rec_nomes[10][21], int rec_pontos[10]);
void GameOver(int pontos);
int GameOverEstaNoOk();

extern void SeparaFrames(char nome[], BITMAP * bitmap[], int frames);
extern void CarregaBitmap (BITMAP ** bitmap, char nome[]);
extern void CarregaSom (SAMPLE ** som, char nome[]);


volatile int tempo_restante = 0,
                game_paused = 0,
                redraw_tempo = 0;

extern int volume;

