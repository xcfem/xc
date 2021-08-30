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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.8 $
// $Date: 2003/03/11 20:42:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/elasticBeamColumn/ElasticBeam3d.h,v $
                                                                        
                                                                        
// File: ~/model/ElasticBeam3d.h
//
// Written: fmk 11/95
// Revised:
//
// Purpose: This file contains the class definition for ElasticBeam3d.
// ElasticBeam3d is a plane frame member.

#ifndef ElasticBeam3d_h
#define ElasticBeam3d_h

#include <domain/mesh/element/truss_beam_column/ProtoBeam3d.h>
#include <utility/matrix/Matrix.h>
#include "domain/mesh/element/truss_beam_column/EsfBeamColumn3d.h"
#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn3d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf3d.h"

namespace XC {
class Channel;
class Information;
class Response;
class SectionForceDeformation;

//! @ingroup BeamColumnElemGrp
//
//! @brief 3D elastic beam element.
class ElasticBeam3d: public ProtoBeam3d
  {
  private:
    int releasez; //!< moment release for bending about z-axis 0=none, 1=I, 2=J, 3=I,J
    int releasey; //!< moment release for bending about y-axis 0=none, 1=I, 2=J, 3=I,J
    mutable EsfBeamColumn3d q;
    FVectorBeamColumn3d q0;  //!< Fixed end forces in basic system (no torsion)
    FVectorBeamColumn3d p0;  //!< Reactions in basic system (no torsion)
 
    CrdTransf3d *theCoordTransf; //!< Coordinate transformation.

    static Matrix K;
    static Vector P;
    
    static Matrix kb;

    void set_transf(const CrdTransf *trf);
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    ElasticBeam3d(int tag= 0);
    ElasticBeam3d(int tag,const Material *m,const CrdTransf *trf);
    ElasticBeam3d(int tag, double A, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2, CrdTransf3d &theTransf, double rho = 0.0, int releasez= 0, int releasey= 0);

    ElasticBeam3d(int tag, int Nd1, int Nd2, SectionForceDeformation *section, CrdTransf3d &theTransf, double rho = 0.0, int relz= 0, int rely= 0);
    ElasticBeam3d(const ElasticBeam3d &);
    ElasticBeam3d &operator=(const ElasticBeam3d &);
    Element *getCopy(void) const;
    ~ElasticBeam3d(void);

    const Vector &computeCurrentStrain(void) const;

    void setDomain(Domain *theDomain);
    
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    virtual int update(void);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;    

    void zeroLoad(void);	
    int addLoad(ElementalLoad *, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    const Vector &getVDirStrongAxisGlobalCoord(bool initialGeometry) const;
    const Vector &getVDirWeakAxisGlobalCoord(bool initialGeometry) const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;    

    //! @brief Axial force which acts over the element in his back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAN1(void) const
      {
        return q.AN1()+p0[0];
      }
    //! @brief Axial force which acts over the element in his front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAN2(void) const
      {
        return q.AN2();
      }
    //! @brief Internal axial force at the back end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getN1(void) const
      { 
        return -q.AN1()-p0[0];
      }
    //! @brief Internal axial force at the front end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getN2(void) const
      {
        return q.AN2();
      }
    //! @brief Internal axial force at the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getN(void) const //Mean axial force.
      {
        return (-q.AN1()-p0[0]+q.AN2())/2.0;
      }
    //! @brief Moment about z axis that is applied over the bar in its back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAMz1(void) const
      {
        return q.Mz1(); 
      }
    //! @brief Moment about z axis applied over the bar in its front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAMz2(void) const
      {
        return q.Mz2();
      }
    //! @brief Internal bending moment about z axis at the back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getMz1(void) const
      {
        return -q.Mz1();
      }
    //! @brief Internal bending moment about z axis at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getMz2(void) const
      {
        return q.Mz2();
      }
    //! @brief Internal shear force in the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getVy(void) const
      {
        return q.Vy(theCoordTransf->getInitialLength()); //Shear along y.
      }
    //! @brief y shear drived over the bar in its back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAVy1(void) const
      {
        return q.Vy(theCoordTransf->getInitialLength())+p0[1];
      }
    //! @brief y shear drived over the bar in its front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAVy2(void) const
      {
        return -q.Vy(theCoordTransf->getInitialLength())+p0[2]; 
      }
    //! @brief Internal y shear force at the back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getVy1(void) const
      {
        return -q.Vy(theCoordTransf->getInitialLength())-p0[1]; 
      }
    //! @brief Internal y shear force at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getVy2(void) const
      {
        return p0[2]-q.Vy(theCoordTransf->getInitialLength());
      }
    //! @brief Internal z shear force in the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getVz(void) const
      {
        return q.Vz(theCoordTransf->getInitialLength());
      }
    //! @brief z shear drived over the bar in its back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAVz1(void) const
      {
        return q.Vz(theCoordTransf->getInitialLength())+p0[3]; 
      }
    //! @brief z shear drived over the bar in its front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getAVz2(void) const
      {
        return -q.Vz(theCoordTransf->getInitialLength())+p0[4]; 
      }
    //! @brief Internal z shear force at the back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getVz1(void) const
      {
        return -q.Vz(theCoordTransf->getInitialLength())-p0[3];
      }
    //! @brief Internal z shear force at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getVz2(void) const
      {
        return p0[4]-q.Vz(theCoordTransf->getInitialLength());
      }
    //! @brief Internal bending moment about y axis at the back end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getMy1(void) const
      {
        return -q.My1();
      }
    //! @brief Internal bending moment about y axis at the front end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getMy2(void) const
      {
        return q.My2();
      }
    //! @brief Internal torsional force in the middle of the element.   
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getT(void) const
      {
        return q.T();
      }
    //! @brief Internal torsional force at the back end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getT1(void) const
      {
        return -q.T1(); //+p0[0];
      }
    //! @brief Internal torsional force at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    inline double getT2(void) const
      {
        return q.T2();
      }

    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);
    
    boost::python::list getValuesAtNodes(const std::string &, bool silent= false) const;
  };
} // end of XC namespace

#endif


