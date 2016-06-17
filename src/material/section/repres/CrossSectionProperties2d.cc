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
//CrossSectionProperties2d.cc

#include <material/section/repres/CrossSectionProperties2d.h>
#include "xc_basic/src/util/inercia.h"
#include "xc_utils/src/geom/sis_ref/EjesPrincInercia2d.h"
#include "domain/mesh/element/Information.h"

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "utility/matrix/ID.h"
#include "domain/mesh/element/Information.h"
#include "domain/component/Parameter.h"
#include "material/section/SectionForceDeformation.h"

XC::Matrix XC::CrossSectionProperties2d::ks2(2,2);
XC::Matrix XC::CrossSectionProperties2d::ks3(3,3);

bool XC::CrossSectionProperties2d::check_values(void)
  {
    bool retval= true;
    if(e <= 0.0)
      {
        std::clog << "CrossSectionProperties2d::check_values -- Input E <= 0.0 ... setting E to 1.0\n";
        e= 1.0;
        retval= false;
      }
    if(g <= 0.0)
      {
        std::clog << "CrossSectionProperties2d::check_values -- Input G <= 0.0 ... setting G to 1.0\n";
        g= 1.0;
        retval= false;
      }
    if(a <= 0.0)
      {
        std::clog << "CrossSectionProperties2d::check_values -- Input A <= 0.0 ... setting A to 1.0\n";
        a= 1.0;
        retval= false;
      }
    if(i <= 0.0)
      {
        std::clog << "CrossSectionProperties2d::check_values -- Input I <= 0.0 ... setting I to 1.0\n";
        i= 1.0;
        retval= false;
      }
    if(alpha <= 0.0)
      {
        std::clog << "CrossSectionProperties2d::check_values -- Input alpha <= 0.0 ... setting alpha to 1.0\n";
        alpha= 1.0;
        retval= false;
      }
    return retval;
  }

//! @brief Constructor.
XC::CrossSectionProperties2d::CrossSectionProperties2d(void)
  :EntCmd(), MovableObject(0), e(0), g(0.0) , a(0), i(0), alpha(0) {}

//! @brief Constructor.
XC::CrossSectionProperties2d::CrossSectionProperties2d(double EA_in, double EI_in)
  : EntCmd(), MovableObject(0), e(1), g(0.0), a(EA_in), i(EI_in), alpha(0)
  { check_values(); }

//! @brief Constructor (seccciones 2d).
XC::CrossSectionProperties2d::CrossSectionProperties2d(double E_in, double A_in, double I_in, double G_in, double a)
  : EntCmd(), MovableObject(0), e(E_in), g(G_in), a(A_in), i(I_in), alpha(a)
  { check_values(); }

//! @brief Returns the angle between the principal axes and the local system.
double XC::CrossSectionProperties2d::getTheta(void) const
  { return 0.0; }

//! @brief Returns the major principal axis of inertia.
double XC::CrossSectionProperties2d::getI1(void) const
  { return I(); }

//! @brief Returns the minor principal axis of inertia.
double XC::CrossSectionProperties2d::getI2(void) const
  { return 0.0; }

//! @brief Returns the principal axis of inertia.
EjesPrincInercia2d XC::CrossSectionProperties2d::getEjesInercia(void) const
  {
    const Pos2d cdg(0,0);
    return EjesPrincInercia2d(cdg,0.0,I(),0.0);
  }
//! @brief Returns principal axis I (strong).
Vector2d XC::CrossSectionProperties2d::getVDirEje1(void) const
  { return getEjesInercia().getVDirEje1(); }
//! @brief Returns principal axis I (strong).
Vector2d XC::CrossSectionProperties2d::getVDirEjeFuerte(void) const
  { return getEjesInercia().getVDirEje1(); }
//! @brief Returns principal axis II (weak).
Vector2d XC::CrossSectionProperties2d::getVDirEje2(void) const
  { return getEjesInercia().getVDirEje2(); }
//! @brief Returns principal axis II (weak).
Vector2d XC::CrossSectionProperties2d::getVDirEjeDebil(void) const
  { return getEjesInercia().getVDirEje2(); }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionTangent2x2(void) const
  {
    ks2(0,0) = EA(); //Rigidez frente al esfuerzo axil.
    ks2(1,1) = EI(); //Rigidez frente al giro en torno a z.
    return ks2;
  }

//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialTangent2x2(void) const
  { return getSectionTangent2x2(); }

//! @brief Devuelve la matriz de flexibilidad.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionFlexibility2x2(void) const
  {
    ks2(0,0) = 1.0/(EA());
    ks2(1,1) = 1.0/(EI());
    return ks2;
  }

//! @brief Devuelve la matriz de flexibilidad noval.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialFlexibility2x2(void) const
  { return getSectionFlexibility2x2(); }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionTangent3x3(void) const
  {
    ks3(0,0)= EA(); //Rigidez frente al esfuerzo axil.
    ks3(1,1)= EI(); //Rigidez frente al giro en torno a z.
    ks3(2,2)= GAAlpha(); //Rigidez frente a la deformación por cortante.
    return ks3;
  }

//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialTangent3x3(void) const
  { return getSectionTangent3x3(); }

//! @brief Devuelve la matriz de flexibilidad.
const XC::Matrix &XC::CrossSectionProperties2d::getSectionFlexibility3x3(void) const
  {
    ks3(0,0)= 1.0/(EA());
    ks3(1,1)= 1.0/(EI());
    ks3(2,2)= 1.0/GAAlpha(); //Rigidez frente a la deformación por cortante.
    return ks3;
  }

//! @brief Devuelve la matriz de flexibilidad noval.
const XC::Matrix &XC::CrossSectionProperties2d::getInitialFlexibility3x3(void) const
  { return getSectionFlexibility3x3(); }


int XC::CrossSectionProperties2d::setParameter(const std::vector<std::string> &argv,Parameter &param,SectionForceDeformation *scc)
  {
    if(argv.size() < 1)
      return -1;

    if(argv[0] == "E")
      {
        param.setValue(E());
        return param.addObject(1,scc);
      }
    if(argv[0] == "A")
      {
        param.setValue(A());
        return param.addObject(2,scc);
      }
    if(argv[0] == "I")
      {
        param.setValue(I());
        return param.addObject(3,scc);
      }
    if(argv[0] == "G")
      {
        param.setValue(G());
        return param.addObject(4,scc);
      }
    if(argv[0] == "alpha")
      {
        param.setValue(Alpha());
        return param.addObject(5,scc);
      }
    return -1;
  }

int XC::CrossSectionProperties2d::updateParameter(int paramID, Information &info)
  {
    if(paramID == 1)
      e= info.theDouble;
    if(paramID == 2)
      a= info.theDouble;
    if(paramID == 3)
      i= info.theDouble;
    if(paramID == 4)
      g= info.theDouble;
    if(paramID == 5)
      alpha = info.theDouble;
    return 0;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::CrossSectionProperties2d::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::CrossSectionProperties2d::sendData(CommParameters &cp)
  { return cp.sendDoubles(e,g,a,i,alpha,getDbTagData(),CommMetaData(0)); }

//! @brief Receives members through the channel being passed as parameter.
int XC::CrossSectionProperties2d::recvData(const CommParameters &cp)
  { return cp.receiveDoubles(e,g,a,i,alpha,getDbTagData(),CommMetaData(0)); }

//! @brief Sends object through the channel being passed as parameter.
int XC::CrossSectionProperties2d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(1);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::CrossSectionProperties2d::recvSelf(const CommParameters &cp)
  {
    inicComm(1);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


const XC::Matrix& XC::CrossSectionProperties2d::getInitialTangentSensitivity3x3(int gradIndex)
  {
    ks3.Zero();
    return ks3;
  }

void XC::CrossSectionProperties2d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {}
    else
      {
        s << "CrossSectionProperties2d, E: " << E() << std::endl;
        s << "\t G: " << G() << std::endl;
        s << "\t A: " << A() << std::endl;
        s << "\tI: " << I() << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os,const XC::CrossSectionProperties2d &c)
  {
    c.Print(os);
    return os;
  }
