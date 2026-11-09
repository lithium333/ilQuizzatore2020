/*
ilQUIZZATORE
by: Lithium333
ultima revisione: 27-11-2020
librerie: SDL2, SDL_image, SDL2_ttf.
*/

/* LIBRERIE */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <magentalib.h>
#include <msgstyle.h>
#ifdef __unix__
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_image.h>
#endif
#ifdef _WIN32
    #include <SDL.h>
    #include <SDL_ttf.h>
    #include <SDL_image.h>
#endif

#define SIZE_L 640
#define SIZE_H 480
#define SIZE_L_FHD 960
#define SIZE_H_FHD 720
#define FILE_HEADER "rules.conf"
#define FILE_POOL "pool.txt"
#define CHAR_ID ''
#define TTF_HEAD_SIZE 18
#define TTF_TEXT_NMAX 80
#define TTF_TEXT_SIZE 16
#define TTF_TEXT_DIST 20

enum {FALSE,TRUE};

/* VARIABILI GLOBALI */
unsigned long int alloc_qty;
int fhd,scala,dim_x,dim_y;
SDL_Window* finestra=NULL;
SDL_Color colore;
SDL_Event interfaccia;
SDL_Surface* bgwpBMP=NULL;
SDL_Surface* bgwpBMP_HD=NULL;
SDL_Surface* logoBMP=NULL;
SDL_Surface* schermo=NULL;
SDL_Rect posiz;
TTF_Font* font_head=NULL;
TTF_Font* font_text=NULL;
TTF_Font* font=NULL;

/* STRUTTURA ::: QUIZ */
typedef struct
    {
    int domande;
    float corretta;
    float sbagliata;
    float neutra;
    int scadenza;
    }
quiz_head;
/* STRUTTURA ::: DOMANDE */
typedef struct
    {
    int count;
    char *quesito;
    char **risposte;
    int soluzione;
    }
quiz_body;
/* STRUTTURA ::: RISPOSTE */
typedef struct
    {
    int index;
    int choice;
    }
quiz_usr;

/* FUNZIONI PROTOTIPI */
void acquisiz(FILE **fptarg, char **strarg);
int conferma(const char arg_title[], const char arg_text[]);
void gfxHD(void);
void initGFX(void);
void lockmenu(void);
void popUp(void);
void textLimited(char strarg[], char separg);
void TextToSurface(TTF_Font* arg_fnt, const char arg_str[], SDL_Color arg_clr, SDL_Rect* arg_rct);

/* ### MAIN ### */
int main(int argc, char* argv[])
{
/* VARIABILI */
char str_buff[1024];
dynvect answ_vett, pool_vett;
FILE *fp;
float punteggio;
int estrazione,i,j,max_ans,num_pool,param,trovato;
quiz_body *test;
quiz_head info;
quiz_usr *vett_usr;
time_t t_cur,t_start;

/* INIZIALIZZAZIONE */
initGFX();
alloc_qty=0;

/* APERTURA HEADER */
if((fp=fopen(FILE_HEADER,"r"))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","HEADER NON TROVATO!",finestra);
    exit(EXIT_FAILURE);
    }

/* LETTURA HEADER */
fscanf(fp,"%d%f%f%f%d",&(info.domande),&(info.corretta),&(info.sbagliata),&(info.neutra),&(info.scadenza));
fclose(fp);

/* ALLOCAZIONE MEMORIA PER IL TEST */
if((vett_usr=(quiz_usr*)malloc(sizeof(quiz_usr)*(info.domande)))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria per il quiz!",finestra);
    exit(EXIT_FAILURE);
    }
else
    {
    alloc_qty=alloc_qty+(sizeof(quiz_usr)*(info.domande));
    }

/* APERTURA POOL */
if((fp=fopen(FILE_POOL,"r"))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","POOL NON TROVATO!",finestra);
    exit(EXIT_FAILURE);
    }

/* LETTURA POOL */
i=0;
while(fscanf(fp,"%d",&max_ans)!=EOF)
    {
    /* CONTROLLO NUMERO RISPOSTE */
    if(max_ans<=0)
        {
        sprintf(str_buff,"POOL NON VALIDO!\n(numero risposte: %d non valido)",max_ans);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE",str_buff,finestra);
        exit(EXIT_FAILURE);
        }
    /* ALLOCAZIONE DINAMICA */
    if(i==0)
        {
        if(!dynVect_alloc(&pool_vett,sizeof(quiz_body),1))
            {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria per il pool!",finestra);
            exit(EXIT_FAILURE);
            }
        else
            {
            alloc_qty=alloc_qty+sizeof(quiz_body);
            }
        }
    else
        {
        if(!dynVect_grow(&pool_vett,1))
            {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria per il pool!",finestra);
            exit(EXIT_FAILURE);
            }
        else
            {
            alloc_qty=alloc_qty+sizeof(quiz_body);
            }
        }
    test=(quiz_body*)(pool_vett.aptr);
    /* ACQUISIZIONE DATI */
    test[i].count=max_ans;
    acquisiz(&fp,&(test[i].quesito));
    for(j=0;j<max_ans;j++)
        {
        if(j==0)
            {
            if(!dynVect_alloc(&answ_vett,sizeof(char*),1))
                {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria!",finestra);
                exit(EXIT_FAILURE);
                }
            else
                {
                alloc_qty=alloc_qty+sizeof(char*);
                }
            }
        else
            {
            if(!dynVect_grow(&answ_vett,1))
                {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria!",finestra);
                exit(EXIT_FAILURE);
                }
            else
                {
                alloc_qty=alloc_qty+sizeof(char*);
                }
            }
        test[i].risposte=(char**)(answ_vett.aptr);
        acquisiz(&fp,&(test[i].risposte[j]));
        }
    /* SCANSIONE CONTROLLATA DELLA SOLUZIONE */
    if(fscanf(fp,"%d",&(test[i].soluzione))!=1)
        {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","POOL NON VALIDO!\n(risposta non numerica o EOF prematuro)",finestra);
        exit(EXIT_FAILURE);
        }
    /* CONTROLLO INDICE SOLUZIONE */
    if((test[i].soluzione<=0)||(test[i].soluzione>max_ans))
        {
        sprintf(str_buff,"POOL NON VALIDO!\n(Indice risposta non valido)\n%d non contenuto nel range (1-%d)",test[i].soluzione,max_ans);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE",str_buff,finestra);
        exit(EXIT_FAILURE);
        }
    i++;
    }
fclose(fp);
num_pool=i;

/* RENDERING DEL LOGO */
posiz.x=(dim_x/2)-((logoBMP->w)/2);
posiz.y=dim_y-(logoBMP->h)-(50*scala)/100;
SDL_BlitSurface(logoBMP,NULL,schermo,&posiz);

/* STAMPA INTRO E INFO */
if((font=TTF_OpenFont("Font.ttf",(22*scala)/100))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile aprire il font!",finestra);
    exit(EXIT_FAILURE);
    }
colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
sprintf(str_buff,"PREMERE INVIO PER INIZIARE (ESC: USCIRE  V: MOD. VIDEO)");
TTF_SizeUTF8(font,str_buff,&param,NULL);
posiz.x=(dim_x/2)-(param/2);
posiz.y=(16*scala)/100;
TextToSurface(font,str_buff,colore,&posiz);
TTF_CloseFont(font);
colore=(SDL_Color){0x00,0xFF,0x00,0xFF};
posiz.x=(48*scala)/100;
posiz.y=(56*scala)/100;
TextToSurface(font_head,"INFO SUL QUIZ:",colore,&posiz);
colore=(SDL_Color){0xFF,0xFF,0x00,0xFF};
posiz.y=(80*scala)/100;
sprintf(str_buff,"Il quiz si compone di %d domande",info.domande);
TextToSurface(font_text,str_buff,colore,&posiz);
posiz.y=posiz.y+(TTF_TEXT_SIZE*scala)/100;
sprintf(str_buff,"Corretta: %.2f",info.corretta);
TextToSurface(font_text,str_buff,colore,&posiz);
posiz.y=posiz.y+(TTF_TEXT_SIZE*scala)/100;
sprintf(str_buff,"Sbagliata: %.2f",info.sbagliata);
TextToSurface(font_text,str_buff,colore,&posiz);
posiz.y=posiz.y+(TTF_TEXT_SIZE*scala)/100;
sprintf(str_buff,"Nulla: %.2f",info.neutra);
TextToSurface(font_text,str_buff,colore,&posiz);
posiz.y=posiz.y+(TTF_TEXT_SIZE*scala)/100;
sprintf(str_buff,"Tempo disponibile: %d minuti",info.scadenza);
TextToSurface(font_text,str_buff,colore,&posiz);
colore=(SDL_Color){0x00,0x7F,0xFF,0xFF};
posiz.x=dim_x/2;
posiz.y=(56*scala)/100;
TextToSurface(font_head,"INFO SUL POOL:",colore,&posiz);
colore=(SDL_Color){0xFF,0xFF,0x00,0xFF};
posiz.y=(80*scala)/100;
sprintf(str_buff,"Il pool dispone di %d domande",num_pool);
TextToSurface(font_text,str_buff,colore,&posiz);
TTF_SizeUTF8(font_head,str_buff,&param,NULL);
posiz.y=posiz.y+(TTF_TEXT_SIZE*(3/2)*scala)/100;
sprintf(str_buff,"Memoria allocata: %.3f kB",((float)(alloc_qty)/1024.0));
TextToSurface(font_text,str_buff,colore,&posiz);
posiz.y=posiz.y+(TTF_TEXT_SIZE*scala)/100;
TextToSurface(font_text,"(include spazio risposte)",colore,&posiz);
sprintf(str_buff,"Software OpenSource interamente sviluppato con strumenti OpenSource!");
TTF_SizeUTF8(font_text,str_buff,&param,NULL);
posiz.x=(dim_x/2)-(param/2);
posiz.y=(dim_y-(40*scala)/100);
colore=(SDL_Color){0x00,0x7F,0xFF,0xFF};
TextToSurface(font_text,str_buff,colore,&posiz);
SDL_UpdateWindowSurface(finestra);
lockmenu();
/* CONTROLLO ABBONDANZA QUIZ */
if(num_pool<info.domande)
    {
    sprintf(str_buff,"Il pool dispone di %d domande\nPer il quiz ne servono %d",num_pool,info.domande);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE",str_buff,finestra);
    exit(EXIT_FAILURE);
    }
/* GENERAZIONE CASUALE */
srand(clock());
i=0;
while(i<info.domande)
    {
    estrazione=(rand())%num_pool;
    trovato=FALSE;
    j=0;
    while((!trovato)&&(j<i))
        {
        if(vett_usr[j].index==estrazione)
            trovato=TRUE;
        else
            j++;
        }
    if(!trovato)
        {
        vett_usr[i].index=estrazione;
        vett_usr[i].choice=0;
        i++;
        }
    }
/* AVVISO INIZIO QUIZ */
SDL_HideWindow(finestra);
popUp();
sprintf(str_buff,"Il quiz sta per iniziare\nTempo a disposizione: %d minuti\nPremendo OK inizia il quiz",info.scadenza);
SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"ATTENZIONE",str_buff,NULL);
SDL_ShowWindow(finestra);
/* STAMPA QUIZ */
i=0;
time(&t_start);
while(i<info.domande)
    {
    /* DISEGNO FRAME */
    SDL_FillRect(schermo,NULL,SDL_MapRGB(schermo->format,0xFF,0xFF,0xFF));
    /* STAMPA TESTATA */
    colore=(SDL_Color){0x7F,0x00,0xFF,0xFF};
    posiz.x=(30*scala)/100;
    posiz.y=(16*scala)/100;
    sprintf(str_buff,"DOMANDA %d/%d",i+1,info.domande);
    TextToSurface(font_head,str_buff,colore,&posiz);
    time(&t_cur);
    sprintf(str_buff,"TEMPO: %02ld:%02ld",(long int)(((info.scadenza)*60)-(t_cur-t_start))/60,(long int)(((info.scadenza)*60)-(t_cur-t_start))%60);
    TTF_SizeUTF8(font_head,str_buff,&param,NULL);
    posiz.x=(dim_x/2)-(param/2);
    colore=(SDL_Color){0x00,0xCF,0x00,0xFF};
    TextToSurface(font_head,str_buff,colore,&posiz);
    /* CALCOLO RISPOSTE DATE */
    colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
    param=0;
    for(j=0;j<info.domande;j++)
        if(vett_usr[j].choice!=0)
            param++;
    sprintf(str_buff,"DATE: %d/%d",param,info.domande);
    TTF_SizeUTF8(font_head,str_buff,&param,NULL);
    posiz.x=dim_x-param-(30*scala)/100;
    TextToSurface(font_head,str_buff,colore,&posiz);
    /* STAMPA QUESITO */
    colore=(SDL_Color){0xFF,0x00,0x00,0xFF};
    posiz.x=(30*scala)/100;
    posiz.y=(50*scala)/100;
    textLimited(test[vett_usr[i].index].quesito,CHAR_ID);
    /* STAMPA RISPOSTE */
    for(j=0;j<test[vett_usr[i].index].count;j++)
        {
        /* STAMPARE SPUNTA */
        colore=(SDL_Color){0x7F,0x7F,0x7F,0xFF};
        if(test[vett_usr[i].index].risposte[j][0]!='\n')
            posiz.y=posiz.y+(TTF_TEXT_DIST*scala)/100;
        if(j+1==vett_usr[i].choice)
            sprintf(str_buff,"[X]%c",'A'+j);
        else
            sprintf(str_buff,"[ ]%c",'A'+j);
        TextToSurface(font_text,str_buff,colore,&posiz);
        /* STAMPA RISPOSTA */
        colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
        textLimited(test[vett_usr[i].index].risposte[j],CHAR_ID);
        }
    SDL_UpdateWindowSurface(finestra);
    /* ACQUISIZIONE */
    while(SDL_PollEvent(&interfaccia))
	    {
	    switch(interfaccia.type)
	        {
	        case SDL_QUIT:
	            if(conferma("AVVISO","Si vuole uscire dal programma?"))
	                exit(EXIT_FAILURE);
	            break;
	        case SDL_KEYDOWN:
	            if((interfaccia.key.keysym.sym>=SDLK_a)&&(interfaccia.key.keysym.sym<SDLK_a+(test[vett_usr[i].index].count)))
	                {
	                vett_usr[i].choice=interfaccia.key.keysym.sym-SDLK_a+1;
	                }
	            else
	                {
	                switch(interfaccia.key.keysym.sym)
	                    {
	                    case SDLK_ESCAPE:
	                        if(conferma("AVVISO","Si vuole uscire dal programma?"))
	                            exit(EXIT_SUCCESS);
	                        break;
	                    case SDLK_LEFT:
	                        if(i>0)
	                            i--;
	                        break;
	                    case SDLK_RIGHT:
	                        if(i<info.domande-1)
	                            i++;
	                        break;
	                    case SDLK_u:
	                        if(conferma("ATTENZIONE","Si vuole terminare il quiz?\n(e passare al punteggio)"))
	                            i=info.domande;
	                        break;
	                    case SDLK_x:
	                        vett_usr[i].choice=0;
	                        break;
	                    }
	                }
	            break;
	        case SDL_MOUSEBUTTONUP:
	            popUp();
	            SDL_ShowWindow(finestra);
	            break;
	        }
	    }

    if((t_cur-t_start)>=(info.scadenza)*60)
        i=info.domande;
    SDL_Delay(10);
    }
/* CALCOLO PUNTEGGIO QUIZ */
punteggio=0;
for(i=0;i<info.domande;i++)
    if(vett_usr[i].choice==0)
        punteggio=punteggio+info.neutra;
    else
        {
        if(vett_usr[i].choice==test[vett_usr[i].index].soluzione)
            punteggio=punteggio+info.corretta;
        else
            punteggio=punteggio+info.sbagliata;
        }
/* STAMPA ESITO */
SDL_FillRect(schermo,NULL,SDL_MapRGB(schermo->format,0x00,0x00,0x00));
if(bgwpBMP!=NULL)
	{
    if(fhd)
        SDL_BlitSurface(bgwpBMP_HD,NULL,schermo,NULL);
    else
        SDL_BlitSurface(bgwpBMP,NULL,schermo,NULL);
    }
sprintf(str_buff,"PREMERE INVIO PER LA REVISIONE (ESC PER USCIRE)");
TTF_SizeUTF8(font_head,str_buff,&param,NULL);
posiz.x=(dim_x/2)-(param/2);
posiz.y=(16*scala)/100;
colore=(SDL_Color){0x7F,0xFF,0xFF,0xFF};
TextToSurface(font_head,str_buff,colore,&posiz);
font=TTF_OpenFont("Font.ttf",36);
sprintf(str_buff,"PUNTEGGIO: %f/%f",punteggio,(info.corretta)*(float)(info.domande));
TTF_SizeUTF8(font,str_buff,&param,NULL);
posiz.x=(dim_x/2)-(param/2);
posiz.y=(180*scala)/100;
colore=(SDL_Color){0x00,0xFF,0x00,0xFF};
TextToSurface(font,str_buff,colore,&posiz);
sprintf(str_buff,"%.2f%%",100*punteggio/((info.corretta)*(float)(info.domande)));
TTF_SizeUTF8(font,str_buff,&param,NULL);
posiz.x=(dim_x/2)-(param/2);
posiz.y=(220*scala)/100;
colore=(SDL_Color){0x00,0x00,0xFF,0xFF};
TextToSurface(font,str_buff,colore,&posiz);
TTF_CloseFont(font);
SDL_UpdateWindowSurface(finestra);
lockmenu();
/* STAMPA REVISIONE */
i=0;
while(i<info.domande)
    {
    SDL_FillRect(schermo,NULL,SDL_MapRGB(schermo->format,0xFF,0xFF,0xFF));
    colore=(SDL_Color){0x7F,0x00,0xFF,0xFF};
    posiz.x=(30*scala)/100;
    posiz.y=(16*scala)/100;
    sprintf(str_buff,"DOMANDA %d/%d",i+1,info.domande);
    TextToSurface(font_head,str_buff,colore,&posiz);
    TTF_SizeUTF8(font_head,"MOD. REVISIONE!!!",&param,NULL);
    posiz.x=dim_x-param-(30*scala)/100;
    colore=(SDL_Color){0x00,0x7F,0x00,0xFF};
    TextToSurface(font_head,"MOD. REVISIONE!!!",colore,&posiz);
    colore=(SDL_Color){0xFF,0x00,0x00,0xFF};
    posiz.x=(30*scala)/100;
    posiz.y=(50*scala)/100;
    textLimited(test[vett_usr[i].index].quesito,CHAR_ID);
    /* STAMPA RISPOSTE */
    for(j=0;j<test[vett_usr[i].index].count;j++)
        {
        /* STAMPARE SPUNTA */
        colore=(SDL_Color){0x7F,0xCF,0x7F,0xFF};
        if(test[vett_usr[i].index].risposte[j][0]!='\n')
            posiz.y=posiz.y+(TTF_TEXT_DIST*scala)/100;
        if(j+1==vett_usr[i].choice)
            sprintf(str_buff,"[X]%c",'A'+j);
        else
            if(j+1==test[vett_usr[i].index].soluzione)
                sprintf(str_buff,"[*]%c",'A'+j);
            else
                sprintf(str_buff,"[ ]%c",'A'+j);
        TextToSurface(font_text,str_buff,colore,&posiz);
        /* STAMPA RISPOSTA */
        colore=(SDL_Color){0xFF,0x7F,0x00,0xFF};
        textLimited(test[vett_usr[i].index].risposte[j],CHAR_ID);
        }
    SDL_UpdateWindowSurface(finestra);
    /* ACQUISIZIONE */
    while(SDL_PollEvent(&interfaccia))
        {
        switch(interfaccia.type)
            {
            case SDL_QUIT:
                exit(EXIT_FAILURE);
                break;
            case SDL_KEYDOWN:
                switch(interfaccia.key.keysym.sym)
                    {
                    case SDLK_LEFT:
                        if(i>0)
                            i--;
                        break;
                    case SDLK_RIGHT:
                        if(i<info.domande-1)
                            i++;
                        break;
                    case SDLK_ESCAPE:
                    case SDLK_u:
                        exit(EXIT_FAILURE);
                        break;
                    case SDLK_x:
                        vett_usr[i].choice=0;
                        break;
                    }

                SDL_ShowWindow(finestra);
                break;
            case SDL_MOUSEBUTTONUP:
                popUp();
                SDL_ShowWindow(finestra);
            }
        }
    SDL_Delay(20);
    }

return EXIT_SUCCESS;
}

/* LETTURA LIMITATA */
void acquisiz(FILE **fptarg, char **strarg)
    {
    char chrarg;
    dynvect str_vett;
    int index;
    index=0;
    /* INSERIMENTO PER CARATTERE */
    while((chrarg=getc(*fptarg))!=CHAR_ID)
        {
        /* CONTROLLO FILE TRANCIATO */
        if(chrarg==EOF)
            {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","POOL NON VALIDO!\n(EOF prematuro)",finestra);
            exit(EXIT_FAILURE);
            }
        /* INIZIALIZZARE VETTORE */
        if(index==0)
            {
            if(!dynVect_alloc(&str_vett,sizeof(char),1))
                {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria!",finestra);
                exit(EXIT_FAILURE);
                }
            else
                {
                alloc_qty=alloc_qty+sizeof(char);
                }
            }
        /* ESTENDERE VETTORE */
        else
            {
            if(!dynVect_grow(&str_vett,1))
                {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria!",finestra);
                exit(EXIT_FAILURE);
                }
            else
                {
                alloc_qty=alloc_qty+sizeof(char);
                }
            }
        /* INSERIRE IL CARATTERE */
        (*strarg)=(char*)(str_vett.aptr);
        (*strarg)[index]=chrarg;
        index++;
        }
    /* INSERIMENTO TERMINATORE STRINGA */
    if(!dynVect_grow(&str_vett,1))
        {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile allocare memoria!",finestra);
        exit(EXIT_FAILURE);
        }
    else
        {
        alloc_qty=alloc_qty+sizeof(char);
        }
    (*strarg)=(char*)(str_vett.aptr);
    (*strarg)[index]=CHAR_ID;
    return;
    }

/* CHIEDERE SE SI VUOLE USCIRE */
int conferma(const char arg_title[], const char arg_text[])
    {
    int buttonval;
    while(SDL_PollEvent(&interfaccia));
    const SDL_MessageBoxData w_cont =
        {
        SDL_MESSAGEBOX_WARNING,NULL,
        arg_title,arg_text,
        2,w_butt,&w_csch
        };
    SDL_HideWindow(finestra);
    SDL_ShowMessageBox(&w_cont, &buttonval);
    SDL_ShowWindow(finestra);
    return buttonval;
    }

/* SCAMBIO MOD. VIDEO */
void gfxHD(void)
{
FILE *setflag;
char msgstr[64];
if(fhd)
	{
	sprintf(msgstr,"Risoluzione in uso: %dx%d\nCambiarla in %dx%d?",SIZE_L_FHD,SIZE_H_FHD,SIZE_L,SIZE_H);
	}
else
	{
	sprintf(msgstr,"Risoluzione in uso: %dx%d\nCambiarla in %dx%d?",SIZE_L,SIZE_H,SIZE_L_FHD,SIZE_H_FHD);
	}
/* CCONFERMA */
if(conferma("Cambio risoluzione",msgstr))
	{
	if((setflag=fopen("fhd.cfg","w"))==NULL)
		{
		SDL_HideWindow(finestra);
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "ERRORE",
            "Impossibile salvare le modifiche su fhd.cfg",
            NULL
        );
		SDL_ShowWindow(finestra);
		}
	else
		{
		fprintf(setflag,"%d",!fhd);
		fclose(setflag);
		SDL_HideWindow(finestra);
		SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_WARNING,
            "ATTENZIONE",
            "Riaprire l'applicazione per rendere effettive le modifiche",
            NULL
        );
		exit(EXIT_SUCCESS);
		}
	}
return;
}

/* INIZIALIZZAZIONE APPLICAZIONE */
void initGFX(void)
{
FILE *setflag;
/* FHD */
scala=100;
dim_x=SIZE_L;
dim_y=SIZE_H;
if((setflag=fopen("fhd.cfg","r"))==NULL)
	{
	printf("fhd.cfg non trovato!\n");
	if((setflag=fopen("fhd.cfg","w"))==NULL)
		{
		printf("Impossibile creare fhd.cfg!\n");
		}
	else
		{
		fprintf(setflag,"%d",0);
		fclose(setflag);
		printf("File fhd.cfg creato!\n");
		}
	}
else
	{
	fscanf(setflag,"%d",&fhd);
	if(fhd)
		{
		scala=150;
		dim_x=SIZE_L_FHD;
		dim_y=SIZE_H_FHD;
		}
	fclose(setflag);
	}
/* INIZIALIZZARE GRAFICA */
if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
    fprintf(stderr,"Impossibile inizializzare SDL!\n");
    exit(EXIT_FAILURE);
    }
/* CREARE FINESTRA */
if((finestra=SDL_CreateWindow("ilQuizzatore v1.3 (rev 2020.11.27)",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,dim_x,dim_y,SDL_WINDOW_SHOWN))==NULL)
    {
    fprintf(stderr,"Impossibile inizializzare la finestra!\n");
    exit(EXIT_FAILURE);
    }
schermo=SDL_GetWindowSurface(finestra);
/* AVVIARE SOTTOSISTEMA PNG */
if((IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)!=IMG_INIT_PNG)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile inizializzare il sottosistema TTF",finestra);
    exit(EXIT_FAILURE);
    }
/* CARICARE IMMAGINE */
if((logoBMP=IMG_Load("logo.png"))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare logo.png",finestra);
    exit(EXIT_FAILURE);
    }
/* CARICARE SFONDI ESITO */
if((bgwpBMP=SDL_LoadBMP("bg.bmp"))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare bg.bmp",finestra);
    exit(EXIT_FAILURE);
    }
if((bgwpBMP_HD=SDL_LoadBMP("bg_hd.bmp"))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare bg_hd.bmp",finestra);
    exit(EXIT_FAILURE);
    }
/* AVVIARE SOTTOSISTEMA TTF */
if(TTF_Init()==-1)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile inizializzare il sottosistema TTF",finestra);
    exit(EXIT_FAILURE);
    }
/* CARICARE FONT: TESTO DOMANDE & RISPOSTE */
if((font_text=TTF_OpenFont("Font.ttf",(TTF_TEXT_SIZE*scala)/100))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare Font.ttf",finestra);
    exit(EXIT_FAILURE);
    }
/* CARICARE FONT: INTESTAZIONE PARAGRAFI (HEAD) */
if((font_head=TTF_OpenFont("Font.ttf",(TTF_HEAD_SIZE*scala)/100))==NULL)
    {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"ERRORE","Impossibile caricare Font.ttf",finestra);
    exit(EXIT_FAILURE);
    }
/* PULIZIA EVENTI */
while(SDL_PollEvent(&interfaccia));
return;
}

/* BLOCCO MENU */
void lockmenu(void)
{
while(1)
    {
    while(SDL_PollEvent(&interfaccia))
        {
        switch(interfaccia.type)
            {
            case SDL_QUIT:
                exit(EXIT_FAILURE);
                break;
            case SDL_KEYDOWN:
                switch(interfaccia.key.keysym.sym)
                    {
                    case SDLK_RETURN:
                        return;
                        break;
                    case SDLK_ESCAPE:
                        exit(EXIT_FAILURE);
                        break;
                    case SDLK_v:
                    	gfxHD();
                    	break;
                    }
                break;
            }
        }
    SDL_UpdateWindowSurface(finestra);
    SDL_Delay(10);
    }
}

/* MESSAGGI MODALI */
void popUp(void)
{
SDL_HideWindow(finestra);
SDL_ShowSimpleMessageBox(
    SDL_MESSAGEBOX_INFORMATION,
    "ISTRUZIONI",
    "Questo programma non ha ancora implementato l'uso dei dispsoitivi HID (mouse)\nUsare le frecce della tastiera per navigare e le lettere per selezionare la risposta\nX per la risposta nulla e U per terminare il quiz\n(ESC per uscire)",
    NULL
);
}

/* STAMPA DEL TESTO */
void textLimited(char strarg[], char separg)
{
char strprovv[1024];
int i,j,k,txt_wide;
i=0;
j=0;
txt_wide=0;
/* CICLO PER CARATTERE */
while(strarg[i]!=separg)
    {
    if(((30*scala)/100+txt_wide)>(dim_x-(30*scala)/100))
        {
        /* ANDARE INDIETRO (FINO AL BLANK) */
        k=i;
        while((strarg[i]!=' ')&&(k-i<50))
            {
            j--;
            i--;
            }
        /* PAROLA TROPPO LUNGA: TAGLIARLA */
        if(k-i==50)
            {
            i=i+49;
            j=j+49;
            }
        /* RENDERING RIGA DI TESTO */
        strprovv[j]=strarg[i];
        strprovv[j+1]='\0';
        TextToSurface(font_text,strprovv,colore,&posiz);
        txt_wide=0;
        j=0;
        posiz.y=posiz.y+(TTF_TEXT_DIST*scala)/100;
        }
    else
        {
        /* RITORNO A CAPO */
        if(strarg[i]=='\n')
            {
            strprovv[j]='\0';
            TextToSurface(font_text,strprovv,colore,&posiz);
            txt_wide=0;
            j=0;
            posiz.y=posiz.y+(TTF_TEXT_DIST*scala)/100;
            }
        /* PROSEGUIMENTO SU RIGA */
        else
            {
            strprovv[j]=strarg[i];
            strprovv[j+1]='\0';
            TTF_SizeUTF8(font_text,strprovv,&txt_wide,NULL);
            j++;
            }
        }
    i++;
    }
/* FINE: RENDERING ULTIMA RIGA */
if(j<(TTF_TEXT_NMAX*scala)/100)
    strprovv[j]='\0';
TextToSurface(font_text,strprovv,colore,&posiz);
posiz.y=posiz.y+(TTF_TEXT_DIST*scala)/100;
return;
}


/* RENDERING TESTO TTF/UTF-8 */
void TextToSurface(TTF_Font* arg_fnt, const char arg_str[], SDL_Color arg_clr, SDL_Rect* arg_rct)
{
/* variabili */
SDL_Surface* arg_sfc;
arg_sfc=TTF_RenderUTF8_Blended(arg_fnt,arg_str,arg_clr);
SDL_BlitSurface(arg_sfc,NULL,schermo,arg_rct);
SDL_FreeSurface(arg_sfc);
return;
}
