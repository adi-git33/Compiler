#include "RuntimeFunctions.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;
std::string temp;

// g++ -std=c++17 exampleProg2.cpp -o ex2.exe
// ./ex2.exe

int main()
{
    SetLangVar<string> highTech;
    SetLangVar<string> gaming;
    SetLangVar<string> software;
    SetLangVar<string> hardware;
    SetLangVar<string> industrial;
    highTech = std::string("{\"Apple\",\"Google\",\"Microsoft\",\"Nvidia\",\"Adobe\",\"Oracle\",\"Sap\"}");
    highTech = highTech + std::string("{\"Paypal\",\"Nice\",\"Amdocs\",\"OpenAI\",\"Ford\",\"Mercedes\"}");
    gaming = std::string("{\"Sony\",\"Apple\",\"Microsoft\",\"Google\",\"Nintendo\",\"Playtika\"}");
    software = std::string("{\"Apple\",\"Microsoft\",\"Oracle\",\"Google\",\"Sap\",\"Paypal\",\"Playtika\",\"Amdocs\",\"OpenAI\"}");
    hardware = std::string("{\"Apple\",\"Nice\",\"Sony\",\"Google\",\"Cummins\",\"Nucor\",\"Microsoft\",\"Nvidia\"}");
    industrial = std::string("{\"Caterpillar\",\"Cummins\",\"Nucor\"}");
    std::cout << "Companies that sell hardware & software:" << ' ';
    std::cout << software * hardware << std::endl;
    SetLangVar<string> highSW;
    highSW = software * highTech;
    if (highSW == software)
    {
        std::cout << "All software companies are high-tech companies:" << ' ';
        std::cout << highSW << std::endl;
    }
    else
    {
        std::cout << "Not alll software companies are high-tech companies:" << ' ';
        std::cout << highSW << std::endl;
    }
    highSW = highSW + "Playtika";
    if (highSW == software)
    {
        std::cout << "Now all software companies are high-tech companies: " << ' ';
        std::cout << highSW << std::endl;
    }
    else
    {
        std::cout << "Not alll software companies are high-tech companies: " << ' ';
        std::cout << highSW << std::endl;
    }
    std::cout << "Companies that do software or hardware:" << ' ';
    std::cout << software + hardware << std::endl;
    if (!(industrial * software))
    {
        std::cout << "No industrial companies sell software" << std::endl;
    }
    std::cout << "Companies that sell Hardware but not Gaming Software: " << ' ';
    std::cout << hardware - (software * gaming) << std::endl;

    return 0;
}