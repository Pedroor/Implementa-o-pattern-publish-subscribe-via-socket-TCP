#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include "packet.h"


int ssthresh = 20;

struct client{
    int id;
    int last_seq;
    int last_size;
    int lastSeqReceive;
};

struct packet syn_packet(int a, int b){
    struct header cabecalho;
    cabecalho.sequence_number = 12345;
    cabecalho.acknowledgment_number = a;
    cabecalho.connection_id = b;
    cabecalho.flag = 2;
    struct packet syn;
    syn.header = cabecalho;
    return syn;
}

int last_ClientID = 1;

struct client clientes[5];
struct packet syn_ack_packet(int a, int b){
    struct header cabecalho;
    cabecalho.sequence_number = 4321;
    cabecalho.acknowledgment_number = 12346;
    cabecalho.connection_id = last_ClientID;
    cabecalho.flag = 3;

    clientes[last_ClientID].id = last_ClientID;
    clientes[last_ClientID].last_seq = a;
    clientes[last_ClientID].last_size = b;
    clientes[last_ClientID].lastSeqReceive = 0;
    last_ClientID++;
    struct packet syn;
    syn.header = cabecalho;
    return syn;
}

void printarPacote(struct packet pacote){
    printf("--- [ID: %d, Seq: %d, ", pacote.header.connection_id, pacote.header.sequence_number);
    printf("Ack: %d, Flag: %d]\n\n", pacote.header.acknowledgment_number, pacote.header.flag);
    //printf("--- [Dado: %s]\n", pacote.data);
}

int getArqLen(FILE *file){
    fseek(file, 0L, SEEK_END);
    int tamanho = ftell(file);
    fseek(file, 0, SEEK_SET);
    return tamanho;
}

char* getFile2(FILE *file, int *currentSize, int posicao, char *filename){
    file = fopen(filename, "rb");
    int tamanhoArquivo = getArqLen(file);
    fseek(file, (posicao-1) * MAX_DATA_SIZE, SEEK_SET);

    int endData = MAX_DATA_SIZE;
 
    int variavelSemNome = tamanhoArquivo - ((posicao - 1) * MAX_DATA_SIZE);
    if (variavelSemNome < MAX_DATA_SIZE)
        endData = variavelSemNome;

    char buffer[endData];
    *currentSize = endData;
    memset(buffer, 0, sizeof(buffer));

    fread(buffer,endData,1,file);
    buffer[endData] = '\0';
    fclose(file);

    char *str = (char*)malloc(endData);
    memset(str, 0, sizeof(str));
    memcpy(str, buffer, endData);

    char teste[MAX_DATA_SIZE];
    memcpy(teste, str, endData);
    return str;
}

char* juntarStrings(char *s1, char *s2){
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
