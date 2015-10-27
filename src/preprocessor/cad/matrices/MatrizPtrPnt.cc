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
//MatrizPtrPnt.cc

#include "MatrizPtrPnt.h"
#include "TritrizPtrPnt.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SP_Constraint.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/cad/Cad.h"
#include "xc_utils/src/base/Record.h"
#include "xc_basic/src/matrices/RangoMatriz.h"
#include "xc_utils/src/nucleo/aux_any.h"


void XC::MatrizPtrPnt::setPnt(const size_t &j,const size_t &k,const int &id_punto)
  {
    if(check_range(j,k))
      {
        Cad *c= getCad();
        Pnt *tmp= at(j,k);
        if(tmp!= nullptr)
          std::clog << "Warning!, position: (" 
                    << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->GetNombre() << std::endl;
        at(j,k)= c->getPuntos().busca(id_punto);
     }
   else
     std::cerr << "(MatrizPtrPnt::setPnt): '"
               << "'; indices: ("
               << j << ',' << k << ") out of range;"
               << " number of rows: " << fls << " number of columns: " << cls
               << std::endl;
  }

XC::Pnt *XC::MatrizPtrPnt::getPnt(const size_t &j,const size_t &k)
  {
    Pnt *retval= nullptr;
    if(check_range(j,k))
      retval= at(j,k);
    return retval;
  }

//! @brief Lee un objeto MatrizPtrPnt desde archivo
bool XC::MatrizPtrPnt::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(MatrizPtrPnt) Procesando comando: " << cmd << std::endl;

    const CmdParser &parser= status.Parser();
    if(cmd == "set_pnt")
      {
        const MapPuntos::Indice id_punto= interpretaSize_t(status.GetString());
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(2,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop());
                if(check_range(j,k))
                  {
                    Cad *c= getCad();
                    if(at(j,k)!= nullptr)
		      std::clog << "¡Ojo!, la posición: (" 
                                << j << "," << k 
                                << ") ya estaba asignada al punto: "
                                << at(j,k)->GetNombre() << std::endl;
                    at(j,k)= c->getPuntos().busca(id_punto);
                  }
                else
                  {
                    const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
	            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                              << "'; índices: ("
                              << j << ',' << k << ") fuera de rango;"
                              << " filas: " << fls << " columnas: " << cls
                              << posLectura << std::endl;
                  }
              }
          }
        else
          {
            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                      << "'; faltan los índices del punto."
                      << posLectura << std::endl;
          }
        return true;
      }
    else if(cmd == "pnt")
      {
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(2,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop());
                Pnt *ptr= nullptr;
                if(check_range(j,k))
                  ptr= at(j,k);
                if(ptr)
                  ptr->LeeCmd(status);
                else
                  {
                    Cad *c= getCad();
                    //No usar Nuevo(status) porque
                    //toma el primer índice de la tritriz
                    //como identificador del punto.
                    ptr= c->getPuntos().Nuevo();
                    ptr->LeeCmd(status);
                    at(j,k)= ptr;
                  }
              }
          }
        else
          {
            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                      << "'; faltan los índices del punto."
                      << posLectura << std::endl;
          }
        return true;
      }
    else if(cmd == "for_each_pnt_fila")
      {
        const std::string bloque= status.GetBloque();
        if(parser.TieneIndices())
          {
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(1,cmd))
              {
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop());
                Pnt *ptr= nullptr;
                if(j<=fls)
                  {
                    for(size_t k=1;k<=cls;k++)
                      {
                        ptr= at(j,k);
                        if(ptr)
                          ptr->EjecutaBloque(status,bloque,nombre_clase()+":"+cmd);
                        else
                          {
                            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
                            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                                      << "'; no se encontró el punto de índices: ("
                                      << j << ',' << k << ")."
                                      << posLectura << std::endl;
                          }
                      }
                  }
                else
                  {
                    const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
                    std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                              << "'; fila: "
                              << j << " fuera de rango (> " << fls << ")."
                              << posLectura << std::endl;
                  }
	      } 
          }
        else
          {
            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                      << "'; faltan el índice de la fila."
                      << posLectura << std::endl;
          }
        return true;
      }
    else if(cmd == "for_each_pnt_col")
      {
        if(parser.TieneIndices())
          {
            const std::string bloque= status.GetBloque();
            interpreta(parser.GetIndices());
            if(InterpreteRPN::HayArgumentos(1,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop());
                Pnt *ptr= nullptr;
                if(k<=cls)
                  {
                    for(size_t j=1;j<=fls;j++)
                      {
                        ptr= at(j,k);
                        if(ptr)
                          ptr->EjecutaBloque(status,bloque,nombre_clase()+":"+cmd);
                        else
                          {
                            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
                            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                                      << "'; no se encontró el punto de índices: ("
                                      << j << ',' << k << ")."
                                      << posLectura << std::endl;
                          }
                      }
                  }
                else
                  {
                    const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
                    std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                              << "'; columna: "
                              << k << " fuera de rango (> " << cls << ")."
                              << posLectura << std::endl;
                  }
	      } 
          }
        else
          {
            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
            std::cerr << "(MatrizPtrPnt) Procesando comando: '" << cmd
                      << "'; falta el índice de la columna."
                      << posLectura << std::endl;
          }
        return true;
      }
    else if(cmd == "copia_rango")
      {
        const Record tmp= interpretaRecord(status.GetBloque());
        const std::vector<size_t> rFilas= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoFilas")));
        const std::vector<size_t> rCols= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoColumnas")));
        const std::vector<size_t> offsetIndices= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("offsetIndices")));
        const Vector3d vectorOffset= convert_to_vector3d(convert_to_boost_any(tmp.GetProp("vectorOffset")));
	const std::deque<size_t> tags= CopiaPuntos(RangoMatriz(RangoIndice(rFilas),RangoIndice(rCols)),offsetIndices,vectorOffset);
        return true;
      }
    else
      return MatrizPtrBase<Pnt>::procesa_comando(status);
  }

//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
XC::Pnt *XC::MatrizPtrPnt::buscaPunto(const size_t &tag)
  {
    Pnt *retval= nullptr;
    Pnt *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->GetTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
const XC::Cad *XC::MatrizPtrPnt::getCad(void) const
  {
    const Cad *retval= nullptr;
    const EntCmd *ptr= Owner();
    assert(ptr);
    const MapEsquemas2d *e2d= dynamic_cast<const MapEsquemas2d *>(ptr);
    if(e2d)
      retval= e2d->getCad();
    else //Tritriz.
      {
        const TritrizPtrPnt *t= dynamic_cast<const TritrizPtrPnt *>(ptr);
        retval= t->getCad();
      }
    assert(retval);
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
XC::Cad *XC::MatrizPtrPnt::getCad(void)
  {
    Cad *retval= nullptr;
    EntCmd *ptr= Owner();
    assert(ptr);
    MapEsquemas2d *e2d= dynamic_cast<MapEsquemas2d *>(ptr);
    if(e2d)
      retval= e2d->getCad();
    else //Tritriz.
      {
        TritrizPtrPnt *t= dynamic_cast<TritrizPtrPnt *>(ptr);
        retval= t->getCad();
      }
    assert(retval);
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
const XC::Pnt *XC::MatrizPtrPnt::buscaPunto(const size_t &tag) const
  {
    const Pnt *retval= nullptr;
    const Pnt *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->GetTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve el punto más próximo al punto que se pasa como parámetro.
XC::Pnt *XC::MatrizPtrPnt::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          ptrPnt= operator()(j,k);
          if(ptrPnt)
            {
              tmp= ptrPnt->DistanciaA2(p);
              if(tmp<d)
                {
                  d= tmp;
                  retval= ptrPnt;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve el punto más próximo al punto que se pasa como parámetro.
const XC::Pnt *XC::MatrizPtrPnt::getNearestPnt(const Pos3d &p) const
  {
    MatrizPtrPnt *this_no_const= const_cast<MatrizPtrPnt *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Devuelve una matriz con los identificadores de los puntos.
m_int XC::MatrizPtrPnt::getTags(void) const
  {
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    m_int retval(nfilas,ncols,-1);
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->GetTag();
        }
    return retval;
  }

//! @brief Devuelve el centroide del esquema.
Pos3d XC::MatrizPtrPnt::getCentroide(void) const
  {
    Pos3d retval;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    const double sz= nfilas*ncols;
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            {
              const Pos3d &p= ptr->GetPos();
              x+= p.x();
              y+= p.y();
              z+= p.z();
            }
        }
    x/=sz; y/=sz; z/=sz;
    retval= Pos3d(x,y,z);
    return retval;
  }

//! @brief Copia los puntos del rango que se pasa como parámetro, colocándolos
//! en las posiciones de la matriz que resultan de sumar a los índices (i,j) del
//! punto los valores del vector offsetIndices es decir (i,j)->(i+offsetIndices[0],j+offsetIndices[1])
//! y desplazando su posición geométrica según el vector vectorOffset.
std::deque<size_t> XC::MatrizPtrPnt::CopiaPuntos(const RangoMatriz &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    Cad *cad= getCad();
    std::deque<size_t> retval;
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    for(size_t i= rfilas.Inf();i<=rfilas.Sup();i++)
      for(size_t j= rcols.Inf();j<=rcols.Sup();j++)
        {
          const Pnt *p= operator()(i,j);
          if(p)
            {
              Pnt *nuevo= cad->getPuntos().Copia(p,vectorOffset);
              (*this)(i+offsetIndices[0],j+offsetIndices[1])= nuevo;
              retval.push_back(nuevo->GetTag());
            }
        }
    return retval;
  }  

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MatrizPtrPnt::GetProp(const std::string &cod) const
  {
    if(cod=="size")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="numFilas")
      {
        tmp_gp_szt= getNumFilas();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="numCols")
      {
        tmp_gp_szt= getNumCols();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod == "pnt")
      {
        if(InterpreteRPN::Pila().size()>1)
          {
            const size_t &k= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const Pnt *ptr= (*this)(j,k);
            return any_const_ptr(ptr);
          }
        else
          {
            err_num_argumentos(std::cerr,2,"GetProp",cod);
            return any_const_ptr();
          }
      }
    else if(cod=="at")
      {
        size_t i= 1;
        size_t j= 1;
        static const Pnt *ptr= nullptr;
        if(InterpreteRPN::Pila().size()>1)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= MatrizPtrPnt::operator()(i,j);
          }
        else if(InterpreteRPN::Pila().size()>0)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= MatrizPtrPnt::operator()(i,j);
          }
        else
          {
            ptr= MatrizPtrPnt::operator()(i,j);
          }
        return any_const_ptr(ptr);
      }
    else if(cod=="getTagPnt")
      {
        size_t i= 1;
        size_t j= 1;
        static const Pnt *ptr= nullptr;
        if(InterpreteRPN::Pila().size()>1)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= MatrizPtrPnt::operator()(i,j);
          }
        else if(InterpreteRPN::Pila().size()>0)
          {
            j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            ptr= MatrizPtrPnt::operator()(i,j);
          }
        else
          {
            ptr= MatrizPtrPnt::operator()(i,j);
          }
        if(ptr)
          tmp_gp_int= ptr->GetTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTagNearestPnt")
      {
	const Pos3d p= popPos3d(cod);
        const Pnt *tmp= getNearestPnt(p);
        if(tmp)
          tmp_gp_int= tmp->GetTag();
        else
	  {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró un punto cercano a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod == "getCentroide")
      {
        tmp_gp_pos3d= getCentroide();
        return any_const_ptr(tmp_gp_pos3d);
      }
    else
      return EntCmd::GetProp(cod);
  }
