#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
int main() {
    std::ifstream inFile;
    inFile.open("../../WebInterface_design/index.html"); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    std::string str = strStream.str(); //str holds the content of the file

    std::cout << str << "\n"; //you can do anything with the string!!!
}