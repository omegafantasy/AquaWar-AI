#include "Action.hpp"
#include<random>
#include<ctime>

int main()
{
    srand(time(0));
    AI *myAI = new AI();
    myAI->run();
    delete (myAI);
    return 0;
}