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
//RegionSecc.cpp
// Written by Remo M. de Souza
// December 1998

#include "RegionSecc.h"
#include "RgSccPoligono.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "xc_utils/src/geom/matriz_FT.h"

XC::RegionSecc::RegionSecc(Material *mat)
  : DiscretBase(mat) {}

//! @brief Lee un objeto XC::RgSccCuadrilatero desde archivo
bool XC::RegionSecc::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(RegionSecc) Procesando comando: " << cmd << std::endl;
    if(cmd == "poligono")
      {
        Poligono2d tmp= getPoligono();
        tmp.LeeCmd(status);
        return true;
      }
    else
      return DiscretBase::procesa_comando(status);
  }

Poligono2d XC::RegionSecc::getPoligono(void) const
  {
    std::cerr << "RegionSecc::getPoligono no implementada." << std::endl;
    Poligono2d retval;
    return retval;
  }

XC::RgSccPoligono XC::RegionSecc::Interseccion(const Semiplano2d &sp) const
  {
    std::list<Poligono2d> tmpList= getPoligono().Interseccion(sp);
    if(tmpList.size()>1)
     std::cerr << "RegionSecc::Interseccion; la región no es simplemente conexa."
               << std::endl;
    Poligono2d tmp= *tmpList.begin();
    return RgSccPoligono(getMaterialPtr(),1,1,tmp);
  }

//! @brief Devuelve las coordenadas del centro de gravedad
const XC::Vector &XC::RegionSecc::Cdg(void) const
  {
    const Pos2d p= getPoligono().Cdg();
    static Vector retval(2);
    retval[0]= p.x();
    retval[1]= p.y();
    return retval;
  }

//! @brief Devuelve la longitud del contorno de la región.
double XC::RegionSecc::Longitud(void) const
  { return getPoligono().Longitud(); }

//! @brief Devuelve el área de la región.
double XC::RegionSecc::Area(void) const
  { return getPoligono().Area(); }

//! @brief Devuelve el momento de inercia respecto al eje paralelo al y por el CDG.
double XC::RegionSecc::Iy(void) const
  { return getPoligono().Ix(); }

//! @brief Devuelve el momento de inercia respecto al eje paralelo al z por el CDG.
double XC::RegionSecc::Iz(void) const
  { return getPoligono().Iy(); }

//! @brief Devuelve el producto de inercia respecto a los ejes paralelos por el CDG.
double XC::RegionSecc::Pyz(void) const
  { return getPoligono().Pxy(); }

//! @brief Devuelve el momento de inercia polar respecto al CDG en ejes locales.
double XC::RegionSecc::Ix(void) const
  { return Iy()+Iz(); }

//! @brief Devuelve el ángulo que define un eje principal de inercia.
double XC::RegionSecc::Theta_p(void) const
  { return getPoligono().Theta_p(); }

//! @brief Devuelve la dirección de un eje principal de inercia (no sabemos si
//! el mayor o el menor
const XC::Vector &XC::RegionSecc::DirEjeI_a(void) const
  {
    const Dir2d p= getPoligono().DirEjeI_a();
    static Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//! @brief Devuelve la dirección del otro eje principal de inercia (no sabemos si
//!     //el mayor o el menor
const XC::Vector &XC::RegionSecc::DirEjeI_b(void) const
  {
    const Dir2d p= getPoligono().DirEjeI_b();
    static Vector retval(2);
    retval[0]= p.dx();
    retval[1]= p.dy();
    return retval;
  }

//     Recta2d EjeI_a(void) const;
//     //Devuelve un eje principal de inercia (no sabemos si
//     //el mayor o el menor).
//     Recta2d EjeI_b(void) const;
//     //Devuelve el otro eje principal de inercia (no sabemos si
//     //el mayor o el menor).
//     Ref2d2d EjesPrincipalesInercia(void) const;
//     //Devuelve los ejes principales de inercia.

//! @brief Devuelve el momento de inercia principal mayor.
double XC::RegionSecc::getI1(void) const
  { return getPoligono().I1(); }

//! @brief Devuelve el momento de inercia principal menor.
double XC::RegionSecc::getI2(void) const
  { return getPoligono().I2(); }

//     inline EjesPrincInercia2d Inercia(void)
//       { return EjesPrincInercia2d(Cdg(),Iy(),Iz(),Pyz()); }

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al CDG.
double XC::RegionSecc::getI(const unsigned short int &i,const unsigned short int &j) const
  { return getPoligono().I(i+1,j+1); }

//! @brief Devuelve el momento de inercia respecto al eje que pasa por O con dirección la de e.
double XC::RegionSecc::getI(const Pos2d &O,const Vector &e) const
  { return getPoligono().I(O,Vector2d(e[0],e[1])); }

//     double I(const Recta2d &r) const;
//       //Devuelve el momento de inercia respecto a la recta que se pasa
//       //como parámetro.

//! @brief Devuelve la componente i,j del tensor de inercia calculado respecto al punto "o".
double XC::RegionSecc::getI(const unsigned short int i,const unsigned short int j,const Pos2d &o) const
  { return getPoligono().I(i+1,j+1,o); }

//! @brief Devuelve el momento polar de inercia respecto al punto o.
double XC::RegionSecc::getIO(const Pos2d &o)
  { return getPoligono().IO(o); }

//! @brief Devuelve el tensor de inercia calculado desde el centro de gravedad del objeto.
XC::Matrix &XC::RegionSecc::getI(void) const
  {
    static Matrix i(2,2);
    const Poligono2d &pol= getPoligono();
    i(0,0)= pol.Ix(); i(0,1)= -pol.Pxy();
    i(1,0)= i(0,1);   i(1,1)= pol.Iy();
    return i;
  }

//! @brief Devuelve el tensor de inercia respector al punto o.
XC::Matrix &XC::RegionSecc::getI(const Pos2d &o) const
  {
    matriz_FT tmp= getPoligono().I(o);
    static Matrix i(2,2);
    i(0,0)= tmp(1,1); i(0,1)= tmp(1,2);
    i(1,0)= tmp(1,2); i(1,1)= tmp(2,2);
    return i;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::RegionSecc::GetProp(const std::string &cod) const
  {
    if(cod=="numCells")
      {
        tmp_gp_szt= getNumCells();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getPoligono")
      {
        tmp_gp_pol2d= getPoligono();
        return any_const_ptr(&tmp_gp_pol2d);
      }
    else if(cod=="getArea")
      {
        tmp_gp_dbl= Area();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getIy")
      {
        tmp_gp_dbl= Iy();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getIz")
      {
        tmp_gp_dbl= Iz();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getPyz")
      {
        tmp_gp_dbl= Pyz();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getCdg")
      {
        static m_double tmp_m_double(1,2);
        const Vector tmp= Cdg();
        tmp_m_double(1,1)= tmp[0];
        tmp_m_double(1,2)= tmp[1];
        return any_const_ptr(tmp_m_double);
      }
    else
      return DiscretBase::GetProp(cod);
  }
