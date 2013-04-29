//Project headers
#include "defines.h"
#include "edge.h"


Edge::Edge() {
    m_pVertex = 0;
}

Edge* Edge::makeEdge() {
    QuadEdge *pQE = new QuadEdge();
    return(pQE->m_edges);
}

void Edge::deleteEdge(Edge *pEdge) {
    NOT_NULL(pEdge);
    QuadEdge* pQE = pEdge->getQuadEdge();

    //If one of the vertices has a reference to this edge, we need
    //to remove it
    if(pEdge->Org()->getEdge() == pEdge) {
        pEdge->Org()->removeEdge(pEdge);
    }
    if(pEdge->Dest()->getEdge() == pEdge->Sym()) {
        pEdge->Dest()->removeEdge(pEdge->Sym());
    }

    //remove pEdge from subdivision
    splice(pEdge, pEdge->Oprev());
    splice(pEdge->Sym(), pEdge->Sym()->Oprev());

    //Move last element of quadedge vector to pQE's spot (adjust index so we can find it later)
    QuadEdgeVector &qeVect = *QuadEdge::s_pEdgeVector;
    qeVect[pQE->m_index] = qeVect[QuadEdge::s_pEdgeVector->size()-1];
    qeVect[pQE->m_index]->m_index = pQE->m_index;
    
    //Remove pQE from vector and delete it
    qeVect.pop_back();
    delete(pQE);
}

//Guibas and Stolfi, simplified since we don't use flips (p98)
void Edge::splice(Edge* pA, Edge* pB) {
    NOT_NULL(pA);
    NOT_NULL(pB);

    Edge *pAlpha = pA->Onext()->Rot(),
         *pBeta  = pB->Onext()->Rot();
            
    Edge *tmp1 = pB->Onext(),
         *tmp2 = pA->Onext(),
         *tmp3 = pBeta->Onext(),
         *tmp4 = pAlpha->Onext();

    pA->m_pNext     = tmp1;
    pB->m_pNext     = tmp2;
    pAlpha->m_pNext = tmp3;
    pBeta->m_pNext  = tmp4;
}

//Guibas and Stolfi (p103)
Edge* Edge::connect(Edge* pA, Edge* pB) {
    NOT_NULL(pA);
    NOT_NULL(pB);
    
    Edge *pE = makeEdge();
    pE->setOrg(pA->Dest());
    pE->setDest(pB->Org());
    splice(pE, pA->Lnext());
    splice(pE->Sym(), pB);
    
    return(pE);
}

