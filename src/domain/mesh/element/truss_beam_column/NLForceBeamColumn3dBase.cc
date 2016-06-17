//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! @brief Constructor de copia.
XC::NLForceBeamColumn3dBase::NLForceBeamColumn3dBase(const NLForceBeamColumn3dBase &otro)
  : BeamColumnWithSectionFDTrf3d(otro), rho(otro.rho), maxIters(otro.maxIters), tol(otro.tol), initialFlag(otro.initialFlag), isTorsion(otro.isTorsion),
    kv(otro.kv), Se(otro.Se), kvcommit(otro.kvcommit), Secommit(otro.Secommit),
    fs(otro.fs), vs(otro.vs), Ssr(otro.Ssr),vscommit(otro.vscommit), sp(otro.sp), p0(), Ki(otro.Ki)
  {}

//! @brief Operador asignación.
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
