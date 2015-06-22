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
//ConvergenceTestTol.cc


#include <solution/analysis/convergenceTest/ConvergenceTestTol.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::ConvergenceTestTol::ConvergenceTestTol(EntCmd *owr,int clasTag)	    	
  : ConvergenceTest(owr,clasTag), tol(0.0) {}


//! @brief Constructor.
XC::ConvergenceTestTol::ConvergenceTestTol(EntCmd *owr,int clasTag,double theTol, int maxIter, int printIt, int normType,int sz_norms)
  : ConvergenceTest(owr,clasTag,maxIter,printIt,normType,sz_norms), tol(theTol) {}

//! @brief Lee un objeto ConvergenceTest desde archivo
bool XC::ConvergenceTestTol::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ConvergenceTest) Procesando comando: " << cmd << std::endl;
    if(cmd=="tol")
      {
        setTolerance(interpretaDouble(status.GetString()));
        return true;
      }
    else
      return ConvergenceTest::procesa_comando(status);
  }

void XC::ConvergenceTestTol::setTolerance(double newTol)
  { tol = newTol; }

double XC::ConvergenceTestTol::getTolerance(void) const
  { return tol; }


//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::ConvergenceTestTol::GetProp(const std::string &cod) const
  {
    if(cod=="tol")
      return any_const_ptr(tol);
    else
      return ConvergenceTest::GetProp(cod);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ConvergenceTestTol::sendData(CommParameters &cp)
  {
    int res= ConvergenceTest::sendData(cp);
    res+= cp.sendDouble(tol,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ConvergenceTestTol::recvData(const CommParameters &cp)
  {
    int res= ConvergenceTest::recvData(cp);
    res+= cp.receiveDouble(tol,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::ConvergenceTestTol::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::ConvergenceTestTol::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
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
