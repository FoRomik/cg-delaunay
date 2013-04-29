//System headers
#include <iostream>
#include <fstream>

//Project headers
#include "vertex.h"
#include "vertexreader.h"

VertexReader::VertexReader(const char *filename) {
    m_filename = filename;
}
Vertex** VertexReader::loadVertices(long *pnVertices) {
    Vertex** ppVertices = 0;
    std::ifstream inputFile;

    inputFile.open(m_filename);
    if(inputFile.is_open()) {
        long nVertices,
             iDim,
             nAttr,
             nMarkers;
        long startIndex = 2;
        long currIndex = 0;
        REAL  x, y;

        std::string line;
        getline(inputFile,line);

        if(4 != sscanf(line.c_str(), "%ld %ld %ld %ld", &nVertices, &iDim,
        		&nAttr, &nMarkers)) {
            //problem reading first line
            fprintf(stderr, "Error reading first line of %s\n", m_filename);
        }
        *pnVertices = nVertices;
        ppVertices = new Vertex*[nVertices];

        while (currIndex < nVertices && !inputFile.eof() ) {
            getline(inputFile,line);
            if(3 != sscanf(line.c_str(), "%ld %lg %lg", &currIndex, &x, &y)) {
                //problem reading first line
                fprintf(stderr, "Error reading line %ld of %s\n",
                		currIndex-startIndex+2, m_filename);
            }
            if(startIndex > 1) {
                startIndex = currIndex;
            }
            Vertex *pNewVertex = new Vertex(currIndex, x, y);
            ppVertices[currIndex-startIndex] = pNewVertex;
        }
        inputFile.close();
    }
    else {
        //problem opening file
        fprintf(stderr, "Error opening %s\n", m_filename);
    }

    return ppVertices;
}
