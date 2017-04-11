//
// Created by Ech on 4/11/17.
//

#ifndef PCH2CSD_UTILS_H
#define PCH2CSD_UTILS_H

#include <stdio.h>

void NM_MakeModulePathInt(char* str, char* dir, int module) {
    sprintf(str, "%s/%d.txt", dir, module);
}



#endif //PCH2CSD_UTILS_H
