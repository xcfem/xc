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
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/Vertex.h,v $
                                                                        
                                                                        
#ifndef Vertex_h
#define Vertex_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for Vertex.
// Vertex is an element of a graph.
//
// What: "@(#) Vertex.h, revA"

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/ID.h"

#define START_VERTEX_NUM 0
namespace XC {

//! @ingroup Graph
//
//! @brief Vértice de un grafo.
class Vertex: public TaggedObject, public MovableObject
  {
  private:
    int myRef; //!< Tag del objeto al que representa el vértice.
    double myWeight; //!< Peso del vértice en el grafo.
    int myColor; //!< Color del vértice en el grafo.
    int myDegree; //!< Grado del vértice en el grafo.
    int myTmp;
    std::set<int> myAdjacency;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    Vertex(int tag= 0, int ref= 0, double weight=0, int color =0);

    virtual void setWeight(double newWeight);
    virtual void setColor(int newColor);
    virtual void setTmp(int newTmp);
    
    virtual int getRef(void) const;
    virtual double getWeight(void) const;
    virtual int getColor(void) const;
    virtual int getTmp(void) const;    

    virtual int addEdge(int otherTag);
    virtual int getDegree(void) const;
    virtual const std::set<int> &getAdjacency(void) const;

    virtual void Print(std::ostream &os, int flag =0);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

