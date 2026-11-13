//
// Created by chiheb on 09/08/24.
//
#include <iostream>
#include <tl/parse.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/print.hh>
#include "SpotMC/ParseFormulaFile.h"


ParseFormulaFile::ParseFormulaFile(const string &fileName) {
    m_file=std::make_unique<ifstream>(ifstream{fileName});
}

ParseFormulaFile::~ParseFormulaFile() {
    if (m_file->is_open()) {
        m_file->close();
    }
}

bool ParseFormulaFile::getNext(Formula &f) {
    if (m_file->is_open()) {
        string input;
        set<string> transitionSet;
        getline(*m_file, input);
        if (input.empty()) return false;
        cout <<"\n***** Property#"<<m_formula_count<<"\n\tChecking formula: " << input << endl;
        spot::parsed_formula pf = spot::parse_infix_psl(input);
        if (pf.format_errors(std::cerr)) {
            std::cerr << "Error in the formula format!\n";
            return false;
        }
        spot::formula fo = pf.f;
        if (!fo.is_ltl_formula()) {
            std::cerr << "Only LTL formulas are supported.\n";
            return false;
        }
        spot::atomic_prop_set p_list;
        spot::atomic_prop_collect(fo, &p_list);
        for (const auto &i: p_list) {
            transitionSet.insert(i.ap_name());
        }
        cout << "\tFormula in SPIN format: ";
        print_spin_ltl(std::cout, fo) << endl;
        cout << "\tBuilding negation of the formula ... ";
        spot::formula not_f = spot::formula::Not(pf.f);
        cout << "done\n";
        ++m_formula_count;
        f= {not_f, std::move(transitionSet)};
        return true;
    }
    return false;
}


