#define MAX_DATA_SIZE 512
#define INITIAL_SEQ_CLIENT 12345

struct header
{
    uint32_t sequence_number;
    uint32_t acknowledgment_number;
    uint16_t connection_id;
    uint16_t flag;
};

struct packet
{
    struct header header;
    char data[MAX_DATA_SIZE];
};
