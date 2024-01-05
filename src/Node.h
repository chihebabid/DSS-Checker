// Node1.h: interface for the Node class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODE1_H__1392115E_4EE1_4060_BB6E_945CC41A21DD__INCLUDED_)
#define AFX_NODE1_H__1392115E_4EE1_4060_BB6E_945CC41A21DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
using std::string;

class Node  {
public:
    void setName(const string name);

    string getName() const;

    Node();

    virtual ~Node();

protected:
    string m_name;
};

#endif // !defined(AFX_NODE1_H__1392115E_4EE1_4060_BB6E_945CC41A21DD__INCLUDED_)
