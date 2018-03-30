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
//LoadCombinationGroup.h

#ifndef LOADCOMBINATIONGROUP_H
#define LOADCOMBINATIONGROUP_H

#include "preprocessor/prep_handlers/LoadHandlerMember.h"
#include <map>
#include "boost/python/list.hpp"

namespace XC {
class LoadCombination;
class LoadHandler;
class Domain;

typedef std::map<std::string,LoadCombination *> LoadCombinationMap; //!< LoadCombinations.

//! @ingroup LPatterns
//
//! @brief Load combination container.
class LoadCombinationGroup: public LoadHandlerMember, public LoadCombinationMap
  {
  protected:
    LoadCombination *find_combination(const std::string &);
    friend class LoadHandler;

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    LoadCombinationGroup(LoadHandler *owr);
    ~LoadCombinationGroup(void);

    boost::python::list getKeys(void) const;
    void remove(const std::string &);
    void clear(void);

    XC::LoadCombination *newLoadCombination(const std::string &,const std::string &);

    void addToDomain(const std::string &);
    void removeFromDomain(const std::string &);
    void removeAllFromDomain(void);

    std::deque<std::string> getListaNombres(void) const;
    const std::string &getNombreLoadCombination(const LoadCombination *) const;

    const LoadCombination *buscaLoadCombination(const std::string &) const;
    const_iterator buscaCombPrevia(const LoadCombination &) const;
    const LoadCombination *getPtrCombPrevia(const LoadCombination &) const;
    const_iterator buscaCombPrevia(const std::string &) const;
    const std::string getNombreCombPrevia(const std::string &) const;
    int getTagCombPrevia(const std::string &) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} // end of XC namespace

#endif
