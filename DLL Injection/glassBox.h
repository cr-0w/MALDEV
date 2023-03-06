#ifndef statSyms_H
#define statSyms_H

#include <conio.h>
#include <stdio.h>
#include <windows.h>

#define KRST "\033[0m"
#define KGRN "\x1B[32m"
#define KRED "\x1B[31m"
#define KYEL "\x1B[33m"

extern char ok[];
extern char in[];
extern char er[];

char ok[] = "(" KGRN "+" KRST ")";
char in[] = "(" KYEL "*" KRST ")";
char er[] = "(" KRED "-" KRST ")";

#endif
