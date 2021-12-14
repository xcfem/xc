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
//Lexico.h

#ifndef LEXICO_H
#define LEXICO_H

#include "OpUnario.h"
#include "OpBinario.h"
#include "Macaulay.h"
#include "OpComparacion.h"
#include "OpBooleano.h"
#include "OpTrigonometricas.h"
#include "Literal.h"
#include <string>
#include "TablaVariables.h"
#include "TablaLiterales.h"
#include "Variable.h"
#include <iostream>


class ArbolExpr;

class Lexico
  {
    friend class ArbolExpr;
  protected:
    TablaLiterales literales;
    TablaVariables vars; //Variables globales.
  public:
    //Paréntesis
    ParentDcho parentd;
    ParentIzdo parenti;

    //Operadores binarios.
    static OpSuma suma;
    static OpResta resta;
    static OpProd prodt;
    static OpDiv divn;
    static OpDivEnt dive;
    static OpMod modulus;
    static OpPot pot;
    static OpMenor menor;
    static OpLessOrEqual op_less_or_equal;
    static OpEqual equal;
    static OpMayor mayor;
    static OpGreaterOrEqual op_greater_or_equal;
    static OpDistinto distinto;
    static OpAnd And;
    static OpOr Or;
    static OpNot Not;
    static OpMax max;
    static OpMin min;
    static OpMacaulay0 F0;
    static OpMacaulayN F1;
    static OpMacaulayN F2;
    static OpMacaulayN F3;
    static OpMacaulayN F4;

    //Operadores unarios.
    static OpNeg neg;
    static OpInv inv;
    static OpSign sign;
    static OpAbs abs;
    static OpSqr sqr;
    static OpSqrt raiz2;
    static OpExp expn;
    static OpLn ln;
    static OpLg lg;
    static OpPi pi;
    static OpSin seno;
    static OpCos cose;
    static OpTan tang;
    static OpAtan arctg;
    static OpSec sec;
    static OpCosec cosec;
    static OpCotg cotg;
    static OpDeg2Rad deg2rad;
    static OpRad2Deg rad2deg;
    static OpRound round;
    static OpTrunc trunc;
    static OpToNum to_num;

/*   public: */
/*     friend class OpSqr; */
/*     friend class OpNeg; */
/*     friend class OpSign; */
/*     friend class OpInv; */
/*     friend class OpAbs; */
/*     friend class OpSqrt; */
/*     friend class OpExp; */
/*     friend class OpLn; */
/*     friend class OpLg; */
/*     friend class OpSin; */
/*     friend class OpCos; */
/*     friend class OpTan; */
/*     friend class OpAtan; */

/*     friend class OpProd; */
/*     friend class OpDiv; */

    Lexico(void);
    inline TablaLiterales &getTablaLiterales(void)
      { return literales; }
    virtual Variable *CalcDirVar(const std::string &nmb);
    Segnal *CalcDirOpUnario(const std::string &nmb);
    Segnal *CalcDirOpBinario(const std::string &nmb);
    virtual Segnal *CalcDir(const std::string &nmb);
    inline Segnal *CalcularDireccion(const std::string &nmb)
      {
        if (nmb.length() <= 0)
	  std::cerr << "Tried to compute the addres"
                    << " of an empty name" << std::endl;
        else
          return CalcDir(nmb);
        return NULL;
      }
/*     inline const Literal *NuevoLiteral(const Literal &l) */
/*       { return literales.Nuevo(l); } */
    inline const Operando *NuevoLiteral(const double &d)
      { return literales.NuevoDbl(d); }
    inline void NuevaRefLiteral(const double &d)
      { literales.NuevaRef(d); }
    inline void NuevaRefLiteral(const Segnal *s)
      { literales.NuevaRef(s); }
    inline void BorraLiteral(const double &d)
      { literales.Borra(d); }
    inline void BorraSiLiteral(const Segnal *s)
      { literales.BorraSiLiteral(s); }
    inline NamedOperand *NuevaVar(const Variable &v)
      { return vars.Nueva(v); }
    inline NamedOperand *NuevaVar(const std::string &palabra)
      { return NuevaVar(Variable(palabra)); }
    void Asigna(const std::string &palabra,const double &d)
      {
        Variable *tmp= CalcDirVar(palabra);
        if(tmp) tmp->Asigna(d);
      }
    void DesAsigna(const std::string &palabra)
      {
        Variable *tmp= CalcDirVar(palabra);
        if(tmp) tmp->DesAsigna();
      }
    void PrintPrioridades(std::ostream &os) const;
    virtual ~Lexico(void) {}
  };

#endif






