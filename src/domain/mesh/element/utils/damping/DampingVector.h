// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  under the terms of the GNU General Public License published by 
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
//DampingVector.h

#ifndef DampingVector_h
#define DampingVector_h

#include <vector>
#include "utility/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableID.h"


namespace XC {
class Damping;
class Domain;

//! @brief Damping pointer container. It's used by
//! elements to store dampings for each integration point.
class DampingVector: public std::vector<Damping *>, public CommandEntity, public MovableObject
  {
  protected:
    void clearAll(void);
    void alloc(const std::vector<Damping *> &);


    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    typedef typename std::vector<Damping *> damping_vector;
    typedef typename damping_vector::iterator iterator;
    typedef typename damping_vector::const_iterator const_iterator;
    typedef typename damping_vector::reference reference;
    typedef typename damping_vector::const_reference const_reference;

    DampingVector(const size_t &n, const Damping *matModel= nullptr);
    DampingVector(const DampingVector &);
    DampingVector &operator=(const DampingVector &);
    ~DampingVector(void)
      { clearAll(); }

    void clearDampings(void);
    void setDamping(const Damping *);
    void setDamping(size_t i,Damping *);
    void setDamping(size_t i, const Damping &);
    void copyPropsFrom(const EntityWithProperties *);

    void setDomain(Domain *domain, int nComp);
    
    bool empty(void) const;
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    std::set<int> getTags(void) const;
    boost::python::list getTagsPy(void) const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };


} // end of XC namespace

#endif
