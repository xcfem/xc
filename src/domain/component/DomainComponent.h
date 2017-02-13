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
// $Date: 2003/10/07 18:57:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/component/DomainComponent.h,v $
                                                                        
                                                                        
#ifndef DomainComponent_h
#define DomainComponent_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for DomainComponent.
// The DomainComponent class is an abstract class, example subclasses include
// Element, Node, SFreedom_Constraint, MFreedom_Constraint, NodalLoad, ElementalLoad. 
// Each of these objects forms part of Domain and has methods to set and obtain
// the associated Domain object.
//
// What: "@(#) DomainComponent.h, revA"

#include "utility/tagged/TaggedObject.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Domain;
class Renderer;
class Information;
class Preprocessor;

//! @brief Objeto que forma parte de un domain.
class DomainComponent: public TaggedObject, public MovableObject
  {
  private:    
    Domain *theDomain; // a pointer to the enclosing Domain object
    size_t idx; //!< @brief Índice del objeto (se emplea sólo para numerar entidades para VTK).
  protected:
    DomainComponent(int tag, int classTag);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ~DomainComponent(void);
    //! @brief Returns the índice (NO EL TAG) del objeto.
    inline size_t getIdx(void) const
      { return idx; }
    virtual void setDomain(Domain *theDomain);
    virtual Domain *getDomain(void) const;
    const Preprocessor *GetPreprocessor(void) const;
    Preprocessor *GetPreprocessor(void);

    void set_indice(const size_t &i);

  };
} // end of XC namespace

#endif


