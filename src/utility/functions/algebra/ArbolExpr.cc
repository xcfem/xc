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
//ArbolExpr.cc
#include "Expresion.h"
#include "ArbolExpr.h"
#include "LexAlgebra.h"
#include "ConjVariables.h"
#include "ExprPostfija.h"
#include "ExprFunctor.h"
#include "utility/utils/text/text_string.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include "MapValores.h"

//! @brief Constructor por defecto.
ArbolExpr::ArbolExpr(void): ExprBase(false), raiz(nullptr) {}

//! @brief Constructor a partir de text string.
ArbolExpr::ArbolExpr(const std::string &str)
  : ExprBase(false), raiz(nullptr)
  { InicFromStr(str); }

//! @brief Constructor de copia.
ArbolExpr::ArbolExpr(const ArbolExpr &otro)
  : ExprBase(otro), raiz(nullptr) 
  {
    if(otro.raiz)
      raiz= otro.raiz->getCopy();
  }

//! @brief Constructor a partir de double.
ArbolExpr::ArbolExpr(const double &d)
  : ExprBase(false), raiz(new Rama(d)) {}

ArbolExpr::~ArbolExpr(void)
  {
    if(raiz)
      {
        delete raiz;
        raiz= nullptr;
      }
  }

//! @brief Comparison operator.
bool ArbolExpr::operator==(const ArbolExpr &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= ExprBase::operator==(other);
        if(this->raiz==other.raiz)
	  retval= true;
        else if((this->raiz) && (other.raiz)) //Not null
	  retval= this->raiz->operator==(*other.raiz);
	else
	  retval= false;
      } 
    return retval;
  }

//! @brief Operador asignación.
ArbolExpr &ArbolExpr::operator=(const ArbolExpr &otro)
  {
    ExprBase::operator=(otro);
    delete raiz;
    raiz= nullptr;
    if(otro.raiz) raiz= otro.raiz->getCopy();
    return *this;
  }

Rama *ArbolExpr::Traduce(const std::string &cadena_entrada)
  {
    Rama *retval= nullptr;
    ExprPostfija postfija(cadena_entrada);
    if(postfija.ErrorTraduccion())
      {
        std::cerr << "ArbolExpr::Traduce; se produjo un error al interpretar la expresión: '"
                  << cadena_entrada << "', se devuelve nullptr." << std::endl;
        err_traduc= true;
      }
    else
      retval= Traduce(postfija);
    return retval;
  }

Rama *ArbolExpr::Traduce(const ExprPostfija &e)
  {
    if (e.Size() == 0) return nullptr;
    Pila pila;
    ExprPostfija::const_iterator_segnales i= e.Begin();
    const Operando *s= nullptr;
    while(i != e.End())
      {
        s= dynamic_cast<const Operando *>(*i);
        i++;
        switch(s->GetClase())
          {
	  case operando:
            InsertaOperando(pila,s);
            break;
	  case op_unitario:
            InsertaOpUnario(pila,s);
            break;
          case op_binario:
            InsertaOpBinario(pila,s);
            break;
	  case parent_dcho:
          case parent_izdo:
	  case fin_exp:
            break;
          }
      }
    return FinExpresion(pila);
  }

void ArbolExpr::InicFromStr(const std::string &str)
  {
    if(raiz)
      {
        delete raiz;
        raiz= nullptr;
      }
    if(isNumber(str)) //La cadena representa un número (no una expresión)
      raiz= new Rama(boost::lexical_cast<double>(str));
    else
      {
        raiz= Traduce(str);
        OperaToNum(); //Procesa los operadores ToNum;
      }
  }


void ArbolExpr::RecorrePreorden(const ExprFunctor &f)
  { if(raiz) ::RecorrePreorden(raiz,f); }
void ArbolExpr::RecorreEnorden(const ExprFunctor &f)
  { if(raiz) ::RecorreEnorden(raiz,f); }
void ArbolExpr::RecorrePostorden(const ExprFunctor &f)
  { if(raiz) ::RecorrePostorden(raiz,f); }
void ArbolExpr::RecorrePreorden(const ExprFunctor &f) const
  { if(raiz) ::RecorrePreorden(raiz,f); }
void ArbolExpr::RecorreEnorden(const ExprFunctor &f) const
  { if(raiz) ::RecorreEnorden(raiz,f); }
void ArbolExpr::RecorrePostorden(const ExprFunctor &f) const
  { if(raiz) ::RecorrePostorden(raiz,f); }


void ArbolExpr::Asigna(const std::string &palabra,const ArbolExpr &a)
  {
    Variable *v= CalcDirVar(palabra);
//     if(!v)
//       std::cerr << "¡Ojo! no se encontró la variable: " << palabra << std::endl;
    if(v && raiz)
      {
        if(raiz->GetData() == v) //Es la propia raiz.
          operator=(a);
        else
          {
            FctorAsigna fa(v,a.raiz);
            RecorrePostorden(fa);
          }
      }
  }

//! @brief Opera todo el árbol.
void ArbolExpr::Opera(void)
  {
    static FctorOpera f;
    RecorrePostorden(f);
  }

//! @brief Procesa los operadores to_num que haya en la expresión.
void ArbolExpr::OperaToNum(void)
  {
     static FctorOperaToNum ftn;
     RecorrePostorden(ftn);
  }

const double &ArbolExpr::ToNum(void) const
  //Devuelve un doble si puede.
  {
    ArbolExpr tmp(*this);
    tmp.Opera();
    const double &retval= tmp.GetValor();
    if(std::isnan(retval))
      std::cerr << "¡Ojo! No se pudo evaluar la expresión: " << *this << std::endl;
    return retval;
  }

//! @brief Devuelve el valor numérico (si puede).
const double &ArbolExpr::ToNum(const std::string &palabra,const double &d) const
  {
    Asigna(palabra,d);
    return ToNum();
  }

const double &ArbolExpr::ToNum(const MapValores &mv) const
  {
    ArbolExpr tmp(*this);
    tmp.Opera();
    for(MapValores::const_iterator i= mv.begin();i!=mv.end();i++)
      tmp.Asigna((*i).first,(*i).second);
    return tmp.ToNum();
  }

//! @brief Cambia de signo la expresión contenida en el árbol.
void ArbolExpr::Neg(void)
  {
    Rama *tmp= raiz;
    raiz= new Rama(&(ptr_lex->neg),nullptr,tmp);
  }

//! @brief Aplica el operador "valor absoluto" a la expresión contenida en el árbol.
void ArbolExpr::Abs(void)
  {
    Rama *tmp= raiz;
    raiz= new Rama(&(ptr_lex->abs),nullptr,tmp);
  }

//! @brief Aplica el operador "raíz cuadrada" a la expresión contenida en el árbol.
void ArbolExpr::Sqrt(void)
  {
    Rama *tmp= raiz;
    raiz= new Rama(&(ptr_lex->raiz2),nullptr,tmp);
  }


//! @brief Diferencia la expresión respecto a la variable cuyo identificador se pasa como parámetro.
void ArbolExpr::Dif(const std::string &var)
  {
    if(raiz)
      {
        Variable *v= CalcDirVar(var);
        if(v)
          {
            Rama tmp= diferencia(*raiz,*v);
            delete raiz;
            raiz= tmp.getCopy();
          }
      }
  }

ArbolExpr &ArbolExpr::operator+=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->suma),a2); }
ArbolExpr &ArbolExpr::operator-=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->resta),a2); }
ArbolExpr &ArbolExpr::operator*=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->prodt),a2); }
ArbolExpr &ArbolExpr::operator/=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->divn),a2); }
ArbolExpr &ArbolExpr::pow(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->pot),a2); }

ArbolExpr &ArbolExpr::AplicaOperador(const Operador *op,const ArbolExpr &a2)
  {
    Rama *tmp= raiz;
    raiz= new Rama(op,tmp,a2.raiz->getCopy());
    return *this;
  }
//! @brief Devuelve el conjunto de variables que aparecen en la expresión.
ConjVariables ArbolExpr::Variables() const
  {
    ConjVariables cvar;
    if(raiz)
      {
        FctorGetVariables fa(cvar);
        RecorrePostorden(fa);
      }
    return cvar;
  }

//! @brief Return true if the expression depends on the variable name
//! being passed as parameter.
bool ArbolExpr::TieneVariable(const std::string &palabra) const
  {
    const Variable *v= CalcDirVar(palabra);
    return TieneVariable(*v);
  }
//! @brief Devuelve verdadero si la expresión depende de la variable que
//! se pasa como parámetro.
bool ArbolExpr::TieneVariable(const VarExpr &var) const
  {
    const Variable *v= dynamic_cast<const Variable *>(&var);
    return TieneVariable(*v);
  }
//! @brief Devuelve verdadero si la expresión depende de la variable que
//! se pasa como parámetro.
bool ArbolExpr::TieneVariable(const Variable &var) const
  {
    ConjVariables vars= Variables();
    return vars.In(&var);
  }

//! @brief Devuelve el número de variables que intervienen en la expresión.
int ArbolExpr::NumVariables(void) const
  { return Variables().size(); }

//! @brief Return the names of the variables.
std::vector<std::string> ArbolExpr::getNamesOfVariables(void) const
  { return Variables().getNames(); }

//! @brief Imprime la expresión en el stream que se pasa como parámetro.
void ArbolExpr::Print(std::ostream &os) const
  { os << GetFullString(); }

//! @brief Devuelve una text string con el contenido de la expresión.
std::string ArbolExpr::GetString(void) const
  {
    std::ostringstream retval;
    Print(retval);
    return retval.str();
  }
//! @brief Devuelve una text string con el contenido de la expresión
//! representando los valores reales con toda la precisión posible.
const std::string &ArbolExpr::GetFullString(void) const
  {
    static std::string retval;
    if(raiz) retval= raiz->GetFullString();
    return retval;
  }

//! @brief Inserta en stream de salida.
std::ostream &operator<<(std::ostream &os,const ArbolExpr &a)
  {
    //a.Print(stream);
    os << a.GetFullString();
    return os;
  }

//! @brief Lee del stream de entrada una expresión entre comillas simples.
std::istream &operator>>(std::istream &is,ArbolExpr &a)
  {
    char c;
    unsigned short int conta= 0; //Cuenta las comillas simples.
    std::string tmp= "";
    while(!is.eof())
      {
        is >> c;
        if(c == '\'')
          {
            conta++;
            if(conta>1) break;
          }
        else
          if(conta==1)
            tmp+= c;
      }
    if(conta<2)
      std::cerr << "Error en lectura de expresión, faltan comillas simples." << std::endl;
    a= ArbolExpr(tmp);
    return is;
  }

//! @brief Devuelve el valor absoluto del argumento.
ArbolExpr abs(const ArbolExpr &otro)
  {
    ArbolExpr retval(otro);
    retval.Abs();
    return retval;
  }

//! @brief Devuelve la raíz cuadrada del argumento.
ArbolExpr sqrt(const ArbolExpr &otro)
  {
    ArbolExpr retval(otro);
    retval.Sqrt();
    return retval;
  }
