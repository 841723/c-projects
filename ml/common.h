#ifndef COMMON_H
#define COMMON_H

#define FALSE 0
#define TRUE 1

#define MAX(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); _a > _b ? _a : _b; })
#define MIN(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); _a < _b ? _a : _b; })

#endif