/* LIBRERIA VETTORI DINAMICI */
/* ULTIMA REVISIONE: 27-NOV-2020 */

/* BASE-DATI VETTORE DINAMICO */
typedef struct {
    int block; /* dimensione singolo elemento */
    long unsigned int count; /* numero di elementi posseduti */
    char *aptr; /* puntatore dell'area di memoria */
} dynvect;

/* FUNZIONE: allocare un nuovo vettore */
int dynVect_alloc(dynvect *arg_dynvect, int arg_block, int arg_count)
    {
    char *arg_aptr;
    (*arg_dynvect).block=arg_block;
    if((arg_aptr=(char*)malloc(arg_count*arg_block))==NULL)
        {
        (*arg_dynvect).aptr=NULL;
        (*arg_dynvect).count=0;
        return 0;
        }
    else
        {
        (*arg_dynvect).count=arg_count;
        (*arg_dynvect).aptr=arg_aptr;
        }
    return 1;
    }

/* FUNZIONE: espandere un vettore esistente */
int dynVect_grow(dynvect *arg_dynvect, long unsigned int arg_grow)
{
char *new_aptr,*old_aptr;
long unsigned int arg_allsize;
arg_allsize=((*arg_dynvect).count)*((*arg_dynvect).block);
if((new_aptr=(char*)malloc(((*arg_dynvect).count+arg_grow)*((*arg_dynvect).block)))==NULL)
    return 0;
old_aptr=(*arg_dynvect).aptr;
(*arg_dynvect).aptr=new_aptr;
memcpy(new_aptr,old_aptr,arg_allsize);
(*arg_dynvect).count=(*arg_dynvect).count+arg_grow;
free(old_aptr);
return 1;
}
