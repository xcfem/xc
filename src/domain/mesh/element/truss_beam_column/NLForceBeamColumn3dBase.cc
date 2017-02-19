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
#include <material/section/SeccionBarraPrismatica.h>

#include "utility/actor/actor/MatrixCommMetaData.h"

const size_t XC::NLForceBeamColumn3dBase::NDM= 3; //!< dimension of the problem (3d)
const int XC::NLForceBeamColumn3dBase::NND= 6; //!< number of nodal dof's
const size_t XC::NLForceBeamColumn3dBase::NEGD= 12; //!< number of element global dof's
const size_t XC::NLForceBeamColumn3dBase::NEBD= 6; //!< number of element dof's in the basic system
const double XC::NLForceBeamColumn3dBase::DefaultLoverGJ= 1.0e-10;
XC::Matrix XC::NLForceBeamColumn3dBase::theMatrix(12,12);
XC::Vector XC::NLForceBeamColumn3dBase::theVector(12);
double XC::NLForceBeamColumn3dBase::workArea[200];

//! @brief alocate section flexibility matrices and section deformation vectors
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
  rho(0.0), maxIters(10), tol(1e-8), initialFlag(0), isTorsion(false),
  kv(NEBD,NEBD), Se(), kvcommit(NEBD,NEBD), Secommit(),
  fs(numSec), vs(numSec), Ssr(numSec),vscommit(numSec), p0()
  {}

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLForceBeamColumn3dBase::NLForceBeamColumn3dBase(int tag,int classTag,int numSec,const Material *m,const CrdTransf *coordTransf)
  : BeamColumnWithSectionFDTrf3d(tag,classTag,numSec,m,coordTransf),
  rho(0.0), maxIters(10), tol(1e-8), initialFlag(0), isTorsion(false),
  kv(NEBD,NEBD), Se(), kvcommit(NEBD,NEBD), Secommit(),
  fs(numSec), vs(numSec), Ssr(numSec),vscommit(numSec), p0()
  {
    isTorsion= theSections.isTorsion();
  }

//! @brief Copy constructor.
XC::NLForceBeamColumn3dBase::NLForceBeamColumn3dBase(const NLForceBeamColumn3dBase &otro)
  : BeamColumnWithSectionFDTrf3d(otro), rho(otro.rho), maxIters(otro.maxIters), tol(otro.tol), initialFlag(otro.initialFlag), isTorsion(otro.isTorsion),
    kv(otro.kv), Se(otro.Se), kvcommit(otro.kvcommit), Secommit(otro.Secommit),
    fs(otro.fs), vs(otro.vs), Ssr(otro.Ssr),vscommit(otro.vscommit), sp(otro.sp), p0(), Ki(otro.Ki)
  {}

//! @brief Assignment operator.
XC::NLForceBeamColumn3dBase &XC::NLForceBeamColumn3dBase::operator=(const NLForceBeamColumn3dBase &)
  {
    std::cerr << "NLForceBeamColumn3dBase: No se debe llamar al operador de asignación."
              << std::endl;
    return *this;
  }

void XC::NLForceBeamColumn3dBase::setSection(const SeccionBarraPrismatica *sccModel)
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

//! @brief Send members through the channel being passed as parameter.
int XC::NLForceBeamColumn3dBase::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf3d::sendData(cp);
    res+= cp.sendDoubles(rho,tol,getDbTagData(),CommMetaData(12));
    res+= cp.sendInts(maxIters,initialFlag,getDbTagData(),CommMetaData(13));
    res+= cp.sendBool(isTorsion,getDbTagData(),CommMetaData(14));
    res+= cp.sendMatrix(kv,getDbTagData(),CommMetaData(15));
    res+= cp.sendVector(Se,getDbTagData(),CommMetaData(16));
    res+= cp.sendMatrix(kvcommit,getDbTagData(),CommMetaData(17));
    res+= cp.sendVector(Secommit,getDbTagData(),CommMetaData(18));
    res+= cp.sendMatrices(fs,getDbTagData(),CommMetaData(19));;
    res+= cp.sendVectors(vs,getDbTagData(),CommMetaData(20));
    res+= cp.sendVectors(Ssr,getDbTagData(),CommMetaData(21));
    res+= cp.sendVectors(vscommit,getDbTagData(),CommMetaData(22));
    res+= cp.sendMatrix(sp,getDbTagData(),CommMetaData(23));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(24));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::NLForceBeamColumn3dBase::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf3d::recvData(cp);
    res+= cp.receiveDoubles(rho,tol,getDbTagData(),CommMetaData(12));
    res+= cp.receiveInts(maxIters,initialFlag,getDbTagData(),CommMetaData(13));
    res+= cp.receiveBool(isTorsion,getDbTagData(),CommMetaData(14));
    res+= cp.receiveMatrix(kv,getDbTagData(),CommMetaData(15));
    res+= cp.receiveVector(Se,getDbTagData(),CommMetaData(16));
    res+= cp.receiveMatrix(kvcommit,getDbTagData(),CommMetaData(17));
    res+= cp.receiveVector(Secommit,getDbTagData(),CommMetaData(18));
    res+= cp.receiveMatrices(fs,getDbTagData(),CommMetaData(19));;
    res+= cp.receiveVectors(vs,getDbTagData(),CommMetaData(20));
    res+= cp.receiveVectors(Ssr,getDbTagData(),CommMetaData(21));
    res+= cp.receiveVectors(vscommit,getDbTagData(),CommMetaData(22));
    res+= cp.receiveMatrix(sp,getDbTagData(),CommMetaData(23));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(24));
    return res;
  }
