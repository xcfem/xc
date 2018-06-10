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
//AnalysisAggregationMap.h

#ifndef MAPSOLUMETHOD_H
#define MAPSOLUMETHOD_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/handler/DataOutputHandler.h"
#include "AnalysisAggregation.h"

namespace XC {

class ProcSoluControl;
class ModelWrapper;

//!  @ingroup Solu
//! 
//! @brief Solution methods container.
class AnalysisAggregationMap: public EntCmd
  {
  private:
    typedef std::map<std::string,AnalysisAggregation> map_solu_method;
    typedef map_solu_method::iterator iterator;
    typedef map_solu_method::const_iterator const_iterator;

    map_solu_method solu_methods; //!< Solution method container.
  public:
    AnalysisAggregationMap(ProcSoluControl *owr);

    inline iterator begin()
      { return solu_methods.begin(); }
    inline iterator end()
      { return solu_methods.end(); }
    inline const_iterator begin() const
      { return solu_methods.begin(); }
    inline const_iterator end() const
      { return solu_methods.end(); }

    inline bool empty(void) const
      { return solu_methods.empty(); }
    inline size_t size(void) const
      { return solu_methods.size(); }

    bool AnalysisAggregationExists(const std::string &) const;
    AnalysisAggregation &createAnalysisAggregation(const std::string &,ModelWrapper *);
    const AnalysisAggregation *getAnalysisAggregation(const std::string &) const;
    AnalysisAggregation *getAnalysisAggregation(const std::string &);

    AnalysisAggregation &newAnalysisAggregation(const std::string &,const std::string &);

    void revertToStart(void);
    void clearAll(void);
  };

} // end of XC namespace

#endif
