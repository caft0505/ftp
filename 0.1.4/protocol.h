#ifndef _PROTOCOL_H
#define _PROTOCOL_H

// Title information.
#define PT_HEAD     0xAA

#define PT_ERROR    0x00
#define PT_OK       0x01

#define PT_DATA     0x02
#define PT_OVER     0x03

// Assistent operator.
#define PT_LS       0x10
#define PT_CD       0x11
#define PT_EXIT     0x12
#define PT_RM       0x13
#define PT_MKDIR    0x14

#define PT_CDLOCAL  0x15
#define PT_CDREMOTE 0x16

#define CLEAR       0x17
#define PT_PWD      0x18

#define PT_EENT     0x19
#define PT_NOENT    0x1a
#define PT_DOERR    0x1b

// Upload file.
#define PT_UPREQ    0x30
#define PT_FEXIST   0x32
#define PT_COVER    0x33
#define PT_NOCOVER  0x34

// Download file.
#define PT_DWREQ    0x35
#define PT_FNOEXIST 0x37

/*************************************/
// Command.
#define CMD_UPLD        PT_UPREQ
#define CMD_DWLD        PT_DWREQ
#define CMD_LS          PT_LS
#define CMD_CD          PT_CD
#define CMD_CDLOCAL     PT_CDLOCAL
#define CMD_CDREMOTE    PT_CDREMOTE
#define CMD_RM          PT_RM
#define CMD_EXIT        PT_EXIT
#define CMD_CLEAR       CLEAR
#define CMD_PWD         PT_PWD
#define CMD_MKDIR       PT_MKDIR

#define CMD_ERR         0xff

#endif
