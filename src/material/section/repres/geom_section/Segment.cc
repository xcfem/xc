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
//Segment.cc

#include "Segment.h"
#include "GeomSection.h"
#include "Spot.h"
#include "xc_utils/src/geom/d2/BND2d.h"

#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Segment::Segment(GeomSection *m,const size_t &ndiv)
  : Eje(m,ndiv), p1(nullptr), p2(nullptr) {}

//! @brief Constructor.
XC::Segment::Segment(const std::string &nombre,GeomSection *m,const size_t &ndiv)
  : Eje(nombre,m,ndiv), p1(nullptr), p2(nullptr) {}

//! @brief Devuelve un apuntador al punto inicial.
XC::Spot *XC::Segment::P1(void)
  { return p1; }
//! @brief Devuelve un apuntador (constante) al punto inicial.
const XC::Spot *XC::Segment::P1(void) const
  { return p1; }
//! @brief Devuelve un apuntador al punto final.
XC::Spot *XC::Segment::P2(void)
  { return p2; }
//! @brief Devuelve un apuntador (constante) al punto final.
const XC::Spot *XC::Segment::P2(void) const
  { return p2; }


//! @brief Assigns end points.
void XC::Segment::setEndPoints(size_t idP1, size_t idP2)
  {
    p1= BuscaSpot(idP1);
    if(!p1)
      std::cerr << "Segment::setEndPoints, point: '" 
                << idP1 << "' not found \n";
    actualiza_topologia();
    p2= BuscaSpot(idP2);
    if(!p2)
      std::cerr << "Segment::setEndPoints, point: '" 
                << idP2 << "' not found \n";
    actualiza_topologia();
  }

//! @brief Devuelve el vértice de índice i.
const XC::Spot *XC::Segment::GetVertice(const size_t &i) const
  {
    if(i==1)
      return p1;
    else
      return p2;
  }

//! @brief Devuelve el vértice de índice i.
XC::Spot *XC::Segment::GetVertice(const size_t &i)
  {
    if(i==1)
      return p1;
    else
      return p2;
  }

//! @brief Asigna el vértice de índice i.
void XC::Segment::SetVertice(const size_t &i,Spot *p)
  {
    if(i==1)
      p1= p;
    else
      p2= p;    
  }

//! @brief Devuelve el intervalo 2D ocupado por el segmento.
BND2d XC::Segment::Bnd(void) const
  {
    BND2d retval;
    if(p1) retval+= p1->GetPos();
    if(p2) retval+= p2->GetPos();
    return retval;
  }

//! @brief Interfaz con VTK.
int XC::Segment::getVtkCellType(void) const
  { return VTK_LINE; }

//! @brief Interfaz con el formato MED de Salome.
int XC::Segment::getMEDCellType(void) const
  { return MED_SEG2; }

double XC::Segment::getLong(void) const
  {
    double retval= -1;
    if(p1 && p2)
      retval= dist(p1->GetPos(),p2->GetPos());
    else
      std::cerr << "Endpoint not defined in line: '" << GetNombre()
                 << std::endl;
    return retval;
  }
