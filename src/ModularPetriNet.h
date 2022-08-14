// ModularPetriNet.h: interface for the ModularPetriNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODULARPETRINET_H__D284E8F0_4D61_4B06_B64D_CD07F031C825__INCLUDED_)
#define AFX_MODULARPETRINET_H__D284E8F0_4D61_4B06_B64D_CD07F031C825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Element_dss.h"
#include "ModularSpace.h"
#include "PetriNet.h"
#include "NodeSG.h"
#include "NodeSGSimplified.h"
#include "GrapheSync.h"
#include "Operations.h"
#include "DistributedState.h"
#include "RElement_dss.h"
#include "BaseMPNet.h"
#include <vector>



/*class SCC;-
class ProductSCC;*/
#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*


//class DistributedState;
typedef vector<Marking> ListMarqLoc;
class ModularSpace;
class PetriNet;
class NodeSGSimplified;
using namespace std;
//#define ListProductFusion std::vector<Element_dss>
typedef std::vector<Element_dss> ListProductFusion;
typedef vector<RElement_dss> RListProductFusion;
class ModularPetriNet : public BaseMPNet {
public:
    string  getProductSCCName(ProductSCC *pss);
	void setSync(vector<string> liste_code_transitions);
	void extractEnabledFusion(ProductSCC* product,vector<Element_dss> &list_elt);
	ListGlobalStates computeSychronisedProduct(vector<ListLocalStates> &states_enabling_fusion);
	void extractionFusion(vector<ListMarqLoc*>& liste,NodeSG* node);
	void renommerTransitions(vectorString liste_transitions);
	ModularSpace* constructReducedStateSpace();
	DistributedState* buildDSS();
    DistributedState* buildReducedDSS();

	Marking getMarquageModule(const int index);
	Fusion* getFusion(const int index);
	int getCountFusions();
	void addSync(const string transition_name);
	void addModule(PetriNet* petri);
	inline int getModulesCount() {	return m_modules.size(); }
	void writeToFile(const string filename);
	void writeTextFile(const string filename);
	ModularPetriNet();
	virtual ~ModularPetriNet();
    void extractEnabledFusionReduced(vector<MetaState*>& list_ms,vector<RElement_dss> &list_elt);
    void extractEnabledFusionV2(vector<MetaState *> &list_ms,vector<RElement_dss> &list_elt);
private:

	vector<NodeSGSimplified>* calculerProduitSynchronises(vector<ListMarqLoc> &list);
	void constructSync2();
	void reduceMarquageName(NodeSG* node,Fusion *fusion);
	GrapheSync* m_graphe_sync;
	void printMarquage();
	ModularSpace* m_espace;
	DistributedState* m_dss;
	vector<Fusion*> getFusionsFranchissables();
	vector<Fusion*> m_fusions;

};

#endif // !defined(AFX_MODULARPETRINET_H__D284E8F0_4D61_4B06_B64D_CD07F031C825__INCLUDED_)
