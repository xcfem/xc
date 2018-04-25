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

//! \ingroup OneDimensionalElem
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

    inline double getAN1(void) //Axial force which acts over the element in his back end.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.AN1()+p0[0];
      }
    inline double getAN2(void) //Axial force which acts over the element in his front end.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.AN2();
      }
    inline double getN1(void) //Axial force in the front end.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.AN1()-p0[0];
      }
    inline double getN2(void) //Axial force in the back end.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.AN2();
      }
    inline double getN(void) //Mean axial force.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return (-Secommit.AN1()-p0[0]+Secommit.AN2())/2.0;
      }
    inline double getAMz1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Mz1(); //Momento z drived over the bar in its back end.
      }
    inline double getAMz2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Mz2(); //Momento z drived over the bar in its front end.
      }
    inline double getMz1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.Mz1(); //Momento z in its back end.
      }
    inline double getMz2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.Mz2(); //Momento z in its front end.
      }
    inline double getVy(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vy(theCoordTransf->getInitialLength()); //Cortante y.
      }
    inline double getAVy1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vy(theCoordTransf->getInitialLength())+p0[1]; //Cortante y drived over the bar in its back end.
      }
    inline double getAVy2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.Vy(theCoordTransf->getInitialLength())+p0[2]; //Cortante y drived over the bar in its front end.
      }
    inline double getVy1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.Vy(theCoordTransf->getInitialLength())-p0[1]; //Cortante y in its back end.
      }
    inline double getVy2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vy(theCoordTransf->getInitialLength())-p0[2]; //Cortante y in its front end.
      }
    inline double getVz(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vz(theCoordTransf->getInitialLength()); //Cortante z.
      }
    inline double getAVz1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vz(theCoordTransf->getInitialLength())+p0[3]; //Cortante z drived over the bar in its back end.
      }
    inline double getAVz2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.Vz(theCoordTransf->getInitialLength())+p0[4]; //Cortante z drived over the bar in its front end.
      }
    inline double getVz1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return -Secommit.Vz(theCoordTransf->getInitialLength())-p0[3]; //Cortante z in its back end.
      }
    inline double getVz2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.Vz(theCoordTransf->getInitialLength())-p0[4]; //Cortante z in its front end.
      }
    inline double getMy1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.My1(); //Momento y en el extremo dorsal.
      }
    inline double getMy2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.My2(); //Momento y en el extremo frontal.
      }
    inline double getT(void) //Element's torque.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.T();
      }
    inline double getT1(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.T1(); //+p0[0]; //Torsor en el extremo dorsal.
      }
    inline double getT2(void)
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return Secommit.T2(); //Torsor en el extremo frontal.
      }

  };
} // end of XC namespace

#endif

