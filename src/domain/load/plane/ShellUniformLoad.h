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
//ShellUniformLoad.h
                                              
#ifndef ShellUniformLoad_h
#define ShellUniformLoad_h

#include "ShellMecLoad.h"

namespace XC {

//! @ingroup ElemLoads
//
//! @brief Carga uniforme sobre elementos shell.
class ShellUniformLoad : public ShellMecLoad
  {
  protected:
    DbTagData &getDbTagData(void) const;
  public:
    ShellUniformLoad(int tag,const double &,const double &,const double &,const ID &theElementTags);
    ShellUniformLoad(int tag, const Vector &Fxyz, const ID &theElementTags);
    ShellUniformLoad(int tag= 0);

    std::string Categoria(void) const;
    int getType(void);
    inline const double &Wx(void) const
      { return Axial1; }
    inline const double &Wy(void) const
      { return Axial2; }
    inline const double &Wz(void) const
      { return Trans; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    void addReactionsInBasicSystem(const double &,const double &,FVectorShell &);
    void addFixedEndForcesInBasicSystem(const double &,const double &loadFactor,FVectorShell &);

    //! @brief Returns pressure vectors (one for each element) expressed in elemnt local coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getLocalPressures(void) const
      { return getLocalForces(); }
    //! @brief Returns distributed moments (one for each element) expressed in element local coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getDistributedLocalMoments(void) const
      { return getLocalMoments(); }
    //! @brief Returns pressure vectors (one for each element) expressed in global coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getGlobalPressures(void) const
      { return getGlobalForces(); }
    //! @brief Returns distributed moments (one for each element) expressed in global coordinates. Is simply a convenience function that makes the distributedness more explicit.
    inline const Matrix &getDistributedGlobalMoments(void) const
      { return getGlobalMoments(); }

    virtual SVD3d getResultant(const Pos3d &p= Pos3d(), bool initialGeometry= true) const;

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

