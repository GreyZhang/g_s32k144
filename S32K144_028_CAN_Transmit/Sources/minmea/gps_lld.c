#include "gps_lld.h"


void gps_lld_display_msg_type(enum minmea_sentence_id type)
{
    switch (type)
    {
    case MINMEA_INVALID:
        printf("message type is MINMEA_INVALID\n");
        break;
    case MINMEA_UNKNOWN:
        printf("message type is MINMEA_UNKNOWN\n");
        break;
    case MINMEA_SENTENCE_RMC:
        printf("message type is MINMEA_SENTENCE_RMC\n");
        break;
    case MINMEA_SENTENCE_GGA:
        printf("message type is MINMEA_SENTENCE_GGA\n");
        break;
    case MINMEA_SENTENCE_GSA:
        printf("message type is MINMEA_SENTENCE_GSA\n");
        break;
    case MINMEA_SENTENCE_GLL:
        printf("message type is MINMEA_SENTENCE_GLL\n");
        break;
    case MINMEA_SENTENCE_GST:
        printf("message type is MINMEA_SENTENCE_GST\n");
        break;
    case MINMEA_SENTENCE_GSV:
        printf("message type is MINMEA_SENTENCE_GSV\n");
        break;
    case MINMEA_SENTENCE_VTG:
        printf("message type is MINMEA_SENTENCE_VTG\n");
        break;
    case MINMEA_SENTENCE_ZDA:
        printf("message type is MINMEA_SENTENCE_ZDA\n");
        break;
    default:
        printf("wrong type is input!\n");
        break;
    }
}
