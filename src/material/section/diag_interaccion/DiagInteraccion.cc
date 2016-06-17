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
//DiagInteraccion.cc

#include "DiagInteraccion.h"
#include "xc_utils/src/geom/d2/MallaTriang3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d2/Triangulo3d.h"
#include "xc_basic/src/util/mchne_eps.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "utility/matrix/Vector.h"

#include "material/section/fiber_section/FiberSectionBase.h"
#include "material/section/diag_interaccion/DatosDiagInteraccion.h"



void XC::DiagInteraccion::clasifica_triedro(const Triedro3d &tdro)
  {
    //Clasificamos el triedro por cuadrantes.
    for(int i= 0;i<8;i++)
      if(tdro.TocaCuadrante(i+1)) triedros_cuadrante[i].insert(&tdro);
  }

void XC::DiagInteraccion::clasifica_triedros(void)
  {
    //Clasificamos los triedros por cuadrantes.
    for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
      clasifica_triedro(*i);
  }

//! @brief Constructor por defecto.
XC::DiagInteraccion::DiagInteraccion(void)
  : ClosedTriangleMesh() {}

XC::DiagInteraccion::DiagInteraccion(const Pos3d &org,const MallaTriang3d &mll)
  : ClosedTriangleMesh(org,mll)
  {
    clasifica_triedros();
  }

//! @brief Constructor de copia.
XC::DiagInteraccion::DiagInteraccion(const DiagInteraccion &otro)
  : ClosedTriangleMesh(otro)
  {
    clasifica_triedros();
  }

//! @brief Operador de asignación.
XC::DiagInteraccion &XC::DiagInteraccion::operator=(const DiagInteraccion &otro)
  {
    ClosedTriangleMesh::operator=(otro);
    clasifica_triedros();
    return *this;
  }

//! @brief Constructor virtual.
XC::DiagInteraccion *XC::DiagInteraccion::clon(void) const
  { return new DiagInteraccion(*this); }

//! @brief Busca el triedro que contiene al punto being passed as parameter.
const Triedro3d *XC::DiagInteraccion::BuscaPtrTriedro(const Pos3d &p) const
  {
    const Triedro3d *retval= nullptr;
    if(triedros.empty())
      {
	std::cerr << "DiagInteraccion::BuscaPtrTriedro; la lista de triedros está vacía."
                  << std::endl;
        return retval;
      }
    const int cuadrante= p.Cuadrante();
    const set_ptr_triedros &set_triedros= triedros_cuadrante[cuadrante-1];
    for(set_ptr_triedros::const_iterator i= set_triedros.begin();i!=set_triedros.end();i++)
      if((*i)->In(p,tol))
        {
          retval= *i;
          break;
        }
    if(!retval) //No lo encuentra, lo intentamos por fuerza bruta.
      {
        for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
          {
            if((*i).In(p,tol))
              {
                retval= &(*i);
                break;
              }
          }
      }
    // if(!retval) //No lo encuentra, lo intentamos por fuerza más bruta.
    //   {
    // 	DiagInteraccion::const_iterator i= begin();
    //     const Triedro3d *tr= &(*i);
    //     retval= tr;
    //     double distMin= tr->PseudoDist(p);
    //     double dist= distMin;
    //     i++;
    //     for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
    //       {
    //         tr= &(*i);
    //         dist= tr->PseudoDist(p);
    //         if(dist<distMin)
    //           {
    //             distMin= dist;
    //             retval= tr;
    //           }
    //       }
    //     if(distMin>tol)
    //       retval= nullptr;
    //   }
    if(!retval) //Sigue sin encontrarlo, buscamos aquel cuyo eje está más próximo.
      {
	DiagInteraccion::const_iterator i= begin();
        const Triedro3d *tr= &(*i);
        SemiRecta3d rayo(tr->Cuspide(),p);
        Recta3d eje= tr->Eje();
        double angMin= angulo(eje,rayo);
        double angTmp= angMin;
        retval= tr;
        i++;
        for(;i!=end();i++)
          {
            tr= &(*i);
            rayo= SemiRecta3d(tr->Cuspide(),p);
            angTmp= angulo(tr->Eje(),rayo);
            if(angTmp<angMin)
              {
                angMin= angTmp;
                retval= tr;
              }
          }
      }
//     if(!retval) //Si sigue sin encontrarlo buscamos un punto en la misma línea.
//       {
//         const GEOM_FT denom= 100.0; //Abs(p.VectorPos())/100;
//         const Pos3d perturb= Pos3d(p.x()/denom,p.y()/denom,p.z()/denom); // + Vector3d(tol/1e2,tol/1e2,tol/1e2);
//         for(XC::DiagInteraccion::const_iterator i= begin();i!=end();i++)
//           {
//             if((*i).In(perturb,tol))
//               {
//                 retval= &(*i);
//                 break;
//               }
//           }
//       }
    return retval;
  }

//! @brief Devuelve la intersección de la semirrecta que une el origen (0,0,0) y el
//! el punto p con el diagrama de interacción.
GeomObj::list_Pos3d XC::DiagInteraccion::get_interseccion(const Pos3d &p) const
  {
    GeomObj::list_Pos3d lst_intersec;
    const Pos3d O= Pos3d(0.0,0.0,0.0);
    //Buscamos el triedro que contiene a p.
    const Triedro3d *i= BuscaPtrTriedro(p);
    if(!i)
      {
	std::cerr << "DiagInteraccion::get_interseccion: no se encontró un triedro que contuviera a:"
                  << p << " cuadrante: " << p.Cuadrante() << std::endl;
      }
    else
      {
        Triangulo3d triang(i->Vertice(1),i->Vertice(2),i->Vertice(3));
        //Calculamos el plano que contiene al triangulo.
        const Plano3d plano= triang.GetPlano();
        SemiRecta3d Op(O,p);
        //Calculamos la intersección entre la semirrecta y dicho plano.
        lst_intersec= interseccion(plano,Op);
        if(lst_intersec.empty())
          {
            
	    std::cerr << "No intersecan. " << std::endl
                      << " Area triangulo: " << triang.Area() << std::endl
                      << " vértice 1: " << i->Vertice(1)
                      << " vertice 2: " << i->Vertice(2)
                      << " vertice 3: " << i->Vertice(3) << std::endl
                      << " terna: " << p << std::endl;
            exit(-1);
          }
      }
    return lst_intersec;
  }

//! @brief Devuelve el factor de capacidad para la terna de esfuerzos que se pasan como parámetro.
double XC::DiagInteraccion::FactorCapacidad(const Pos3d &esf_d) const
  {
    double retval= 1e6;
    assert(rMax>0.0);
    static const Pos3d O= Pos3d(0.0,0.0,0.0);
    const double d= dist(O,esf_d); //Distancia desde la terna de esfuerzos al origen.
    const double umbralMax= rMax*10.0;
    if(d<mchne_eps_dbl) //Si el punto está muy cerca del origen.
      retval= 0.0;//Devolvemos el máximo factor de capacidad que puede presentarse.
    else if(d>umbralMax) //El punto está lejos de la superficie del diagrama.
      retval= d/rMax;
    else
      {
        const double umbralMin= rMin/10.0;
        if(d<umbralMin) //El punto está dentro de la superficie del diagrama.
          retval= d/rMin;
        const GeomObj::list_Pos3d lst_intersec= get_interseccion(esf_d);
        if(!lst_intersec.empty())
          {
            const Pos3d C= *(lst_intersec.begin());
            const Segmento3d sOC(O,C);
            retval= d/sOC.Longitud();
          }
        else
          {
	    std::cerr << "DiagInteraccion::FactorCapacidad; no se encontró la intersección para la terna: "
                      << esf_d << std::endl;
            retval= d/rMin; //No ha encontrado la intersección.
          }
      }
    return retval;
  }

XC::Vector XC::DiagInteraccion::FactorCapacidad(const GeomObj::list_Pos3d &lp) const
  {
    Vector retval(lp.size());
    int i= 0;
    for(GeomObj::list_Pos3d::const_iterator j= lp.begin();j!=lp.end(); j++, i++)
      retval[i]= FactorCapacidad(*j);
    return retval;
  }


void XC::DiagInteraccion::Print(std::ostream &os) const
  {
    std::cerr << "DiagInteraccion::Print no implementada." << std::endl;
  }

void XC::DiagInteraccion::setMatrizPosiciones(const Matrix &m)
  {
    ClosedTriangleMesh::setMatrizPosiciones(m);
    clasifica_triedros();   
  }

XC::DiagInteraccion XC::calc_diag_interaccion(const FiberSectionBase &scc,const DatosDiagInteraccion &datos= DatosDiagInteraccion())
  {
    DiagInteraccion retval;
    FiberSectionBase *tmp= dynamic_cast<FiberSectionBase *>(scc.getCopy());
    if(tmp)
      {
        retval= tmp->GetDiagInteraccion(datos);
        delete tmp;
      }
    else
      std::cerr << "XC::calc_diag_interaccion, no se pudo obtener una copia de la sección."
                << std::endl;
    return retval;
  }

