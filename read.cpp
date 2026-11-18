#include "gnss_processor.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


int main()
{

    GnssProcessor processor;


    std::ifstream file("gnss.csv");


    if(!file.is_open())
    {
        std::cout << "Cannot open gnss.csv\n";
        return 1;
    }


    std::string line;


    // Skip CSV header
    getline(file,line);


    while(getline(file,line))
    {

        std::stringstream ss(line);


        GnssFrame frame;

        char comma;


        ss >> frame.timestamp_ms >> comma
           >> frame.lat >> comma
           >> frame.lon >> comma
           >> frame.hdop >> comma
           >> frame.satellites_used >> comma
           >> frame.fix_type;



        bool result = processor.process(frame);


        if(result)
            std::cout << "ACCEPTED\n";
        else
            std::cout << "REJECTED\n";


        std::cout << "------------------\n";
    }


    file.close();


    return 0;
}