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
//MapLineas.h

#ifndef MAPLineas_H
#define MAPLineas_H

#include "MapEnt.h"
#include "Edge.h"

class Recta3d;
class Plano3d;

namespace XC {

class Edge;
class Linea;
class LineaTramos;
class CmbEdge;
class ArcoCircunf;

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapLineas: public MapEnt<Edge>
  {
  private:
    void UpdateSets(Edge *) const;
  protected:
    template <class E>
    Edge *Crea(void);


  public:
    MapLineas(Cad *cad= NULL);

    Vector3d getVector(const Indice &i,const Indice &j) const;
    Recta3d getRecta(const Indice &i,const Indice &j) const;
    Plano3d getPlano(const Indice &i,const Indice &j,const Indice &k) const;

    template <class E>
    Edge *Nueva(void);
    Edge *Line(Pnt *,Pnt *);
    Edge *DividedLine(Pnt *,Pnt *);
    Edge *Arc(Pnt *,Pnt *,Pnt *);
    Edge *LineSequence(void);
    Edge *Copia(const Edge *l);
 
    Linea *newLine(const size_t &, const size_t &);
    LineaTramos *newDividedLine(const size_t &, const size_t &);
    ArcoCircunf *newCircleArc(const size_t &, const size_t &, const size_t &);
    CmbEdge *newLineSequence(void);
  };

//! @brief Crea una nueva línea.
template <class E>
Edge *XC::MapLineas::Crea(void)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    Edge *retval= new E(preprocessor);
    retval->Nombre()= "l"+boost::lexical_cast<std::string>(getTag());
    (*this)[getTag()]= retval;
    UpdateSets(retval);
    tag++;
    return retval;
  }

//! @brief Crea un nuevo Edge.
template <class E>
Edge *MapLineas::Nueva(void)
  {
    Edge *retval= busca(getTag());
    if(!retval) //El edge es nuevo.
      retval= Crea<E>();
    return retval;
  }

} //end of XC namespace
#endif
