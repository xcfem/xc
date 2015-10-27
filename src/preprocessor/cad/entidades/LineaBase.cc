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
//LineaBase.cc

#include "LineaBase.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "utility/matrix/util_matrix.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"

//! @brief Constructor.
XC::LineaBase::LineaBase(Preprocessor *m,const size_t &nd)
  : Edge(m,nd), p1(nullptr), p2(nullptr) {}

//! @brief Constructor.
XC::LineaBase::LineaBase(const std::string &nombre,Preprocessor *m,const size_t &nd)
  : Edge(nombre,m,nd), p1(nullptr), p2(nullptr) {}

//! @brief Devuelve un apuntador (constante) al punto inicial.
const XC::Pnt *XC::LineaBase::P1(void) const
  { return p1; }
//! @brief Devuelve un apuntador (constante) al punto final.
const XC::Pnt *XC::LineaBase::P2(void) const
  { return p2; }

//! @brief Lee un objeto LineaBase desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - p1: Lee el punto inicial.
//! - p2: Lee el punto final.
bool XC::LineaBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(LineaBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "p1")
      {
        const MapPuntos::Indice id_punto= interpretaSize_t(status.GetString());
        SetVertice(1,BuscaPnt(id_punto));
	if(!p1)
	  std::cerr << "LineaBase; " << cmd << " , no se encontró el punto: '" 
                    << id_punto << "'.\n";
        return true;
      }
    else if(cmd == "p2")
      {
        const MapPuntos::Indice id_punto= interpretaSize_t(status.GetString());
        SetVertice(2,BuscaPnt(id_punto));
	if(!p2)
	  std::cerr << "LineaBase; " << cmd << " , no se encontró el punto: '" 
                    << id_punto << "'.\n";
        return true;
      }
    else if(cmd == "puntos")
      {
        std::vector<MapPuntos::Indice> tmp= crea_vector_size_t(status.GetString());
        if(tmp.size()>1)
          {
            if(tmp[0]==tmp[1])
	      std::cerr << "LineaBase; " << cmd
                   << ", los extremos de la línea coinciden: '["
                   << tmp[0] << "," << tmp[1] << "]'.\n";
            SetVertice(1,BuscaPnt(tmp[0]));
            SetVertice(2,BuscaPnt(tmp[1]));
          }
        else
	  std::cerr << "LineaBase; " << cmd
                   << ", se esperaban los identificadores de dos puntos"
                   << ", se obtuvo: '["
                   << tmp[0] << "," << tmp[1] << "]'.\n";
        return true;
      }
    else
      return Edge::procesa_comando(status);
  }

//! @brief Devuelve el vértice de índice i.
const XC::Pnt *XC::LineaBase::GetVertice(const size_t &i) const
  {
    if(i==1)
      return p1;
    else
      return p2;
  }

//! @brief Asigna el vértice de índice i.
void XC::LineaBase::SetVertice(const size_t &i,Pnt *p)
  {
    if((i==1) && (p1!=p))
      {
        if(p1) p1->borra_linea(this);
        p1= p;
      }
    else if(p2!=p)
      {
        if(p2) p2->borra_linea(this);
        p2= p;
      }
    actualiza_topologia();
  }

//! @brief Asigna el vértice de índice i.
void XC::LineaBase::SetVertice(const size_t &i,const size_t &id_punto)
  {
    SetVertice(i,BuscaPnt(id_punto));
    if(!GetVertice(i))
      std::cerr << "LineaBase; no se encontró el punto: '" 
                << id_punto << "' \n";
  }

//! @brief Asigna los extremos de la línea.
void XC::LineaBase::SetVertices(Pnt *pA,Pnt *pB)
  {
    if(pA==pB)
      std::cerr << "¡Ojo!, LineaBase::SetVertices; "
                << "se asigna el mismo punto a ambos extremos de la linea: '"
                << GetNombre() << "'." << std::endl;
    SetVertice(1,pA);
    SetVertice(2,pB);
    if(p1==p2)
      std::cerr << "LineaBase::SetVertices; ¡ojo!, la linea: '" << GetNombre()
                << "' tiene ambos extremos en el mismo punto."
                << std::endl; 
    actualiza_topologia();
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
//!
//! Soporta las propiedades:
//! -getVtkCellType: Tipo de celda adecuado para VTK.
any_const_ptr XC::LineaBase::GetProp(const std::string &cod) const
  {
    return Edge::GetProp(cod);
  }

