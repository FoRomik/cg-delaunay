#ifndef EDGE__H
#define EDGE__H

//
//System headers
//
#include <assert.h>
#include <vector>

//Project headers
//
#include "defines.h"
#include "basetypes.h"
#include "predicates.h"
#include "vertex.h"

class QuadEdge;

typedef std::vector<QuadEdge*> QuadEdgeVector;

//Edge(here) is also called 'edge reference' in Guibas & Stolfi
class Edge {
public:
    static Edge* makeEdge();
    static void  deleteEdge(Edge *pEdge);
    static void  splice(Edge* pA, Edge* pB);
    static Edge* connect(Edge* pA, Edge* pB);

    inline INDEX getID() const {  return(m_ID); }
    inline void  setID(INDEX id) {  this->m_ID = id; }

    //Edge functions from Guibas & Stolfi paper
    inline Edge* Rot() const;
    inline Edge* InvRot() const;
    inline Edge* Sym() const;
    inline Edge* Onext() const   { return(m_pNext); }
    inline Edge* Oprev() const   { return(Rot()->Onext()->Rot()); }
    inline Edge* Dnext() const  { return(Sym()->Onext()->Sym()); }
    inline Edge* Dprev() const  { return(InvRot()->Onext()->InvRot()); }
    inline Edge* Lnext() const  { return(InvRot()->Onext()->Rot()); }
    inline Edge* Lprev() const  { return(Onext()->Sym()); }
    inline Edge* Rnext() const  { return(Rot()->Onext()->InvRot()); }
    inline Edge* Rprev() const  { return(Sym()->Onext()); }

    //Like valid from p113, but 'this' pointer is the 'e' in the paper
    //and 'pE' is the 'basel' in the paper
    inline bool valid(Edge *pE) const { return(orient2d(this->Dest()->Pos(),
    		pE->Dest()->Pos(), pE->Org()->Pos()) > 0); }
    
    //Get related Vertex from edge
    inline Vertex* Org() const  { return(m_pVertex); }
    inline Vertex* Dest() const { return(Sym()->m_pVertex); }
    inline void    setOrg(Vertex *pOrg) { NOT_NULL(pOrg);
    									  this->m_pVertex = pOrg;
    									  pOrg->setEdge(this); }
    inline void    setDest(Vertex *pDest) { NOT_NULL(pDest);
    										Sym()->m_pVertex = pDest;
    										pDest->setEdge(this->Sym()); }
  
    inline bool leftOf(Vertex *pV) const  { return(orient2d(pV->Pos(),
    		this->Org()->Pos(), this->Dest()->Pos()) > 0); }
    inline bool rightOf(Vertex *pV) const { return(orient2d(pV->Pos(),
    		this->Dest()->Pos(), this->Org()->Pos()) > 0); }

    inline bool getMark() const;
    
    inline QuadEdge* getQuadEdge() const { return(m_pQE); }
protected:
    Edge(); //Only the QuadEdge class can make an edge reference
    ~Edge() { }

public:
    unsigned char m_index; /*0,1,2, or 3, depending on where this reference
    is in the array of edges held by its edge record */

    INDEX     m_ID;

    //Pointer to edge record to which this edge belongs
    QuadEdge *m_pQE;
    //Next edge ccw in orbit of m_pVertex
    Edge     *m_pNext;
    //Origin of this edge reference
    Vertex   *m_pVertex;
    
    /*The QuadEdge can destroy the edge references its using ,
     destructor is protected*/
    friend class QuadEdge;
};


//QuadEdge is also called "edge record" in Guibas & Stolfi
class QuadEdge {
public:
    QuadEdge();
    ~QuadEdge() { }
    
    //Marking edged after consideration
    inline void mark(bool val) { m_mark = val; }
    inline bool getMark() const { return(m_mark); }
        
    //This needs to be the first piece of data ensuring that the "this" pointer
    //== m_edges pointer
    Edge   m_edges[4];
    //The QuadEdge's index in the static vector that holds all allocated QuadEdges
    INDEX  m_index;
    //An edge marked true means this edge has been considered
    bool   m_mark;
    
    static INDEX s_uNextID; //every edge has a unique ID
    static QuadEdgeVector *s_pEdgeVector;
};


//Edge methods requiring the QuadEdge definition go here
inline Edge* Edge::Rot() const     { return(m_pQE->m_edges+((m_index+1)&0x3)); }
inline Edge* Edge::InvRot() const  { return(m_pQE->m_edges+((m_index+3)&0x3)); }
inline Edge* Edge::Sym() const     { return(m_pQE->m_edges+((m_index+2)&0x3)); }
inline bool Edge::getMark() const  { return(m_pQE->getMark()); }

#endif //EDGE__H

