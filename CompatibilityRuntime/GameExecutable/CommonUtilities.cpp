#include "CommonUtilities.h"

bool stringsEqualIgnoreCaseAsciiOnly(const char *a, const char *b) {
    char chB;

    while(*a) {
        char chA = *a++;
        chB = *b++;

        if(chA >= 'A' && chA <= 'Z')
            chA = chA - 'A' + 'a';

        if(chB >= 'A' && chB <= 'Z')
            chB = chB - 'A' + 'a';

        if(chA != chB)
            return false;
    }

    return *b == 0;

}
