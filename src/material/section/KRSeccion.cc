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
//KRSeccion.cc

#include "KRSeccion.h"
#include "xc_utils/src/base/CmdStatus.h"

double XC::KRSeccion::value= 0.0;
double XC::KRSeccion::vas1= 0.0;
double XC::KRSeccion::vas2= 0.0;
double XC::KRSeccion::vas1as2= 0.0;

void XC::KRSeccion::libera(void)
  {
    if(R)
      {
        delete R;
        R= nullptr;
      }
    if(K)
      {
        delete K;
        K= nullptr;
      }
  }

void XC::KRSeccion::alloc(const size_t &dim)
  {
    libera();
    R= new Vector(rData,dim);
    K= new Matrix(kData,dim,dim);
  }

void XC::KRSeccion::copia(const KRSeccion &otra)
  {
    libera();
    rData[0]= otra.rData[0]; rData[1]= otra.rData[1];
    rData[2]= otra.rData[2]; rData[3]= otra.rData[3];

    kData[0]= otra.kData[0]; kData[1]= otra.kData[1]; kData[2]= otra.kData[2]; kData[3]= otra.kData[3];
    kData[4]= otra.kData[4]; kData[5]= otra.kData[5]; kData[6]= otra.kData[6]; kData[7]= otra.kData[7];
    kData[8]= otra.kData[8]; kData[9]= otra.kData[9]; kData[10]= otra.kData[10]; kData[11]= otra.kData[11];
    kData[12]= otra.kData[12]; kData[13]= otra.kData[13]; kData[14]= otra.kData[14]; kData[15]= otra.kData[15];

    alloc(otra.dim());
  }

//! @brief Lee un objeto XC::KRSeccion desde archivo
bool XC::KRSeccion::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(KRSeccion) Procesando comando: " << cmd << std::endl;
    if(cmd == "K") //Rigidez.
      {
        if(K)
          K->LeeCmd(status);
        else
          {
            status.GetBloque();
            std::cerr << "FiberSection3d::procesa_comando; la sección no tiene matriz de rigidez." 
                      << std::endl;
          }
        return true;
      }
    else if(cmd == "R") //Resultante de tensiones.
      {
        if(R)
          R->LeeCmd(status);
        else
          {
            status.GetBloque();
            std::cerr << "FiberSection3d::procesa_comando; la sección no tiene resultante de tensiones." 
                      << std::endl;
          }
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

void XC::KRSeccion::zero(void)
  {
    rData[0]= 0.0; rData[1]= 0.0;
    rData[2]= 0.0; rData[3]= 0.0;

    kData[0]= 0.0; kData[1]= 0.0; kData[2]= 0.0; kData[3]= 0.0;
    kData[4]= 0.0; kData[5]= 0.0; kData[6]= 0.0; kData[7]= 0.0;
    kData[8]= 0.0; kData[9]= 0.0; kData[10]= 0.0; kData[11]= 0.0;
    kData[12]= 0.0; kData[13]= 0.0; kData[14]= 0.0; kData[15]= 0.0;
  }

//! @brief Constructor.
XC::KRSeccion::KRSeccion(const size_t &dim)
  : R(nullptr), K(nullptr)
  {
    alloc(dim);
    zero();
  }

//! @brief Constructor de copia.
XC::KRSeccion::KRSeccion(const KRSeccion &otra)
  : R(nullptr), K(nullptr)
  {
    copia(otra);
  }

//! @brief Operador asignación.
XC::KRSeccion &XC::KRSeccion::operator=(const KRSeccion &otro)
  {
    copia(otro);
    return *this;
  }

//! @brief Destructor.
XC::KRSeccion::~KRSeccion(void)
  {
    libera();
  }
