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
//NLForceBeamColumn2dBase.cpp

#include <domain/mesh/element/truss_beam_column/NLForceBeamColumn2dBase.h>
#include <material/section/PrismaticBarCrossSection.h>

#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::NLForceBeamColumn2dBase::theMatrix(6,6);
XC::Vector XC::NLForceBeamColumn2dBase::theVector(6);
double XC::NLForceBeamColumn2dBase::workArea[100];

//! @brief Allocate section flexibility matrices and section deformation vectors
void XC::NLForceBeamColumn2dBase::resizeMatrices(const size_t &nSections)
  {
    fs.resize(nSections);
    vs.resize(nSections);
    Ssr.resize(nSections);
    vscommit.resize(nSections);
  }

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLForceBeamColumn2dBase::NLForceBeamColumn2dBase(int tag,int classTag,int numSec)
  : BeamColumnWithSectionFDTrf2d(tag,classTag,numSec),
    maxIters(10), tol(1e-8), initialFlag(0),
    kv(NEBD,NEBD), Se(NEBD), kvcommit(NEBD,NEBD), Secommit(NEBD),
    fs(numSec), vs(numSec), Ssr(numSec), vscommit(numSec), p0()
  {}

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLForceBeamColumn2dBase::NLForceBeamColumn2dBase(int tag,int classTag,int numSec,const Material *m,const CrdTransf *coordTransf)
  : BeamColumnWithSectionFDTrf2d(tag,classTag,numSec,m,coordTransf),
    maxIters(10), tol(1e-8), initialFlag(0),
    kv(NEBD,NEBD), Se(NEBD), kvcommit(NEBD,NEBD), Secommit(NEBD),
    fs(numSec), vs(numSec), Ssr(numSec), vscommit(numSec), p0()
  {}

//! @brief Copy constructor.
XC::NLForceBeamColumn2dBase::NLForceBeamColumn2dBase(const NLForceBeamColumn2dBase &other)
  : BeamColumnWithSectionFDTrf2d(other), maxIters(other.maxIters), tol(other.tol), initialFlag(other.initialFlag),
    kv(other.kv), Se(other.Se), kvcommit(other.kvcommit), Secommit(other.Secommit),
    fs(other.fs), vs(other.vs), Ssr(other.Ssr), vscommit(other.vscommit), sp(other.sp), p0(other.p0), Ki(other.Ki)
  {}

//! @brief Assignment operator.
XC::NLForceBeamColumn2dBase &XC::NLForceBeamColumn2dBase::operator=(const NLForceBeamColumn2dBase &)
  {
    std::cerr << "NLForceBeamColumn2dBase: No se debe llamar al operador de asignación."
              << std::endl;
    return *this;
  }

// ~NLForceBeamColumn2dBase():
//         destructor
//      delete must be invoked on any objects created by the object
XC::NLForceBeamColumn2dBase::~NLForceBeamColumn2dBase(void)
  {}


int XC::NLForceBeamColumn2dBase::getNumDOF(void) const
  { return NEGD; }


const XC::Matrix &XC::NLForceBeamColumn2dBase::getTangentStiff(void) const
  {
    // Will remove once we clean up the corotational 2d transformation -- MHS
    theCoordTransf->update();
    static Matrix K;
    K= theCoordTransf->getGlobalStiffMatrix(kv, Se);
    if(isDead())
      K*=dead_srf;
    return K;
  }


const XC::Vector &XC::NLForceBeamColumn2dBase::getResistingForce(void) const
  {
    // Will remove once we clean up the corotational 2d transformation -- MHS
    theCoordTransf->update();
    Vector p0Vec= p0.getVector();
    static Vector retval;
    retval= theCoordTransf->getGlobalResistingForce(Se, p0Vec);
    if(isDead())
      retval*=dead_srf;
    return retval;
  }


void XC::NLForceBeamColumn2dBase::initializeSectionHistoryVariables(void)
  {
    const size_t nSections= getNumSections();
    for(size_t i= 0;i<nSections; i++)
      {
        int order = theSections[i]->getOrder();

        fs[i] = Matrix(order,order);
        vs[i] = Vector(order);
        Ssr[i] = Vector(order);

        vscommit[i] = Vector(order);
      }
  }

//! @brief Send members through the communicator argument.
int XC::NLForceBeamColumn2dBase::sendData(Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf2d::sendData(comm);
    res+= comm.sendDouble(tol,getDbTagData(),CommMetaData(13));
    res+= comm.sendInts(maxIters,initialFlag,getDbTagData(),CommMetaData(14));
    res+= comm.sendMatrix(kv,getDbTagData(),CommMetaData(15));
    res+= comm.sendVector(Se,getDbTagData(),CommMetaData(16));
    res+= comm.sendMatrix(kvcommit,getDbTagData(),CommMetaData(17));
    res+= comm.sendVector(Secommit,getDbTagData(),CommMetaData(18));
    res+= comm.sendMatrices(fs,getDbTagData(),CommMetaData(19));;
    res+= comm.sendVectors(vs,getDbTagData(),CommMetaData(20));
    res+= comm.sendVectors(Ssr,getDbTagData(),CommMetaData(21));
    res+= comm.sendVectors(vscommit,getDbTagData(),CommMetaData(22));
    res+= comm.sendMatrix(sp,getDbTagData(),CommMetaData(23));
    res+= p0.sendData(comm,getDbTagData(),CommMetaData(24));
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::NLForceBeamColumn2dBase::recvData(const Communicator &comm)
  {
    int res= BeamColumnWithSectionFDTrf2d::recvData(comm);
    res+= comm.receiveDouble(tol,getDbTagData(),CommMetaData(13));
    res+= comm.receiveInts(maxIters,initialFlag,getDbTagData(),CommMetaData(14));
    res+= comm.receiveMatrix(kv,getDbTagData(),CommMetaData(15));
    res+= comm.receiveVector(Se,getDbTagData(),CommMetaData(16));
    res+= comm.receiveMatrix(kvcommit,getDbTagData(),CommMetaData(17));
    res+= comm.receiveVector(Secommit,getDbTagData(),CommMetaData(18));
    res+= comm.receiveMatrices(fs,getDbTagData(),CommMetaData(19));;
    res+= comm.receiveVectors(vs,getDbTagData(),CommMetaData(20));
    res+= comm.receiveVectors(Ssr,getDbTagData(),CommMetaData(21));
    res+= comm.receiveVectors(vscommit,getDbTagData(),CommMetaData(22));
    res+= comm.receiveMatrix(sp,getDbTagData(),CommMetaData(23));
    res+= p0.receiveData(comm,getDbTagData(),CommMetaData(24));
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
boost::python::list XC::NLForceBeamColumn2dBase::getValuesAtNodes(const std::string &code, bool silent) const
  {
    boost::python::list retval;
    if(code=="N")
      {
	retval.append(getN1());
	retval.append(getN2());
      }
    else if((code=="M") || (code=="Mz"))
      {
	retval.append(getM1());
	retval.append(getM2());
      }
    else if((code=="V") || (code=="Vy") || (code=="Q") || (code=="Qy"))
      {
	retval.append(getV1());
	retval.append(getV2());
      }
    else
      retval= BeamColumnWithSectionFDTrf2d::getValuesAtNodes(code, silent); 
    return retval;
  }
