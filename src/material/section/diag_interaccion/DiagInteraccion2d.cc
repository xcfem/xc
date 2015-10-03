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
//DiagInteraccion2d.cc

#include "DiagInteraccion2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"
#include "utility/matrix/Vector.h"
#include "xc_utils/src/base/utils_any.h"
#include "material/section/fiber_section/FiberSectionBase.h"
#include "material/section/diag_interaccion/DatosDiagInteraccion.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"

inline double angle(const Pos2d &p)
  {
    return atan2(p.y(),p.x());
  }

inline bool comp(const Pos2d &a, const Pos2d &b)
  { return (angle(a) < angle(b)); }

GeomObj::list_Pos2d sort_points(const GeomObj::list_Pos2d &pts)
  {
    GeomObj::list_Pos2d tmp(pts);
    std::sort(tmp.begin(),tmp.end(),comp);
    return tmp;
  }

//! @brief Constructor por defecto.
XC::DiagInteraccion2d::DiagInteraccion2d(void)
  : Poligono2d() {}

XC::DiagInteraccion2d::DiagInteraccion2d(const Poligono2d &pts)
  : Poligono2d(pts) //sort_points(pts))
  { }

//! @brief Constructor virtual.
XC::DiagInteraccion2d *XC::DiagInteraccion2d::clon(void) const
  { return new DiagInteraccion2d(*this); }

//! @brief Devuelve la intersección de la semirrecta que une el origen (0,0,0) y el
//! el punto p con el diagrama de interacción.
Pos2d XC::DiagInteraccion2d::get_interseccion(const Pos2d &p) const
  {
    const Pos2d O= Pos2d(0.0,0.0);
    //Buscamos el triedro que contiene a p.
    SemiRecta2d Op(O,p);
    const Segmento2d sg= Clip(Op);
    const Pos2d p1= sg.Origen();
    const Pos2d p2= sg.Destino();
    if(p1!=O)
      return p1;
    else
      return p2;
  }

//! @brief Convierte el diagrama en un rombo con vértices en los
//! extremos del diagrama de interacción.
void XC::DiagInteraccion2d::Simplify(void)
  {
    const double xMax= GetMax(1);
    const double xMin= GetMin(1);
    const double x0= (xMax+xMin)/2.0;
    const double yMax= GetMax(2);
    const double yMin= GetMin(2);
    const double y0= (yMax+yMin)/2.0;
    const Pos2d p1= Pos2d(x0,yMin);
    const Pos2d p2= Pos2d(xMax,y0);
    const Pos2d p3= Pos2d(x0,yMax);
    const Pos2d p4= Pos2d(xMin,y0);
    erase();
    push_back(p1);
    push_back(p2);
    push_back(p3);
    push_back(p4);
  }

//! @brief Devuelve el factor de capacidad para la terna de esfuerzos que se pasan como parámetro.
double XC::DiagInteraccion2d::FactorCapacidad(const Pos2d &esf_d) const
  {
    double retval= 1e6;
    static const Pos2d O= Pos2d(0.0,0.0);
    const double d= dist(O,esf_d); //Distancia desde la terna de esfuerzos al origen.
    if(d<mchne_eps_dbl) //Si el punto está muy cerca del origen.
      retval= 0.0;//Devolvemos el máximo factor de capacidad que puede presentarse.
    else
      {
        const Pos2d C= get_interseccion(esf_d);
        const Segmento2d sOC(O,C);
        retval= d/sOC.Longitud();
      }
    return retval;
  }

XC::Vector XC::DiagInteraccion2d::FactorCapacidad(const GeomObj::list_Pos2d &lp) const
  {
    Vector retval(lp.size());
    int i= 0;
    for(GeomObj::list_Pos2d::const_iterator j= lp.begin();j!=lp.end(); j++, i++)
      retval[i]= FactorCapacidad(*j);
    return retval;
  }


void XC::DiagInteraccion2d::Print(std::ostream &os) const
  {
    std::cerr << "DiagInteraccion2d::Print no implementada." << std::endl;
  }


XC::DiagInteraccion2d XC::calc_diag_interaccionNMy(const FiberSectionBase &scc,const DatosDiagInteraccion &datos= DatosDiagInteraccion())
  {
    DiagInteraccion2d retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetDiagInteraccionNMy(datos);
        delete tmp;
      }
    else
      std::cerr << "XC::calc_diag_interaccion, no se pudo obtener una copia de la sección."
                << std::endl;
    return retval;
  }

