#include <time.h>
#include <stdlib.h>

#include "common.h"
#include "random.h"

/**********************************************
 Global Variables
**********************************************/
int _rand_has_been_inited = FALSE;


/**********************************************
  Structs
**********************************************/



/**********************************************
 Helper functions
**********************************************/



/**********************************************
   Exported functions
***********************************************/

float get_random(float min, float max) {
    if (!_rand_has_been_inited) {
        srand(time(NULL));
        _rand_has_been_inited = TRUE;
    }

    float r = min + (float)rand() / ((float)RAND_MAX / (max - min));
    return r;
}

