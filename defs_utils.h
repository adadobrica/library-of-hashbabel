// Copyright 2022 Dobrica Nicoleta Adriana
#ifndef DEFS_UTILS_H_
#define DEFS_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "ht_utils.h"
#include "book_utils.h"
#include "user_utils.h"

void ADD_DEF(hashtable_t **library);
void GET_DEF(hashtable_t **library);
void RMV_DEF(hashtable_t **library);

#endif  //  DEFS_UTILS_H_
