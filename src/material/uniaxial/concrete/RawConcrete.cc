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
//RawConcrete.cpp

#include <material/uniaxial/concrete/RawConcrete.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/Information.h>
#include <cmath>
#include <cfloat>


//! @brief Constructor.
XC::RawConcrete::RawConcrete(int tag, int classTag, double FPC, double EPSC0, double EPSCU)
  :UniaxialMaterial(tag, classTag), fpc(FPC), epsc0(EPSC0), epscu(EPSCU) {}

//! @brief Constructor
XC::RawConcrete::RawConcrete(int tag, int classTag)
  :UniaxialMaterial(tag, classTag), fpc(0.0), epsc0(0.0), epscu(0.0) {}

void XC::RawConcrete::setFpc(const double &d)
  {
    fpc= d;
    if(fpc > 0.0)
      {
        fpc= -fpc;
        std::clog << "!Ojo!, los parámetros del hormigón deben ser negativos." << std::endl;
      }
    setup_parameters();
  }

double XC::RawConcrete::getFpc(void) const
  { return fpc; }

void XC::RawConcrete::setEpsc0(const double &d)
  {
    epsc0= d;
    if(epsc0 > 0.0)
      {
        epsc0= -epsc0;
        std::clog << "!Ojo!, los parámetros del hormigón deben ser negativos." << std::endl;
      }
    setup_parameters();
  }

double XC::RawConcrete::getEpsc0(void) const
  { return epsc0; }

void XC::RawConcrete::setEpscu(const double &d)
  {
    epscu= d;
    if(epscu > 0.0)
      {
        epscu= -epscu;
        std::clog << "!Ojo!, los parámetros del hormigón deben ser negativos." << std::endl;
      }
  }

double XC::RawConcrete::getEpscu(void) const
  { return epscu; }

