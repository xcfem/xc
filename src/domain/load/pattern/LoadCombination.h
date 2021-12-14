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
//LoadCombination.h

#ifndef LOADCOMBINATION_H
#define LOADCOMBINATION_H

#include "LoadPatternCombination.h"

namespace XC {
class MapLoadPatterns;
class LoadPattern;
class LoadHandler;
class LoadCombinationGroup;

//! @ingroup LPatterns
//
//! @brief Base class for load pattern combinations
//! (1.5*selfWeight+1.0*permanentLoad+1.6*trafficLoad ...).
class LoadCombination: public LoadPatternCombination
  {
  protected:
    friend class Domain;
    friend class FEM_ObjectBroker;

    LoadCombination &add(const LoadCombination &);
    LoadCombination &subtract(const LoadCombination &);

    int recvDescomp(void);


    friend class LoadCombinationGroup;
    LoadCombination(LoadCombinationGroup *owr= nullptr,const std::string &nm= "",int tag= 0,LoadHandler *ll= nullptr);

  public:

    const LoadCombinationGroup *getGroup(void) const;
    LoadCombinationGroup *getGroup(void);

    LoadCombination &multiplica(const float &);
    LoadCombination &divide(const float &);
    LoadCombination &add(const std::string &);
    LoadCombination &subtract(const std::string &);
    LoadCombination &asigna(const std::string &);
    inline LoadCombination &operator+=(const LoadCombination &c)
      { return add(c); }
    LoadCombination &operator-=(const LoadCombination &c)
      { return subtract(c); }
    LoadCombination &operator*=(const float &f)
      { return multiplica(f); }
    LoadCombination &operator/=(const float &f)
      { return divide(f); }
    LoadCombination operator+(const LoadCombination &) const;
    LoadCombination operator-(const LoadCombination &) const;
    LoadCombination operator*(const float &) const;
    LoadCombination operator/(const float &) const;
    bool operator==(const LoadCombination &) const;
    bool operator!=(const LoadCombination &) const;
    bool dominaA(const LoadCombination &other) const;

    const LoadCombination *getPtrCombPrevia(void) const;
    const std::string getNombreCombPrevia(void) const;
    int getTagCombPrevia(void) const;
    const std::string getComponentsCombPrevia(void) const;
    const std::string getComponentsRestoSobrePrevia(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };

std::ostream &operator<<(std::ostream &os,const LoadCombination &);

} // end of XC namespace

#endif
