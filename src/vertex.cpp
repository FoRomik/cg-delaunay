//Project headers
#include "edge.h"
#include "vertex.h"

void Vertex::removeEdge(Edge *pEdge) {
    NOT_NULL(pEdge);
    Edge *pNext = pEdge->Onext();
    this->m_pEdge = (pNext != m_pEdge) ? pNext : 0;
}

Edge* Vertex::getCCWHullEdge() {
    bool isStarted = false;
    Edge *pStart = this->m_pEdge;
    Edge *pCurr = pStart;
    
    if(this->m_pEdge == 0) {
        return(0);
    }
    while(!isStarted || pCurr != pStart) {
        isStarted = true;
        if(!pCurr->rightOf(pCurr->Onext()->Dest()) && !pCurr->rightOf
        		(pCurr->Oprev()->Dest())) {
            this->setEdge(pCurr);
            return(pCurr);
        }
        pCurr = pCurr->Onext();
    }
    return(0);
}

Edge* Vertex::getCWHullEdge() {
    bool isStarted = false;
    Edge *pStart = this->m_pEdge;
    Edge *pCurr = pStart;
    
    if(this->m_pEdge == 0) {
        return(0);
    }
    while(!isStarted || pCurr != pStart) {
        isStarted = true;
        if(!pCurr->leftOf(pCurr->Onext()->Dest()) &&
        		!pCurr->leftOf(pCurr->Oprev()->Dest())) {
            return(pCurr);
        }
        pCurr = pCurr->Onext();
    }
    return(0);
}
