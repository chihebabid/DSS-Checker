//
// Created by chiheb on 09/08/24.
//
#include <iostream>
#include <tl/parse.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/print.hh>
#include "ParseFormulaFile.h"


ParseFormulaFile::ParseFormulaFile(const string &fileName) {
    pFile=std::make_unique<ifstream>(ifstream{fileName});
}

ParseFormulaFile::~ParseFormulaFile() {
    if (pFile->is_open()) {
        pFile->close();
    }
}

bool ParseFormulaFile::getNext(Formula &f) {
    if (pFile->is_open()) {
        string input;
        set<string> transitionSet;
        getline(*pFile, input);
        cout <<"        Loaded formula: " << input << endl;
        spot::parsed_formula pf = spot::parse_infix_psl(input);
        if (pf.format_errors(std::cerr))
            return false;
        spot::formula fo = pf.f;
        if (!fo.is_ltl_formula()) {
            std::cerr << "Only LTL formulas are supported.\n";
            exit(0);
        }
        spot::atomic_prop_set *p_list = spot::atomic_prop_collect(fo, nullptr);
        for (const auto &i: *p_list) {
            transitionSet.insert(i.ap_name());
        }
        cout << "Formula in SPIN format: ";
        print_spin_ltl(std::cout, fo) << endl;

        cout << "Building negation of the formula ... ";
        spot::formula not_f = spot::formula::Not(pf.f);
        cout << "done\n"
             << endl;
        f= {not_f, transitionSet};
    }
    return false;
}
