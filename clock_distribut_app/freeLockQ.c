/*
 * ringBuffer.c
 *
 *  Created on: Apr 17, 2014
 *      Author: root
 */
/* host header file */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
#include <semaphore.h>
#include "config.h"

#if	0
/* package header files */
#include <ti/syslink/Std.h>     /* must be first */
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/SysLink.h>

/* local header file */
#include "../shared/AppCommon.h"
#include "../shared/SystemCfg.h"
#endif

#include "freeLockQ.h"
//#include "log.h"



void printf_lq_num(queue_t *pque_coll)
{
	SysInfoTrace(".....SN_Free_Lock_Queue_num is full[%d]\n",queue_size(pque_coll));

}
/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/
#if 1
int SN_Free_Lock_Queue_full(queue_t *pque_coll) {
	if (pque_coll == NULL) {
		SysErrorTrace(".......SN_Free_Lock_Queue_full queue is null\n");
		return -1;
	}
	if (queue_size(pque_coll) >= QUEUE_MAX_SIZE) {
		SysErrorTrace(".....SN_Free_Lock_Queue_full,queue is full\n");
		return 1;
	} else
		return 0;
}
int SN_Free_Lock_Queue_Create(queue_t **pque_coll) {
	if (NULL == pque_coll) {
		//SysErrorTrace("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc\n");
		return -1;
	}

	type_register(SN_Msg_Param, NULL, NULL, NULL, NULL);
	type_duplicate(SN_Msg_Param,struct SN_MsgParam);
	*pque_coll = create_queue(SN_Msg_Param);

	if (*pque_coll == NULL) {
		//SysErrorTrace("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj\n");
		SysErrorTrace("SN_Free_Lock_Queue_Create failed!\n");
		return -1;
	}
	queue_init(*pque_coll);
	return 0;
}

int SN_Free_Lock_Queue_Get(SN_Msg_Param *node, queue_t *pque_coll) {
	if (NULL == pque_coll || NULL == node) {
		if (NULL == pque_coll)
			return -1;
	}
	//if (queue_empty(pque_coll))
	if (queue_size(pque_coll) <= 0) {
		SysErrorTrace("in SN_Free_Lock_Queue_Get error, queue_size(pque_coll) = %d\n ",queue_size(pque_coll));
		return -1;
	}
	//printf_lq_num(pque_coll);
//	SysDebugTrace("！！SN_Free_Lock_Queue_Get after pop msg_num = %d\n",queue_size(pque_coll));

	SN_Msg_Param *p = (SN_Msg_Param *) queue_front(pque_coll);

	if (NULL == p)
	{
	    SysErrorTrace("SN_Free_Lock_Queue_Get error,queue_front return NULL\n");
		return -1;
	}

	memcpy(node, p, sizeof(SN_Msg_Param));
	//memcpy(node, (SN_Msg_Param *) queue_front(pque_coll), sizeof(SN_Msg_Param));
	return 0;
}

int SN_Free_Lock_Queue_Push(const char *data, uint16 length, uint16 cmd,
		queue_t *pque_coll)
//int SN_Free_Lock_Queue_Push(Msg_Param node, queue_t *pque_coll)
{
	int ret = 0;
	QUEUE_MSG_T * tmpmsg = (QUEUE_MSG_T *)data; 
	
	if (NULL == data || NULL == pque_coll) {
		SysErrorTrace(
				"SN_Free_Lock_Queue_push NULL == data || NULL == pque_coll\n");
		if (NULL == data)
			SysErrorTrace("SN_Free_Lock_Queue_push NULL == data \n");
		else if (NULL == pque_coll)
			SysErrorTrace("SN_Free_Lock_Queue_push NULL == pque_coll\n");
		return -1;
	}
	if (SN_Free_Lock_Queue_full(pque_coll) == 1) {
		SysErrorTrace("SN_Free_Lock_Queue_push SN_Free_Lock_Queue_full\r\n");
		
		SysErrorTrace("@@PUSH:tmpmsg->msg_type[%x]length[%d]cmd[%d]\r\n",tmpmsg->msg_type,length,cmd);
		ret = SN_Free_Lock_Queue_Pop(pque_coll);
                if(ret == -1)
                {
                        SysErrorTrace("SN_Free_Lock_Queue_Push  queue is full,pop error\n ");

                        return -1;
                }
                SysDebugTrace("SN_Free_Lock_Queue_Push queue is full,pop success,queue_size = %d\n",queue_size(pque_coll));

	}
	SysDebugTrace("@@@PUSH:tmpmsg->msg_type[%x]length[%d]cmd[%d] queue_size[%d]\r\n",tmpmsg->msg_type,length,cmd,queue_size(pque_coll));
//	SysDebugTrace("@@@PUSH:QUEUE_LEN[%d],sizeof(SN_Msg_Param)[%d]\r\n",QUEUE_LEN,sizeof(SN_Msg_Param));

	

	SN_Msg_Param node;
	node.cmd = cmd;
	node.length = length;

#if 0
	node.data = (char *)malloc(node.length);
	if(NULL == node.data)
	{
		SysErrorTrace("malloc failed!\n");
		return -1;
	}
#endif
	memcpy(node.data, data, node.length);

	queue_push(pque_coll, &node);

#if 0
	if(NULL !=node.data )
	{
		free(node.data);
		node.data = NULL;
	}
#endif
	return 0;
}
int SN_Free_Lock_Queue_Pop(queue_t *pque_coll) {
	if (NULL == pque_coll)
		return -1;
	if (queue_empty(pque_coll))
		return -1;
	
//	SysDebugTrace("!!SN_Free_Lock_Queue_Pop before pop msg_num = %d\n",queue_size(pque_coll));
	queue_pop(pque_coll);
//	SysDebugTrace("!!SN_Free_Lock_Queue_Pop after pop msg_num = %d\n",queue_size(pque_coll));
	return 0;
}
int SN_Free_Lock_Queue_Destroy(queue_t **pque_coll) {
	if (NULL == pque_coll || NULL == *pque_coll)
		return -1;
	queue_destroy(*pque_coll);
	return 0;
}
#endif
