#include <iostream>
#include <string>
#include "highSpeedTrain.h"

/* This file is given as a sample test.
 * Run button triggers the code below.
 * You can change it for your debugging and further testing purposes.
 * It will not be used in the official grading.
 */

int main(){
    HighSpeedTrain *hst1 = new HighSpeedTrain();
    std::cout << *hst1 << std::endl;
    HighSpeedTrain *hst2 = new HighSpeedTrain("Turin", "Florence", 150); 
    std::cout << *hst2 << std::endl;
    HighSpeedTrain *hst3 = new HighSpeedTrain("Florence", "Rome", 90);
    HighSpeedTrain hst4 = *hst3;
    std::cout << hst4 << std::endl;
    
    std::cout << (*hst1) + (*hst2) << std::endl;
    std::cout << (*hst2) + (*hst3) << std::endl;
    
    std::cout << (*hst3 < *hst2) << std::endl;
    std::cout << (*hst3 > *hst2) << std::endl;
    return 0;
}