// Place.h: interface for the Place class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLACE_H__4A191FB4_6CFF_4348_9744_00D6AE78A3E1__INCLUDED_)
#define AFX_PLACE_H__4A191FB4_6CFF_4348_9744_00D6AE78A3E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Node.h"

class Place : public Node {
public:
    void subTokens(const uint32_t nb);

    void addTokens(const uint32_t addValue);

    void setTokens(const uint32_t value);

    uint32_t getTokens();

    Place()=default;

    virtual ~Place()=default;

private:

    uint32_t m_nb_tokens {0};
};

#endif // !defined(AFX_PLACE_H__4A191FB4_6CFF_4348_9744_00D6AE78A3E1__INCLUDED_)
