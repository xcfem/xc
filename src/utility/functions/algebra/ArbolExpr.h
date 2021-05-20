// -*-c++-*-
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
//ArbolExpr.h

#ifndef ARBOLEXPR_H
#define ARBOLEXPR_H

#include "Rama.h"
#include <iostream>
#include <stack>
#include <vector>
#include "ExprBase.h"
#include "PilaPunteros.h"

class Operador;
class ConjVariables;
class ExprPostfija;
class VarExpr;
class ExprFunctor;
class MapValores;

//! @brief Árbol que representa una expresión matemática.
class ArbolExpr : public ExprBase
  {
  private:
    friend class Lexico; //!< Léxico para interpretar la expresión.

    Rama *raiz;

    typedef PilaPunteros<Rama> Pila;
    Rama *NuevoNodo(const Operando *s, Rama *izdo= NULL, Rama *dcho= NULL)
      {
        Rama *nuevo_nodo= new Rama(s,izdo,dcho);
        return nuevo_nodo;
      }
    inline void InsertaOperando(Pila &pila,const Operando *s)
      {
        Rama *opndo= NuevoNodo(s,NULL,NULL);
        pila.push(opndo);
      }
    inline void InsertaOpUnario(Pila &pila,const Operando *s)
      {
        Rama *opndo= pila.Pop();
        Rama *opuni= NuevoNodo(s,NULL,opndo);
        pila.push(opuni);
      }
    inline void InsertaOpBinario(Pila &pila,const Operando *s)
      {
        Rama *opndo2= pila.Pop();
        Rama *opndo1= pila.Pop();
        Rama *opbin= NuevoNodo(s,opndo1,opndo2);
        pila.push(opbin);
      }
    inline Rama *FinExpresion(Pila &pila)
      {
        Rama *retval= pila.Pop();
        if (pila.size() > 0) std::cerr << "Error en la expresion" << std::endl;
        return retval;
      }
  protected:
    Rama *Traduce(const ExprPostfija &e);
    Rama *Traduce(const std::string &cadena_entrada);
    void InicFromStr(const std::string &str);
    void Opera(void);
    void OperaToNum(void);
    inline const double &GetValor(void) const
      { return raiz->GetValor(); }
    ArbolExpr &AplicaOperador(const Operador *op,const ArbolExpr &a2);
    void Neg(void);
    void Abs(void);
    void Sqrt(void);
    void Dif(const std::string &var);
    inline void Simplifica(void)
      { if(raiz) raiz= simplifica(raiz); }
    inline void Distrib(void)
      { if(raiz) raiz= distrib(raiz); }
  public:
    ArbolExpr(void); 
    ArbolExpr(const std::string &str);
    ArbolExpr(const ArbolExpr &otro);
    explicit ArbolExpr(const double &d);
    virtual bool operator==(const ArbolExpr &) const;
    ArbolExpr &operator=(const ArbolExpr &otro);
    ArbolExpr &operator+=(const ArbolExpr &a2);
    ArbolExpr &operator-=(const ArbolExpr &a2);
    ArbolExpr &operator*=(const ArbolExpr &a2);
    ArbolExpr &operator/=(const ArbolExpr &a2);
    inline ArbolExpr operator-(void)
      {
        ArbolExpr retval(*this);
        retval.Neg();
        return retval;
      }
    ArbolExpr &pow(const ArbolExpr &a2);
    const double &ToNum(void) const;
    //! @brief Devuelve el valor numérico (si puede).
    const double &ToNum(const std::string &palabra,const double &d) const;
    const double &ToNum(const MapValores &) const;
    inline virtual bool Evaluable(void) const
      { 
        bool retval= false;
        if(raiz) retval= raiz->Evaluable();
        return retval;
      }
    void RecorrePreorden(const ExprFunctor &f);
    void RecorreEnorden(const ExprFunctor &f);
    void RecorrePostorden(const ExprFunctor &f);
    void RecorrePreorden(const ExprFunctor &f) const;
    void RecorreEnorden(const ExprFunctor &f) const;
    void RecorrePostorden(const ExprFunctor &f) const;
    void Asigna(const std::string &palabra,const ArbolExpr &a);
    static void Asigna(const std::string &palabra,const double &d)
      { ExprBase::Asigna(palabra,d); }
    void DesAsigna(const std::string &palabra)
      { ExprBase::DesAsigna(palabra); }

    ConjVariables Variables(void) const;
    bool TieneVariable(const std::string &palabra) const;
    bool TieneVariable(const VarExpr &var) const;
    bool TieneVariable(const Variable &var) const;
    int NumVariables(void) const;
    std::vector<std::string> getNamesOfVariables(void) const;

    std::string GetString(void) const;
    const std::string &GetFullString(void) const;

    void Print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &stream,const ArbolExpr &a);
    friend ArbolExpr abs(const ArbolExpr &);
    friend ArbolExpr sqrt(const ArbolExpr &);
    virtual ~ArbolExpr(void);
  };

std::ostream &operator<<(std::ostream &stream,const ArbolExpr &a);
std::istream &operator>>(std::istream &stream,ArbolExpr &a);

ArbolExpr abs(const ArbolExpr &);
ArbolExpr sqrt(const ArbolExpr &);

#endif




