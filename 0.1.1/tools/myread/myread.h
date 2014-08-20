#ifndef _MYSHELL_TOOLS_MYREAD_MYREAD_H_
#define _MYSHELL_TOOLS_MYREAD_MYREAD_H_

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../list/list.h"
#include "../formart/formart.h"

#define BUFFSIZE	512

//Get the file name(store it to list),
//find the file's name contain the tail
//of 'childStr',
int FindFileName(Node *resultHead, const char *path,
			const char *childStr);

//Show more information association with the 'Tab'
//
//Input: head
int ShowMore(Node *head);

//Show the find result.
//If the pointer noShow is not NULL
//show file's name without '*noShow'
//and store it into cmd,
//else show the full file name.
//If bSpace is 1 print a blank-space.
int ShowResult(Node *resultHead, const char *noShow,\
			int bSpace, char *cmd);

//Get the index of the last same word,
//which means the word before index
//in source as same as in dest.
//If return -1, means string dest and
//string source isn't the same class.
int GetIndexOfSameWord(const char *dest,
		       const char *source);

//Response for clicking 'Tab' button
int Tab(Node *resultHead, const char *info,\
	const char *path, int ti, char *cmd);

//Enable not canonical mode:
//not echo, immediately return when get one
//character
int InitKeyboard(struct termios *termios_p);

//Recover canonical mode
int CloseKeyboard(struct termios termios_p);

//Return the tail of cmd
const char * GetTail(const char *cmd, char token);

//This is my_read function,
//answer to click button 'Tab', 'up', 'down', 'left'
//'right' and 'Backspace'.
int my_read(Node *cmdHead, const char *info,
		char *cmd, int cmdMax);

//Add nch to cmd int pos index of cmd.
int Add(char *cmd, int pos, char nch);

//Delete word in cmd with the pos index
int Delete(char *cmd, int pos);

//This function will execute when click
//button 'up' or 'down'.
int UpOrDown(Node *cmdHead, int cursorPos,
	       int nodePos, char *lastBuf,
				char *cmd);



#endif
