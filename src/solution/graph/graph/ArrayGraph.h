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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/ArrayGraph.h,v $
                                                                        
                                                                        
// File: ~/graph/graph/ArrayGraph.h
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for ArrayGraph.
// The vertices in an ArrayGraph are held in an array. This is more efficient
// than holding them in a List data structure, but problems can arise with
// large Graphs in getting enough contiguous memory for the array.
//
// What: "@(#) ArrayGraph.h, revA"

#ifndef ArrayGraph_h
#define ArrayGraph_h

#include "Graph.h"
#include <solution/graph/graph/ArrayVertexIter.h>
#include <vector>

namespace XC {
//! @ingroup Graph
//
//! @brief Array de grafos.
class ArrayGraph: public Graph
  {
  private:
    int numVertex;
    int lastEmpty;
    std::vector<Vertex *> theVertices;
    ArrayVertexIter myIter;
    void libera(void);
  protected:
    int getArraySize(void) const;

    friend class ArrayVertexIter;
    ArrayGraph(const ArrayGraph &);
    ArrayGraph &operator=(const ArrayGraph &);
  public:
    ArrayGraph(int arraySize);
    virtual ~ArrayGraph(void);

    virtual bool addVertex(const Vertex &vertexPtr);
    virtual int addEdge(int vertexTag, int otherVertexTag);
    
    virtual Vertex *getVertexPtr(int vertexTag);
    virtual const Vertex *getVertexPtr(int vertexTag) const;
    virtual VertexIter &getVertices(void);
    int getNumVertex(void) const;
    int getNumEdge(void) const;

    virtual void Print(std::ostream &s) const;
    friend std::ostream &operator<<(std::ostream &s, const ArrayGraph &M);    
  };
} // end of XC namespace

#endif

