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
//InteractionDiagramData

#ifndef INTERACTIONDIAGRAMDATA_H
#define INTERACTIONDIAGRAMDATA_H


#include "PivotsUltimateStrains.h"

namespace XC {


//! \@ingroup MATSCCDiagInt
//
//! @brief Parameters needed to obtain the interaction diagram.
class InteractionDiagramData: public CommandEntity
  {
    double threshold; //!< Minimal distance between diagram points.
    double inc_eps; //!< Strain step size.
    double inc_t; //!< Angle step size.
    PivotsUltimateStrains agot_pivots; //!< Ultimate strains at pivots.
    std::string concrete_set_name; //!< Concrete fibers set name.
    int concrete_tag; //!< Concrete material tag.
    std::string reinforcement_set_name; //!< Steel fibers set name. 
    int reinforcement_tag; //!< Steel material tag.
  public:
    InteractionDiagramData(void);
    InteractionDiagramData(const double &u,const double &inc_e,const double &inc_t= M_PI/4,const PivotsUltimateStrains &agot= PivotsUltimateStrains());

    inline const double &getThreshold(void) const
      { return threshold; }
    inline void setThreshold(const double &v)
      { threshold= v; }
    inline const double &getIncEps(void) const
      { return inc_eps; }
    inline void setIncEps(const double &v)
      { inc_eps= v; }
    inline const double &getIncTheta(void) const
      { return inc_t; }
    inline void setIncTheta(const double &v)
      { inc_t= v; }
    inline const PivotsUltimateStrains &getPivotsUltimateStrains(void) const
      { return agot_pivots; }
    inline void setPivotsUltimateStrains(const PivotsUltimateStrains &v)
      { agot_pivots= v; }
    inline const std::string &getConcreteSetName(void) const
      { return concrete_set_name; }
    inline void setConcreteSetName(const std::string &v)
      { concrete_set_name= v; }
    inline const int &getConcreteTag(void) const
      { return concrete_tag; }
    inline void setConcreteTag(const int &v)
      { concrete_tag= v; }
    inline const std::string &getRebarSetName(void) const
      { return reinforcement_set_name; }
    inline void setRebarSetName(const std::string &v)
      { reinforcement_set_name= v; }
    inline const int &getReinforcementTag(void) const
      { return reinforcement_tag; }
    inline void setReinforcementTag(const int &v)
      { reinforcement_tag= v; }
  };

} // end of XC namespace

#endif
