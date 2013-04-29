//
//System headers
//


//
//External headers
//


//
//Project headers
//
#include "basetypes.h"
#include "vertex.h"

void delaunay(Vertex **ppVertices, //the array of vertex pointers
              long numVertices,         //the number of vertices in ppVertices
              Edge **pLe,          //a pointer to the ccw convex hull edge out
              //of leftmost vertex will be returned in this variable
              Edge **pRe,          //a pointer to the cw convex hull edge out
              //of rightmost vertex will be returned in this variable
              bool useAltCuts = false, //alternate between vertical
              //and horizontal cuts?
              bool useVertical = true); //current iteration splits points
              //using a vertical cut (If False, then horizontal cuts are used)
