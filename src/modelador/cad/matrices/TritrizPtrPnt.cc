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
//TritrizPtrPnt.cc

#include "TritrizPtrPnt.h"
#include "modelador/cad/entidades/Pnt.h"
#include "xc_basic/src/funciones/algebra/integ_num.h"
#include "xc_utils/src/base/Lista.h"
#include <boost/any.hpp>
#include "domain/domain/Domain.h"
#include "domain/constraints/SP_Constraint.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Poligono3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "boost/lexical_cast.hpp"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "modelador/cad/Cad.h"
#include "xc_utils/src/base/Record.h"
#include "xc_utils/src/geom/pos_vec/RangoTritriz.h"
#include "xc_basic/src/matrices/RangoMatriz.h"
#include "xc_utils/src/nucleo/aux_any.h"

//! @brief Constructor por defecto.
XC::TritrizPtrPnt::TritrizPtrPnt(const size_t capas)
  : TritrizPtrBase<MatrizPtrPnt>(capas) {}

//! @brief Constructor.
XC::TritrizPtrPnt::TritrizPtrPnt(const size_t capas,const size_t filas,const size_t cols)
  : TritrizPtrBase<MatrizPtrPnt>(capas,filas,cols) {}

void XC::TritrizPtrPnt::setPnt(const size_t &i,const size_t &j,const size_t &k,const int &id_punto)
  {
    if(check_range(i,j,k))
      {
        Cad *c= getCad();
        Pnt *tmp= TritrizPtrPnt::operator()(i,j,k);
        if(tmp!= nullptr)
          std::clog << "Warning!, position: (" 
                    << i << "," << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->GetNombre() << std::endl;
        TritrizPtrPnt::operator()(i,j,k)= c->getPuntos().busca(id_punto);
      }
    else
     std::cerr << "(MatrizPtrPnt::setPnt): '"
               << "'; indices: ("
               << i << ','  << j << ',' << k << ") out of range;"
               << " number of layers: " << GetCapas() << " number of rows: " << getNumFilas() << " number of columns: " << getNumCols()
               << std::endl;
  }

XC::Pnt *XC::TritrizPtrPnt::getPnt(const size_t &i,const size_t &j,const size_t &k)
  { return getAtIJK(i,j,k); }


//! @brief Lee un objeto TritrizPtrPnt desde archivo
//! Soporta los comandos:
bool XC::TritrizPtrPnt::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(TritrizPtrPnt) Procesando comando: " << cmd << std::endl;

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
                const size_t i= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                if(check_range(i,j,k))
                  {
                    Cad *c= getCad();
                    Pnt *tmp= TritrizPtrPnt::operator()(i,j,k);
                    if(tmp!= nullptr)
		      std::clog << "¡Ojo!, la posición: (" 
                                << i << "," << j << "," << k 
                                << ") ya estaba asignada al punto: "
                                << tmp->GetNombre() << std::endl;
		    TritrizPtrPnt::operator()(i,j,k)= c->getPuntos().busca(id_punto);
                  }
                else
                  {
                    const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
	            std::cerr << "(TritrizPtrPnt) Procesando comando: '" << cmd
                              << "'; índices: ("
                              << i << ',' << j << ',' << k << ") fuera de rango;"
                              << " capas: " << GetCapas() << " filas: " << getNumFilas() << " columnas: " << getNumCols()
                              << posLectura << std::endl;
                  }
              }
          }
        else
          {
            const std::string posLectura= get_ptr_status()->getPosicionLecturaActual();
            std::cerr << "(TritrizPtrPnt) Procesando comando: '" << cmd
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
            if(InterpreteRPN::HayArgumentos(3,cmd))
              {
                const size_t k= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t j= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                const size_t i= convert_to_size_t(InterpreteRPN::Pila().Pop()); 
                Pnt *ptr= (*this)(i,j,k);
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
                    (*this)(i,j,k)= ptr;
                  }
              }
          }
        else
	  std::cerr << "(TritrizPtrPnt) Procesando comando: '" << cmd
                            << "'; faltan los índices del punto.\n";
        return true;
      }
    else if(cmd=="punto_con_tag")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(fnc_args.size()<1)
          std::cerr << "puntos - uso: puntos(numPuntos) " << std::endl;
        const int tag= convert_to_int(fnc_args[0]); //Tag del punto.
        Pnt *tmp= buscaPunto(tag);
        if(tmp)
          tmp->LeeCmd(status);
        else
          {
	    std::cerr << "(TritrizPtrPnt) Procesando comando: " << cmd
                      << " no se encotró el punto: " << tag
                      << " en este conjunto. Se ignora la entrada." << std::endl;
            status.GetBloque();
          }
        return true;
      }
    else if(cmd == "copia_rango")
      {
        const Record tmp= interpretaRecord(status.GetBloque());
        const std::vector<size_t> rCapas= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoCapas")));
        const std::vector<size_t> rFilas= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoFilas")));
        const std::vector<size_t> rCols= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoColumnas")));
        const std::vector<size_t> offsetIndices= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("offsetIndices")));
        const Vector3d vectorOffset= convert_to_vector3d(convert_to_boost_any(tmp.GetProp("vectorOffset")));
	const std::deque<size_t> tags= CopiaPuntos(RangoTritriz(RangoIndice(rCapas),RangoIndice(rFilas),RangoIndice(rCols)),offsetIndices,vectorOffset);
        return true;
      }
    else if(cmd == "crea_cuadrilateros")
      {
        const Record tmp= interpretaRecord(status.GetBloque());
        const std::vector<size_t> rCapas= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoCapas")));
        const std::vector<size_t> rFilas= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoFilas")));
        const std::vector<size_t> rCols= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("rangoColumnas")));
        const std::vector<size_t> offsetIndices= convert_to_vector_size_t(convert_to_boost_any(tmp.GetProp("offsetIndices")));
	//XXX const std::deque<size_t> tags= CreaCuadrilateros(RangoTritriz(RangoIndice(rCapas),RangoIndice(rFilas),RangoIndice(rCols)),offsetIndices);
        return true;
      }
    else
      return TritrizPtrBase<MatrizPtrPnt>::procesa_comando(status);
  }

//! @brief Devuelve el centroide del esquema.
Pos3d XC::TritrizPtrPnt::getCentroide(void) const
  {
    Pos3d retval;
    const size_t ncapas= GetCapas();
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrPnt &capa= operator()(i);
        Pos3d p= capa.getCentroide();
        x+= p.x();
        y+= p.y();
        z+= p.z(); 
      }
    x/=ncapas; y/=ncapas; z/=ncapas;
    retval= Pos3d(x,y,z);
    return retval;
  }


//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
XC::Pnt *XC::TritrizPtrPnt::buscaPunto(const int &tag)
  {
    Pnt *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrPnt &capa= operator()(i);
        retval= capa.buscaPunto(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
const XC::Cad *XC::TritrizPtrPnt::getCad(void) const
  {
    const Cad *retval= nullptr;
    const EntCmd *ptr= Owner();
    assert(ptr);
    const MapEsquemas3d *e3d= dynamic_cast<const MapEsquemas3d *>(ptr);
    if(e3d)
      retval= e3d->getCad();
    assert(retval);
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
XC::Cad *XC::TritrizPtrPnt::getCad(void)
  {
    Cad *retval= nullptr;
    EntCmd *ptr= Owner();
    assert(ptr);
    MapEsquemas3d *e3d= dynamic_cast<MapEsquemas3d *>(ptr);
    if(e3d)
      retval= e3d->getCad();
    assert(retval);
    return retval;
  }

//! @brief Devuelve el punto más próximo al punto que se pasa como parámetro.
const XC::Pnt *XC::TritrizPtrPnt::getNearestPnt(const Pos3d &p) const
  {
    TritrizPtrPnt *this_no_const= const_cast<TritrizPtrPnt *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Devuelve el punto más próximo al punto que se pasa como parámetro.
XC::Pnt *XC::TritrizPtrPnt::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    const size_t ncapas= GetCapas();
    double d= DBL_MAX;
    double tmp;
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrPnt &capa= operator()(i);
        ptrPnt= capa.getNearestPnt(p);
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

//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
const XC::Pnt *XC::TritrizPtrPnt::buscaPunto(const int &tag) const
  {
    const Pnt *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrPnt &capa= operator()(i);
        retval= capa.buscaPunto(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Copia los puntos del rango que se pasa como parámetro, colocándolos
//! en las posiciones de la matriz que resultan de sumar a los índices (i,j) del
//! punto los valores del vector offsetIndices es decir (i,j)->(i+offsetIndices[0],j+offsetIndices[1])
//! y desplazando su posición geométrica según el vector vectorOffset.
std::deque<size_t> XC::TritrizPtrPnt::CopiaPuntos(const RangoTritriz &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    Cad *cad= getCad();
    std::deque<size_t> retval;
    const RangoIndice &rcapas= rango.GetRangoCapas();
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    for(size_t i= rcapas.Inf();i<=rcapas.Sup();i++)
      for(size_t j= rfilas.Inf();j<=rfilas.Sup();j++)
        for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
          {
            const Pnt *p= operator()(i,j,k);
            if(p)
              {
                Pnt *nuevo= cad->getPuntos().Copia(p,vectorOffset);
                (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= nuevo;
                retval.push_back(nuevo->GetTag());
              }
          }
    return retval;
  }  

//! @brief Devuelve los puntos del rango que se pasa como parámetro.
XC::TritrizPtrPnt XC::TritrizPtrPnt::getRangoPuntos(const RangoTritriz &rango)
  {
    TritrizPtrPnt retval(rango.NumCapas(),rango.NumFilas(),rango.NumCols());
    const RangoIndice &rcapas= rango.GetRangoCapas();
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    const size_t rcapas_inf= rcapas.Inf();
    const size_t rfilas_inf= rfilas.Inf();
    const size_t rcols_inf= rcols.Inf();
    for(size_t i= rcapas_inf;i<=rcapas.Sup();i++)
      for(size_t j= rfilas_inf;j<=rfilas.Sup();j++)
        for(size_t k= rcols_inf;k<=rcols.Sup();k++)
          {
            Pnt *p= operator()(i,j,k);
            if(p)
              retval(i-rcapas_inf+1,j-rfilas_inf+1,k-rcols_inf+1)= p;
          }
    return retval;
  }

//! @brief Devuelve los puntos cuyos índices se pasan como parámetro.
XC::Pnt *XC::TritrizPtrPnt::getPunto(const VIndices &iPunto)
  {
    Pnt *retval= nullptr;
    if(iPunto.size()>2)
      {
        if((iPunto[0]>0) && (iPunto[1]>0) && (iPunto[2]>0))
          { retval= (*this)(iPunto[0],iPunto[1],iPunto[2]); }
      }
    else
      std::cerr << "TritrizPtrPnt::getPunto; el vector de índices: "
                << iPunto << " no es válido." << std::endl;
    return retval;    
  }

//! @brief Devuelve los puntos cuyos índices se pasan como parámetro.
XC::TritrizPtrPnt XC::TritrizPtrPnt::getPuntos(const TritrizIndices &indices)
  {
    const size_t nCapas= indices.GetCapas();
    const size_t nFilas= indices.getNumFilas();
    const size_t nCols= indices.getNumCols();
    TritrizPtrPnt retval(nCapas,nFilas,nCols);
    for(size_t i= 1;i<= nCapas;i++)
      for(size_t j= 1;j<= nFilas;j++)
        for(size_t k= 1;k<= nCapas;k++)
          {
            const VIndices iPunto= indices(i,j,k);
            if(iPunto.size()>2)
              { retval(i,j,k)= getPunto(iPunto); }
            else
	      std::cerr << "TritrizPtrPnt::getPuntos; el vector de índices: "
                        << iPunto << " no es válido." << std::endl;
          }
    return retval;
  }

//! @brief Devuelve los puntos cuyos índices se pasan como parámetro.
XC::MatrizPtrPnt XC::TritrizPtrPnt::getPuntos(const MatrizIndices &indices)
  {
    const size_t nFilas= indices.getNumFilas();
    const size_t nCols= indices.getNumCols();
    MatrizPtrPnt retval(nFilas,nCols);
    for(size_t i= 1;i<= nFilas;i++)
      for(size_t j= 1;j<= nCols;j++)
        {
          const VIndices iPunto= indices(i,j);
          if(iPunto.size()>2)
            { retval(i,j)= getPunto(iPunto); }
          else
            std::cerr << "TritrizPtrPnt::getPuntos; el vector de índices: "
                      << iPunto << " no es válido." << std::endl;
        }
    return retval;
  }

//! @brief Devuelve la celda formada por los puntos que se obtienen de las
//! posiciones de la tritriz que resultan de sumar a los índices (i,j,k) del
//! punto los valores del vector offsetIndices es decir:
//! Punto (i,j,k): (i+offsetIndices(i,j,k)[0],j+offsetIndices(i,j,k)[1],k+offsetIndices(i,j,k)[2])
XC::TritrizPtrPnt XC::TritrizPtrPnt::getCeldaPuntos(const size_t &i,const size_t &j,const size_t &k,const TritrizIndices &offsetIndices)
  {
    VIndices org(3);
    org[0]= i;org[1]= j;org[2]= k;
    TritrizIndices tmp(offsetIndices);
    tmp.Offset(org);
    return getPuntos(tmp);
  }

//! @brief Devuelve la celda formada por los puntos que se obtienen de las
//! posiciones de la tritriz que resultan de sumar a los índices (i,j) del
//! punto los valores del vector offsetIndices es decir:
//! Punto (i,j): (i+offsetIndices(i,j)[0],j+offsetIndices(i,j)[1],k+offsetIndices(i,j)[2])
XC::MatrizPtrPnt XC::TritrizPtrPnt::getCeldaPuntos(const size_t &i,const size_t &j,const MatrizIndices &offsetIndices)
  {
    VIndices org(2);
    org[0]= i;org[1]= j;
    MatrizIndices tmp(offsetIndices);
    tmp.Offset(org);
    return getPuntos(tmp);
  }

// //! @brief Crea superficies cuadriláteras entre los los puntos del rango que se pasa como parámetro, colocándolos
// //! entre las posiciones de la tritriz que resultan de sumar a los índices (i,j) del
// //! punto los valores del vector offsetIndices es decir:
// //! Punto 1: (i+offsetIndices[0,0],j+offsetIndices[0,1],k+offsetIndices[0,2])
// //! Punto 2: (i+offsetIndices[1,0],j+offsetIndices[1,1],k+offsetIndices[1,2])
// //! Punto 3: (i+offsetIndices[2,0],j+offsetIndices[2,1],k+offsetIndices[2,2])
// //! ...
// //! @param nf: Número de filas de la matriz de apuntadores a punto.
// //! @param nc: Número de columnas de la matriz de apuntadores a punto.
// std::deque<size_t> XC::TritrizPtrPnt::CreaCuadrilateros(const RangoTritriz &rango,const size_t &nf,const size_t &nc,const m_int &offsetIndices,const double &elemSizeI,const double &elemeSizeJ)
//   {
//     Cad *cad= getCad();
//     std::deque<size_t> retval;
//     const RangoIndice &rcapas= rango.GetRangoCapas();
//     const RangoIndice &rfilas= rango.GetRangoFilas();
//     const RangoIndice &rcols= rango.GetRangoCols();
//     for(size_t i= rcapas.Inf();i<=rcapas.Sup();i++)
//       for(size_t j= rfilas.Inf();j<=rfilas.Sup();j++)
//         for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
//           {
//             const Pnt *p= operator()(i,j,k);
//             if(p)
//               {
//                 Pnt *nuevo= cad->getPuntos().Copia(p,vectorOffset);
//                 (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= nuevo;
//                 retval.push_back(nuevo->GetTag());
//               }
//           }
//     return retval;
//   }  

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::TritrizPtrPnt::GetProp(const std::string &cod) const
  {
    if(cod=="size")
      {
        tmp_gp_szt= NumPtrs();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="numCapas")
      {
        tmp_gp_szt= GetCapas();
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
    else if(cod == "punto")
      {
        if(InterpreteRPN::Pila().size()>2)
          {
            const size_t &k= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const Pnt *ptr= (*this)(i,j,k);
            return any_const_ptr(ptr);
          }
        else
          {
            err_num_argumentos(std::cerr,3,"GetProp",cod);
            return any_const_ptr();
          }
      }
    else if(cod == "getTagPnt")
      {
        if(InterpreteRPN::Pila().size()>2)
          {
            const size_t &k= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &j= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const size_t &i= convert_to_size_t(InterpreteRPN::Pila().Pop());
            const Pnt *ptr= (*this)(i,j,k);
            tmp_gp_int= ptr->GetTag();
            return any_const_ptr(tmp_gp_int);
          }
        else
          {
            err_num_argumentos(std::cerr,3,"GetProp",cod);
            return any_const_ptr();
          }
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
    else
      return EntCmd::GetProp(cod);
  }

void XC::TritrizPtrPnt::Print(std::ostream &os) const
  {
    const size_t ncapas= GetCapas();
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t i=1;i<=ncapas;i++)
      {
        for(size_t j=1;j<=nfilas;j++)
          {
            for(size_t k=1;k<=ncols;k++)
	      os << (*this)(i,j,k)->GetTag() << " ";
	    os << std::endl;
          }
        os << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os, const TritrizPtrPnt &t)
  {
    t.Print(os);
    return os;
  }

//! @brief Devuelve los índices de los puntos (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<size_t> XC::getIdPuntosQuad(const TritrizPtrPnt::const_ref_capa_i_cte &puntos,const size_t &j,const size_t &k)
  {
    std::vector<size_t> retval(4,-1);
    const size_t nfilas= puntos.getNumFilas();
    const size_t ncols= puntos.getNumCols();
    if(j>=nfilas)
      {
        std::cerr << "getIdPuntosQuad; índice de fila j= " << j << " fuera de rango.\n";
        return retval;
      }
    if(k>=ncols)
      {
        std::cerr << "getIdPuntosQuad; índice de columna k= " << k << " fuera de rango.\n";
        return retval;
      }


    Pos3d p1;
    const Pnt *ptr= puntos(j,k);
    if(ptr)
      {
        retval[0]= ptr->GetTag();
        if(retval[0]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j << ',' << k << ").\n";
        p1= ptr->GetPos();
      }

    Pos3d p2;
    ptr= puntos(j,k+1);
    if(ptr)
      {
        retval[1]= ptr->GetTag();
        if(retval[1]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j << ',' << k+1 << ").\n";
        p2= ptr->GetPos();
      }

    Pos3d p3;
    ptr= puntos(j+1,k+1);
    if(ptr)
      {
        retval[2]= ptr->GetTag();
        if(retval[2]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j+1 << ',' << k+1 << ").\n";
        p3= ptr->GetPos();
      }

    Pos3d p4;
    ptr= puntos(j+1,k);
    if(ptr)
      {
        retval[3]=ptr->GetTag();
        if(retval[3]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j+1 << ',' << k << ").\n";
        p4= ptr->GetPos();
      }

//     const Vector3d v2= p2-p1;
//     const Vector3d v3= p3-p2;
//     const Vector3d v4= p4-p3;
//     const Vector3d v1= p1-p4;
//     const double d1= dot(v1,v3);
//     const double d2= dot(v2,v4);
//     if((d1>0))
//       {
//         std::swap(p3,p2);
//         std::swap(retval[2],retval[1]);
//       }
//     if((d2>0))
//       {
// 	std::swap(p3,p4);
//         std::swap(retval[2],retval[3]);
//       }

    std::list<Pos3d> posiciones;
    posiciones.push_back(p1);
    posiciones.push_back(p2);
    posiciones.push_back(p3);
    posiciones.push_back(p4);
    Poligono3d tmp(posiciones.begin(),posiciones.end());
    const double area= tmp.Area();
    if(area<1e-3)
      {
        std::cerr << "Al obtener la celda de índices (" << j << ',' << k
                  << ") se obtuvo un área muy pequeña (" << area << ").\n";
        std::cerr << " posición del punto (j,k) " << p1 << std::endl;
	std::cerr << " posición del punto (j+1,k) " << p2 << std::endl;
	std::cerr << " posición del punto (j+1,k+1) " << p3 << std::endl;
	std::cerr << " posición del punto (1,k+1) " << p4 << std::endl;
      }
    return retval;
  }

