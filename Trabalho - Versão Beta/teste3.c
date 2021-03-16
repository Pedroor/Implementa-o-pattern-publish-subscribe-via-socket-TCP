#include "essentials.c"
char *Arquivo = "foto.jpg";

int getSize(char *str){
    int i;
    for(i = 0; str[i] != '\0';i++){}
    return i;
}



int main(){
    FILE *file = fopen(Arquivo, "ab+");
    int fileSize = getArqLen(file);

    int numeroDePacotes = ceil(fileSize/(float)(MAX_DATA_SIZE));
    int i;
    for (i = 0; i < numeroDePacotes;i++){
        int currentSize;
        char* str = getFile2(file, &currentSize, i + 1, Arquivo);
        
        FILE *write_ptr;
        write_ptr = fopen("NOVONOVO","ab");
        fwrite(str, currentSize,1,write_ptr);
        fclose(write_ptr);
        //printf("Escrito parte %d\n", i);
        //printf("TAMANHO DO DADO: %ld\n", sizeof(str)*strlen(str));
    }
    fclose(file);
}