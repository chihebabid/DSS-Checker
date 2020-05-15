// ConstructPetriFromFile.h: interface for the CConstructPetriFromFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTRUCTPETRIFROMFILE_H__55A5BDAE_1305_40DF_BD3B_F0957889E117__INCLUDED_)
#define AFX_CONSTRUCTPETRIFROMFILE_H__55A5BDAE_1305_40DF_BD3B_F0957889E117__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ModularPetriNet.h"
class CConstructPetriFromFile
{
public:
	CModularPetriNet* getModularPetrinet();
	void setFileName(const string name);
    string getFileName();
	CConstructPetriFromFile();
	virtual ~CConstructPetriFromFile();
private:
	string getNextWord();
	FILE* fp;
	string m_nom_fichier;
};

#endif // !defined(AFX_CONSTRUCTPETRIFROMFILE_H__55A5BDAE_1305_40DF_BD3B_F0957889E117__INCLUDED_)
