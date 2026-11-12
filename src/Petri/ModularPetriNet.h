// ModularPetriNet.h: interface for the ModularPetriNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULARPETRINET_H__D284E8F0_4D61_4B06_B64D_CD07F031C825__INCLUDED_)
#define AFX_MODULARPETRINET_H__D284E8F0_4D61_4B06_B64D_CD07F031C825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <set>

#include "Petri/PetriNet.h"
#include "Legacy/NodeSG.h"
#include "DSS/DistributedState.h"
#include "misc.h"

#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*

class PetriNet;

using namespace std;

typedef std::vector<Element_dss> ListProductFusion;
typedef vector<RElement_dss> RListProductFusion;

class ModularPetriNet  {
public:
    static ModularPetriNet &getInstance();

    ModularPetriNet(const ModularPetriNet &) = delete;
    ModularPetriNet &operator=(const ModularPetriNet &) = delete;
    ModularPetriNet(ModularPetriNet &&) = delete;
    ModularPetriNet &operator=(ModularPetriNet &&) = delete;

    string getProductSCCName(const ProductSCC *pss);

    void setSync(vector<string> transition_codes);

    void extractEnabledFusion(ProductSCC *product, vector<Element_dss> &list_elt);

    ListGlobalStates computeSynchronizedProduct(vector<ListLocalStates> &states_enabling_fusion);

    DistributedState *buildDSS();

    void addSync(const string transition_name);

    void addModule(PetriNet *petri);

    static size_t getModuleCount();

    void writeToFile(const string filename);

    void writeTextFile(const string filename);

    void extractEnabledFusionReduced(vector<MetaState *> &list_ms, vector<RElement_dss> &list_elt);

    size_t getModule(const std::set<string> &list_transitions);
    PetriNet *getModule(const int index);

private:
    ModularPetriNet();

    ~ModularPetriNet();

    vector<PetriNet *> m_modules;
    DistributedState *m_dss;
    vector<TransitionFusionSet *> m_fusions;
};

#endif // !defined(AFX_MODULARPETRINET_H__D284E8F0_4D61_4B06_B64D_CD07F031C825__INCLUDED_)
