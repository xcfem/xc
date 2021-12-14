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
//OpUnario.cc
#include "OpUnario.h"
#include "Operando.h"
#include "Literal.h"
#include "OpBinario.h"
#include "Lexico.h"
#include "ArbolExpr.h"
#include "Rama.h"
#include "utility/utils/misc_utils/matem.h"

Rama *OpUnario::operator()(Rama *r) const
  { return new Rama(this,NULL,r); }

const Operando &OpNeg::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(-v2->GetValor()); }
Rama OpNeg::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpNeg::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     r->PutDcho(diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpInv::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(1.0/v2->GetValor()); }
Rama OpInv::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpInv::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().neg(Lex().inv(Lex().sqr(Copia(r->GetDcho()))));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpSign::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(sgn(v2->GetValor())); }
Rama OpSign::Diferencia(const Variable &,const Rama &) const
  { std::cerr<< "OpSign::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama(); }


const Operando &OpAbs::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(fabs(v2->GetValor())); }
Rama OpAbs::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpAbs::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().sign(Copia(r->GetDcho()));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpSqr::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(v2->GetValor()*v2->GetValor()); }
Rama OpSqr::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpSqr::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *r2= Lex().prodt(new Rama(2.0),Copia(r->GetDcho()));
//     r=Lex().prodt(r2,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpSqrt::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(sqrt(v2->GetValor())); }
Rama OpSqrt::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpSqrt::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().inv(Lex().prodt(new Rama(2.0),Copia(r)));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpExp::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(exp(v2->GetValor())); }
Rama OpExp::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpExp::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Copia(r);
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpLn::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(log(v2->GetValor())); }
Rama OpLn::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpLn::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().inv(Copia(r->GetDcho()));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpLg::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(log10(v2->GetValor())); }
Rama OpLg::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpLg::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().inv(Lex().prodt(new Rama(log(10.0)),Copia(r->GetDcho())));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpRound::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(rint(v2->GetValor())); }

const Operando &OpTrunc::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(floor(v2->GetValor())); }

const Operando &OpToNum::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(v2->GetValor()); }
