#ifndef NFA_H
#define NFA_H

#include <stdbool.h>

#include "thompson.h"

bool handleInput(const char *input, struct NfaInfo *nfa, bool endAnchor);

#endif
