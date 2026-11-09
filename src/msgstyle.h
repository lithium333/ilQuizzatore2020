/* DEFINIZIONI MESSAGGI MODALI */
/* ULTIMA REVISIONE: 27-NOV-2020 */

#ifdef __unix__
    #include <SDL2/SDL.h>
#endif
#ifdef _WIN32
    #include <SDL.h>
#endif

/* PULSANTI */
const SDL_MessageBoxButtonData w_butt[] = 
    {
    {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,0,"NO"},
    {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,1,"SI"},
    };

/* TEMA FINESTRE DIALOGO */
const SDL_MessageBoxColorScheme w_csch = 
    {
    {
        {0xAF,0xAF,0xAF}, /* [COLORE: SFONDO FINESTRA] */
        {0x00,0x00,0x00}, /* [COLORE: TESTO] */    
        {0XFF,0XFF,0X00}, /* [COLORE: BORDI PULS] */
        {0x00,0x7F,0x00}, /* [COLORE: PULSANTE] */
        {0xAF,0xAF,0xAF} /* [COLORE: PULSANTE SELEZ.] */
    }
    };


/*
WARNING:
a causa di alcune caratteristiche insite della libreria:
- i colori delle finestre sono disponibili solo per Linux/Unix
- le peculiarità WARNING/INFO sono disponibili solo per Windows
*/
