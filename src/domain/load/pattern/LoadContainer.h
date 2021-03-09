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
                                                                        
// $Revision: 1.12 $
// $Date: 2005/11/22 19:44:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/LoadContainer.h,v $
                                                                        
                                                                        
#ifndef LoadContainer_h
#define LoadContainer_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class NodalLoad;
class ElementalLoad;
class NodalLoadIter;
class ElementalLoadIter;
class Vector;
class TaggedObjectStorage;
class Domain;

//! @ingroup BoundCond
//!
//!
//! @defgroup LPatterns Load patterns.
//
//! @ingroup LPatterns
//
//! @brief A LoadContainer object is used to 
//! to store loads on nodes and elements.
class LoadContainer: public CommandEntity, public MovableObject
  {
    friend class LoadPattern;
  private:
    // storage objects for the loads.
    TaggedObjectStorage  *theNodalLoads; //!< Nodal load container.
    TaggedObjectStorage  *theElementalLoads; //!< Elemental load container.

    // iterator objects for the objects added to the storage objects
    NodalLoadIter       *theNodIter; //!< Iterator over nodal loads.
    ElementalLoadIter   *theEleIter; //!< Iterator over elemental loads.

    void free_containers(void);
    void free_iterators(void);
    void alloc_containers(void);
    void alloc_iterators(void);
    void free(void);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

    // methods to add loads
    virtual bool addNodalLoad(NodalLoad *);
    virtual bool addElementalLoad(ElementalLoad *);

    virtual bool removeNodalLoad(int tag);
    virtual bool removeElementalLoad(int tag);
  public:
    LoadContainer(CommandEntity *);
    virtual ~LoadContainer(void);

    virtual void setDomain(Domain *theDomain);


    virtual NodalLoadIter &getNodalLoads(void);
    virtual ElementalLoadIter &getElementalLoads(void);
    int getNumNodalLoads(void) const;
    int getNumElementalLoads(void) const;
    int getNumLoads(void) const;

    // methods to remove things (loads, time_series,...)
    virtual void clearAll(void);
    virtual void clearLoads(void);

    // methods to apply loads
    virtual void applyLoad(const double &);
    
    // methods for o/p
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    virtual void Print(std::ostream &s, int flag =0) const;

    virtual LoadContainer *getCopy(void);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual void applyLoadSensitivity(const double &);
    virtual int  setParameter(const std::vector<std::string> &, Parameter &);
    virtual int  updateParameter(int parameterID, Information &);
    virtual int  activateParameter(int parameterID);
    Vector getExternalForceSensitivity(int gradNumber);
    // AddingSensitivity:END ///////////////////////////////////////////
  };

} // end of XC namespace

#endif







