#ifndef __PROTOCOL_COMMON_H__
#define __PROTOCOL_COMMON_H__
#include "oss.h"
/////////////////////////////////////////////////////////////////
#define   MSISDN_LEN        (16)
#define   MAX_IMSI_LEN      (16)
#define   IMSI_LEN          (15)
#define   IMEI_LEN          (15)

#define   IPV4_ADDR_LEN     (4)
#define   MAX_OPERATOR_NUM  (8)

#define  MAX_MSG_LEN        (4096) 
#define  MAX_SMS_LEN        (256)
#define  MAX_CREDITS_MSG_LEN  MAX_SMS_LEN

#define   MCC_LEN  (3)
#define   MNC_LEN  (2)
#define   MAX_MNC_PER_OPERATOR (4)

#define  MSG_FROM_QUERY_SERVER      0
#define  MSG_FROM_MODEM_CONTROLLER  1
#define  MSG_FROM_NOTIFY_SERVER     2

/////////////////////////////////////////////////////////////////
#define   VAR_NAME(p)                          (#p)
/////////////////////////////////////////////////////////////////

#define  MAX_MODEM_NUM_PER_UNIT      (16)
#define  MAX_UART_PORT_LEN          (128)
/////////////////////////////////////////////////////////////////
#define  MOBILE_NOTIFY_UDP_SERVER_PORT           (4224)
#define  MODEM_CONTROLLER_UDP_SERVER_PORT        (4223)
#define  MOBILE_ROUTE_UDP_SERVER_PORT            (4222)
#define  MOBILE_ROUTE_TCP_SERVER_PORT            (4222)

/*************************************************************/
typedef struct TagMccMnc{
	u8   isValid;
	u8   abyMcc[MCC_LEN];
	u8   abyMnc[MNC_LEN];
	u8   num;
}TPlmn;

/*************************************************************/
typedef struct  TagMsgHead {
	u32   iMsgType;
	u32   iMsgLen;
}TMsgHead;


/////////////////////////////////////////////////////////////////


#endif