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
//BaseElasticSection3d.h

#ifndef BaseElasticSection3d_h
#define BaseElasticSection3d_h

#include "BaseElasticSection.h"
#include <utility/matrix/Vector.h>
#include "material/section/repres/CrossSectionProperties3d.h"

namespace XC {

//! @ingroup MATSCCElastica
//
//! @brief Base class for cross sections with linear elastic material
//! on a three-dimensional problem (6 DOFs on each section).
//!
//! The generalized deformation vector is:
//! \f[
//! esec := \left[
//!   \begin{array}{c}
//!       \epsilon_a
//!       \kappa_z
//!       \kappa_y
//!       \gamma_y
//!       \gamma_z
//!       \phi
//!   \end{array} 
//! \right]
//! \f]
//! where \f$\epsilon_a\f$ is the axial strain, \f$\kappa_z\f$ is
//! the curvature about the local z-axis, \f$\kappa_z\f$ is the curvature
//! about the local z-axis, \f$\gamma_y\f$ is the shear strain along the
//! local y-axis, \f$\gamma_z\f$ is the shear strain along the local z-axis,
//! and \f$\phi\f$ is the angle of twist.
class BaseElasticSection3d: public BaseElasticSection
  {
  protected:
    CrossSectionProperties3d ctes_scc; //!< Mechanical properties of the section.

    int sendData(CommParameters &);
    int recvData(const CommParameters &);


  public:
    BaseElasticSection3d(int tag,int classTag,const size_t &dim,const CrossSectionProperties3d &ctes,MaterialHandler *mat_ldr= nullptr);    
    BaseElasticSection3d(int tag,int classTag,const size_t &dim,MaterialHandler *mat_ldr= nullptr);    

    const CrossSectionProperties3d &getCrossSectionProperties(void) const;
    void setCrossSectionProperties(const CrossSectionProperties3d &);  
    void sectionGeometry(const std::string &);

    inline double getRho(void) const
      { return ctes_scc.getRho(); }
    inline void setRho(const double &r)
      { ctes_scc.setRho(r); }

    double getStrain(const double &y,const double &z) const;

    void Print(std::ostream &s, int flag =0) const;
  };

//! @brief Printing sturr.
inline std::ostream &operator<<(std::ostream &os,const BaseElasticSection3d &scc)
  {
    scc.Print(os);
    return os;
  }
} // end of XC namespace

#endif
