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
//ExprPostfija.cc

#include "ExprPostfija.h"

void ExprPostfija::TrataParentDcho(Pila &pila,const Segnal *t)
  {
    t= pila.Pop();
    while(t->GetClase() != parent_izdo)
      {
        PonerSegnal(t);
        t= pila.Pop();
      }
  }
void ExprPostfija::TrataOpBinario(Pila &pila,const Segnal *t)
  {
    bool fin_derecha= false;
    do
      {
        if (pila.empty()) 
          fin_derecha= true;
        else
          if((*pila.top()).GetClase() == parent_izdo)
            fin_derecha= true;
          else
            if((*pila.top()).GetPrioridad() < t->GetPrioridad())
              fin_derecha= true;
            else
              {
                fin_derecha= false;
                PonerSegnal(pila.Pop());
              }
      }
    while(!fin_derecha);
    pila.push(t);
  }
void ExprPostfija::TrataFinExpr(Pila &pila)
  {
    while(pila.size()>0)
      { PonerSegnal(pila.Pop()); }
  }

void ExprPostfija::Traduce(const ExprInfija &infija)
  {
    borra_lista();//erase(begin(),end());
    if (infija.Vacia()) return;
    Pila pila;
    ExprInfija::const_iterator_segnales i= infija.Begin();
    while(i!= infija.End())
      {
        switch((*i)->GetClase())
          {
          case operando:
            PonerSegnal(*i);
            break;
          case parent_izdo:
            pila.push(*i);
            break;
          case parent_dcho:
            TrataParentDcho(pila,*i);
            break;
          case op_unitario:
          case op_binario:
            TrataOpBinario(pila,*i);
            break;
          case fin_exp:
            break;
          }
        i++;
      }
    TrataFinExpr(pila);
  }

//! @brief Traduce la expresión que se lee desde el istream.
void ExprPostfija::Traduce(std::istream &is)
  {
    ExprInfija infja;
    is >> infja;
    if(infja.ErrorTraduccion())
      {
        std::cerr << "ExprPostfija::Traduce; se produjo un error al interpretar la expresión." << std::endl;
        err_traduc= true;
      }
    else
      Traduce(infja);
  }
