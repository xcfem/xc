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
//ArcoCircunf.cc

#include "modelador/Modelador.h"
#include "ArcoCircunf.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/geom/d2/SectorCircular3d.h"
#include "xc_utils/src/nucleo/CmdParser.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::ArcoCircunf::ArcoCircunf(Modelador *m)
  : LineaBase(m), p3(nullptr) {}

//! @brief Constructor.
XC::ArcoCircunf::ArcoCircunf(const std::string &nombre,Modelador *m)
  : LineaBase(nombre,m), p3(nullptr) {}

//! @brief Constructor virtual.
XC::SetEstruct *XC::ArcoCircunf::getCopy(void) const
  { return new ArcoCircunf(*this); }

//! @brief Devuelve un apuntador (constante) al punto intermedio.
const XC::Pnt *XC::ArcoCircunf::P3(void) const
  { return p3; }

//! @brief Lee un objeto ArcoCircunf desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - p3: Lee el punto intermedio.
bool XC::ArcoCircunf::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ArcoCircunf) Procesando comando: " << cmd << std::endl;
    if(cmd == "p3")
      {
        const MapPuntos::Indice id_punto= interpretaSize_t(status.GetString());
        SetVertice(3,BuscaPnt(id_punto));
	if(!P3())
	  std::cerr << "ArcoCircunf; " << cmd << " , no se encontró el punto: '" 
                    << id_punto << "' \n";
        return true;
      }
    else
      return LineaBase::procesa_comando(status);
  }

//! @brief Devuelve el vértice de índice i.
const XC::Pnt *XC::ArcoCircunf::GetVertice(const size_t &i) const
  {
    if(i<3)
      return LineaBase::GetVertice(i);
    else
      return p3;
  }


//! @brief Asigna el vértice de índice i.
void XC::ArcoCircunf::SetVertice(const size_t &i,Pnt *p)
  {
    if(i<3)
      LineaBase::SetVertice(i,p);
    else
      {
        if(p3) p3->borra_linea(this);
        p3= p;
        if(p3)
          {
            p3->setGenMalla(false); //Punto intermedio de la línea.
            p3->inserta_linea(this);
          }
      }
  }

//! @brief Devuelve el sector circular definido por el arco.
const SectorCircular3d XC::ArcoCircunf::get_sector_circular3d(void) const
  {
    SectorCircular3d retval;
    if(p1 && p2 && p3)
      retval= SectorCircular3d(p1->GetPos(),p3->GetPos(),p2->GetPos());
    else
      std::cerr << "ArcoCircunf::get_sector_circular3d; el arco no esta definido." << std::endl;
    return retval;

  }

//! @brief Devuelve la longitud de la línea.
double XC::ArcoCircunf::getLongitud(void) const
  {
    double retval= 0;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().LongitudArco();
    else
      std::cerr << "error en ArcoCircunf::getLongitud, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el ángulo comprendido por el arco.
double XC::ArcoCircunf::getAnguloComprendido(void) const
  {
    double retval= 0;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().AnguloComprendido();
    else
      std::cerr << "error en ArcoCircunf::getAnguloComprendido, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el ángulo inicial.
double XC::ArcoCircunf::getTheta1(void) const
  {
    double retval= 0;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().Theta1();
    else
      std::cerr << "error en ArcoCircunf::getTheta1, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el ángulo final.
double XC::ArcoCircunf::getTheta2(void) const
  {
    double retval= 0;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().Theta2();
    else
      std::cerr << "error en ArcoCircunf::getTheta2, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el centro.
Pos3d XC::ArcoCircunf::getCentro(void) const
  {
    Pos3d retval;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().Centro();
    else
      std::cerr << "error en ArcoCircunf::getCentro, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el punto inicial.
Pos3d XC::ArcoCircunf::getPInic(void) const
  {
    Pos3d retval;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().PInic();
    else
      std::cerr << "error en ArcoCircunf::getPInic, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el punto final.
Pos3d XC::ArcoCircunf::getPFin(void) const
  {
    Pos3d retval;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().PFin();
    else
      std::cerr << "error en ArcoCircunf::getPFin, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el punto medio.
Pos3d XC::ArcoCircunf::getPMed(void) const
  {
    Pos3d retval;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().PMed();
    else
      std::cerr << "error en ArcoCircunf::getPMed, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

//! @brief Devuelve el radio del arco.
double XC::ArcoCircunf::getRadio(void) const
  {
    double retval= 0;
    if(p1 && p2 && p3)
      retval= get_sector_circular3d().Radio();
    else
      std::cerr << "error en ArcoCircunf::getRadio, el arco: '" << GetNombre()
                << " no está definido." << std::endl;
    return retval;
  }

void XC::ArcoCircunf::actualiza_topologia(void)
  {
    LineaBase::actualiza_topologia();
    if(P3()) P3()->inserta_linea(this);
  }

BND3d XC::ArcoCircunf::Bnd(void) const
  { return get_sector_circular3d().Bnd(); }

//! @brief Devuelve ndiv+1 posiciones equiespaciadas a lo largo del arco.
MatrizPos3d XC::ArcoCircunf::get_posiciones(void) const
{ return get_sector_circular3d().PuntosArco(NDiv()+1); }

//! @brief Interfaz con VTK.
int XC::ArcoCircunf::getVtkCellType(void) const
  { return VTK_QUADRATIC_EDGE; }

//! @brief Interfaz con el formato MED de Salome.
int XC::ArcoCircunf::getMEDCellType(void) const
  { return MED_SEG3; }

//! @brief Returns k-points.
XC::ID XC::ArcoCircunf::getKPoints(void) const
  {
    ID retval(3);
    retval[0]= P1()->GetTag();
    retval[1]= P2()->GetTag();
    retval[2]= P2()->GetTag();
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
//!
//! Soporta las propiedades:
any_const_ptr XC::ArcoCircunf::GetProp(const std::string &cod) const
  {
    static Pos3d P;
    if(cod=="getAngComp") //Devuelve el ángulo comprendido.
      {
        tmp_gp_dbl= getAnguloComprendido();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getTheta1") //Devuelve el ángulo inicial respecto al eje x.
      {
        tmp_gp_dbl= getTheta1();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getTheta2") //Devuelve el ángulo final respecto al eje x.
      {
        tmp_gp_dbl= getTheta2();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getCentro")
      {
        P= getCentro();
        return any_const_ptr(&P);
      }
    else if(cod=="getPInic")
      {
        P= getPInic();
        return any_const_ptr(&P);
      }
    else if(cod=="getPFin")
      {
        P= getPFin();
        return any_const_ptr(&P);
      }
    else if(cod=="getPMed")
      {
        P= getPMed();
        return any_const_ptr(&P);
      }
    else if(cod=="getRadio")
      {
        tmp_gp_dbl= getRadio();
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return LineaBase::GetProp(cod);
  }

