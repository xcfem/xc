// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ExprInfija.h

#ifndef EXPR_INFIJA_H
#define EXPR_INFIJA_H

//#include <iostream>
#include "Expresion.h"
#include <stack>
#include <string>

class OpBinario;

class ExprInfija: public Expresion
  {
  private:
    std::stack<std::string> op_bin_en_espera; //!< Operadores binarios prefijos en espera de encontrar la coma.
    
    void InsertaPalabra(const std::streampos &pos,Segnal *t);
    std::streampos EncontrarPalabra(const std::streampos &pos,std::istream &is);
    std::streampos EncontrarNumero(const std::streampos &pos,std::istream &is);
    std::streampos EncontrarSimbolo(const std::streampos &pos,std::istream &is,int &cta_parent);
    bool InsertaOpUnario(const std::streampos &pos,Segnal *t);
    void InsertaOpBinario(const std::streampos &pos,Segnal *t);
  protected:
    Segnal *InsertaNuevaPalabra(const std::streampos &pos,const std::string &palabra);
    Segnal *InsertaNeg(const std::streampos &pos,char op);
    void CompruebaParentesis( const std::streampos &pos, int &cta, const Segnal *t);
    virtual void Traduce(std::istream &is);
  public:
    //! @brief Constructor por defecto.
    ExprInfija(void)
      : Expresion(), op_bin_en_espera()
      {}
    //! @brief Constructor por defecto.
    ExprInfija(const std::string &str)
      : Expresion(), op_bin_en_espera()
      { Expresion::operator =(str); }
    //! @brief Constructor.
    ExprInfija(std::istream &is)
      : Expresion(), op_bin_en_espera()
      { Traduce(is); }
    //! @brief Constructor de copia.
    ExprInfija(const ExprInfija &other)
      : Expresion(other), op_bin_en_espera(other.op_bin_en_espera) {}
    ExprInfija &operator=(const ExprInfija &);
    ExprInfija &operator=(const std::string &str);
  };

#endif
