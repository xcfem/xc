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
//MapModelWrapper.h

#ifndef MAPSOLUMODEL_H
#define MAPSOLUMODEL_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "ModelWrapper.h"

namespace XC {

class ProcSoluControl;
class ModelWrapper;
class Domain;

//!  @ingroup Solu
//! 
//! @brief Finite element model wrappers container.
class MapModelWrapper: public EntCmd
  {
  public:
    typedef std::map<std::string,ModelWrapper> map_model_wrapper;
    typedef map_model_wrapper::iterator iterator;
    typedef map_model_wrapper::const_iterator const_iterator;

  private:
    map_model_wrapper solu_models; //!< model wrapper container.
  protected:

  public:
    MapModelWrapper(ProcSoluControl *owr);

    inline iterator begin()
      { return solu_models.begin(); }
    inline iterator end()
      { return solu_models.end(); }
    inline const_iterator begin() const
      { return solu_models.begin(); }
    inline const_iterator end() const
      { return solu_models.end(); }

    inline bool empty(void) const
      { return solu_models.empty(); }
    inline size_t size(void) const
      { return solu_models.size(); }

    bool existeModelWrapper(const std::string &) const;
    ModelWrapper &creaModelWrapper(const std::string &);
    const ModelWrapper *getModelWrapper(const std::string &) const;
    ModelWrapper *getModelWrapper(const std::string &);

    void clearAll(void);
  };

} // end of XC namespace

#endif
