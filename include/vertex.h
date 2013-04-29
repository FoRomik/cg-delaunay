#ifndef VERTEX__H
#define VERTEX__H

//
//System headers
//
#include <assert.h>

//
//External headers
//


//
//Project headers
//
#include "defines.h"
#include "basetypes.h"

class Edge;

#define ELEM_SWAP(a,b) { Vertex *pt=(a);(a)=(b);(b)=pt; }

class Vertex {
public:
    Vertex(INDEX ind, REAL x, REAL y, Edge *pEdge=0) { setID(ind); setX(x); setY(y); this->m_pEdge = pEdge; } 

    ~Vertex() { }
    
    //Make lexicographic comparison operators for (X,Y)
    bool operator== (const Vertex& right) const { return(this->pos[0]==right.pos[0] && this->pos[1]==right.pos[1]); }
    bool operator!= (const Vertex& right) const { return(this->pos[0]!=right.pos[0] || this->pos[1]!=right.pos[1]); }
    bool operator< (const Vertex& right) const { return(this->pos[0] < right.pos[0] || (this->pos[0]==right.pos[0] && this->pos[1] < right.pos[1])); }
    bool operator> (const Vertex& right) const { return(this->pos[0] > right.pos[0] || (this->pos[0]==right.pos[0] && this->pos[1] > right.pos[1])); }

    //Make lexicographic comparison for (Y,X)
    inline bool ltY(const Vertex& right) const { return(this->pos[1] < right.pos[1] || (this->pos[1]==right.pos[1] && this->pos[0] < right.pos[0])); }
    inline bool gtY(const Vertex& right) const { return(this->pos[1] > right.pos[1] || (this->pos[1]==right.pos[1] && this->pos[0] > right.pos[0])); }

    inline INDEX getID() const { return id; }
    inline void setID(INDEX ind) { this->id = ind; }

    inline void setX(REAL x) { this->pos[0] = x; }
    inline void setY(REAL y) { this->pos[1] = y; }
    inline const REAL* Pos() const { return(this->pos); }
    
    inline Edge *getEdge() const { return(m_pEdge); }
    inline void setEdge(Edge *pEdge) { NOT_NULL(pEdge); this->m_pEdge = pEdge; }
    void removeEdge(Edge *pEdge);
    
    Edge* getCCWHullEdge(); //If this is a convex hull vertex, return the CCW hull edge out of it
    Edge* getCWHullEdge();  //If this is a convex hull vertex, return the CW hull edge out of it

    //These will be used by STL methods to sort or to do median partition
    inline static bool lessX(Vertex *pA, Vertex *pB) { return(pA->pos[0] < pB->pos[0] || (pA->pos[0]==pB->pos[0] && pA->pos[1] < pB->pos[1])); }
    inline static bool lessY(Vertex *pA, Vertex *pB) { return(pA->pos[1] < pB->pos[1] || (pA->pos[1]==pB->pos[1] && pA->pos[0] < pB->pos[0])); }

private:
    INDEX id;
    REAL  pos[2];   //actual vertex coords
    Edge  *m_pEdge; //Pointer to some edge that has this vertex as origin
};


#endif //VERTEX__H
