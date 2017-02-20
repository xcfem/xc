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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/10 01:07:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/Vertex.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation for XC::Vertex.
// Vertex is an element of a graph.
//
// What: "@(#) Vertex.C, revA"

#include <solution/graph/graph/Vertex.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
XC::Vertex::Vertex(int tag, int ref, double weight, int color)
  :TaggedObject(tag), MovableObject(0), myRef(ref), myWeight(weight), myColor(color), 
   myDegree(0), myTmp(0) {}    


//! @brief Assigns a weight to the vertex.
void XC::Vertex::setWeight(double newWeight) 
  { myWeight = newWeight; }

//! @brief Assigns a color to the vertex.
void XC::Vertex::setColor(int newColor) 
  { myColor = newColor; }

void XC::Vertex::setTmp(int newTmp) 
  { myTmp = newTmp; }

int XC::Vertex::getRef(void) const 
  { return myRef; }

//! @brief Returns the weight of the vertex.
double XC::Vertex::getWeight(void) const 
  { return myWeight; }

//! @brief Returns the color of the vertex.
int XC::Vertex::getColor(void) const
  { return myColor; }

int XC::Vertex::getTmp(void) const
  { return myTmp; }

//! @brief Appends an edge to the vertex.
int XC::Vertex::addEdge(int otherTag)
  {
    if(otherTag != this->getTag()) // don't allow itself to be added
      myAdjacency.insert(otherTag);
    return 0;
  }

//! @brief Returns the grado del vértice en el grafo.
int XC::Vertex::getDegree(void) const
  { return myDegree; }

//! @brief Return the lista de adyacencia del vértice en el grafo.
const std::set<int> &XC::Vertex::getAdjacency(void) const
  { return myAdjacency; }

//! @brief Imprime.
void XC::Vertex::Print(std::ostream &s, int flag)
  {
    s << this->getTag() << " " ;
    s << myRef << " ";
    if (flag == 1) 
	s << myWeight << " " ;
    else if (flag ==2) 
	s << myColor << " " ;
    else if (flag == 3)
        s << myWeight << " " << myColor << " " ;    
    else if (flag == 4)
      s << myWeight << " " << myColor << " " << myTmp << " " ;

    s << "ADJACENCY: ";
    for(std::set<int>::const_iterator i= myAdjacency.begin();i!=myAdjacency.end();i++)
      s << ' ' << *i;    	
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::Vertex::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendDouble(myWeight,getDbTagData(),CommMetaData(1));
    res+= cp.sendInts(myRef,myColor,myDegree,myTmp,getDbTagData(),CommMetaData(2));
    const ID tmp(myAdjacency);
    res+= cp.sendID(tmp,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::Vertex::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveDouble(myWeight,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInts(myRef,myColor,myDegree,myTmp,getDbTagData(),CommMetaData(2));
    ID tmp;
    res+= cp.receiveID(tmp,getDbTagData(),CommMetaData(3));
    const int sz= tmp.Size();
    for(int i= 0;i<sz;i++)
      myAdjacency.insert(tmp[i]);
    return res;
  }

//! @brief Envía.
int XC::Vertex::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe.
int XC::Vertex::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
