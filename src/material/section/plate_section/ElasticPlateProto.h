// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//ElasticPlateProto.h

#ifndef ElasticPlateProto_h
#define ElasticPlateProto_h


#include "ElasticPlateBase.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/actor/actor/MovableVector.h"


namespace XC {
//! @brief Base class for ElasticPlateSection and ElasticMembranePlateSection.
//! @ingroup MATPLAC
template <int SZ>
class ElasticPlateProto: public ElasticPlateBase
  {
  protected:
    Vector trialStrain;
    Vector initialStrain;
    static Vector stress;
    static Matrix tangent;

    int sendData(Communicator &);
    int recvData(const Communicator &);
  public : 
    ElasticPlateProto(int tag,int classTag);
    ElasticPlateProto(int classTag);
    ElasticPlateProto(int tag,int classTag, double E, double nu, double h, double rho);

    //send back order of strain in vector form
    int getOrder(void) const;

    //get the strain and integrate plasticity equations
    int setInitialSectionDeformation(const Vector &strain);
    int setTrialSectionDeformation(const Vector &strain_from_element);
    //send back the strain
    void zeroInitialSectionDeformation(void)
      { initialStrain.Zero(); }
    inline const Vector &getInitialSectionDeformation(void) const
      { return initialStrain; }
    const Vector &getSectionDeformation(void) const;

    int revertToStart(void);
  };

//static vector and matrices
template <int SZ>
XC::Vector XC::ElasticPlateProto<SZ>::stress(SZ);
template <int SZ>
XC::Matrix XC::ElasticPlateProto<SZ>::tangent(SZ,SZ);


template <int SZ>
XC::ElasticPlateProto<SZ>::ElasticPlateProto(int tag,int classTag)
  : ElasticPlateBase(tag, classTag), trialStrain(SZ), initialStrain(SZ) {}

//null constructor
template <int SZ>
XC::ElasticPlateProto<SZ>::ElasticPlateProto(int classTag)
  : ElasticPlateBase( 0, classTag), trialStrain(SZ), initialStrain(SZ) {}

//full constructor
template <int SZ>
XC::ElasticPlateProto<SZ>::ElasticPlateProto(int tag, int classTag,
                                             double young,
                                             double poisson,
					     double thickness,
					     double rho)
  : ElasticPlateBase(tag,classTag,young,poisson,thickness, rho), trialStrain(SZ), initialStrain(SZ) {}

template <int SZ>
int XC::ElasticPlateProto<SZ>::getOrder(void) const
  { return SZ; }

//get the strain 
template <int SZ>
int XC::ElasticPlateProto<SZ>::setInitialSectionDeformation(const Vector &strain)
  {
    initialStrain= strain;
    return 0;
  }

//get the strain 
template <int SZ>
int XC::ElasticPlateProto<SZ>::setTrialSectionDeformation(const Vector &strain)
  {
    trialStrain= strain;
    return 0;
  }

//send back the strain
template <int SZ>
const XC::Vector &XC::ElasticPlateProto<SZ>::getSectionDeformation(void) const
  {
    static Vector retval;
    retval= trialStrain-initialStrain;
    return retval;
  }

//@ brief Revert the material to its initial state.
template <int SZ>
int XC::ElasticPlateProto<SZ>::revertToStart(void)
  {
    trialStrain.Zero();
    initialStrain.Zero();
    return ElasticPlateBase::revertToStart();
  }

//! @brief Send data through the communicator argument.
template <int SZ>
int XC::ElasticPlateProto<SZ>::sendData(Communicator &comm)
  {
    int res= ElasticPlateBase::sendData(comm);
    res+= comm.sendVector(trialStrain,getDbTagData(),CommMetaData(6));
    res+= comm.sendVector(initialStrain,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receive data through the communicator argument.
template <int SZ>
  int XC::ElasticPlateProto<SZ>::recvData(const Communicator &comm)
  {
    int res= ElasticPlateBase::recvData(comm);
    res+= comm.receiveVector(trialStrain,getDbTagData(),CommMetaData(6));
    res+= comm.receiveVector(initialStrain,getDbTagData(),CommMetaData(7));
    return res;
  }

} // end of XC namespace

#endif
