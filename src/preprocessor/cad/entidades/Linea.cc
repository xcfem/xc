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
//Linea.cc

#include "Linea.h"
#include "preprocessor/Preprocessor.h"
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "utility/matrix/util_matrix.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Linea::Linea(Preprocessor *m,const size_t &nd)
  : LineaBase(m,nd) {}

//! @brief Constructor.
XC::Linea::Linea(const std::string &nombre,Preprocessor *m,const size_t &nd)
  : LineaBase(nombre,m,nd) {}

//! @brief Virtual constructor.
XC::SetEstruct *XC::Linea::getCopy(void) const
  { return new Linea(*this); }

//! @brief Return the line length.
double XC::Linea::getLongitud(void) const
  {
    double retval= 0;
    if(check_points())
      retval= dist(p1->GetPos(),p2->GetPos());
    return retval;
  }

//! @brief Returns the parameter of the point in the line (distance to the line's first point measured over the line)
double XC::Linea::getLambda(const Pos3d &p) const
  {
    double retval= 0;
    if(check_points())
      retval= getSegmento().getLambda(p);
    return retval;
  }

//! @brief Returns the segment than links the line ends.
Segmento3d XC::Linea::getSegmento(void) const
  {
    Segmento3d retval;
    if(check_points())
      retval= Segmento3d(p1->GetPos(),p2->GetPos());
    return retval;
  }

//! @brief Returns the squared distance to the
//! position being passed as parameter.
double XC::Linea::DistanciaA2(const Pos3d &pt) const
  { return getSegmento().dist2(pt); }

//! @brief Divides the line by the point being passed as parameter.
XC::Edge *XC::Linea::split_at(Pnt *p,const double &lambda,const double &longitud)
  {
    Cad &cad= get_preprocessor()->getCad();
    Edge *tmp= nullptr;
    if(lambda<0)
      tmp= cad.getLineas().createLine(p,P1());
    else if(lambda>longitud)
      tmp= cad.getLineas().createLine(P2(),p);
    else
      {
        tmp= cad.getLineas().createLine(p,p2);
        SetVertices(p1,p);
      }
    Linea *retval= dynamic_cast<Linea *>(tmp);
    assert(retval);
    //Settint the number of divisions so
    //the element size remains almost constant.
    const double sz_elem= longitud/NDiv();
    SetNDiv(ceil(getLongitud()/sz_elem));
    retval->SetNDiv(ceil(retval->getLongitud()/sz_elem));
    //copiamos the sets.
    std::set<SetBase *> sets= get_sets();
    retval->add_to_sets(sets);
    return retval;
  }

//! @brief Divides the line by the point being passed as parameter.
XC::Edge *XC::Linea::splitAtPoint(Pnt *p)
  {
    Edge *retval= nullptr;
    if(p)
      {
        const Pos3d pN= p->GetPos();
        const Segmento3d s= getSegmento();
        const double lambda= s.getLambda(pN);
        const double l= s.Longitud();
        retval= split_at(p,lambda,l);
      }
    else
      {
        std::cerr << "error Linea::splitAtPoint: pointer to point is null."
                  << std::endl;
      }
    return retval;
  }

//! @brief Divides the line by the point obtained by: p1+lambda*VDir().
XC::Edge *XC::Linea::splitAtLambda(const double &lambda)
  {
    Edge *retval= nullptr;
    const Segmento3d s= getSegmento();
    const Pos3d pN= s.PtoParametricas(lambda);
    Pnt *p= crea_punto(pN);
    const double l= s.Longitud();
    retval= split_at(p,lambda,l);
    return retval;
  }

//! @brief Divides the line by the point obtained by: p1+lambda*VDir().
XC::Edge *XC::Linea::splitAtCooNatural(const double &chi)
  {
    Edge *retval= nullptr;
    const Segmento3d s= getSegmento();
    const Pos3d pN= s.PtoCooNatural(chi);
    Pnt *p= crea_punto(pN);
    retval= split_at(p,s.getParamCooNatural(chi),s.Longitud());
    return retval;
  }

BND3d XC::Linea::Bnd(void) const
  {
    BND3d retval;
    if(p1) retval+= p1->GetPos();
    if(p2) retval+= p2->GetPos();
    return retval;
  }

//! @brief Returns ndiv+1 positions equally spaced along the line.
MatrizPos3d XC::Linea::get_posiciones(void) const
  {
    MatrizPos3d retval;
    if(check_points())
      retval= MatrizPos3d(p1->GetPos(),p2->GetPos(),NDiv());
    return retval;
  }

//! @brief Returns the vector that goes for back end to front end.
const XC::Vector &XC::Linea::getVector(void) const
  {
    static XC::Vector retval(3);
    if(check_points())
      retval= (*p2)-(*p1);
    return retval;
  }

//! @brief Returns a unit vector in the tangent direction in the point
//! defined by s.
//! @param s: parameter that defines the point of tangency.
const XC::Vector &XC::Linea::getTang(const double &s) const
  {
    static Vector retval(3);
    retval= getVector();
    retval.Normalize();
    return retval;
  }

//! @brief Interfaz con VTK.
int XC::Linea::getVtkCellType(void) const
  { return VTK_LINE; }


//! @brief Interfaz con el formato MED de Salome.
int XC::Linea::getMEDCellType(void) const
  { return MED_SEG2; }
