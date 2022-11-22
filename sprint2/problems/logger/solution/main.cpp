#include "my_logger.h"

int main(){
    int x = 12;
    int y = -21;
    LOG("User entered numbers "sv, x, " and "sv, y);
    return 0;
}