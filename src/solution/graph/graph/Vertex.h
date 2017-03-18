//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
//! @brief Vertex of a graph.
class Vertex: public TaggedObject, public MovableObject
  {
  private:
    int myRef; //!< Tag of the object represented by the vertex.
    double myWeight; //!< Vertex weigth on the graph.
    int myColor; //!< Color of the vertex.
    int myDegree; //!< Degree of the vertex.
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

