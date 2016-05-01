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
//ListReinfLayer.cc

#include "ListReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/BarraSuelta.h"
#include "material/section/repres/geom_section/reinfLayer/StraightReinfLayer.h"
#include "material/section/repres/geom_section/reinfLayer/CircReinfLayer.h"
#include "preprocessor/loaders/MaterialLoader.h"

#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"

#include "material/section/repres/geom_section/GeomSection.h"

//! @brief Liberta todas las pociciones.
void XC::ListReinfLayer::libera(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copia las ReinfLayer de otro contenedor.
void XC::ListReinfLayer::copia(const ListReinfLayer &otra)
  {
    libera();
    for(const_iterator i=otra.begin();i!=otra.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::ListReinfLayer::ListReinfLayer(GeomSection *owr,MaterialLoader *ml)
  :  l_reg(), SeccionInerte(owr), material_loader(ml) {}

//! @brief Constructor de copia.
XC::ListReinfLayer::ListReinfLayer(const ListReinfLayer  &otro)
  : l_reg(), material_loader(otro.material_loader)
  { copia(otro); }

//! @brief Operador asignación.
XC::ListReinfLayer &XC::ListReinfLayer::operator=(const ListReinfLayer &otro)
  {
    SeccionInerte::operator=(otro);
    material_loader= otro.material_loader;
    copia(otro);
    return *this;
  }

XC::StraightReinfLayer *XC::ListReinfLayer::newStraightReinfLayer(const std::string &cod_mat)
  {
    StraightReinfLayer tmp(this,material_loader->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<StraightReinfLayer *>(ptr);
  }

XC::CircReinfLayer *XC::ListReinfLayer::newCircReinfLayer(const std::string &cod_mat)
  {
    CircReinfLayer tmp(this,material_loader->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<CircReinfLayer *>(ptr);
  }

XC::BarraSuelta *XC::ListReinfLayer::newReinfBar(const std::string &cod_mat)
  {
    BarraSuelta tmp(this,material_loader->find_ptr(cod_mat));
    ReinfLayer *ptr= push_back(tmp);
    ptr->set_owner(this);
    return dynamic_cast<BarraSuelta *>(ptr);
  }

//! @brief Destructor.
XC::ListReinfLayer::~ListReinfLayer(void)
  { libera(); }

//! @brief Borra las armaduras definidas.
void XC::ListReinfLayer::clear(void)
  { libera(); }

//! @brief Agrega una región al contenedor.
XC::ReinfLayer *XC::ListReinfLayer::push_back(const ReinfLayer &reg)
  {
    ReinfLayer *tmp= reg.getCopy();
    l_reg::push_back(tmp);
    return tmp;
  }

//! @brief Devuelve una referencia al objeto propietario.
const XC::GeomSection *XC::ListReinfLayer::getGeomSection(void) const
  {
    const EntCmd *owr= Owner();
    assert(owr);
    const GeomSection *retval= dynamic_cast<const GeomSection *>(owr);
    assert(retval);
    return retval;
  }

//! @brief Devuelve el valor mínimo del recubrimiento de las barras.
double XC::ListReinfLayer::getRecubrimiento(void) const
  {
    double retval= 0;
    const_iterator i= begin();
    retval= (*i)->getRecubrimiento();
    i++;
    for(const_iterator i=begin();i!=end();i++)
      retval= std::min(retval,(*i)->getRecubrimiento());
    return retval;
  }

//! @brief Devuelve el número total de celdas.
size_t XC::ListReinfLayer::getNumReinfBars(void) const
  {
    int nbars= 0;
    for(const_iterator i=begin();i!=end();i++)
      nbars+= (*i)->getNumReinfBars();
    return nbars;
  }

// //! Devuelve el subconjunto de barras de ésta que cumplen la condición que se
// //! pasa como parámetro.
// void XC::ListReinfLayer::Cumplen(const std::string &cond,ListReinfLayer &retval,bool clear)
//   {
//     if(clear) retval.clear();
//     const_iterator i= begin();
//     for(;i!= end();i++)
//       (*i)->Cumplen(cond,retval,false);
//   }

//! Devuelve el subconjunto de barras cuyo centro esta contenido en el polígono.
void XC::ListReinfLayer::getBarrasIn(const Poligono2d &plg,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      (*i)->getBarrasIn(plg,retval,false);
  }

//! Devuelve el subconjunto de barras cuyo centro esta contenido en el semiplano.
void XC::ListReinfLayer::getBarrasIn(const Semiplano2d &sp,ListReinfLayer &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      (*i)->getBarrasIn(sp,retval,false);
  }

//! @brief Devuelve el área homogeneizada de las regiones.
double XC::ListReinfLayer::getAreaSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getReinfBars().getAreaSeccHomogeneizada(E0);
    return retval;
  }

XC::Vector XC::ListReinfLayer::getCdgSeccHomogeneizada(const double &E0) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        peso= (*i)->getReinfBars().getAreaSeccHomogeneizada(E0);
        retval+= peso*(*i)->getReinfBars().getCdgSeccHomogeneizada(E0);
        divisor+= peso;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al y por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::ListReinfLayer::getIySeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgSeccHomogeneizada(E0)[1]-zCdg;
        retval+= (*i)->getReinfBars().getIySeccHomogeneizada(E0)+(*i)->getReinfBars().getAreaSeccHomogeneizada(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::ListReinfLayer::getIzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgSeccHomogeneizada(E0)[0]-yCdg;
        retval+= (*i)->getReinfBars().getIzSeccHomogeneizada(E0)+(*i)->getReinfBars().getAreaSeccHomogeneizada(E0)*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección homogeneizada respecto a los ejes paralelos al y y al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::ListReinfLayer::getPyzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[0];
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getReinfBars().getCdgSeccHomogeneizada(E0)[0]-yCdg)*((*i)->getReinfBars().getCdgSeccHomogeneizada(E0)[1]-zCdg);
        retval+= (*i)->getReinfBars().getPyzSeccHomogeneizada(E0)+(*i)->getReinfBars().getAreaSeccHomogeneizada(E0)*d2;
      }
    return retval;
  }

//! @brief Devuelve el área bruta de las regiones.
double XC::ListReinfLayer::getAreaSeccBruta(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->getReinfBars().getAreaSeccBruta();
    return retval;
  }

XC::Vector XC::ListReinfLayer::getCdgSeccBruta(void) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        peso= (*i)->getReinfBars().getAreaSeccBruta();
        retval+= peso*(*i)->getReinfBars().getCdgSeccBruta();
        divisor+= peso;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección bruta respecto al eje paralelo al y por el CDG.
double XC::ListReinfLayer::getIySeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccBruta()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgSeccBruta()[1]-zCdg;
        retval+= (*i)->getReinfBars().getIySeccBruta()+(*i)->getReinfBars().getAreaSeccBruta()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección bruta respecto al eje paralelo al z por el CDG.
double XC::ListReinfLayer::getIzSeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccBruta()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->getReinfBars().getCdgSeccBruta()[0]-yCdg;
        retval+= (*i)->getReinfBars().getIzSeccBruta()+(*i)->getReinfBars().getAreaSeccBruta()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección bruta respecto a los ejes paralelos al y y al z por el CDG.
double XC::ListReinfLayer::getPyzSeccBruta(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const double zCdg= getCdgSeccBruta()[0];
    const double yCdg= getCdgSeccBruta()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->getReinfBars().getCdgSeccBruta()[0]-yCdg)*((*i)->getReinfBars().getCdgSeccBruta()[1]-zCdg);
        retval+= (*i)->getReinfBars().getPyzSeccBruta()+(*i)->getReinfBars().getAreaSeccBruta()*d2;
      }
    return retval;
  }

void XC::ListReinfLayer::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
