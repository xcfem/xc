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
//OpTrigonometricas.cc

#include "OpTrigonometricas.h"
#include "Literal.h"
#include "OpBinario.h"
#include "Rama.h"
#include "Lexico.h"
#include "utility/utils/misc_utils/matem.h"


const Operando &OpPi::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(M_PI); }

const Operando &OpSin::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(sin(v2->GetValor())); }
Rama OpSin::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpSin::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().cose(Copia(r->GetDcho())); //coseno
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpCos::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(cos(v2->GetValor())); }
Rama OpCos::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpCos::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().neg(Lex().seno(Copia(r->GetDcho()))); //-seno
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpTan::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(tan(v2->GetValor())); }
Rama OpTan::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpTan::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().suma(new Rama(1.0),Lex().sqr(Copia(r)));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpAtan::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(atan(v2->GetValor())); }
Rama OpAtan::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpAtan::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ri= Lex().inv(Lex().suma(new Rama(1.0),Lex().sqr(Copia(r->GetDcho()))));
//     r=Lex().prodt(ri,diferencia(r->GetDcho(),v));
//     return r;
  }

const Operando &OpSec::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(1/cos(v2->GetValor())); }

const Operando &OpCosec::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(1/sin(v2->GetValor())); }

const Operando &OpCotg::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(1/tan(v2->GetValor())); }

const Operando &OpDeg2Rad::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(DegToRad(v2->GetValor())); }
Rama OpDeg2Rad::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpDeg2Rad::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *c= Lex().divn(new Rama(M_PI),new Rama(180));
//     Rama *bp= diferencia(Copia(r->GetDcho()),v);
//     r= Lex().prodt(c,bp);
//     return r;
  }

const Operando &OpRad2Deg::Opera(const Operando *,const Operando *v2) const
  { return *Lex().NuevoLiteral(RadToDeg(v2->GetValor())); }
Rama OpRad2Deg::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpRad2Deg::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *c= Lex().divn(new Rama(180),new Rama(M_PI));
//     Rama *bp= diferencia(Copia(r->GetDcho()),v);
//     r= Lex().prodt(c,bp);
//     return r;
  }
