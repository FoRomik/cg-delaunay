//Project headers
#include "edge.h"

//Initialize static members
INDEX QuadEdge::s_uNextID = 4;
QuadEdgeVector *QuadEdge::s_pEdgeVector = new QuadEdgeVector();

QuadEdge::QuadEdge() {
    this->m_index = s_pEdgeVector->size();
    this->m_mark = false;
        
    INDEX id = QuadEdge::s_uNextID;
    for(int i=0; i < 4; i++) {
        m_edges[i].m_pQE  = this;
        m_edges[i].m_index = static_cast<unsigned char>(i);
        m_edges[i].m_pNext = m_edges+((4-i)%4);
        m_edges[i].m_ID = id+i;
    }

    s_pEdgeVector->push_back(this);
    QuadEdge::s_uNextID = id+4;
}
