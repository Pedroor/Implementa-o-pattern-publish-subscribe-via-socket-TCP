#include "essentials.c"


int main(int argc, char *argv[]){
    if (argc != 4){
        puts("Argumento(s) inesperado");
        exit(0);
    }
    char* IP = argv[1];
    int PORT = atoi(argv[2]);
    char* FILENAME = argv[3];

    if (strcmp(argv[1],"localhost") == 0)
        IP = "127.0.0.1";
    int sockfd, n; 
    struct sockaddr_in servaddr; 
      
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_family = AF_INET;    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){ 
        printf("\n Erro: Conexão falhou \n"); 
        exit(0); 
    }
    FILE *file = fopen(FILENAME, "rb");
    int tamanhoArquivo = getArqLen(file);
    int numeroDePacotes = ceil(tamanhoArquivo/(float)(MAX_DATA_SIZE));
    int atualTamanho;
    getFile2(file, &atualTamanho, numeroDePacotes, FILENAME);
    struct packet syn = syn_packet(INITIAL_SEQ_CLIENT + ((numeroDePacotes - 1)*MAX_DATA_SIZE), atualTamanho);
    sendto(sockfd, &syn, sizeof(struct packet), 0, (struct sockaddr*)NULL, sizeof(servaddr));
    puts("<-- Enviou Syn...");


    struct packet buffer;
    struct packet buffer2;
    int auth = 0;
    int pacoteAtual = 1;
    struct timeval tv;
    int j;
    int janela = 1;
    int bufJ = 0;
    int atraso = 0;

    tv.tv_sec = 3;
    tv.tv_usec = 0;
    while(1){

        if (auth == 0){   
            recvfrom(sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr*) NULL, NULL);
            printf("ConnectionID: %d\n", buffer.header.connection_id);
            if (buffer.header.flag == 3){
                printf("--> Recebeu Syn+Ack \n");
                printarPacote(buffer);
                auth = 1;
                buffer.header.flag = 4; // Somente para entrar no IF abaixo //
            }         
        }      
        
        if (auth == 1 && buffer.header.flag == 4){
            if (pacoteAtual <= numeroDePacotes){
                int i;
                for (i = 0; i < numeroDePacotes && bufJ != numeroDePacotes;i++){
                    printf("[JANELA %d] [SSTHRESH %d]\n", janela, ssthresh);
                    
                    struct packet *pacote;
                    for (j = 0; j < janela;j++){
                        if (pacoteAtual == 4){
                            atraso = 1;
                            i++;
                            bufJ++;
                            pacoteAtual++;
                            puts("Foi pro futuro");
                        }else if (atraso == 1){
                            i = i - 2;
                            bufJ = bufJ - 2;
                            pacoteAtual=  pacoteAtual - 2;
                            atraso = 2;
                            puts("Foi pro passado");
                        }else if (atraso == 2){
                            i = i + 1;
                            bufJ = bufJ + 1;
                            pacoteAtual =  pacoteAtual + 1;
                            atraso = 3;
                            puts("Voltou para o presente");
                        }
                        if (bufJ < numeroDePacotes){
                            int currentSize;
                                pacote = malloc(sizeof (struct packet));
                                char *buffData = getFile2(file, &currentSize, bufJ+1, FILENAME);   
                                pacote->header.connection_id = buffer.header.connection_id;
                                pacote->header.sequence_number = buffer.header.acknowledgment_number + (bufJ * MAX_DATA_SIZE);
                                
                                if (i == 0)
                                    pacote->header.flag = 4;
                                else
                                pacote->header.flag = 0;
                                pacote->header.acknowledgment_number = 0;
                                memcpy (pacote->data, buffData, currentSize);

                           
                            usleep(10*1000);
                            sendto(sockfd, pacote, sizeof(struct packet), 0, (struct sockaddr*)NULL, sizeof(servaddr));
                            
                            printf("<-- Pacote %d enviado (DATA_LENGTH: %d):\n", pacoteAtual, currentSize);
                            printarPacote(*pacote);
                            pacoteAtual++;
                            bufJ++;
                        }    
                        //i++;
                    }
                    //printf("Antes de receber: BufJ: %d BufJreceive: %d\n", bufJ, bufJReceive);
                    int recValue;
                    int saidasRecV = 0;
                    if (bufJ != numeroDePacotes){
                        for (j = 0; j < janela;j++){     
                            //setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
                            //recValue = recvfrom(sockfd, &buffer2, sizeof(buffer2), 0, (struct sockaddr*)NULL, NULL);
                            if (recValue < 0){
                                puts("______________TIMEOUT______________");
                                printf("Janela do Timeout: %d\n", janela);
                                
                                int numeroMagico = (pacote->header.sequence_number - buffer2.header.acknowledgment_number)/MAX_DATA_SIZE;
                                i=-numeroMagico + saidasRecV;
                                bufJ-=numeroMagico + saidasRecV;
                                pacoteAtual-=numeroMagico + saidasRecV;
                                ssthresh = janela;

                                printf("Thresh do Timeout = %d\n", ssthresh);
                                janela = 1;
                            }
                            else{
                                saidasRecV++;
                            }  
                        }
                    }
                    printf("Saíram do Recv: %d\n", saidasRecV);
                    if (recValue < 0){
                        janela = 1;
                    }else{
                        if (janela  >= ssthresh){
                            janela++;
                        }else{
                            janela+=janela;
                        }
                    }
                        
                    // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
                    // int recValue = recvfrom(sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
                    // if (recValue < 0){
                    //     puts("______________TIMEOUT______________");
                    //     i--;
                    //     pacoteAtual--;
                    // }else{
                    //     printf("--> ACK recebido\n");
                    //     if (pacoteAtual == numeroDePacotes + 1){
                    //         struct packet *pacote = malloc(sizeof (struct packet) + MAX_DATA_SIZE);
                    //         pacote->header.connection_id = buffer.header.connection_id;
                    //         pacote->header.sequence_number = buffer.header.acknowledgment_number;
                    //         pacote->header.acknowledgment_number = 0;
                    //         pacote->header.flag = 1; // FIN
                    //         printf("<-- Pacote FIN:\n");
                    //         auth = 2;
                    //         printarPacote(*pacote);
                    //         sendto(sockfd, pacote, sizeof(struct packet), 0, (struct sockaddr*)NULL, sizeof(servaddr));
                    //     }
                    // }

                }
            }
        }
        if (auth == 2 && buffer.header.flag == 4){
            recvfrom(sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
            puts ("--> ACK do FIM Recebido");
            printarPacote(buffer);
            recvfrom(sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
            if (buffer.header.flag = 1){
                puts ("--> FIN Recebido");
                printarPacote(buffer);
                
                struct packet *pacote = malloc(sizeof (struct packet) + MAX_DATA_SIZE);
                pacote->header.connection_id = buffer.header.connection_id;
                pacote->header.sequence_number = buffer.header.acknowledgment_number;
                pacote->header.acknowledgment_number = 0;
                pacote->header.flag = 4;
                puts("<-- Enviando ACK final");
                //sendto(sockfd, pacote, sizeof(struct packet), 0, (struct sockaddr*)NULL, sizeof(servaddr));
                exit(0);
            }
        }
    }

}