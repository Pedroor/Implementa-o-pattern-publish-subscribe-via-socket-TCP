#include "essentials.c"
char *Arquivo = "arquivo.txt";

char* getDataPacket2(int posicao,  char *FILENAME){
    FILE *file = fopen(FILENAME, "rb");
    char c;
    char buffer[MAX_DATA_SIZE];
    int tamanhoArquivo = getArqLen(file);

    fseek(file, (posicao-1) * MAX_DATA_SIZE, SEEK_SET);
    int fimLoop = MAX_DATA_SIZE;
    int variavelSemNome = tamanhoArquivo - ((posicao - 1) * MAX_DATA_SIZE);

    printf(" VARIAVEL SEM NOME: %d", variavelSemNome);

    if (variavelSemNome < MAX_DATA_SIZE)
        fimLoop = variavelSemNome;

    fread(buffer,sizeof(buffer),1,file);
    
    int i;
    buffer[fimLoop] = '\0';
    char *str = (char*)malloc(fimLoop*sizeof(char));
    strcpy(str, buffer);
    fclose(file);
    return str;
}

int main(int argc, char *argv[]){
    FILE *ArquivoOriginal;
    ArquivoOriginal = fopen(Arquivo,"rb");

    int tamanhoArquivo = getArqLen(ArquivoOriginal);
    int numeroDePacotes = ceil(tamanhoArquivo/(float)(MAX_DATA_SIZE));

    int pacoteAtual = 1;
    printf("TAMANHO ARQUIVO: %d, NUMERO DE PACOTES: %d", tamanhoArquivo, numeroDePacotes);

    char *str = getDataPacket2(1, Arquivo);
    int tamanho = strlen(str);

    FILE *write_ptr;
    write_ptr = fopen("NOVONOVO","ab");

    printf("\nTamanho: %d\n", tamanho);
    printf("MSG1:%s\n", str);

    char newBuffer[tamanho];
    strcpy(newBuffer, str);

    printf("MSG1:%s\n", str);
    printf("Buffer:%s\n", newBuffer);
    fwrite(newBuffer,sizeof(newBuffer),1,write_ptr);

    //fread(buffer,sizeof(buffer),1,ptr);
    /*fseek(ptr, 2, SEEK_SET);
    char c;
    c=fgetc(ptr);
	printf("CHAR = [%c]", c);
   

    char *str = buffer;
    printf("%s", str);
    */
    /*char buffer2[tamanhoArquivo];
    int i;
    for(i = 0; i < tamanhoArquivo; i++){
        buffer2[i] = str[i];
    }
    FILE *write_ptr;
    write_ptr = fopen("NOVONOVO","wb");
    fwrite(buffer2,sizeof(buffer2),1,write_ptr);*/

    // ver o free 
   /* while(pacoteAtual <= numeroDePacotes){
        FileToWrite = fopen("RESULTADO","ab");
        printf("\nDATA_LENGTH: %ld", strlen(getDataPacket2(pacoteAtual, Arquivo)));
        fwrite(getDataPacket2(pacoteAtual, FileToWrite),strlen(getDataPacket2(pacoteAtual,FileToWrite)),1,FileToWrite);
        fclose(FileToWrite);
        pacoteAtual++;
    }*/
   /*
    char buffer[512000];
    FILE *ptr;
    ptr = fopen("foto.jpg","rb");
    fread(buffer,sizeof(buffer),1,ptr);
    printf("%s", buffer);
    fseek(ptr, 0L, SEEK_END);
    int sz = ftell(ptr);
    printf("TAMANHO = %d", sz);

    FILE *write_ptr;

    write_ptr = fopen("fotoEscrita.jpg","wb");

   fwrite(buffer,sizeof(buffer),1,write_ptr);*/

}
