#ifndef __LS_H_
#define __LS_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <assert.h>

#include "../list/list.h"
#include "../formart/formart.h"

#define BUFFMAX		256
#define ONE_K		1024
#define PRECISION 	0.0000001
#define OFFSET      50

#define DEF	0//00
#define _A	1//01
#define _L	2//10
//#define _AL	3//11
#define _H	4//100

#define IS_UR(mode)	(mode & S_IRUSR)
#define IS_UW(mode)	(mode & S_IWUSR)
#define IS_UX(mode)	(mode & S_IXUSR)

#define IS_GR(mode)	(mode & S_IRGRP)
#define IS_GW(mode)	(mode & S_IWGRP)
#define IS_GX(mode)	(mode & S_IXGRP)

#define IS_OR(mode)	(mode & S_IROTH)
#define IS_OW(mode)	(mode & S_IWOTH)
#define IS_OX(mode)	(mode & S_IXOTH)
#define IS_OT(mode)	(mode & S_ISVTX)
#define IS_LOWER_OT(mode)	(IS_OT(mode) && IS_OX(mode))

#define IS_X(mode)	(IS_UX(mode) || IS_GX(mode) || IS_OX(mode))

#define IS_SU(mode)	(S_ISUID & mode)
#define IS_SG(mode)	(S_ISGID & mode)
#define IS_S(mode)	((IS_SU(mode)) || (IS_SG(mode)))
#define IS_LOWER_SU(mode)	(IS_UX(mode) && IS_SU(mode))
#define IS_LOWER_SG(mode)	(IS_GX(mode) && IS_SG(mode))

typedef int arg_type;

//Return the value of sdata raised to
//the power of exponent.
//
//Input: sdata, exponent
float _pow(float sdata, int exponent);

//Get max length of these file's link-num,
//ownername, ownergroupname and size.
//
//Input: statbuf, type, maxStyle
//Output: maxStyle
int get_length(struct stat statbuf, arg_type type, char maxStyle[]);

//Get print style for '-l'.
//It only to get the length of file-linknum,
//file-size, file-owner and file-ownergroup.
//
//Input: filePath, type
//Output: arg
int get_l_style(const char *filePath, arg_type type, char style[]);

//This function is used to get the file's detail-information
//when mark '-l'
//
//Input: statbuf, type, style
//Output: temStore
void print_l(struct stat statbuf, arg_type type,
                    char style[], char tmpStore[]);

//Scan the specified directory
//
//Input: dir, type
//Output: head
bool show_dir(const char *dir, arg_type type, Node *head);

//Scan the specified file
//
//Input: file, type, statbuf, style
//Output: head
bool show_file(const char *file, arg_type type,\
	struct stat statbuf, char style[], Node *head);

//Copy source list to dest list
//
//Input: source
//Output: dest
int copy_list(Node *dest, const Node *source);

//Show not mark '-l' result
//
//Input: head
int show_no_l(Node *head);

//Show mark '-l' result.
//
//Input: head
int show_l(Node *head);

// Get info from buff.
int ls(const char *argv[]);

#endif
