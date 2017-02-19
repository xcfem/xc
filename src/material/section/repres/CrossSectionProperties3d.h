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
//CrossSectionProperties3d.h

#ifndef CrossSectionProperties3d_h
#define CrossSectionProperties3d_h

#include "CrossSectionProperties2d.h"

namespace XC {

//! @ingroup MATSCCRepres
//
//! @brief Mechanical properties of a section (area, moments of inertia,...)
//! in a three-dimensional problem (six degrees of freedom on each section).
class CrossSectionProperties3d: public CrossSectionProperties2d
  {
  private:
    double iy, iyz, j;
    static Matrix ks4;
    static Matrix ks6;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    CrossSectionProperties3d(double E, double A, double Iz, double Iy, double G, double J);
    CrossSectionProperties3d(double EA, double EIz, double EIy, double GJ);
    CrossSectionProperties3d(void);

    bool check_values(void);
    inline double &Iz(void)
      { return CrossSectionProperties2d::I(); }
    inline const double &Iz(void) const
      { return CrossSectionProperties2d::I(); }
    inline void setIz(const double &i)
      { CrossSectionProperties2d::I()= i; }
    inline double &Iy(void)
      { return iy; }
    inline const double &Iy(void) const
      { return iy; }
    inline void setIy(const double &i)
      { iy= i; }
    inline double &Iyz(void)
      { return iyz; }
    inline const double &Iyz(void) const
      { return iyz; }
    inline void setIyz(const double &i)
      { iyz= i; }
    inline double &J(void)
      { return j; }
    inline const double &J(void) const
      { return j; }
    inline void setJ(const double &i)
      { j= i; }
    //! @brief Returns the z bending stiffness.
    inline double EIz(void) const
      { return CrossSectionProperties2d::EI(); }
    //! @brief Returns the y bending stiffness.
    inline double EIy(void) const
      { return E()*iy; }
    //! @brief Returns the colaboración del producto
    //! de inercia a la bending stiffness.
    inline double EIyz(void) const
      { return E()*iyz; }
    //! @brief Returns the torsional stiffness.
    inline double GJ(void) const
      { return G()*j; }

    double getTheta(void) const;
    double getI1(void) const;
    double getI2(void) const;
    EjesPrincInercia2d getEjesInercia(void) const;
    Vector2d getVDirEje1(void) const;
    Vector2d getVDirStrongAxis(void) const;
    Vector2d getVDirEje2(void) const;
    Vector2d getVDirWeakAxis(void) const;

    const Matrix &getSectionTangent4x4(void) const;
    const Matrix &getInitialTangent4x4(void) const;
    const Matrix &getSectionFlexibility4x4(void) const;
    const Matrix &getInitialFlexibility4x4(void) const;
    const Matrix &getSectionTangent6x6(void) const;
    const Matrix &getInitialTangent6x6(void) const;
    const Matrix &getSectionFlexibility6x6(void) const;
    const Matrix &getInitialFlexibility6x6(void) const;

    void gira(const double &theta);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    int setParameter(const std::vector<std::string> &argv,Parameter &param,SectionForceDeformation *scc);
    int updateParameter(int parameterID, Information &info);


    void Print (std::ostream &s, int flag = 0) const;
  };

} // end of XC namespace

#endif
