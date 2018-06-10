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
//NLForceBeamColumn3dBase.h

#ifndef NLForceBeamColumn3dBase_h
#define NLForceBeamColumn3dBase_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/EsfBeamColumn3d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf3d.h"

namespace XC {

//! @ingroup OneDimensionalElem
//
//! @brief Base clas for 3D force beam column elements with PrismaticBarCrossSection type material.
class NLForceBeamColumn3dBase: public BeamColumnWithSectionFDTrf3d
  {
    NLForceBeamColumn3dBase &operator=(const NLForceBeamColumn3dBase &);
  protected:
    static const size_t NDM; //!< dimension of the problem (3d)
    static const int NND; //!< number of nodal dof's
    static const size_t NEGD; //!< number of element global dof's
    static const size_t NEBD; //!< number of element dof's in the basic system
    static const double DefaultLoverGJ;

    
    // internal data
    double rho; //!<mass density per unit length
    int maxIters; //!<maximum number of local iterations
    double tol;	 //!<tolerance for relative energy norm for local iterations

    int initialFlag; //!<indicates if the element has been initialized
    bool isTorsion;
	
    Matrix kv; //!<stiffness matrix in the basic system 
    EsfBeamColumn3d Se; //!<element resisting forces in the basic system

    Matrix kvcommit; //!<commited stiffness matrix in the basic system
    EsfBeamColumn3d Secommit; //!<commited element end forces in the basic system

    std::vector<Matrix> fs; //!<array of section flexibility matrices
    std::vector<Vector> vs; //!<array of section deformation vectors
    std::vector<Vector> Ssr; //!<array of section resisting force vectors
    std::vector<Vector> vscommit; //!<array of commited section deformation vectors

    Matrix sp; //!<Applied section forces due to element loads, 5 x nSections
    FVectorBeamColumn3d p0; //!<Reactions in the basic system due to element loads

    mutable Matrix Ki;

    static Matrix theMatrix;
    static Vector theVector;
    static double workArea[];

    void resizeMatrices(const size_t &nSections);
    void initializeSectionHistoryVariables(void);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    NLForceBeamColumn3dBase(int tag,int classTag,int numSec= 0);
    NLForceBeamColumn3dBase(int tag,int classTag,int numSec,const Material *theSection,const CrdTransf *coordTransf);
    NLForceBeamColumn3dBase(const NLForceBeamColumn3dBase &otro);

    void setSection(const PrismaticBarCrossSection *sccModel);

    int getNumDOF(void) const;

    double getRho(void) const
      { return rho; }
    void setRho(const double &r)
      { rho= r; }

    const Matrix &getTangentStiff(void) const;

    const Vector &getResistingForce(void) const;

    //! Axial force which acts over the element at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAN1(void) 
      {
        return Secommit.AN1()+p0[0];
      }
    //! @brief Axial force which acts over the element at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAN2(void) 
      {
        return Secommit.AN2();
      }
    //! @brief Axial force at the front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getN1(void) 
      {                 
        return -Secommit.AN1()-p0[0];
      }
    //! @brief Axial force at the back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getN2(void) 
      {
        return Secommit.AN2();
      }
    //! @brief Mean axial force.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getN(void) 
      {
        return (-Secommit.AN1()-p0[0]+Secommit.AN2())/2.0;
      }
    //! @brief Moment about z axis drived over the bar at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAMz1(void)
      {
        return Secommit.Mz1(); 
      }
    //! @brief Moment about z axis drived over the bar at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAMz2(void)
      {
        return Secommit.Mz2(); 
      }
    //! @brief Moment about z axis at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getMz1(void)
      {
        return -Secommit.Mz1(); 
      }
    //! @brief Moment about z axis at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getMz2(void)
      {
        return -Secommit.Mz2(); 
      }
    //! @brief y shear.
    inline double getVy(void)
      {
        return Secommit.Vy(theCoordTransf->getInitialLength()); 
      }
    //! @brief y shear drived over the bar at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAVy1(void)
      {
        return Secommit.Vy(theCoordTransf->getInitialLength())+p0[1]; 
      }
    //! @brief y shear drived over the bar at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAVy2(void)
      {
        return -Secommit.Vy(theCoordTransf->getInitialLength())+p0[2]; 
      }
    //! @brief y shear at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getVy1(void)
      {
        return -Secommit.Vy(theCoordTransf->getInitialLength())-p0[1]; 
      }
    //! @brief y shear at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getVy2(void)
      {
        return Secommit.Vy(theCoordTransf->getInitialLength())-p0[2]; 
      }
    //! @brief z shear.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getVz(void)
      {
        return Secommit.Vz(theCoordTransf->getInitialLength()); 
      }
    //! @brief z shear drived over the bar at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAVz1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vz(theCoordTransf->getInitialLength())+p0[3]; 
      }
    //! @brief z shear drived over the bar at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getAVz2(void)
      {
        return -Secommit.Vz(theCoordTransf->getInitialLength())+p0[4]; 
      }
    //! @brief z shear at its back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getVz1(void)
      {
        return -Secommit.Vz(theCoordTransf->getInitialLength())-p0[3]; 
      }
    //! @brief z shear at its front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getVz2(void)
      {
        return Secommit.Vz(theCoordTransf->getInitialLength())-p0[4];
      }
    //! @brief Moment about y axis at the back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getMy1(void)
      {
        return Secommit.My1();
      }
    //! @brief Moment about y axis at the front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getMy2(void)
      {
        return Secommit.My2();
      }
    //! @brief Element's torque.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getT(void) 
      {
        return Secommit.T();
      }
    //! @brief Torsor at the back end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getT1(void)
      {
        return Secommit.T1(); //+p0[0]; 
      }
    //! @brief Torsor at the front end.
    //! ¡Warning! call "calc_resisting_force" before calling this method.
    inline double getT2(void)
      {
        return Secommit.T2(); 
      }

  };
} // end of XC namespace

#endif

