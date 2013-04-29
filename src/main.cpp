//System headers
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <sys/time.h>
#include <cstring>
#include <sys/resource.h>

//Project headers
#include "basetypes.h"
#include "delaunay.h"
#include "vertexreader.h"
#include "edge.h"

const char* ARG_ALT_CUTS = "-a";
const char* ARG_HORZ_CUTS = "-h";
const char* ARG_VERT_CUTS = "-v";

void usage(char* progname) {
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr, "%s <infile.node> [outfile.ele] [logfile.log] [%s]\n\n", progname, ARG_ALT_CUTS);
    fprintf(stderr, "infile.node .... The input node file (required)\n");
    fprintf(stderr, "outfile.ele .... The output ele file. If omitted, output written to stdout\n");
    fprintf(stderr, "logfile.log .... The log file. If omitted, logging information written to stderr\n");
    fprintf(stderr, "%s .... If present, cuts dividing recursive calls will alternate between\n        vertical and horizontal. Otherwise, only vertical cuts will be used.\n\n", ARG_ALT_CUTS);
    fprintf(stderr, "%s .... If present, Horizontal cuts dividing recursive calls.\n        Otherwise, only vertical cuts will be used.\n\n", ARG_HORZ_CUTS);
    fprintf(stderr, "%s .... If present, Vertical cuts dividing recursive calls.\n        by Default, only vertical cuts will be used.\n\n", ARG_VERT_CUTS);
    fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
    VertexReader *pReader = 0; //Object to read vertices
    Vertex **ppVertices = 0;   //Array to hold vertices
    char *strInFile = 0,       //Name of input file
         *strOutFile = 0;      //Name of output file  
    FILE *pOutFile = stdout,   //file to write output ele file to
         *pLogFile = stderr;   //file to write logging info to
    bool useAltCuts = false;   //If alternate between vertical and horizontal cuts
    bool useHorzCuts = false;   //If use horizontal cuts

    long nVertices,               //Number of vertices
         nEdges,               //Number of edges in triangulation
         nPointsOnConvHull,            //Number of points on convex hull
         nTriangles;           //Number of triangles in triangulation
    Edge *pLe = 0, *pRe = 0;
    QuadEdgeVector &qeVect = *QuadEdge::s_pEdgeVector; //We store a reference to the vector of quadedges
    struct timeval  tvStart,   //Time we start triangulating
                    tvEnd;     //Time we end
    struct timezone tz;        //Needed to make time calls
    double elapsedTime = 0.0;  //Will eventually hold the running time
    
    //
    //Handle command-line args
    //
    for(int argCount = 1; argCount < argc; argCount++) {
        if(argv[argCount][0] == '-') { //this is a command switch
            if(strcmp(argv[argCount], ARG_ALT_CUTS) == 0) {
                useAltCuts = true;
            }
            else if(strcmp(argv[argCount], ARG_HORZ_CUTS) == 0) {
                useHorzCuts = true;
            }
            else if(strcmp(argv[argCount], ARG_VERT_CUTS) == 0) {
            	useAltCuts = false;
            	useHorzCuts = false; //explicitly setting
            }
        }
        else if(pReader==0) { //we'll assume it's an input file if we haven't seen one yet
            strInFile = argv[argCount];
            pReader = new VertexReader(strInFile);
        }
        else if(pOutFile==stdout) { //we'll assume the second non-switch is an output file
            strOutFile = argv[argCount];
            pOutFile = fopen(strOutFile, "w");
            assert(pOutFile != 0);
        }
        else if(pLogFile==stderr) { //otherwise, we'll assume it's a log file
            pLogFile = fopen(argv[argCount], "w");
            assert(pLogFile != 0);
        }
    }
    if(pReader==0) {
        //Caught an error!...
        usage(argv[0]);
        return(EXIT_FAILURE);
    }
    
    //Log actions...
    fprintf(pLogFile, "\nUsing input file '%s'\n", strInFile);
    if(pOutFile==stdout) {
        fprintf(pLogFile, "Using stdout for output\n");
    }
    else {
        fprintf(pLogFile, "Using output file '%s'\n", strOutFile);
    }
    if(useAltCuts) {
        fprintf(pLogFile, "Using alternating vertical and horizontal cuts\n");
    }
    else if(useHorzCuts) {
        fprintf(pLogFile, "Using horizontal cuts\n");
    }
    else {
        fprintf(pLogFile, "Using vertical cuts only\n");
    }
    fprintf(pLogFile, "\n");
    
    //
    //Read vertices from file
    //
    fprintf(pLogFile, "Reading input file..."); fflush(pLogFile);
    ppVertices = pReader->loadVertices(&nVertices);
    if(ppVertices == 0) {
        return(EXIT_FAILURE);
    }
    fprintf(pLogFile, "Done!\n");

    //initialize Robust floating-point Predicates
    exactinit();
    
    //Triangulate the input vertices
    fprintf(pLogFile, "Triangulating..."); fflush(pLogFile);
    gettimeofday(&tvStart, &tz);  //Start timer
    if (useAltCuts){ //Triangulate w/Alternating cuts
    	delaunay(ppVertices, nVertices, &pLe, &pRe, useAltCuts, true);
    }
    else if(useHorzCuts){ //Triangulate w/Horizontal cuts
    	delaunay(ppVertices, nVertices, &pLe, &pRe, useHorzCuts, false);
    }
    else{ //Triangulate w/Vertical cuts
    	delaunay(ppVertices, nVertices, &pLe, &pRe, false, true);
    }
    gettimeofday(&tvEnd, &tz);    //End timer
    fprintf(pLogFile, "Done!\n\n");
        
    elapsedTime = static_cast<double>(tvEnd.tv_sec - tvStart.tv_sec) + tvEnd.tv_usec/1000000.0 - tvStart.tv_usec/1000000.0;
    
    //With number of vertices and edges,compute the number of vertices on convex hull
    //and then compute the number of triangles to expect
    nEdges = qeVect.size();
    nPointsOnConvHull = 3*(nVertices-1) - nEdges;
    nTriangles = 2*(nVertices-1) - nPointsOnConvHull;

    //Log statistics
    fprintf(pLogFile, "Results:\n");
    fprintf(pLogFile, "Number of input vertices:  %ld\n", nVertices);
    fprintf(pLogFile, "Number of vertices in Convex hull: %ld\n", nPointsOnConvHull);
    fprintf(pLogFile, "Number of edges in triangulation: %ld\n", nEdges);
    fprintf(pLogFile, "Number of Triangles:   %ld\n--------------------------\n", nTriangles);
    fprintf(pLogFile, "Elapsed time: %0.8f sec\n\n", elapsedTime);

    //Output the triangles
    fprintf(pOutFile, "%ld 3 0 0\n", nTriangles);
    long count = 1; //The id of the triangle we're outputting next
    for(long i=0; i < nEdges; i++) {
        QuadEdge *pQE = qeVect[i];
        Edge     *pE  = pQE->m_edges;
        Edge     *pS  = pE->Sym();
        
        //This is a somewhat convoluted way of checking if the triangles bounded by the edge are
        //    a) valid triangles, AKA edge not on convex hull
        //    b) haven't been seen yet (mark == false)
        if(pE->getID() == pE->Lnext()->Lnext()->Lnext()->getID() &&
           !(pE->Lnext()->getMark() | pE->Lprev()->getMark()) &&
           (nPointsOnConvHull>3 || orient2d(pE->Org()->Pos(), pE->Dest()->Pos(), pE->Lnext()->Dest()->Pos()) > 0)) {
            fprintf(pOutFile, "%ld %ld %ld %ld\n", count++, pE->Org()->getID(), pE->Dest()->getID(), pE->Lnext()->Dest()->getID());
        }
        if(pS->getID() == pS->Lnext()->Lnext()->Lnext()->getID() &&
           !(pS->Lnext()->getMark() | pS->Lprev()->getMark()) &&
           (nPointsOnConvHull>3 || orient2d(pS->Org()->Pos(), pS->Dest()->Pos(), pS->Lnext()->Dest()->Pos()) > 0)) {
            fprintf(pOutFile, "%ld %ld %ld %ld\n", count++, pS->Org()->getID(), pS->Dest()->getID(), pS->Lnext()->Dest()->getID());
        }

        pQE->mark(true);
    }

    fprintf(pLogFile, "Cleaning up..."); fflush(stderr);

    //
    //Clean up the mess we've made
    //
    if(pOutFile != stdout) {
        fclose(pOutFile);
    }

    for(int i=0; i< nVertices; i++) {
        delete(ppVertices[i]);
    }
    for(int i=0; i< nEdges; i++) {
        delete(qeVect[i]);
    }
        
    delete pReader;
    delete[] ppVertices;
    delete QuadEdge::s_pEdgeVector;
    
    fprintf(pLogFile, "Done!\n\n");
    
    if(pLogFile != stderr) {
        fclose(pLogFile);
    }
    
    return(EXIT_SUCCESS);
}
