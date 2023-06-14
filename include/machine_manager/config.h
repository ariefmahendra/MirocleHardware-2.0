
#define _MACHINEDEBUG 1

#if _MACHINEDEBUG
#define _INTERNET_MANAGER_DEBUG 0
#define _MAX_MANAGER_DEBUG 0
#define _MQT_MANAGER_DEBUG 0
#define _PASIEN_MANAGER_DEBUG 0
#define _PERIPHERAL_MANAGER_DEBUG 0
#define _PREFERENCE_MANAGER_DEBUG 0
#define _TERAPI_MANAGER_DEBUG 1
#define _TIME_MANAGER_DEBUG 0
#define DISPLAY_DEBUGGER 0
#endif

#define DEVICE_ID "mirocle_001"

#define MQ_TOPIC_DATA_REQUEST "data_request"
#define MQ_TOPIC_DATA_REPLY "data_reply"
#define MQ_TOPIC_DATA_TERAPI_SEND "data_terapi_send"
#define MQ_TOPIC_STATUS "data_status"

#define MQ_COMMAND_START "terapi_begin"
#define MQ_COMMAND_STOP "terapi_end"
#define MQ_COMMAND_DATA_TERAPI "terapi_data"

#define MQ_FIELD_INFO_PASIEN "info_pasien"

#define MQ_FIELD_SERI_ALAT "seri_alat"
#define MQ_FIELD_ID_TERAPI "id_terapi"
#define MQ_FIELD_SEQUENCE_NUM "sequence"
#define MQ_FIELD_START_TIME "waktu_mulai"
#define MQ_FIELD_END_TIME "waktu_selesai"
#define MQ_FIELD_HEART_RATE "detakJantung"
#define MQ_FIELD_HEART_RATE_AVERAGE "detakJantungRataRata"
#define MQ_FIELD_HEART_BEAT_COUNT "jumlahDetakJantung"
#define MQ_FIELD_O2_SATURATION "saturasiOksigen"
#define MQ_FIELD_CALORY "kalori"
#define MQ_FIELD_CALORY_TOTAL "kaloriTotal"
#define MQ_FIELD_ROTATION_COUNT "putaranPedal"
#define MQ_FIELD_DURATION "durasi"
#define MQ_BUFFER_SIZE (1024 * 5)


#define mq_server "broker.emqx.io"
#define mq_port 1883
#define mq_client_id "mirocle1"
#define mq_username "mirocle"
#define mq_password "123"
