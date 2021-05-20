//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//Expresion.cc

#include "Expresion.h"
#include "Lexico.h"
#include <sstream>

void Expresion::borra_lista(void)
  {
    l_Expr::const_iterator i;
    for(i= lista.begin();i!= lista.end();i++)
      Lex().BorraSiLiteral(*i);
    lista.erase(lista.begin(),lista.end());
  }

void Expresion::update_nueva_lista(void)
  {
    l_Expr::const_iterator i;
    for(i= lista.begin();i!= lista.end();i++)
      Lex().NuevaRefLiteral(*i);
  }

//! @brief Inserts a literal on the list, this functions exists
//! to avoid calling PonerSegnal
//! and to increment by error, the literal reference counter.
void Expresion::PonerDouble(const double &d)
  { lista.push_back(const_cast<Operando *>(Lex().NuevoLiteral(d))); }

//! @brief Inserta una señal en la lista.
void Expresion::PonerSegnal(const Segnal *s)
  {
    if(s)
      {
        lista.push_back(const_cast<Segnal *>(s)); //Chapucilla.
        Lex().NuevaRefLiteral(s);
      }
    else
      std::cerr << "Se intento insertar señal nula." << std::endl;
  }

//! Devuelve verdadero si
//! -La expresión está vacía.
//! -El último valor es una de estas dos cosas
//!       - un paréntesis izquierdo.
//!       - un operador.
bool Expresion::Inicial(void)
 {
   if(lista.empty())
     return true;
   else
     {
       Clase k= lista.back()->GetClase();
       return ( (k == parent_izdo) || (k == op_unitario) || (k == op_binario) );
     }
 }


Expresion::~Expresion(void)
  {  borra_lista(); }

//! @brief Crea la expresión a partir de la cadena de caracteres que se pasa como parámetro.
void Expresion::Traduce(const std::string &str)
  {
    if(!str.empty())
      {
        std::istringstream iss_traduce(str);
        Traduce(iss_traduce);
        if(ErrorTraduccion())
          std::cerr << "Expresion::Traduce; se produjo un error al interpretar la expresión: '"
                    << str << "'\n";
      }
  }

//! @brief Operador salida.
std::ostream &operator <<(std::ostream &stream,const Expresion &expr)
  {
    Expresion::l_Expr::const_iterator i;
    for(i= expr.lista.begin();i!= expr.lista.end();i++)
      stream << **i << ' ';
    return stream;
  }
