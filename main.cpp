// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
// uniqname: hgrobel
//  main.cpp
//  project2

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <deque>
#include <stack>
#include <queue>
#include <string>
#include <sstream>
#include <stdio.h>
#include "getopt.h"

#include "gameplay.hpp"
//#include "xcode_redirect.hpp"

using namespace std;


void printHelp() {
    //implement
}

void getMode(int argc, char * argv[]) {
    
        
//        bool modeSpecified = false;
//        uint32_t num = 0;
        opterr = false;
        int choice;
//        int count = 0;
        int option_index = 0;
        option long_options[] = {
            {"verbose", no_argument, nullptr, 'v'},
            {"statistics", required_argument, nullptr, 's'},
            {"median", no_argument, nullptr, 'm'},
            {"help", no_argument, nullptr, 'h'},
            { nullptr, 0,                 nullptr, '\0' }
        };
    
    //LEGAL: ./zombbb -mv < infile.txt, ./zombbb --statistics 10 -v
    //ILLEGAL: ./zombbb -M (The -m option must be lower case), ./zombbb -s (When provided, the -s option has a required argument <num>)
    
        while ((choice = getopt_long(argc, argv, "vs:mh", long_options, &option_index)) != -1) {
            switch (choice) {
            
            case 'v':
                    verbose = true;
                    break;
            case 's':
                    stats = true;
                    num = atoi(optarg);
                    break;
            case 'm':
                    median = true;
                    break;
            case 'h':
                printHelp();
//                    modeSpecified = true;
                exit(0);
            
    
            default:
                cerr << "Unknown command line option" << endl;
                exit(1);
            } // switch
        } // while
    
//        if(count == 0) {
//            cerr << "Stack or queue must be specified" << endl;
//            exit(1);
//        }
//
//        if (modeSpecified == false) {
//            cerr << "Stack or queue must be specified" << endl;
//            exit(1);
//        } // if

    
}


int main(int argc, char *argv[]) {
    
    xcode_redirect(argc, argv);
    
    Game g;
    getMode(argc, argv);
    g.readHeader();
    g.readFile();
    g.createOutput();
    
    
    return 0;
    
}
