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

//! @brief Constructor virtual.
XC::SetEstruct *XC::Linea::getCopy(void) const
  { return new Linea(*this); }

//! @brief Devuelve la longitud de la línea.
double XC::Linea::getLongitud(void) const
  {
    double retval= 0;
    if(p1 && p2)
      retval= dist(p1->GetPos(),p2->GetPos());
    else
      std::cerr << "error Linea::getLongitud la línea: '" << GetNombre()
                << " no está definida." << std::endl;
    return retval;
  }

//! @brief Returns the parameter of the point in the line (distance to the line's first point measured over the line)
double XC::Linea::getLambda(const Pos3d &p) const
  {
    double retval= 0;
    if(p1 && p2)
      retval= getSegmento().getLambda(p);
    else
      std::cerr << "error Linea::getLambda the line: '" << GetNombre()
                << " undefined." << std::endl;
    return retval;
  }

//! @brief Devuelve el segmento que une los extremos
//! de la línea.
Segmento3d XC::Linea::getSegmento(void) const
  {
    Segmento3d retval;
    if(p1 && p2)
      retval= Segmento3d(p1->GetPos(),p2->GetPos());
    else
      std::cerr << "error Linea::getSegmento la línea: '" << GetNombre()
                << " no está definida." << std::endl;
    return retval;
  }

//! @brief Devuelve el cuadrado de la distancia a
//! la posición being passed as parameter.
double XC::Linea::DistanciaA2(const Pos3d &pt) const
  {
    const double retval= getSegmento().dist2(pt);
    return retval;
  }

//! @brief Divide la línea por el punto being passed as parameter.
XC::Edge *XC::Linea::split_at(Pnt *p,const double &lambda,const double &longitud)
  {
    Cad &cad= get_preprocessor()->getCad();
    Edge *tmp= nullptr;
    if(lambda<0)
      tmp= cad.getLineas().Line(p,P1());
    else if(lambda>longitud)
      tmp= cad.getLineas().Line(P2(),p);
    else
      {
        tmp= cad.getLineas().Line(p,p2);
        SetVertices(p1,p);
      }
    Linea *retval= dynamic_cast<Linea *>(tmp);
    assert(retval);
    //Asignamos número de divisiones de manera
    //que se conserve aproximadamente el tamaño
    //del elemento.
    const double sz_elem= longitud/NDiv();
    SetNDiv(ceil(getLongitud()/sz_elem));
    retval->SetNDiv(ceil(retval->getLongitud()/sz_elem));
    //copiamos los conjuntos.
    std::set<SetBase *> sets= get_sets();
    retval->add_to_sets(sets);
    return retval;
  }

//! @brief Divide la línea por el punto being passed as parameter.
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
        std::cerr << "error Linea::splitAtPoint: se pasó un puntero nulo."
                  << std::endl;
      }
    return retval;
  }

//! @brief Divide la línea por el punto que se obtiene mediante: p1+lambda*VDir().
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

//! @brief Divide la línea por el punto que se obtiene mediante: p1+lambda*VDir().
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

//! @brief Devuelve ndiv+1 posiciones equiespaciadas a lo largo de la línea.
MatrizPos3d XC::Linea::get_posiciones(void) const
  {
    MatrizPos3d retval;
    if(p1 && p2)
      retval= MatrizPos3d(p1->GetPos(),p2->GetPos(),NDiv());
    else
      std::cerr << "Linea::get_posiciones; la línea no esté definida." << std::endl;
    return retval;
  }

//! @brief Devuelve el vector que une los extremos de la línea
const XC::Vector &XC::Linea::getVector(void) const
  {
    static XC::Vector retval(3);
    if(p1 && p2)
      retval= (*p2)-(*p1);
    else
      std::cerr << "Linea::getVector; alguno de los punteros a punto es nulo." << std::endl;
    return retval;
  }

//! @brief Devuelve un vector unitario tangente a la línea en el punto s
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
