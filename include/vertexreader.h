#ifndef VERTEXREADER__H
#define VERTEXREADER__H

//
//System headers
//

//
//External headers
//


//
//Project headers
//


class VertexReader {
public:
    VertexReader(const char *filename);
    ~VertexReader() { }
    
    Vertex** loadVertices(long *pnVertices);

private:
    const char *m_filename;
};

#endif //VERTEXREADER__H
