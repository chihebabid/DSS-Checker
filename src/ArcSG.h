// ArcSG.h: interface for the ArcSG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCSG_H__9424E6A1_C7B0_43C9_8388_E0925890B9A0__INCLUDED_)
#define AFX_ARCSG_H__9424E6A1_C7B0_43C9_8388_E0925890B9A0__INCLUDED_

#include "NodeSG.h"    // Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NodeSG.h"
#include "NodeSGSimplified.h"
#include "TransitionFusionSet.h"

class NodeSG;

class ArcSG {
public:
    TransitionFusionSet *getFusion();

    ArcSG(const ArcSG &val)=default;

    ArcSG &operator=(const ArcSG &val)=default;

    bool operator==(ArcSG &arc);

    NodeSGSimplified *getMarquageSource();

    NodeSG *getNodeSGDest();

    ArcSG()=default;

    virtual ~ArcSG()=default;

private:
    TransitionFusionSet *m_fusion;
    NodeSG *m_destination;
    NodeSGSimplified m_marquage_depart;
};


inline TransitionFusionSet *ArcSG::getFusion() {
    return m_fusion;
}

#endif // !defined(AFX_ARCSG_H__9424E6A1_C7B0_43C9_8388_E0925890B9A0__INCLUDED_)
