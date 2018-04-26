/*
 * freeLockQ.h
 *
 *  Created on: Apr 17, 2014
 *      Author: root
 */

#ifndef FREELOCKQ_H_
#define FREELOCKQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstl/cqueue.h>
#include "snmp.h"
//#include "main.h"

#define QUEUE_MAX_SIZE  2048

#define		__LOOP_QUEUE__
#undef		__LOOP_QUEUE__

/* ring buffer [RESERVE] */
typedef	struct{
	char *buffer;
	uint32 size;
	uint32 in;
	uint32 out;
}RBuffer;

#ifdef	__LOOP_QUEUE__
#define		QUEUE_NODE_LEN		10
#define		QUEUE_NODE_DATA_LEN	4096
typedef	struct MsgParam Msg_Param;
struct MsgParam{
	uint16 cmd;
	uint16 length;
	char data[QUEUE_NODE_DATA_LEN];
};

typedef	struct LinkQueue{
	uint32 head, tail;
	Msg_Param node[QUEUE_NODE_LEN];
	sem_t sem;
}LinkQueue;

#else
/* data struct */
typedef	struct MsgParam	Msg_Param;
struct MsgParam{
	uint16 cmd;
	uint16 length;
	Msg_Param *next;
	char *data;
};

typedef	struct LinkQueue{
	Msg_Param *head;
	Msg_Param *tail;
	sem_t sem;
	//pthread_mutex_t		qmutex;
}LinkQueue;
#endif

typedef struct{
	LinkQueue send;
	LinkQueue recv;
}DataObject;

/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/
#if 1

#define  QUEUE_LEN  sizeof(QUEUE_MSG_T)

typedef	struct SN_MsgParam	SN_Msg_Param;
struct SN_MsgParam{
	uint16 cmd;
	uint16 length;
	int8 data[QUEUE_LEN];
};

typedef struct SN_LinkQueue{
	queue_t  *queue;
	sem_t sem;
}SN_LinkQueue;

typedef struct SN_DataObject{
	SN_LinkQueue send;
	SN_LinkQueue recv;
	SN_LinkQueue send_ack;
}SN_DataObject;

#endif


/* free lock */
#define	CAS	__sync_bool_compare_and_swap


/* global data */
/*
extern	Msg_Param *headDMR, *tailDMR;	//DMR data Queue
extern	Msg_Param *headPar, *tailPar;	//configer data Queue
extern	Msg_Param *headDSP, *tailDSP;	//send data to DSP Queue
extern	Msg_Param *headSyslinkAddr, *tailSyslinkAddr;
*/
/*
extern	LinkQueue		linkRtpSingle, linkRtpMulti;
extern	LinkQueue		linkDspSend, linkDspRecv;
extern	LinkQueue		linkDsp1Send, linkDsp1Recv;
extern	LinkQueue		linkFbSend, linkFbRecv;
extern	LinkQueue		linkSnmpSend, linkSnmpRecv;
*/

/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/
#if 1
int SN_Free_Lock_Queue_Create(queue_t **pque_coll);
int SN_Free_Lock_Queue_Get(SN_Msg_Param *node, queue_t *pque_coll);
int SN_Free_Lock_Queue_Push(const char *data, uint16 length, uint16 cmd,
		queue_t *pque_coll);
int SN_Free_Lock_Queue_Pop(queue_t *pque_coll);
int SN_Free_Lock_Queue_Destroy(queue_t **pque_coll);

#endif


/* extern function */
extern	int	Free_Lock_Queue_Create(LinkQueue *q);
extern	int	Free_Lock_Queue_Destroy(LinkQueue *q);
extern	int	Free_Lock_Queue_Push(const char *data, uint16 length, uint16 cmd, LinkQueue *q);
extern	int	Free_Lock_Queue_Pop(Msg_Param* node, LinkQueue* q);
extern	int	Free_Lock_Queue_Get(Msg_Param* node, LinkQueue* q);

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFFER_H_ */
