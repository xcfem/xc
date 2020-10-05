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
//NLForceBeamColumn3dBase.cpp

#include <domain/mesh/element/truss_beam_column/NLForceBeamColumn3dBase.h>
#include <material/section/PrismaticBarCrossSection.h>

#include "utility/actor/actor/MatrixCommMetaData.h"

const size_t XC::NLForceBeamColumn3dBase::NDM= 3; //!< dimension of the problem (3d)
const int XC::NLForceBeamColumn3dBase::NND= 6; //!< number of nodal dof's
const size_t XC::NLForceBeamColumn3dBase::NEGD= 12; //!< number of element global dof's
const size_t XC::NLForceBeamColumn3dBase::NEBD= 6; //!< number of element dof's in the basic system
const double XC::NLForceBeamColumn3dBase::DefaultLoverGJ= 1.0e-10;
XC::Matrix XC::NLForceBeamColumn3dBase::theMatrix(12,12);
XC::Vector XC::NLForceBeamColumn3dBase::theVector(12);
double XC::NLForceBeamColumn3dBase::workArea[200];

//! @brief Allocate section flexibility matrices and section deformation vectors
void XC::NLForceBeamColumn3dBase::resizeMatrices(const size_t &nSections)
  {
    fs.resize(nSections);
    vs.resize(nSections);
    Ssr.resize(nSections);
    vscommit.resize(nSections);
  }

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLForceBeamColumn3dBase::NLForceBeamColumn3dBase(int tag,int classTag,int numSec)
  : BeamColumnWithSectionFDTrf3d(tag,classTag,numSec),
  maxIters(10), tol(1e-8), initialFlag(0), isTorsion(false),
  kv(NEBD,NEBD), Se(), kvcommit(NEBD,NEBD), Secommit(),
  fs(numSec), vs(numSec), Ssr(numSec),vscommit(numSec), p0()
  {}

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLForceBeamColumn3dBase::NLForceBeamColumn3dBase(int tag,int classTag,int numSec,const Material *m,const CrdTransf *coordTransf)
  : BeamColumnWithSectionFDTrf3d(tag,classTag,numSec,m,coordTransf),
  maxIters(10), tol(1e-8), initialFlag(0), isTorsion(false),
  kv(NEBD,NEBD), Se(), kvcommit(NEBD,NEBD), Secommit(),
  fs(numSec), vs(numSec), Ssr(numSec),vscommit(numSec), p0()
  {
    isTorsion= theSections.isTorsion();
  }

//! @brief Copy constructor.
XC::NLForceBeamColumn3dBase::NLForceBeamColumn3dBase(const NLForceBeamColumn3dBase &other)
  : BeamColumnWithSectionFDTrf3d(other), maxIters(other.maxIters), tol(other.tol), initialFlag(other.initialFlag), isTorsion(other.isTorsion),
    kv(other.kv), Se(other.Se), kvcommit(other.kvcommit), Secommit(other.Secommit),
    fs(other.fs), vs(other.vs), Ssr(other.Ssr),vscommit(other.vscommit), sp(other.sp), p0(), Ki(other.Ki)
  {}

//! @brief Assignment operator.
XC::NLForceBeamColumn3dBase &XC::NLForceBeamColumn3dBase::operator=(const NLForceBeamColumn3dBase &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; assignment operator must not be called."
              << std::endl;
    return *this;
  }

void XC::NLForceBeamColumn3dBase::setSection(const PrismaticBarCrossSection *sccModel)
  {
    BeamColumnWithSectionFDTrf3d::setSection(sccModel);
    isTorsion= theSections.isTorsion();
  }


int XC::NLForceBeamColumn3dBase::getNumDOF(void) const
  { return NEGD; }


const XC::Matrix &XC::NLForceBeamColumn3dBase::getTangentStiff(void) const
  {
    // Will remove once we clean up the corotational 3d transformation -- MHS
    theCoordTransf->update();
    static Matrix K;
    K= theCoordTransf->getGlobalStiffMatrix(kv, Se);
    if(isDead())
      K*=dead_srf;
    return K;
  }


const XC::Vector &XC::NLForceBeamColumn3dBase::getResistingForce(void) const
  {
    // Will remove once we clean up the corotational 3d transformation -- MHS
    theCoordTransf->update();
    Vector p0Vec= p0.getVector();
    static Vector retval;
    retval= theCoordTransf->getGlobalResistingForce(Se, p0Vec);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }


void XC::NLForceBeamColumn3dBase::initializeSectionHistoryVariables(void)
  {
    const size_t nSections= getNumSections();
    for(size_t i= 0;i<nSections; i++)
      {
        const int order = theSections[i]->getOrder();
        fs[i] = Matrix(order,order);
        vs[i] = Vector(order);
        Ssr[i] = Vector(order);
        vscommit[i]= Vector(order);
      }
  }

//! @brief Send members through the communicator argument.
int XC::NLForceBeamColumn3dBase::sendData(Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf3d::sendData(comm);
    res+= comm.sendDouble(tol,getDbTagData(),CommMetaData(13));
    res+= comm.sendInts(maxIters,initialFlag,getDbTagData(),CommMetaData(14));
    res+= comm.sendBool(isTorsion,getDbTagData(),CommMetaData(15));
    res+= comm.sendMatrix(kv,getDbTagData(),CommMetaData(16));
    res+= comm.sendVector(Se,getDbTagData(),CommMetaData(17));
    res+= comm.sendMatrix(kvcommit,getDbTagData(),CommMetaData(18));
    res+= comm.sendVector(Secommit,getDbTagData(),CommMetaData(19));
    res+= comm.sendMatrices(fs,getDbTagData(),CommMetaData(20));
    res+= comm.sendVectors(vs,getDbTagData(),CommMetaData(21));
    res+= comm.sendVectors(Ssr,getDbTagData(),CommMetaData(22));
    res+= comm.sendVectors(vscommit,getDbTagData(),CommMetaData(23));
    res+= comm.sendMatrix(sp,getDbTagData(),CommMetaData(24));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(25));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::NLForceBeamColumn3dBase::recvData(const Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf3d::recvData(comm);
    res+= comm.receiveDouble(tol,getDbTagData(),CommMetaData(13));
    res+= comm.receiveInts(maxIters,initialFlag,getDbTagData(),CommMetaData(14));
    res+= comm.receiveBool(isTorsion,getDbTagData(),CommMetaData(15));
    res+= comm.receiveMatrix(kv,getDbTagData(),CommMetaData(16));
    res+= comm.receiveVector(Se,getDbTagData(),CommMetaData(17));
    res+= comm.receiveMatrix(kvcommit,getDbTagData(),CommMetaData(18));
    res+= comm.receiveVector(Secommit,getDbTagData(),CommMetaData(19));
    res+= comm.receiveMatrices(fs,getDbTagData(),CommMetaData(20));
    res+= comm.receiveVectors(vs,getDbTagData(),CommMetaData(21));
    res+= comm.receiveVectors(Ssr,getDbTagData(),CommMetaData(22));
    res+= comm.receiveVectors(vscommit,getDbTagData(),CommMetaData(23));
    res+= comm.receiveMatrix(sp,getDbTagData(),CommMetaData(24));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(25));
    return res;
  }

//! @brief Return a python list with the values of the argument property
//! at element nodes.
//!
//! When the property requested its located at the integration point this
//! function is responsible of the extrapolation of values from
//! Gauss points to nodes.
//! @param code: identifier of the requested value.
//! @param silent: if true don't complaint about non-existen property.
boost::python::list XC::NLForceBeamColumn3dBase::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    if(code=="N")
      {
	retval.append(getN1());
	retval.append(getN2());
      }
    else if(code=="Vy" || code== "Qy")
      {
	retval.append(getVy1());
	retval.append(getVy2());
      }
    else if(code=="Vz" || code== "Qz")
      {
	retval.append(getVz1());
	retval.append(getVz2());
      }
    else if(code=="T")
      {
	retval.append(getT1());
	retval.append(getT2());
      }
    else if(code=="My")
      {
	retval.append(getMy1());
	retval.append(getMy2());
      }
    else if(code=="Mz")
      {
	retval.append(getMz1());
	retval.append(getMz2());
      }
    else
      retval= BeamColumnWithSectionFDTrf3d::getValuesAtNodes(code, silent); 
    return retval;
  }
