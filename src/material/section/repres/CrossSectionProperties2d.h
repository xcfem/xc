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
//CrossSectionProperties2d.h

#ifndef CrossSectionProperties2d_h
#define CrossSectionProperties2d_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"

class PrincipalAxesOfInertia2D;
class Vector2d;

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Information;
class Parameter;
class Matrix;
class Vector;
class ID;
class SectionForceDeformation;

//! @ingroup MATSCCRepres
//
//! @brief Mechanical (E, G) and mass properties of a section (area, moments of inertia,...)
//! for a bi-dimensional problem (three DOF for each section).
class CrossSectionProperties2d: public CommandEntity, public MovableObject
  {
  private:
    double e, g, a, i, alpha;
    double rho;  //!< Material density.
    static Matrix ks2;
    static Matrix ks3;
  protected:
    virtual DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    CrossSectionProperties2d(double E, double A, double I, double G= 0.0,double alpha= 0.0, double rho= 0.0);
    CrossSectionProperties2d(double EA, double EI);
    CrossSectionProperties2d(const SectionForceDeformation &);
    CrossSectionProperties2d(void); 
    bool check_values(void);
    inline double &E(void)
      { return e; }
    inline const double &E(void) const
      { return e; }
    inline void setE(const double &ee)
      { e= ee; }
    inline double &G(void)
      { return g; }
    inline const double &G(void) const
      { return g; }
    inline void setG(const double &gg)
      { g= gg; }
    inline double &A(void)
      { return a; }
    inline const double &A(void) const
      { return a; }
    inline void setA(const double &aa)
      { a= aa; }
    inline double &Alpha(void)
      { return alpha; }
    inline const double &Alpha(void) const
      { return alpha; }
    inline void setAlpha(const double &al)
      { alpha= al; }
    inline double getRho(void) const
      { return rho; }
    inline void setRho(const double &r)
      { rho= r; }
    inline double &I(void)
      { return i; }
    inline const double &I(void) const
      { return i; }
    inline void setI(const double &ii)
      { i= ii; }
    //! @brief Return axial stiffness.
    inline double EA(void) const
      { return e*a; }
    //! @brief Return z bending stiffness.
    inline double EI(void) const
      { return e*i; }
    //! @brief Returns shear stiffness.
    inline double GAAlpha(void) const
      { return g*a*alpha; }

    virtual double getTheta(void) const;
    virtual double getI1(void) const;
    virtual double getI2(void) const;
    virtual PrincipalAxesOfInertia2D getInertiaAxes(void) const;
    virtual Vector2d getAxis1VDir(void) const;
    virtual Vector2d getVDirStrongAxis(void) const;
    virtual Vector2d getAxis2VDir(void) const;
    virtual Vector2d getVDirWeakAxis(void) const;

    const Matrix &getSectionTangent2x2(void) const;
    const Matrix &getInitialTangent2x2(void) const;
    const Matrix &getSectionFlexibility2x2(void) const;
    const Matrix &getInitialFlexibility2x2(void) const;
    const Matrix &getSectionTangent3x3(void) const;
    const Matrix &getInitialTangent3x3(void) const;
    const Matrix &getSectionFlexibility3x3(void) const;
    const Matrix &getInitialFlexibility3x3(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    int setParameter(const std::vector<std::string> &,Parameter &,SectionForceDeformation *);
    int updateParameter(int parameterID, Information &info);

    const Matrix& getInitialTangentSensitivity3x3(int gradIndex);


    virtual void Print (std::ostream &s, int flag = 0) const;
  };

std::ostream &operator<<(std::ostream &,const CrossSectionProperties2d &);

} // end of XC namespace

#endif
