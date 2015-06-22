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
//RgQuadCell.cpp

#include <material/section/repres/geom_section/region/RgQuadCell.h> 
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

void XC::RgQuadCell::libera(void) const
  {
    if(rejilla) delete rejilla;
    rejilla= nullptr;
  }

const Rejilla2d &XC::RgQuadCell::alloc(const Rejilla2d &ptos) const
  {
    libera();
    rejilla= new Rejilla2d(ptos);
    return *rejilla;
  }

XC::RgQuadCell::RgQuadCell(Material *mat)
  : RegionSecc(mat), rejilla(nullptr), nDivIJ(1), nDivJK(1) {}

XC::RgQuadCell::RgQuadCell(Material *mat, int numSubdivIJ, int numSubdivJK)
  : RegionSecc(mat), rejilla(nullptr), nDivIJ(numSubdivIJ), nDivJK(numSubdivJK) {}

//! @brief Constructor de copia.
XC::RgQuadCell::RgQuadCell(const RgQuadCell &otro)
  : RegionSecc(otro), rejilla(nullptr), nDivIJ(otro.nDivIJ), nDivJK(otro.nDivJK) 
  {
    if(otro.rejilla)
      alloc(*otro.rejilla);
  }

//! @brief Operador asignación.
XC::RgQuadCell &XC::RgQuadCell::operator=(const RgQuadCell &otro)
  {
    RegionSecc::operator=(otro);
    if(otro.rejilla)
      alloc(*otro.rejilla);
    nDivIJ= otro.nDivIJ;
    nDivJK= otro.nDivJK;
    return *this;
  }

//! @brief Lee un objeto RgQuadCell desde archivo
bool XC::RgQuadCell::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(RgQuadCell) Procesando comando: " << cmd << std::endl;
    if(cmd == "nDivIJ") //Número de divisiones en lados IJ y KL.
      {
        nDivIJ= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "nDivJK") //Número de divisiones en lados JK y LI.
      {
        nDivJK= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "nDivCirc") //Número de divisiones en la dirección tangencial.
      {
        nDivIJ= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "nDivRad") //Número de divisiones en la dirección radial.
      {
        nDivJK= interpretaInt(status.GetString());
        return true;
      }
    else if(cmd == "creaRejilla")
      {
        status.GetBloque(); //Ignoramos argumentos.
        Malla();
        return true;
      }
    else if(cmd == "rejilla")
      {
        if(rejilla)
          rejilla->LeeCmd(status);
        else
          {
	    std::cerr << "No se ha creado la rejilla para esta región." << std::endl;        
            status.GetBloque(); //Ignoramos argumentos.
          }
        return true;
      }
    else
      return RegionSecc::procesa_comando(status);
  }

XC::RgQuadCell::~RgQuadCell(void)
  { libera(); }

//! @brief Asigna el número de divisiones en cada dirección.
void XC::RgQuadCell::setDiscretization(int numSubdivIJ, int numSubdivJK)
  {
    nDivIJ = numSubdivIJ;
    nDivJK = numSubdivJK;
  }

//! @brief Devuelve el número de divisiones en cada dirección.
void XC::RgQuadCell::getDiscretization(int &numSubdivIJ, int &numSubdivJK) const
  {
     numSubdivIJ = nDivIJ;
     numSubdivJK = nDivJK;
  }

//! @brief Devuelve el número de celdas.
int XC::RgQuadCell::getNumCells (void) const
  { return nDivIJ * nDivJK; }


//! @brief Devuelve el centro de gravedad de la rejilla.
XC::Vector XC::RgQuadCell::getCdgRejilla(void) const
  {
    Vector retval(2);
    assert(rejilla);
    Pos2d c= rejilla->Cdg();
    retval[0]= c.x();
    retval[1]= c.y();
    return retval;
  }

//! @brief Devuelve las coordenadas del punto i,j.
XC::Vector XC::RgQuadCell::getVertCoords(const size_t &i,const size_t &j) const
  {
    Vector retval(2);
    assert(rejilla);
    retval[0]= (*rejilla)(i,j).x();
    retval[1]= (*rejilla)(i,j).y();
    return retval;
  }

//! @brief Devuelve las coordenadas de los vértices de la celda i,j.
//                                                                             i+1,j +---+ i+1,j+1
//                                                                                   |   |
//                                                                                   |   |
//                                                                                   |   |
//                                                                               i,j +---+ i,j+1
XC::Matrix XC::RgQuadCell::getCellVertCoords(const size_t &i,const size_t &j) const
  {
    Matrix retval(4,2);
    assert(rejilla);
    retval(0,0)= (*rejilla)(i,j).x();
    retval(0,1)= (*rejilla)(i,j).y();
    retval(1,0)= (*rejilla)(i,j+1).x();
    retval(1,1)= (*rejilla)(i,j+1).y();
    retval(2,0)= (*rejilla)(i+1,j+1).x();
    retval(2,1)= (*rejilla)(i+1,j+1).y();
    retval(3,0)= (*rejilla)(i+1,j).x();
    retval(3,1)= (*rejilla)(i+1,j).y();
    return retval;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::RgQuadCell::GetProp(const std::string &cod) const
  {
    if(cod=="nDivIJ")
      return any_const_ptr(nDivIJ);
    else if(cod=="nDivJK")
      return any_const_ptr(nDivJK);
    else if(cod=="nDivRad")
      return any_const_ptr(nDivRad());
    else if(cod=="nDivCirc")
      return any_const_ptr(nDivCirc());
    if(cod=="getNumPuntos")
      {
        tmp_gp_szt= (nDivIJ+1)*(nDivJK+1);
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="rejilla")
      {
        if(rejilla)
          return any_const_ptr(rejilla);
        else
	  std::cerr << "No se ha creado la rejilla para esta región." << std::endl;
        return any_const_ptr();
      }
    else
      return RegionSecc::GetProp(cod);
  }
