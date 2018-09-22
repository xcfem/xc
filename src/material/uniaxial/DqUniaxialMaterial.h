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
//DqUniaxialMaterial.h
//deque de pointers (se emplear en la clase Set).


#ifndef DQUNIAXIALMATERIAL_H
#define DQUNIAXIALMATERIAL_H

#include "xc_utils/src/kernel/CommandEntity.h"
#include <deque>
#include <material/uniaxial/UniaxialMaterial.h>


namespace XC {

//! @ingroup MatUnx
//
//! @brief Pointers to UniaxialMaterial container.
class DqUniaxialMaterial: public CommandEntity, public MovableObject, public std::deque<UniaxialMaterial *>
  {
    void free_mem(void);
  public:
    typedef std::deque<UniaxialMaterial *> lst_ptr;
    typedef lst_ptr::const_iterator const_iterator;
    typedef lst_ptr::iterator iterator;
    typedef lst_ptr::reference reference;
    typedef lst_ptr::const_reference const_reference;
    typedef lst_ptr::size_type size_type;
  protected:
    void copy_list(const DqUniaxialMaterial &,SectionForceDeformation *s= nullptr);
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);


  public:
    DqUniaxialMaterial(CommandEntity *owner= nullptr,const size_t &sz= 0);
    DqUniaxialMaterial(CommandEntity *owner,const UniaxialMaterial &);
    DqUniaxialMaterial(CommandEntity *owner,const UniaxialMaterial &um,const size_t &sz);
    DqUniaxialMaterial(CommandEntity *owner,const UniaxialMaterial *um,const size_t &sz);
    DqUniaxialMaterial(const DqUniaxialMaterial &);
    DqUniaxialMaterial(const DqUniaxialMaterial &,SectionForceDeformation *s);
    DqUniaxialMaterial &operator=(const DqUniaxialMaterial &);
    ~DqUniaxialMaterial(void);
    void push_back(const UniaxialMaterial *,SectionForceDeformation *s= nullptr);
    void push_front(const UniaxialMaterial *,SectionForceDeformation *s= nullptr);
    inline iterator begin(void)
      { return lst_ptr::begin(); }
    const_iterator begin(void) const
      { return lst_ptr::begin(); }
    iterator end(void)
      { return lst_ptr::end(); }
    const_iterator end(void) const
      { return lst_ptr::end(); }
    void clear(void);
    void clearAll(void);
    inline size_type size(void) const
      { return lst_ptr::size(); }
    void resize(const size_t &n);

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);

    int zeroInitialStrain(void);
    int setInitialStrain(const Vector &def,const size_t &offset);
    int setTrialStrain(const Vector &def,const size_t &offset);
    int setTrialStrain(const double &strain,const double &strainRate);
    void getStrain(Vector &,const size_t &offset) const;
    void getInitialStrain(Vector &,const size_t &offset) const;
    void getTangent(Matrix &,const size_t &offset) const;
    void getInitialTangent(Matrix &,const size_t &offset) const;
    void getFlexibility(Matrix &f,const size_t &offset) const;
    void getInitialFlexibility(Matrix &f,const size_t &offset) const;
    void getStress(Vector &,const size_t &offset) const;

    inline reference operator[](const size_t i)
      { return lst_ptr::operator[](i); }
    inline const_reference operator[](const size_t i) const
      { return lst_ptr::operator[](i); }

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;

  };

} //end of XC namespace
#endif

