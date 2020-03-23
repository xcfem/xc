// -*-c++-*-
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
** (C) Copyright 2001, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.7 $
// $Date: 2008/08/26 15:38:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/Parameter.h,v $

#ifndef Parameter_h
#define Parameter_h

#include "utility/tagged/TaggedObject.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {

class DomainComponent;
class Channel;
class FEM_ObjectBroker;
class Domain;

class Parameter: public TaggedObject, public MovableObject
  {
  private:
    Information theInfo;
    double currentValue;

    enum {initialSize = 64};
    enum {expandSize = 128};

    std::vector<DomainComponent *> theComponents;
    int numComponents;
    int maxNumComponents;

    std::vector<MovableObject *> theObjects;
    int numObjects;
    int maxNumObjects;

    std::vector<int> parameterID;

    int gradIndex; // 0,...,nparam-1
  public:
    Parameter(int tag, DomainComponent *theObject, const std::vector<std::string> &);
    Parameter(const Parameter &param);
    Parameter(int tag, int classTag = PARAMETER_TAG_Parameter);
    Parameter();
  
    virtual void Print(std::ostream &s, int flag =0) const;
  
    virtual int update(int newValue); 
    virtual int update(double newValue); 
    virtual int activate(bool active);
    virtual double getValue(void) {return theInfo.theDouble;}
    virtual void setValue(double newValue) {theInfo.theDouble = newValue;}

    virtual int addComponent(DomainComponent *theObject, const std::vector<std::string> &);  
    virtual int addObject(int parameterID, MovableObject *object);

    void setGradIndex(int gradInd) {gradIndex = gradInd;}
    int getGradIndex(void) {return gradIndex;}

    virtual inline std::string getType(void)
      { return "FEModel"; }

    virtual void setDomain(Domain *theDomain);
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
