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

//! \ingroup OneDimensionalElem
//
//! @brief 3D elastic beam element.
class ElasticBeam3d : public ProtoBeam3d
  {
  private:
    Vector eInic; //!< section initial deformations
    double rho;  //!< Density.
    int sectionTag;
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
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    ElasticBeam3d(int tag= 0);
    ElasticBeam3d(int tag,const Material *m,const CrdTransf *trf);
    ElasticBeam3d(int tag, double A, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2,
		  CrdTransf3d &theTransf, double rho = 0.0, int sectionTag = 0);

    ElasticBeam3d(int tag, int Nd1, int Nd2, SectionForceDeformation *section, CrdTransf3d &theTransf, double rho = 0.0);
    ElasticBeam3d(const ElasticBeam3d &otro);
    ElasticBeam3d &operator=(const ElasticBeam3d &otro);
    Element *getCopy(void) const;
    ~ElasticBeam3d(void);

    int setInitialSectionDeformation(const Vector&);
    inline const Vector &getInitialSectionDeformation(void) const
      { return eInic; }
    const Vector &getSectionDeformation(void) const;

    void setDomain(Domain *theDomain);
    
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    double getRho(void) const
      { return rho; }
    void setRho(const double &r)
      { rho= r; }
    const Vector &getInitialStrain(void) const
      { return eInic; }
    void setInitialStrain(const Vector &e)
      { eInic= e; }
    
    int update(void);
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;    

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    const Vector &getVDirStrongAxisGlobalCoord(void) const;
    const Vector &getVDirWeakAxisGlobalCoord(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);    

    inline double getAN1(void) //Axial force which acts over the element in his back end.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return q.AN1()+p0[0];
      }
    inline double getAN2(void) //Axial force which acts over the element in his front end.
      {                 //¡Warning! call "calc_resisting_force" before calling this method.
        return q.AN2();
      }
    //! @brief Internal axial force at the back end.   
    inline double getN1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.AN1()-p0[0];
      }
    //! @brief Internal axial force at the front end.   
    inline double getN2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.AN2();
      }
    //! @brief Internal axial force at the middle of the element.
    inline double getN(void) //Mean axial force.
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return (-q.AN1()-p0[0]+q.AN2())/2.0;
      }
    inline double getAMz1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Mz1(); //Momento z que se ejerce sobre la barra en su extremo dorsal.
      }
    inline double getAMz2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Mz2(); //Momento z que se ejerce sobre la barra en su extremo frontal.
      }
    //! @brief Internal bending moment at the back end.
    inline double getMz1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.Mz1(); //Momento z en su extremo dorsal.
      }
    //! @brief Internal bending moment at the front end.
    inline double getMz2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Mz2(); //Momento z en su extremo frontal.
      }
    //! @brief Internal shear force in the middle of the element.
    inline double getVy(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Vy(theCoordTransf->getInitialLength()); //Cortante y.
      }
    inline double getAVy1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Vy(theCoordTransf->getInitialLength())+p0[1]; //Cortante y que se ejerce sobre la barra en su extremo dorsal.
      }
    inline double getAVy2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.Vy(theCoordTransf->getInitialLength())+p0[2]; //Cortante y que se ejerce sobre la barra en su extremo frontal.
      }
    //! @brief Internal shear force at the back end.
    inline double getVy1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.Vy(theCoordTransf->getInitialLength())-p0[1]; //Cortante y en su extremo dorsal.
      }
    //! @brief Internal shear force at the front end.
    inline double getVy2(void)
      {  //¡Warning! call "calc_resisting_force" before calling this method.
        return p0[2]-q.Vy(theCoordTransf->getInitialLength()); //Cortante y en su extremo frontal.
      }
    //! @brief Internal shear force in the middle of the element.
    inline double getVz(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Vz(theCoordTransf->getInitialLength()); //Cortante z.
      }
    inline double getAVz1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.Vz(theCoordTransf->getInitialLength())+p0[3]; //Cortante z que se ejerce sobre la barra en su extremo dorsal.
      }
    inline double getAVz2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.Vz(theCoordTransf->getInitialLength())+p0[4]; //Cortante z que se ejerce sobre la barra en su extremo frontal.
      }
    //! @brief Internal shear force at the back end.
    inline double getVz1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.Vz(theCoordTransf->getInitialLength())-p0[3]; //Cortante z en su extremo dorsal.
      }
    //! @brief Internal shear force at the front end.
    inline double getVz2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return p0[4]-q.Vz(theCoordTransf->getInitialLength()); //Cortante z en su extremo frontal.
      }
    //! @brief Internal bending moment at the back end.   
    inline double getMy1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.My1(); //Momento y en el extremo dorsal.
      }
    //! @brief Internal bending moment at the front end.   
    inline double getMy2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.My2(); //Momento y en el extremo frontal.
      }
    //! @brief Internal torsional force in the middle of the element.   
    inline double getT(void) //Torsor en la barra.
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.T();
      }
    //! @brief Internal torsional force at the back end.   
    inline double getT1(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return -q.T1(); //+p0[0]; //Torsor en el extremo dorsal.
      }
    //! @brief Internal torsional force at the front end.
    inline double getT2(void)
      { //¡Warning! call "calc_resisting_force" before calling this method.
        return q.T2(); //Torsor en el extremo frontal.
      }

    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);
  };
} // end of XC namespace

#endif


