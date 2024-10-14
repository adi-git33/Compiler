#include "RuntimeFunctions.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

// g++ -std=c++17 exampleProg1.cpp -o ex1.exe
// ./ex1.exe

using namespace std;
std::string temp;

int main()
{
    SetLangVar<string> classCol;
    SetLangVar<string> highGradesStudents;
    SetLangVar<string> lowGradeStudent;
    SetLangVar<string> avgGradeStudents;
    SetLangVar<int> grades;
    SetLangVar<int> gradesHigh;
    int grd;
    std::string student;
    classCol = std::string("{\"Rafi_Suisa\",\"Tamar_Evan\",\"Avi_Maoz\",\"Eli_Kamer\",\"Shlomit_Raz\",\"Haim_Mizrachi\",\"Moshe_Samocha\",\"Tali_Raban\",\"Sharon_Tal\",\"Gal_Elbaz\"}");
    for (int i = 0; i < classCol.getSize(); i++)
    {
        if (classCol.getSize() >= i)
            student = classCol[i];
        std::cout << "Grades for: " << ' ';
        std::cout << student << std::endl;

        std::cout << ">";
        std::cin >> grd;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (grd >= 90)
        {
            gradesHigh = gradesHigh + grd;

            highGradesStudents = highGradesStudents + student;
        }
    }
    if (!!gradesHigh)
    {
        std::cout << "Number of top grades: " << ' ';
        std::cout << gradesHigh.getSize()
                  << std::endl;

        std::cout << "Top grades are: " << ' ';
        std::cout << gradesHigh << std::endl;

        std::cout << "High grade students are: " << ' ';
        std::cout << highGradesStudents << std::endl;
    }
    std::cout << "Low Grade Students >";
    std::getline(std::cin, temp);
    lowGradeStudent.insertFromString(temp);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    temp.clear();
    for (int i = 0; i < lowGradeStudent.getSize(); i++)
    {
        if (lowGradeStudent.getSize() >= i)
            student = lowGradeStudent[i];
        std::cout << "Get better next time: " << ' ';
        std::cout << student << std::endl;
    }
    avgGradeStudents = classCol - highGradesStudents - lowGradeStudent;
    std::cout << "Students with good grades: " << ' ';
    std::cout << avgGradeStudents << std::endl;
    return 0;
}