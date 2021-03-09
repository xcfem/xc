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
//ElasticBaseMaterial.h

#ifndef ElasticBaseMaterial_h
#define ElasticBaseMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Base class for uniaxial elastic materials.
class ElasticBaseMaterial: public UniaxialMaterial
  {
  protected:
    double trialStrain; //! Material trial strain.
    double E; //!< Elastic modulus.
    double ezero; //!< Initial deformation.
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

    inline virtual double def_total(void) const { return trialStrain-ezero; }
  public:
    ElasticBaseMaterial(int tag= 0, int classtag= 0, double E= 0.0, double e0= 0.0);    

    inline double getE(void) const
      {return E;}
    inline void setE(const double &e)
      {E= e;}

    int revertToStart(void);
    
    int setInitialStrain(double strain);
    inline double getStrain(void) const
      { return trialStrain; }
    inline double getInitialStrain(void) const
      { return ezero; }
    inline double getInitialTangent(void) const
      { return E; }

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
  };
} // end of XC namespace


#endif
