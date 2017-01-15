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
//Block.h
//Objeto superficie del preprocesador.

#ifndef BLOCK_H
#define BLOCK_H

#include "Body.h"

namespace XC {

//! \ingroup CadEnt
//!
//! @brief Representación de un sólido de seis caras.
class Block: public Body
  {
    Cara sups[6];
    size_t indice(Face *s) const;
    void coloca(const size_t &i,Face *s);
  protected:
    void inserta(const size_t &);
    //XXX void add_puntos(const std::vector<size_t> &);
    void add_caras(const std::vector<size_t> &);
    void crea_nodos_caras(void);
    TritrizPos3d get_posiciones(void) const;

    virtual Cara *GetCara(const size_t &i);
  public:
    Block(Preprocessor *m,const std::string &nombre= "");
    SetEstruct *getCopy(void) const;
    //! @brief Devuelve el número de líneas del objeto.
    virtual size_t NumLineas(void) const;
    virtual size_t NumVertices(void) const;
    virtual size_t NumCaras(void) const;
    void actualiza_topologia(void);
    virtual std::set<const Face *> GetSuperficies(void);
    const Cara *GetCara(const size_t &i) const;
    const CmbEdge::Lado *GetArista(const size_t &i) const;
    Pnt *GetVertice(const size_t &i);
    const Pnt *GetVertice(const size_t &i) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    size_t NDivI(void) const;
    size_t NDivJ(void) const;
    size_t NDivK(void) const;
    bool checkNDivs(void) const;
    void crea_nodos(void);
    void Malla(dir_mallado dm);


  };


} //end of XC namespace

#endif
