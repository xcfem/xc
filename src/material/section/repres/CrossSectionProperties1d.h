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
//CrossSectionProperties2d.h

#ifndef CrossSectionProperties1d_h
#define CrossSectionProperties1d_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"

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
//! @brief Mechanical (E, G) and mass properties of a section (area,
//! moments of inertia,...) for a bi-dimensional problem (three DOF
//! for each section).
class CrossSectionProperties1d: public CommandEntity, public MovableObject
  {
  private:
    double e; //!< Elastic modulus.
    double a; //!< Area. 
    double rho;  //!< Material density.
    static Matrix ks;
  protected:
    virtual DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    CrossSectionProperties1d(double E, double A, double rho= 0.0);
    CrossSectionProperties1d(double EA);
    CrossSectionProperties1d(const SectionForceDeformation &);
    CrossSectionProperties1d(void); 
    bool check_values(void);
    inline double &E(void)
      { return e; }
    inline const double &E(void) const
      { return e; }
    inline void setE(const double &ee)
      { e= ee; }
    inline double &A(void)
      { return a; }
    inline const double &A(void) const
      { return a; }
    inline void setA(const double &aa)
      { a= aa; }
    inline double getRho(void) const
      { return rho; }
    inline void setRho(const double &r)
      { rho= r; }
    inline double getLinearRho(void) const
      { return rho*a; }
    //! @brief Return axial stiffness.
    inline double EA(void) const
      { return e*a; }
    
    const Matrix &getSectionTangent1x1(void) const;
    const Matrix &getInitialTangent1x1(void) const;
    const Matrix &getSectionFlexibility1x1(void) const;
    const Matrix &getInitialFlexibility1x1(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    int setParameter(const std::vector<std::string> &,Parameter &,SectionForceDeformation *);
    int updateParameter(int parameterID, Information &info);

    virtual void Print(std::ostream &s, int flag = 0) const;
  };

std::ostream &operator<<(std::ostream &,const CrossSectionProperties1d &);

} // end of XC namespace

#endif
