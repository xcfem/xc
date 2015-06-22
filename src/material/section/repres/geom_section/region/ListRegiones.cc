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
//ListRegiones.cc

#include "ListRegiones.h"
#include <material/section/repres/geom_section/region/RgSccCuadrilatero.h>
#include <material/section/repres/geom_section/region/RgSccCirc.h>
#include <material/section/repres/geom_section/region/RgSccPoligono.h>
#include "material/uniaxial/UniaxialMaterial.h" 
#include "modelador/loaders/MaterialLoader.h"
#include "utility/matrix/Vector.h"
#include "xc_utils/src/geom/pos_vec/Dir2d.h"
#include "xc_utils/src/geom/pos_vec/Vector2d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/sis_ref/Ref2d2d.h"

#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/bool_op_poligono2d.h"

//! @brief Liberta todas las pociciones.
void XC::ListRegiones::libera(void)
  {
    for(iterator i=begin();i!=end();i++)
      delete *i;
    erase(begin(),end());
  }

//! @brief Copia las regiones de otro contenedor.
void XC::ListRegiones::copia(const ListRegiones &otra)
  {
    libera();
    for(const_iterator i=otra.begin();i!=otra.end();i++)
      push_back(*(*i)->getCopy());
  }

//! @brief Constructor.
XC::ListRegiones::ListRegiones(MaterialLoader *ml)
  : l_reg(), material_loader(ml) {}

//! @brief Constructor de copia.
XC::ListRegiones::ListRegiones(const ListRegiones  &otro)
  : l_reg(), material_loader(otro.material_loader)
  { copia(otro); }

//! @brief Operador asignación.
XC::ListRegiones &XC::ListRegiones::operator=(const ListRegiones &otro)
  {
    SeccionInerte::operator=(otro);
    material_loader= otro.material_loader;
    copia(otro);
    return *this;
  }

//! @brief Aggregates a new quadrilateral region.
XC::RgSccCuadrilatero *XC::ListRegiones::newQuadRegion(const std::string &cod_mat)
  {
    Material *mat= material_loader->find_ptr(cod_mat);
    if(!mat)
      std::cerr << "ListRegiones::newQuadRegion; ¡ojo!, no se encontró el material: '"
                << cod_mat << "' deberá asignarse un material a la región.\n";
    RgSccCuadrilatero tmp(mat);
    RgSccCuadrilatero *ptr= dynamic_cast<XC::RgSccCuadrilatero *>(push_back(tmp));
    ptr->set_owner(this);
    return ptr;
  }

//! @brief Aggregates a new quadrilateral region.
XC::RgSccCirc *XC::ListRegiones::newCircularRegion(const std::string &cod_mat)
  {
    Material *mat= material_loader->find_ptr(cod_mat);
    if(!mat)
      std::cerr << "ListRegiones::newQuadRegion; ¡ojo!, no se encontró el material: '"
                << cod_mat << "' deberá asignarse un material a la región.\n";
    RgSccCirc tmp(mat);
    RgSccCirc *ptr= dynamic_cast<XC::RgSccCirc *>(push_back(tmp));
    ptr->set_owner(this);
    return ptr;
  }

//! @brief Lee un objeto XC::GeomSection desde archivo
bool XC::ListRegiones::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_log= "(ListRegiones) Procesando comando: "+cmd;
    if(verborrea>2)
      std::clog <<  str_log << std::endl;

    const CmdParser &parser= status.Parser();
    if(cmd == "reg_cuad")
      {
        std::string cod_mat= "nil";
        std::deque<boost::any> fnc_indices;
        if(parser.TieneIndices())
          {
            fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              cod_mat= convert_to_string(fnc_indices[0]); //Código del material.
            Material *mat= material_loader->find_ptr(cod_mat);
            if(!mat)
	      std::cerr << str_log + "; ¡ojo!, no se encontró el material: '"
                        << cod_mat << "' deberá asignarse un material a la región.\n";
            RgSccCuadrilatero tmp(mat);
            RegionSecc *ptr= push_back(tmp);
            ptr->set_owner(this);
            ptr->LeeCmd(status);
          }
        else
	  std::cerr << "Error; " << cmd << " - uso: " << cmd << "[id_material]" << std::endl;
        return true;
      }
    else if(cmd == "reg_circ")
      {
        std::string cod_mat= "nil";
        std::deque<boost::any> fnc_indices;
        if(parser.TieneIndices())
          {
            fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
                cod_mat= convert_to_string(fnc_indices[0]); //Código del material.
            Material *mat= material_loader->find_ptr(cod_mat);
            if(!mat)
	      std::cerr << str_log + "; ¡ojo!, no se encontró el material: '"
                        << cod_mat << " deberá asignarse un material a la región.\n";
            RgSccCirc tmp(mat);
            RegionSecc *ptr= push_back(tmp);
            ptr->set_owner(this);
            ptr->LeeCmd(status);
          }
        else
	  std::cerr << "Error; " << cmd << " - uso: " << cmd << "[id_material]" << std::endl;
        return true;
      }
    else if(cmd == "for_each")
      {
        EjecutaBloqueForEach(status,status.GetBloque());
        return true;
      }
    else if(cmd == "clear")
      {
        libera();
        return true;
      }
    else
      return SeccionInerte::procesa_comando(status);
  }

//! @brief Destructor.
XC::ListRegiones::~ListRegiones(void)
  { libera(); }

//! @brief Borra todas las regiones.
void XC::ListRegiones::clear(void)
  { libera(); }

//! @brief Agrega una región al contenedor.
XC::RegionSecc *XC::ListRegiones::push_back(const RegionSecc &reg)
  {
    RegionSecc *tmp= reg.getCopy();
    l_reg::push_back(tmp);
    return tmp;
  }

XC::ListRegiones::const_iterator XC::ListRegiones::begin(void) const
  { return l_reg::begin(); }
XC::ListRegiones::const_iterator XC::ListRegiones::end(void) const
  { return l_reg::end(); }
XC::ListRegiones::iterator XC::ListRegiones::begin(void)
  { return l_reg::begin(); }
XC::ListRegiones::iterator XC::ListRegiones::end(void)
  { return l_reg::end(); }

void XC::ListRegiones::EjecutaBloqueForEach(CmdStatus &status,const std::string &bloque)
  {
    const std::string nmbBlq= nombre_clase()+":for_each";
    for(iterator i= begin();i!=end();i++)
      (*i)->EjecutaBloque(status,bloque,nmbBlq);
  }

//! @brief Devuelve el número total de celdas.
size_t XC::ListRegiones::getNumCells(void) const
  {
    size_t ncells= 0;
    for(const_iterator i=begin();i!=end();i++)
      {
        const RegionSecc *ptr= *i;
        ncells+= ptr->getNumCells();
      }
    return ncells;
  }

//! @brief Devuelve una lista con los contornos de las regiones.
std::list<Poligono2d> XC::ListRegiones::getContornosRegiones(void) const
  {
    std::list<Poligono2d> retval;
    for(const_iterator i= begin();i!=end();i++)
      retval.push_back((*i)->getPoligono());
    return retval;
  }

//! @brief Devuelve el contorno de las regiones.
std::list<Poligono2d> XC::ListRegiones::getContorno(void) const
  {
    std::list<Poligono2d> retval= join(getContornosRegiones());
    return retval;
  }

XC::ListRegiones XC::ListRegiones::Interseccion(const Semiplano2d &sp) const
  {
    ListRegiones retval(material_loader);
    for(const_iterator i= begin();i!=end();i++)
      retval.push_back((*i)->Interseccion(sp));
    return retval;
  }

//! @brief Devuelve el área de las regiones.
double XC::ListRegiones::getAreaSeccBruta(void) const
  {
    double retval= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      retval+= (*i)->Area();
    return retval;
  }

//! @brief Devuelve el centro de gravedad de la sección bruta.
XC::Vector XC::ListRegiones::getCdgSeccBruta(void) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        peso= (*i)->Area();
        if(peso>0)
          {
            retval+= peso*(*i)->Cdg();
            divisor+= peso;
          }
        else
          std::cerr << "ListRegiones::getCdgSeccBruta; la región: "
                    << *i << " tiene peso nulo o negativo." 
                        << std::endl;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al y por el CDG.
double XC::ListRegiones::getIySeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccBruta()[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->Cdg()[1]-zCdg;
        retval+= (*i)->Iy()+(*i)->Area()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al z por el CDG.
double XC::ListRegiones::getIzSeccBruta(void) const
  {
    double retval= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccBruta()[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d= (*i)->Cdg()[0]-yCdg;
        retval+= (*i)->Iz()+(*i)->Area()*sqr(d);
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección homogeneizada respecto a los ejes paralelos al y y al z por el CDG.
double XC::ListRegiones::getPyzSeccBruta(void) const
  {
    double retval= 0.0;
    double d2= 0.0;
    const Vector cooCdg= getCdgSeccBruta();
    const double zCdg= cooCdg[1];
    const double yCdg= cooCdg[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        d2= ((*i)->Cdg()[0]-yCdg)*((*i)->Cdg()[1]-zCdg);
        retval+= (*i)->Pyz()+(*i)->Area()*d2;
      }
    return retval;
  }

//! Devuelve el subconjunto de regiones de éste que cumplen la condición que se
//! pasa como parámetro.
void XC::ListRegiones::Cumplen(const std::string &cond,ListRegiones &retval,bool clear)
  {
    if(clear) retval.clear();
    const_iterator i= begin();
    for(;i!= end();i++)
      if((*i)->verdadero(cond))
        retval.push_back(**i);
  }


//! @brief Devuelve el área homogeneizada de las regiones.
double XC::ListRegiones::getAreaSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            retval+= n*(*i)->Area();
          }
        else
	  std::cerr << "ListRegiones::getAreaSeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

XC::Vector XC::ListRegiones::getCdgSeccHomogeneizada(const double &E0) const
  {
    Vector retval(2);
    double peso= 0.0;
    double divisor= 0.0;
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            peso= mat->getTangent()/E0*(*i)->Area();
            if(peso>0)
              {
                retval+= peso*(*i)->Cdg();
                divisor+= peso;
              }
            else
	      std::cerr << "ListRegiones::getCdgSeccHomogeneizada; la región: "
                        << *i << " tiene peso nulo o negativo." 
                        << std::endl;
          }
        else
	  std::cerr << "ListRegiones::getCdgSeccHomogeneizada; no se pudo obtener el material la región." << std::endl;
      }
    retval/= divisor;
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al y por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::ListRegiones::getIySeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double zCdg= getCdgSeccHomogeneizada(E0)[1];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->Cdg()[1]-zCdg;
            retval+= n*((*i)->Iy()+(*i)->Area()*sqr(d));
          }
        else
	  std::cerr << "ListRegiones::getIySeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! @brief Devuelve el momento de inercia de la sección homogeneizada respecto al eje paralelo al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::ListRegiones::getIzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    double d= 0.0;
    const double yCdg= getCdgSeccHomogeneizada(E0)[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d= (*i)->Cdg()[0]-yCdg;
            retval+= n*((*i)->Iz()+(*i)->Area()*sqr(d));
          }
        else
	  std::cerr << "ListRegiones::getIySeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! @brief Devuelve el producto de inercia de la sección homogeneizada respecto a los ejes paralelos al y y al z por el CDG.
//! @param E0: Módulo elástico de referencia.
double XC::ListRegiones::getPyzSeccHomogeneizada(const double &E0) const
  {
    double retval= 0.0;
    double n= 0.0;
    double d2= 0.0;
    const Vector cooCdg= getCdgSeccHomogeneizada(E0);
    const double zCdg= cooCdg[1];
    const double yCdg= cooCdg[0];
    for(const_iterator i= begin();i!=end();i++)
      {
        const UniaxialMaterial *mat= dynamic_cast<const UniaxialMaterial *>((*i)->getMaterialPtr());
        if(mat)
          {
            n= mat->getTangent()/E0;
            d2= ((*i)->Cdg()[0]-yCdg)*((*i)->Cdg()[1]-zCdg);
            retval+= n*((*i)->Pyz()+(*i)->Area()*d2);
          }
        else
	  std::cerr << "ListRegiones::getIySeccHomogeneizada; no se pudo obtener el material la región." << std::endl; 
      }
    return retval;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ListRegiones::GetProp(const std::string &cod) const
  {
    if(cod=="numRegiones")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    if(cod=="numCeldas")
      {
        tmp_gp_szt= getNumCells();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="regiones")
      {
        const size_t i= popSize_t(cod);
        RegionSecc *reg= nullptr;
	const_iterator reg_iter=begin();
        if(i<size())
          {
            for(size_t j=0;j<=i;j++,reg_iter++);
            reg= *reg_iter;
          }
        else
	  std::cerr << "Regin of index: " << i << " not found." << std::endl;
        if(reg)
          return any_const_ptr(reg);
        else
          return any_const_ptr();
      }
    else
      return SeccionInerte::GetProp(cod);
  }

void XC::ListRegiones::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }

std::ostream &XC::operator<<(std::ostream &os,const ListRegiones &lr)
  {
    lr.Print(os);
    return os;
  }
