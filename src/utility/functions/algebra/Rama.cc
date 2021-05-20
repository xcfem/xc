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
//Rama.cc
#include "Rama.h"
#include "Operando.h"
#include "OpUnario.h"
#include "ExprFunctor.h"
#include "Lexico.h"
#include <cmath>
#include <iostream>
#include <cassert>

const double &Rama::no_numero= NAN;

//! @brief Constructor.
Rama::Rama(const Operando *s,Rama *i,Rama *j)
  : data(s),izdo(i), dcho(j) 
  {
    Lex().NuevaRefLiteral(data);
  }

//! @brief Constructor.
Rama::Rama(const double &d)
  : data(Lex().NuevoLiteral(d)), izdo(NULL), dcho(NULL) {}

//! @brief Constructor de copia.
Rama::Rama(const Rama &other)
  : data(other.data)
  {
    if(other.izdo) izdo= other.izdo->getCopy();
    if(other.dcho) dcho= other.dcho->getCopy();
    Lex().NuevaRefLiteral(data);
  }

//! @brief Operador asignación.
Rama &Rama::operator=(const Rama &other)
  {
    Borra();
    data= other.data;
    if(other.izdo) izdo= other.izdo->getCopy();
    if(other.dcho) dcho= other.dcho->getCopy();
    Lex().NuevaRefLiteral(data);
    return *this;
  }

//! @brief Comparison operator.
bool Rama::operator==(const Rama &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
	bool eq_izdo= false;
	bool eq_dcho= false;
	if(this->data == other.data)
	  {
	    if(this->izdo)
	      if(other.izdo)
		eq_izdo= ((*this->izdo) == (*other.izdo));
	      else
		return false;
	    else
	      if(other.izdo) return false;
	    if(this->dcho)
	      if(other.dcho)
		eq_izdo= ((*this->dcho) == (*other.dcho));
	      else
		return false;
	    else
	      if(other.dcho) return false;
	  }
	retval= (eq_izdo && eq_dcho);
      } 
    return retval;
  }

bool Rama::operator==(const double &d) const
  {
    if(this->data)
      return (this->data->EqualTo(d));
    else
      return false;
  }

//! @brief Destructor.
Rama::~Rama(void)
  {
    Lex().BorraSiLiteral(data);
    Borra();
  }

Clase Rama::GetClase(void) const
  { return data->GetClase(); }
bool Rama::EsToNum(void) const
  {
    bool retval= false;
    if(GetClase()==op_unitario)
      if(data->getName()==OpToNum().getName())
        retval= true;
    return retval;
  }
const std::string &Rama::StrClase(void) const
  { return data->StrClase(); }
int Rama::GetPrioridad(void) const
  { return data->GetPrioridad(); }

Rama *ContraeIzdo(Rama *raiz)
  {
    raiz->BorraIzdo(); //Borra toda la rama izquierda.
    Rama *retval= raiz->dcho;
    raiz->dcho= NULL;
    delete raiz;
    raiz= NULL;
    return retval;
  }
Rama *ContraeDcho(Rama *raiz)
  {
    raiz->BorraDcho(); //hay que borrar toda la rama.
    Rama *retval= raiz->izdo;
    raiz->izdo= NULL;
    delete raiz;
    raiz= NULL;
    return retval;
  }

//! @brief Ejecuta el operador de éste objeto.
const Operando &Rama::Opera(void)
  {
    if(izdo) //Es operador binario.
      return data->Opera(izdo->data,dcho->data);
    else
      if(dcho) //Es operador unario.
        return data->Opera(NULL,dcho->data);
      else //Es valor.
        return data->Opera(NULL,NULL);
  }
//! @brief Ejecuta todos los operadores desde raíz.
void Rama::Opera(Rama *raiz)
  {
    if(raiz->Evaluable())
      {
        raiz->data= Lex().NuevoLiteral(raiz->Opera().GetValor());
        Lex().BorraSiLiteral(raiz->data); //Para no incrementar otra vez el contador de referencias a literal.
        raiz->BorraHijos();
      }
  }
//! @brief Devuelve verdadero si la rama es evaluable.
bool Rama::Evaluable(void) const
  {
    if(data->Evaluable()) return true; //Es evaluable directamente.
    if(dcho)
      {
        if(izdo) //Existe la rama izquierda y por tanto debe existir la derecha (op. binario).
          if(izdo->Evaluable())
            return dcho->Evaluable();
          else //El izquierdo existe y no es evaluable.
            return false;
        else //No existe la rama izquierda (op unario).
          return dcho->Evaluable();
      }
    else
      return false;
  }

Rama diferencia(const Rama &raiz,const Variable &v)
  {
    if(raiz.GetData())
      return raiz.GetData()->Diferencia(v,raiz);
    else
      {
        std::cerr << "Se intentó diferenciar un operando nulo." << std::endl;
        return Rama();
      }
  }
Rama *simplifica(Rama *raiz)
  {
    if(raiz)
      {
        raiz->PutDcho(simplifica(raiz->getRight()));
        raiz->PutIzdo(simplifica(raiz->getLeft()));
        if(raiz->GetData())
          return raiz= raiz->GetData()->Simplifica(raiz);
        else
          std::cerr << "Se intentó simplificar un operando nulo." << std::endl;
      }
    return raiz;
  }

//! @breif Aplica la propiedad distributiva a través del arbol.
Rama *distrib(Rama *raiz)
  {
    if(raiz)
      {
        Rama *ri= raiz->getLeft();
        if(ri)
          {
            Rama *rid= distrib(ri);
            raiz->izdo= rid;
          }
        Rama *rd= raiz->getRight();
        if(rd)
          {
            Rama *rdd= distrib(rd);
            raiz->dcho= rdd;
          }
        const Operando *data= raiz->GetData();
        if(data)
          raiz= data->Distrib(raiz);
        else
          std::cerr << "Se intentó distribuir un operando nulo." << std::endl;
      }
    return raiz;
  }

void RecorrePreorden(Rama *raiz,const ExprFunctor &f)
  {
    if(raiz)
      {
        f(raiz);
        RecorrePreorden(raiz->getLeft(),f);
        RecorrePreorden(raiz->getRight(),f);
      }
  }
void RecorreEnorden(Rama *raiz,const ExprFunctor &f)
  {
    if(raiz)
      {
        RecorreEnorden(raiz->getLeft(),f);
        f(raiz);
        RecorreEnorden(raiz->getRight(),f);
      }
  }
void RecorrePostorden(Rama *raiz,const ExprFunctor &f)
  {
    if(raiz)
      {
        RecorrePostorden(raiz->getLeft(),f);
        RecorrePostorden(raiz->getRight(),f);
        f(raiz);
      }
  }

void Escribe(const Rama *raiz,const int &prior, std::ostream &stream)
  {
    if(raiz)
      {
        //int parent= ( (raiz->GetPrioridad() <= prior) && (raiz->GetClase()!=operando));
        bool parent= true;
        if(parent) stream << '(';
        Escribe(raiz->getLeft(),raiz->GetPrioridad(),stream);
        stream << *raiz;
        Escribe(raiz->getRight(),raiz->GetPrioridad(),stream);
        if(parent) stream << ')';
      }
  }

//! @brief Devuelve una cadena de caracteres que representa a la expresión
//! representando los literales con la máxima precisión posible.
const std::string &Rama::GetFullString(void) const
  {
    static std::string retval= "";

    std::string str_izdo;
    int prioridad_izdo= 100;
    if(izdo)
      {
        str_izdo= retval= izdo->GetFullString();
        prioridad_izdo= izdo->GetPrioridad();
      }

    std::string str_data;
    int prioridad_data= 100;
    if(data)
      {
        str_data= data->GetFullString();
        prioridad_data= data->GetPrioridad();
      }

    std::string str_dcho;
    int prioridad_dcho= 100;
    if(dcho)
      {
        str_dcho= retval= dcho->GetFullString();
        prioridad_dcho= dcho->GetPrioridad();
      }

    if(data)
      {
        const Posicion pos= data->GetPosicion();
        switch(pos)
          {
          case prefija:
            retval= str_data;
            if(data->GetClase()==op_unitario)
              retval+= "(" + str_dcho + ")";
            else
              if(data->GetClase()==op_binario) //Operador binario prefijo (max,min,...)
                retval+= "(" + str_izdo + "," + str_dcho + ")";
              else
                retval+= str_izdo + str_dcho;
            break;
          case infija:
            if(prioridad_dcho<prioridad_data)
              str_dcho= "(" + str_dcho + ")";
            if(prioridad_izdo<prioridad_data)
              str_izdo= "(" + str_izdo + ")";
            retval= str_izdo + str_data + str_dcho;
            break;
          case sufija:
            retval= str_izdo + str_dcho + str_data;
            break;
          }
      }
    else
      retval= str_izdo+str_dcho;
    return retval;
  }


void Rama::GetVariables(ConjVariables &cv) const
  { data->GetVariables(cv); }

std::ostream &operator <<(std::ostream &stream,const Rama &e)
  {
    if(e.data) stream << (*e.data);
    return stream;
  }
