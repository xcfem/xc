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
//NLForceBeamColumn2dBase.cpp

#include <domain/mesh/element/truss_beam_column/NLForceBeamColumn2dBase.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/coordTransformation/CrdTransf2d.h>
#include <material/section/SeccionBarraPrismatica.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MatrixCommMetaData.h"

XC::Matrix XC::NLForceBeamColumn2dBase::theMatrix(6,6);
XC::Vector XC::NLForceBeamColumn2dBase::theVector(6);
double XC::NLForceBeamColumn2dBase::workArea[100];

//! @brief alocate section flexibility matrices and section deformation vectors
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
    rho(0), maxIters(10), tol(1e-8), initialFlag(0),
    kv(NEBD,NEBD), Se(NEBD), kvcommit(NEBD,NEBD), Secommit(NEBD),
    fs(numSec), vs(numSec), Ssr(numSec), vscommit(numSec), p0()
  {}

// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::NLForceBeamColumn2dBase::NLForceBeamColumn2dBase(int tag,int classTag,int numSec,const Material *m,const CrdTransf *coordTransf)
  : BeamColumnWithSectionFDTrf2d(tag,classTag,numSec,m,coordTransf),
    rho(0), maxIters(10), tol(1e-8), initialFlag(0),
    kv(NEBD,NEBD), Se(NEBD), kvcommit(NEBD,NEBD), Secommit(NEBD),
    fs(numSec), vs(numSec), Ssr(numSec), vscommit(numSec), p0()
  {}

//! @brief Constructor de copia.
XC::NLForceBeamColumn2dBase::NLForceBeamColumn2dBase(const NLForceBeamColumn2dBase &otro)
  : BeamColumnWithSectionFDTrf2d(otro), rho(otro.rho), maxIters(otro.maxIters), tol(otro.tol), initialFlag(otro.initialFlag),
    kv(otro.kv), Se(otro.Se), kvcommit(otro.kvcommit), Secommit(otro.Secommit),
    fs(otro.fs), vs(otro.vs), Ssr(otro.Ssr), vscommit(otro.vscommit), sp(otro.sp), p0(otro.p0), Ki(otro.Ki)
  {}

//! @brief Operador asignación.
XC::NLForceBeamColumn2dBase &XC::NLForceBeamColumn2dBase::operator=(const NLForceBeamColumn2dBase &)
  {
    std::cerr << "NLForceBeamColumn2dBase: No se debe llamar al operador de asignación."
              << std::endl;
    return *this;
  }

//! @brief Lee el objeto desde archivo
bool XC::NLForceBeamColumn2dBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NLForceBeamColumn2dBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "rho")
      {
        rho= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "tol")
      {
        tol= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "maxIters")
      {
        maxIters= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "kv")
      {
        kv.LeeCmd(status);
        return true;
      }
    else if(cmd == "Ki")
      {
        Ki.LeeCmd(status);
        return true;
      }
    else
      return BeamColumnWithSectionFDTrf2d::procesa_comando(status);
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

//! @brief Envía los miembros por el canal que se pasa como parámetro.
int XC::NLForceBeamColumn2dBase::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf2d::sendData(cp);
    res+= cp.sendDoubles(rho,tol,getDbTagData(),CommMetaData(12));
    res+= cp.sendInts(maxIters,initialFlag,getDbTagData(),CommMetaData(13));
    res+= cp.sendMatrix(kv,getDbTagData(),CommMetaData(14));
    res+= cp.sendVector(Se,getDbTagData(),CommMetaData(15));
    res+= cp.sendMatrix(kvcommit,getDbTagData(),CommMetaData(16));
    res+= cp.sendVector(Secommit,getDbTagData(),CommMetaData(17));
    res+= cp.sendMatrices(fs,getDbTagData(),CommMetaData(18));;
    res+= cp.sendVectors(vs,getDbTagData(),CommMetaData(19));
    res+= cp.sendVectors(Ssr,getDbTagData(),CommMetaData(20));
    res+= cp.sendVectors(vscommit,getDbTagData(),CommMetaData(21));
    res+= cp.sendMatrix(sp,getDbTagData(),CommMetaData(22));
    res+= p0.sendData(cp,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Recibe los miembros por el canal que se pasa como parámetro.
int XC::NLForceBeamColumn2dBase::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFDTrf2d::recvData(cp);
    res+= cp.receiveDoubles(rho,tol,getDbTagData(),CommMetaData(12));
    res+= cp.receiveInts(maxIters,initialFlag,getDbTagData(),CommMetaData(13));
    res+= cp.receiveMatrix(kv,getDbTagData(),CommMetaData(14));
    res+= cp.receiveVector(Se,getDbTagData(),CommMetaData(15));
    res+= cp.receiveMatrix(kvcommit,getDbTagData(),CommMetaData(16));
    res+= cp.receiveVector(Secommit,getDbTagData(),CommMetaData(17));
    res+= cp.receiveMatrices(fs,getDbTagData(),CommMetaData(18));;
    res+= cp.receiveVectors(vs,getDbTagData(),CommMetaData(19));
    res+= cp.receiveVectors(Ssr,getDbTagData(),CommMetaData(20));
    res+= cp.receiveVectors(vscommit,getDbTagData(),CommMetaData(21));
    res+= cp.receiveMatrix(sp,getDbTagData(),CommMetaData(22));
    res+= p0.receiveData(cp,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::NLForceBeamColumn2dBase::GetProp(const std::string &cod) const
  {
    if(cod=="rho")
      {
        return any_const_ptr(rho);
      }
    else if(cod=="tol")
      {
        return any_const_ptr(tol);
      }
    else if(cod=="maxIters")
      {
        return any_const_ptr(maxIters);
      }
    else
      return BeamColumnWithSectionFDTrf2d::GetProp(cod);
  }
