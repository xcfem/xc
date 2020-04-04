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
//BaseElasticSection2d.h

#ifndef BaseElasticSection2d_h
#define BaseElasticSection2d_h

#include "BaseElasticSection.h"
#include <utility/matrix/Vector.h>
#include "material/section/repres/CrossSectionProperties2d.h"

namespace XC {

//! @ingroup MATSCCElastica
//
//! @brief Base class for cross sections with linear elastic material
//! on a bi-dimensional problem (3 DOFs on each section).
class BaseElasticSection2d: public BaseElasticSection
  {
  protected:
    CrossSectionProperties2d ctes_scc; //!< Mechanical properties of the section.

    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    BaseElasticSection2d(int tag,int classTag,const size_t &dim, const double &E, const double &A, const double &I,double G, const double &alpha);
    BaseElasticSection2d(int tag,int classTag,const size_t &dim,MaterialHandler *mat_ldr= nullptr);    
    BaseElasticSection2d(int classTag,const size_t &dim);

    const CrossSectionProperties2d &getCrossSectionProperties(void) const;   
    CrossSectionProperties2d &getCrossSectionProperties(void);   
    void setCrossSectionProperties(const CrossSectionProperties2d &);  
    void sectionGeometry(const std::string &);

    inline double getRho(void) const
      { return ctes_scc.getRho(); }
    inline void setRho(const double &r)
      { ctes_scc.setRho(r); }

    double getStrain(const double &y,const double &z= 0) const;

    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
