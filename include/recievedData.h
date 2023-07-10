

#define SIZE_RECIEVED_DATA_LORA 50

typedef struct LoRaReceivedDataStruct{

    uint8_t *data;
    uint8_t rc_Data [SIZE_RECIEVED_DATA_LORA];
    void mallocFree() {
		free(this->data);
	}
    
}LoRaReceivedDataStruct_t;



