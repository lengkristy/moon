/***************************************************************************************
 * this file used to deal with the messsage on the network
 * coding by haoran dai
 * time:2018-09-04 11:15
 **************************************************************************************/
#ifndef _MOON_MSG_H
#define _MOON_MSG_H
#include "moon_protocol.h"

///////////////////////////////////////////////////////////////
///////////////define message struct///////////////////////////

//define message head
typedef struct _MessageHead{
	moon_char msg_id[50];/*message id,represents the unique identity of the message throughout the message chain*/
	int main_msg_num;/*main message number*/
	int sub_msg_num;/*sub message number*/
}MessageHead;

//define message body
typedef struct _MessageBody{
	moon_char* p_content;/*message content*/
}MessageBody;


//define message struct
typedef struct _Message{
	MessageHead *p_message_head;/*message head*/
	MessageBody *p_message_body;/*message body*/
}Message;

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////




#endif