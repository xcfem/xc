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

#ifndef DATOSDIAGINTERACCION_H
#define DATOSDIAGINTERACCION_H


#include "DefAgotPivotes.h"

namespace XC {


//! \@ingroup MATSCCDiagInt
//
//! @brief Parámetros necesarios para la obtención del diagrama de interacción.
class InteractionDiagramData: public EntCmd
  {
    double umbral; //!< Distancia mínima entre puntos del diagrama.
    double inc_eps; //!< Strain step size.
    double inc_t; //!< Angle step size.
    DefAgotPivotes agot_pivotes; //!< Ultimate strains at pivots.
    std::string nmb_set_hormigon; //!< Concrete fibers set name.
    int tag_hormigon; //!< Concrete material tag.
    std::string nmb_set_armadura; //!< Steel fibers set name. 
    int tag_armadura; //!< Steel material tag.
  public:
    InteractionDiagramData(void);
    InteractionDiagramData(const double &u,const double &inc_e,const double &inc_t= M_PI/4,const DefAgotPivotes &agot= DefAgotPivotes());

    inline const double &getUmbral(void) const
      { return umbral; }
    inline void setUmbral(const double &v)
      { umbral= v; }
    inline const double &getIncEps(void) const
      { return inc_eps; }
    inline void setIncEps(const double &v)
      { inc_eps= v; }
    inline const double &getIncTheta(void) const
      { return inc_t; }
    inline void setIncTheta(const double &v)
      { inc_t= v; }
    inline const DefAgotPivotes &getDefsAgotPivotes(void) const
      { return agot_pivotes; }
    inline void setDefsAgotPivotes(const DefAgotPivotes &v)
      { agot_pivotes= v; }
    inline const std::string &getNmbSetHormigon(void) const
      { return nmb_set_hormigon; }
    inline void setNmbSetHormigon(const std::string &v)
      { nmb_set_hormigon= v; }
    inline const int &getTagHormigon(void) const
      { return tag_hormigon; }
    inline void setTagHormigon(const int &v)
      { tag_hormigon= v; }
    inline const std::string &getNmbSetArmadura(void) const
      { return nmb_set_armadura; }
    inline void setNmbSetArmadura(const std::string &v)
      { nmb_set_armadura= v; }
    inline const int &getTagArmadura(void) const
      { return tag_armadura; }
    inline void setTagArmadura(const int &v)
      { tag_armadura= v; }
  };

} // end of XC namespace

#endif
