#ifndef INTERNALARC_H
#define INTERNALARC_H

#include "Transition.h"
class Transition;
class InternalArc
{
    public:
        InternalArc();
        void setSource(Marking *source);
        void setDestination(Marking *dest);
        void setTranition(Transition *tr);
        virtual ~InternalArc();
        Marking *getSource();
        Marking *getDestination();
        Transition *getTransition();

    protected:

    private:
        Marking* m_source;
        Marking* m_destination;
        Transition * m_transition;
};

#endif // INTERNALARC_H
