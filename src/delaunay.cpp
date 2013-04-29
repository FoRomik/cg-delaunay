//System headers
#include <algorithm>
#include <cmath>

//Project headers
#include "delaunay.h"
#include "predicates.h"
#include "vertex.h"
#include "edge.h"


/*Type of Cut Strategy is defined as:
 * (useAltCut, useVertical) = (true, true)  => Alternating Cuts
 * (useAltCut, useVertical) = (true, false) => Horizontal Cuts*
 * (useAltCut, useVertical) = (false, true)  => Vertical Cuts
 * */
void delaunay(Vertex **ppVertices, long numVertices, Edge **ppLe, Edge **ppRe,
		bool useAltCuts, bool useVertical) {
    NOT_NULL(ppVertices);
    
    if(numVertices == 2) {
        Edge *pA = Edge::makeEdge();
        if(*ppVertices[0] > *ppVertices[1]) {
            ELEM_SWAP(ppVertices[0], ppVertices[1]);
        }
        pA->setOrg(ppVertices[0]);
        pA->setDest(ppVertices[1]);
        *ppLe = pA;
        *ppRe = pA->Sym();
    }
    else if(numVertices == 3) {
        Edge *pA = Edge::makeEdge(),
             *pB = Edge::makeEdge(),
             *pC = 0;

        if(*ppVertices[0] > *ppVertices[1]) {
            ELEM_SWAP(ppVertices[0], ppVertices[1]);
        }
        if(*ppVertices[1] > *ppVertices[2]) {
            ELEM_SWAP(ppVertices[1], ppVertices[2]);
        }

        Edge::splice(pA->Sym(), pB);
        pA->setOrg(ppVertices[0]);
        pA->setDest(ppVertices[1]);
        pB->setOrg(ppVertices[1]);
        pB->setDest(ppVertices[2]);
        
        REAL ccw = orient2d(ppVertices[0]->Pos(), ppVertices[1]->Pos(), ppVertices[2]->Pos());
        if(ccw > 0) {
            pC = Edge::connect(pB, pA);
            *ppLe = pA;
            *ppRe = pB->Sym();
        }
        else if(ccw < 0) {
            pC = Edge::connect(pB, pA);
            *ppLe = pC->Sym();
            *ppRe = pC;
        }
        else {
            *ppLe = pA;
            *ppRe = pB->Sym();
        }
    }
    else {
        long middle = std::floor(numVertices/2);
        Edge *pLdo = 0, 
             *pLdi = 0, 
             *pRdi = 0, 
             *pRdo = 0;

        //These vertices are used for merging a horizontal cut
        Vertex *pBotMax = 0, //highest vertex of bottom half
               *pTopMin = 0, //lowest vertex of top half
               *pMin    = 0, //Lexicographically max vertex
               *pMax    = 0; //Lexicographically min vertex
        
        //Find median partition by X or Y, depending on whether we're using a vertical cut
        std::nth_element(ppVertices, ppVertices+middle, ppVertices+numVertices, useVertical ? Vertex::lessX : Vertex::lessY);

        //Recursive calls
        delaunay(ppVertices, middle, &pLdo, &pLdi, useAltCuts, useAltCuts ? !useVertical : useVertical);
        delaunay(ppVertices+middle, numVertices - middle, &pRdi, &pRdo, useAltCuts, useAltCuts ? !useVertical : useVertical);
        
        //Modify ldi be highest in bottom half and rdi to be lowest in top half
        if(!useVertical) {
            pBotMax = ppVertices[0];
            pTopMin = ppVertices[middle];
            pMin    = (*ppVertices[0] < *ppVertices[middle]) ? ppVertices[0] : ppVertices[middle];
            pMax    = (*ppVertices[0] > *ppVertices[middle]) ? ppVertices[0] : ppVertices[middle];;
            for(long i=1; i < middle; i++) {
                if(*ppVertices[i] < *pMin) {
                    pMin = ppVertices[i];
                }
                else if(*ppVertices[i] > *pMax) {
                    pMax = ppVertices[i];
                }
                
                if(ppVertices[i]->gtY(*pBotMax)) {
                    pBotMax = ppVertices[i];
                }
            }
            for(long i=middle+1; i < numVertices; i++) {
                if(*ppVertices[i] < *pMin) {
                    pMin = ppVertices[i];
                }
                else if(*ppVertices[i] > *pMax) {
                    pMax = ppVertices[i];
                }
                
                if(ppVertices[i]->ltY(*pTopMin)) {
                    pTopMin = ppVertices[i];
                }
            }
            
            pLdi = pBotMax->getCWHullEdge();
            pRdi = pTopMin->getCCWHullEdge();
        }
        
        
        //Compute the lower common tangent of two sets of vertices
        while (1) {
            if(pLdi->leftOf(pRdi->Org())) {
                pLdi = pLdi->Lnext();
            }
            else if(pRdi->rightOf(pLdi->Org())) {
                pRdi = pRdi->Rprev();
            }
            else {
                break;
            }
        }

        // Create a first cross edge pBasel from pRdi.origin to pLdi.origin
        Edge *pBasel = Edge::connect(pRdi->Sym(), pLdi);

        if(pLdi->Org() == pLdo->Org()) {
            pLdo = pBasel->Sym();
        }
        if(pRdi->Org() == pRdo->Org()) {
            pRdo = pBasel;
        }

        //Merging two halves
        while(1) {
            //Locate the first Left point pLcand to be encou
        	Edge *pLcand = pBasel->Sym()->Onext();
            bool leftFinished  = !pLcand->valid(pBasel);
            if(!leftFinished) {
                while(incircle(pBasel->Dest()->Pos(), 
                               pBasel->Org()->Pos(), 
                               pLcand->Dest()->Pos(),  
                               pLcand->Onext()->Dest()->Pos()) > 0) {
                
                    Edge *pT = pLcand->Onext();
                    Edge::deleteEdge(pLcand);
                    pLcand = pT;
                }
            }
            
            //Symmetrically locate the first R point to be hit
            Edge *pRcand = pBasel->Oprev();
            bool rightFinished = !pRcand->valid(pBasel);
            if(!rightFinished) {
                while(incircle(pBasel->Dest()->Pos(), 
                               pBasel->Org()->Pos(), 
                               pRcand->Dest()->Pos(),  
                               pRcand->Oprev()->Dest()->Pos()) > 0) {
                
                    Edge *pT = pRcand->Oprev();
                    Edge::deleteEdge(pRcand);
                    pRcand = pT;
                }
            }

            //both are invalid, pBasel is upper common tangent
            if(leftFinished && rightFinished) {
                break;
            }
            //the next cross edge is to be connected to either pLcand.dest or pRcand.dest
            if(leftFinished || 
               (!rightFinished && incircle(pLcand->Dest()->Pos(), 
                                                  pLcand->Org()->Pos(), 
                                                  pRcand->Org()->Pos(), 
                                                  pRcand->Dest()->Pos()) > 0)) {
                pBasel = Edge::connect(pRcand, pBasel->Sym());
            }
            else {
                pBasel = Edge::connect(pBasel->Sym(), pLcand->Sym());
            }
        }

        //Modify pLdo and pRdo if we merging a horizontal cut
        if(!useVertical) {
            pLdo = pMin->getCCWHullEdge();
            pRdo = pMax->getCWHullEdge();
        }

        *ppLe = pLdo;
        *ppRe = pRdo;
    }
    return;
}

