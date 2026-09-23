// only put things required by main.c
#ifndef SHUNTING_H
#define SHUNTING_H

#include "tokenize.h"
// returning struct because it has length and pointer value of array
// so we are doing pass by value we are getting the copy
// getting copy of address is no issue as address points still to array
struct token_list convertIntoPost(struct token_list tokens);
struct token_list insertConcats(struct token_list infix);

#endif
