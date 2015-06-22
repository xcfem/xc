//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/03 23:11:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/Graph.h,v $
                                                                        
                                                                        
#ifndef Graph_h
#define Graph_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for Graph.
// The Graph class provides the abstraction of a graph, a collection of
// vertices and edges. The Graph class is a container class which stores
// and provides access to Vertex objects. The Vertices contain information 
// about the edges in this design.
//
// What: "@(#) Graph.h, revA"

#include <iostream>
#include "utility/actor/actor/MovableObject.h"
#include "utility/tagged/storage/ArrayOfTaggedObjects.h"
#include "solution/graph/graph/VertexIter.h"

namespace XC {
class Vertex;
class VertexIter;
class TaggedObjectStorage;
class Channel;
class FEM_ObjectBroker;

//! @ingroup Graph
//
//! @brief The Graph class provides the abstraction of a graph, a collection of
//! vertices and edges. The Graph class is a container class which stores
//! and provides access to Vertex objects. The Vertices contain information 
//! about the edges in this design.
class Graph: public MovableObject
  {
  protected:
    ArrayOfTaggedObjects myVertices;
    VertexIter theVertexIter;
    int numEdge;
    int nextFreeTag;

    void inic(const size_t &);
    void copia(const Graph &other);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    Graph(void);
    Graph(int numVertices);    
    Graph(const Graph &other);
    Graph &operator=(const Graph &other);

    virtual bool addVertex(const Vertex &vertexPtr, bool checkAdjacency = true);
    virtual int addEdge(int vertexTag, int otherVertexTag);
    
    virtual Vertex *getVertexPtr(int vertexTag);
    virtual const Vertex *getVertexPtr(int vertexTag) const;
    virtual VertexIter &getVertices(void);
    virtual int getNumVertex(void) const;
    virtual int getNumEdge(void) const;
    virtual int getFreeTag(void);
    virtual bool removeVertex(int tag, bool removeEdgeFlag = true);
    const Vertex *BuscaRef(int ref) const;
    void getBand(int &,int &) const;
    int getVertexDiffMaxima(void) const;
    int getVertexDiffExtrema(void) const;


    virtual int merge(Graph &other);
    
    virtual void Print(std::ostream &os, int flag =0);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend std::ostream &operator<<(std::ostream &s, Graph &M);    
  };
} // fin namespace XC

#endif

