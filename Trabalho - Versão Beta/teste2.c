#include "essentials.c"
char *Arquivo = "foto.jpg";

int getSize(char *str){
    int i;
    for(i = 0; str[i] != '\0';i++){}
    return i;
}

char* getFile(FILE *file, int *currentSize){
    int tamanhoArquivo = getArqLen(file);
    printf("Tamanho do Arquivo: %d\n", tamanhoArquivo);

    char buffer[tamanhoArquivo];
    *currentSize = tamanhoArquivo;
    memset(buffer, 0, sizeof(buffer));
    fread(buffer,sizeof(buffer),1,file);

    printf("\n%s\n", buffer);
    fclose(file);
    printf("Tamanho do Buffer: %ld\n", strlen(buffer));

    char *str = (char*)malloc(sizeof(buffer));
    memset(str, 0, sizeof(str));
    memcpy(str, buffer, tamanhoArquivo);

    return str;
}

int main(){
    FILE *file = fopen(Arquivo, "ab+");
    int currentSize;
    char* str = getFile(file, &currentSize);

    printf("Tamanho da String recebida: %ld - %d\n", strlen(str), currentSize);
    //printf("\n%s\n", str);

    FILE *write_ptr;
    write_ptr = fopen("NOVONOVO","w");
    fwrite(str, currentSize,1,write_ptr);
    fclose(write_ptr);
}