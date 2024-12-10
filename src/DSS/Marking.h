// BitsVector.h: interface for the BitsVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITSVECTOR_H__FCB7EB69_05AC_41F8_9A21_7807D91657A0__INCLUDED_)
#define AFX_BITSVECTOR_H__FCB7EB69_05AC_41F8_9A21_7807D91657A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once

#endif // _MSC_VER > 1000

#include <vector>
#include <iostream>
#include <bitset>

class Transition;

class SCC;

using namespace std;
typedef unsigned char Octet;
typedef vector<std::bitset<8>> VecCar;

class Marking {
public:
    int index; // Used in Tarjan algorithm
    int lowlink; // Used in Tarjan algorithm
    bool onstack; // Used in Tarjan algorithm
    void setVide(bool v);

    inline bool isVide() const {
        return m_vide;
    }

    Marking &operator=(const Marking &newvec);

    Marking(const Marking &value);

    Octet get8BitsValue(const int index);

    bool operator==(const Marking &newvec) const {
        if (this->m_vide == true && newvec.m_vide == true) {
            return true;
        } else if ((this->m_vide) == (!newvec.m_vide)) return false;

        if (m_vector.size() != newvec.m_vector.size()) return false;
        for (unsigned int i = 0; i < m_vector.size(); i++)
            if (m_vector[i] != newvec.m_vector[i]) return false;
        return true;
    }



    void add8BitsValue(const Octet val);

    void addSucc(Transition *, Marking *);

    vector<pair<Transition *, Marking *>> &getListSucc();

    Marking();

    virtual ~Marking();

    SCC *getSCCContainer() const;
    void setSCCContainer(SCC *c);
    friend ostream &operator<<(ostream &stream, const Marking &);

private:
    bool m_vide;
    VecCar m_vector;
    vector<pair<Transition *, Marking *>> ml_succ;
    SCC *m_sccContainer;

};

inline void Marking::setVide(bool v) {
    m_vide = v;
}


#endif // !defined(AFX_BITSVECTOR_H__FCB7EB69_05AC_41F8_9A21_7807D91657A0__INCLUDED_)
