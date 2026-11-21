// ConstructPetriFromFile.h: interface for the ConstructPetriFromFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTRUCTPETRIFROMFILE_H__55A5BDAE_1305_40DF_BD3B_F0957889E117__INCLUDED_)
#define AFX_CONSTRUCTPETRIFROMFILE_H__55A5BDAE_1305_40DF_BD3B_F0957889E117__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Petri/ModularPetriNet.h"

class ConstructPetriFromFile {
public:
    ModularPetriNet *getModularPetrinet();

    void setFileName(const string name);

    string getFileName();

    // Print every place, transition and arc while loading (otherwise one summary line per module)
    void setVerbose(bool verbose) { m_verbose = verbose; }

    ConstructPetriFromFile();

    virtual ~ConstructPetriFromFile();

private:
    string getNextWord();

    FILE *m_file;
    string m_file_name;
    bool m_verbose {false};
};

#endif // !defined(AFX_CONSTRUCTPETRIFROMFILE_H__55A5BDAE_1305_40DF_BD3B_F0957889E117__INCLUDED_)
