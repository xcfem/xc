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

#ifndef ElasticBeam3dBase_h
#define ElasticBeam3dBase_h

#include "domain/mesh/element/truss_beam_column/ProtoBeam3d.h"

namespace XC {
class Channel;
class Information;
class Response;
class SectionForceDeformation;

//! @brief Base class for 3D elastic beam elements.
//! @ingroup BeamColumnElemGrp
class ElasticBeam3dBase: public ProtoBeam3d
  {
  protected: 
    CrdTransf3d *theCoordTransf; //!< Coordinate transformation.

    void set_transf(const CrdTransf *trf);

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    ElasticBeam3dBase(int tag, int classTag);
    ElasticBeam3dBase(int tag, int classTag,const Material *m,const CrdTransf *trf);
    ElasticBeam3dBase(int tag, int classTag, double A, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2, CrdTransf3d &theTransf, double rho = 0.0);
    ElasticBeam3dBase(int tag, int classTag, double A, double alpha_y, double alpha_z, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2, CrdTransf3d &theTransf, double rho = 0.0);

    ElasticBeam3dBase(int tag, int classTag, int Nd1, int Nd2, SectionForceDeformation *section, CrdTransf3d &theTransf, double rho = 0.0);
    ElasticBeam3dBase(const ElasticBeam3dBase &);
    ElasticBeam3dBase &operator=(const ElasticBeam3dBase &);
    ~ElasticBeam3dBase(void);

    void setDomain(Domain *theDomain);
    
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    const Vector &getVDirStrongAxisGlobalCoord(bool initialGeometry) const;
    const Vector &getVDirWeakAxisGlobalCoord(bool initialGeometry) const;    

    //! @brief Internal axial force at the back end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getN1(void) const= 0;
    //! @brief Internal axial force at the front end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getN2(void) const= 0;
    //! @brief Internal axial force at the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getN(void) const= 0; //Mean axial force.
    //! @brief Internal bending moment about z axis at the back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getMz1(void) const= 0;
    //! @brief Internal bending moment about z axis at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getMz2(void) const= 0;
    //! @brief Internal shear force in the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getVy(void) const= 0;
    //! @brief Internal y shear force at the back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getVy1(void) const= 0;
    //! @brief Internal y shear force at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getVy2(void) const= 0;
    //! @brief Internal z shear force in the middle of the element.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getVz(void) const= 0;
    //! @brief Internal z shear force at the back end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getVz1(void) const= 0;
    //! @brief Internal z shear force at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getVz2(void) const= 0;
    //! @brief Internal bending moment about y axis at the back end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getMy1(void) const= 0;
    //! @brief Internal bending moment about y axis at the front end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getMy2(void) const= 0;
    //! @brief Internal torsional force in the middle of the element.   
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getT(void) const= 0;
    //! @brief Internal torsional force at the back end.   
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getT1(void) const= 0;
    //! @brief Internal torsional force at the front end.
    //! Warning! call "calc_resisting_force" before calling this method.
    virtual double getT2(void) const= 0;
  };
} // end of XC namespace

#endif


