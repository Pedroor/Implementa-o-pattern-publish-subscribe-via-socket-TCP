#include "essentials.c"
int lastSeqReceive;
int main(int argc, char *argv[]){
    if (argc != 3){
        puts("Argumento(s) inesperado");
        exit(0);
    }
    char *result = juntarStrings(argv[2], "/");
    int PORT = atoi(argv[1]);

    int listenfd; 
    struct sockaddr_in servaddr; 
    bzero(&servaddr, sizeof(servaddr)); 
  
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);         
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_family = AF_INET;  
   
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // FLAGS
    // 1 = FIN          001
    // 2 = SYN          010
    // 3 = SYN + ACK    011
    // 4 = ACK          100
    int var = 0;
    while(1){
    
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        struct packet *buffer = malloc(sizeof (struct packet));
        memset(buffer, 0, sizeof buffer);
        int n = recvfrom(listenfd, buffer, sizeof(struct packet), 0, (struct sockaddr*)&cliaddr,&len);
        
        if (buffer->header.flag == 2){
            struct packet syn_ack = syn_ack_packet(buffer->header.acknowledgment_number, buffer->header.connection_id);        
            sendto(listenfd, &syn_ack, sizeof(struct packet), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
            printf("--- Cliente registado ID %d\n", clientes[last_ClientID-1].id);
            clientes[last_ClientID-1].lastSeqReceive = buffer->header.sequence_number;
            clientes[last_ClientID-1].seqEsperada = buffer->header.sequence_number + MAX_DATA_SIZE;
            buffer->header.connection_id = last_ClientID-1;

            puts("--- Syn + Ack enviado.");
        }
         if (clientes[buffer->header.connection_id].lastSeqReceive != buffer->header.sequence_number 
        /*&& clientes[buffer->header.connection_id].lastSeqReceive < buffer->header.sequence_number*/){
            var++;
            //if (var == 2){sleep(4);}
            if (buffer->header.flag == 4 || buffer->header.flag == 0){
                //usleep(50*1000);
                struct client *cliente = &clientes[buffer->header.connection_id];
                if (clientes[buffer->header.connection_id].seqEsperada != buffer->header.sequence_number && buffer->header.flag == 0){
                    if (cliente->pacotesBuffer->header.sequence_number == buffer->header.sequence_number - MAX_DATA_SIZE){
                    puts("Retirando pacote do buffer");
                    cliente->nPacotes--;
                    //&buffer, cliente->pacotesBuffer[cliente->nPacotes];
                    }else{
                        puts("--- [FORA DE ORDEM DETECTADA");
                        puts("--- Salvando pacote em um buff...");
                        cliente->pacotesBuffer[cliente->nPacotes] = *buffer;
                        printarPacote(cliente->pacotesBuffer[cliente->nPacotes]);
                        cliente->nPacotes++;
                    }
                }
                if (cliente->nPacotes == 0){
                    cliente->lastSeqReceive = buffer->header.sequence_number;
                    cliente->seqEsperada = buffer->header.sequence_number + MAX_DATA_SIZE;
                }
                mkdir(argv[2], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

                char bufID[2];    
                sprintf(bufID, "%d", buffer->header.connection_id);
                if (buffer->header.flag == 4)
                    printf("<-- Recebeu ACK do cliente %s\n", bufID);
                else
                    printf("<-- Recebeu SEG do cliente %s\n", bufID);

                printf("Recebi seq: %d\n", buffer->header.sequence_number);
                if (cliente->nPacotes > 0){
                    if (cliente->pacotesBuffer->header.sequence_number == buffer->header.sequence_number + MAX_DATA_SIZE){
                        puts("aaaaaaaaaaaaaaaaaaaaa retirar");
                            FILE *write_ptr;
                            write_ptr = fopen(juntarStrings(result, bufID),"ab");
                            if (buffer->header.sequence_number > cliente->last_seq){
                                fwrite(buffer->data, cliente->last_size,1,write_ptr);
                                fwrite(cliente->pacotesBuffer->data, cliente->last_size,1,write_ptr);
                            }else{
                                fwrite(buffer->data, MAX_DATA_SIZE,1,write_ptr);
                                 fwrite(cliente->pacotesBuffer->data, MAX_DATA_SIZE,1,write_ptr);
                            }
                            fclose(write_ptr);
                        printf("Escreveu %d\n", buffer->header.sequence_number);
                        printf("Se esperada: %d", cliente->seqEsperada);
                        printf("bbbb %d", cliente->pacotesBuffer->header.sequence_number);
                        cliente->lastSeqReceive = buffer->header.sequence_number;
                        cliente->seqEsperada = buffer->header.sequence_number + MAX_DATA_SIZE;
                        buffer->header.sequence_number = cliente->pacotesBuffer->header.sequence_number;
                    
                        strcpy(buffer->data, cliente->pacotesBuffer->data);
                    }
                }
                printarPacote(*buffer);
               
                if (cliente->nPacotes == 0){
                    FILE *write_ptr;
                    write_ptr = fopen(juntarStrings(result, bufID),"ab");
                    if (buffer->header.sequence_number > cliente->last_seq){
                        fwrite(buffer->data, cliente->last_size,1,write_ptr);
                    }else{
                        fwrite(buffer->data, MAX_DATA_SIZE,1,write_ptr);
                    }
                    fclose(write_ptr);
                    printf("Escreveu %d\n", buffer->header.sequence_number);
                }
                struct packet pacote;
                pacote.header.connection_id = buffer->header.connection_id;
                pacote.header.sequence_number = 0;
                if (cliente->nPacotes == 0){
                    pacote.header.acknowledgment_number = cliente->seqEsperada + MAX_DATA_SIZE;
                }
                pacote.header.flag = 4;
               
                sendto(listenfd, &pacote, sizeof(struct packet), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
                printf("Enviando pacote %d p/ cliente %s\n", var,bufID);
                printf("Espero receber: %d\n\n", cliente->seqEsperada);


            }else if(buffer->header.flag == 1){
                clientes[buffer->header.connection_id].lastSeqReceive = buffer->header.sequence_number;
                clientes[buffer->header.connection_id].seqEsperada = buffer->header.sequence_number + MAX_DATA_SIZE;
                
                printf("<-- Recebeu FIN do cliente %d\n", buffer->header.connection_id);
                struct packet pacote;
                pacote.header.connection_id = buffer->header.connection_id;
                pacote.header.sequence_number = 0;
                pacote.header.acknowledgment_number = buffer->header.sequence_number + 1;
                pacote.header.flag = 4;
                sendto(listenfd, &pacote, sizeof(struct packet), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
                puts("--- Enviando ACK do FIM");
            
                pacote.header.acknowledgment_number = 0;
                pacote.header.flag = 1;
                sendto(listenfd, &pacote, sizeof(struct packet), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
                printf("--- Conexão com cliente %d fechada.\n", pacote.header.connection_id);
            }

        }
        //sleep(1);
        //usleep(100*1000);

    }
}