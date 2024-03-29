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
#include "Fusion.h"

class NodeSG;

class ArcSG {
public:
    Fusion *getFusion();

    ArcSG(const ArcSG &val);

    ArcSG &operator=(const ArcSG &val);

    bool operator==(ArcSG &arc);

    NodeSGSimplified *getMarquageSource();

    NodeSG *getNodeSGDest();

    ArcSG();

    virtual ~ArcSG();

private:
    Fusion *m_fusion;
    NodeSG *m_destination;
    NodeSGSimplified m_marquage_depart;


};


inline Fusion *ArcSG::getFusion() {
    return m_fusion;
}

#endif // !defined(AFX_ARCSG_H__9424E6A1_C7B0_43C9_8388_E0925890B9A0__INCLUDED_)
