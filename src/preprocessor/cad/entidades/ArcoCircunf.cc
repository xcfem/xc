//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ArcoCircunf.cc

#include "preprocessor/Preprocessor.h"
#include "ArcoCircunf.h"
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/geom/d2/SectorCircular3d.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::ArcoCircunf::ArcoCircunf(Preprocessor *m)
  : LineaBase(m), p3(nullptr) {}

//! @brief Constructor.
XC::ArcoCircunf::ArcoCircunf(const std::string &nombre,Preprocessor *m)
  : LineaBase(nombre,m), p3(nullptr) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::ArcoCircunf::getCopy(void) const
  { return new ArcoCircunf(*this); }

//! @brief Returns a constant pointer to punto intermedio.
const XC::Pnt *XC::ArcoCircunf::P3(void) const
  { return p3; }

//! @brief Returns the vértice de índice i.
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
            p3->setGenMesh(false); //Punto intermedio de la línea.
            p3->inserta_linea(this);
          }
      }
  }

//! @brief Checks that the points are defined.
bool XC::ArcoCircunf::check_points(void) const
  {
    bool retval= false;
    if(p1 && p2 && p3)
      retval= true;
    else
      std::cerr << nombre_clase() << "::" << __FUNCTION__
	        << "; arc: '" << GetNombre()
                << " is not defined." << std::endl;
    return retval;
  }
  
//! @brief Returns the sector circular definido por el arco.
const SectorCircular3d XC::ArcoCircunf::get_sector_circular3d(void) const
  {
    SectorCircular3d retval;
    if(check_points())
      retval= SectorCircular3d(p1->GetPos(),p3->GetPos(),p2->GetPos());
    return retval;
  }

//! @brief Return the longitud de la línea.
double XC::ArcoCircunf::getLongitud(void) const
  {
    double retval= 0;
    if(check_points())
      retval= get_sector_circular3d().LongitudArco();
    return retval;
  }

//! @brief Returns the ángulo comprendido por el arco.
double XC::ArcoCircunf::getAnguloComprendido(void) const
  {
    double retval= 0;
    if(check_points())
      retval= get_sector_circular3d().AnguloComprendido();
    return retval;
  }

//! @brief Returns the ángulo inicial.
double XC::ArcoCircunf::getTheta1(void) const
  {
    double retval= 0;
    if(check_points())
      retval= get_sector_circular3d().Theta1();
    return retval;
  }

//! @brief Returns the ángulo final.
double XC::ArcoCircunf::getTheta2(void) const
  {
    double retval= 0;
    if(check_points())
      retval= get_sector_circular3d().Theta2();
    return retval;
  }

//! @brief Returns the parameter of the point in the arc (distance to the arc's first point measured over the arc)
double XC::ArcoCircunf::getLambda(const Pos3d &p) const
  {
    double retval= 0;
    if(check_points())
      retval= get_sector_circular3d().getLambda(p);
    return retval;
  }

//! @brief Returns the centro.
Pos3d XC::ArcoCircunf::getCentro(void) const
  {
    Pos3d retval;
    if(check_points())
      retval= get_sector_circular3d().Centro();
    return retval;
  }

//! @brief Returns the start point.
Pos3d XC::ArcoCircunf::getPInic(void) const
  {
    Pos3d retval;
    if(check_points())
      retval= get_sector_circular3d().PInic();
    return retval;
  }

//! @brief Returns the end point.
Pos3d XC::ArcoCircunf::getPFin(void) const
  {
    Pos3d retval;
    if(check_points())
      retval= get_sector_circular3d().PFin();
    return retval;
  }

//! @brief Returns the punto medio.
Pos3d XC::ArcoCircunf::getPMed(void) const
  {
    Pos3d retval;
    if(check_points())
      retval= get_sector_circular3d().PMed();
    return retval;
  }

//! @brief Returns the radio del arco.
double XC::ArcoCircunf::getRadio(void) const
  {
    double retval= 0;
    if(check_points())
      retval= get_sector_circular3d().Radio();
    return retval;
  }

void XC::ArcoCircunf::actualiza_topologia(void)
  {
    LineaBase::actualiza_topologia();
    if(P3()) P3()->inserta_linea(this);
  }

BND3d XC::ArcoCircunf::Bnd(void) const
  { return get_sector_circular3d().Bnd(); }

//! @brief Returns ndiv+1 posiciones equiespaciadas a lo largo del arco.
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
