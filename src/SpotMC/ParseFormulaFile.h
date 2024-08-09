//
// Created by chiheb on 09/08/24.
//

#ifndef DISTRIBUTEDSTATESPACE_PARSEFORMULAFILE_H
#define DISTRIBUTEDSTATESPACE_PARSEFORMULAFILE_H
#include <string>
#include <fstream>
#include <memory>
#include "misc.h"

using namespace std;

class ParseFormulaFile {
    unique_ptr<ifstream> pFile {nullptr};
public:
    ParseFormulaFile(const string &fileName);
    bool getNext(Formula &f);
    ~ParseFormulaFile();
};


#endif //DISTRIBUTEDSTATESPACE_PARSEFORMULAFILE_H
