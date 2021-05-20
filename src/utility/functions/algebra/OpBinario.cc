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
//OpBinario.cc
#include "OpBinario.h"
#include "Rama.h"
#include "Lexico.h"
#include "Literal.h"
#include "utility/utils/misc_utils/matem.h"
#include <cstdlib>

Rama *OpBinario::operator()(Rama *r1,Rama *r2) const
  { return new Rama(this,r1,r2); }

Rama OpSuma::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpSuma::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama retval(r.GetData());
//     if(r.getLeft())
//       retval.PutIzdo(diferencia(*r.getLeft(),v));
//     if(r.getRight())
//       retval.PutDcho(diferencia(*r.getRight(),v));
//    return retval;
  }
Rama *OpSuma::Simplifica(Rama *r) const
  {
    if(*(r->getLeft()) == 0.0) r= ContraeIzdo(r);
    if(*(r->getRight()) == 0.0) r= ContraeDcho(r);
    return r;
  }
const Operando &OpSuma::Opera(const Operando *v1,const Operando *v2) const
  { return *Lex().NuevoLiteral(v1->GetValor() + v2->GetValor()); }

Rama OpResta::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpResta::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama retval(r.GetData());
//     retval.PutIzdo(diferencia(r->getLeft(),v));
//     retval.PutDcho(diferencia(r->getRight(),v));
//     return r;
  }

const Operando &OpResta::Opera(const Operando *v1,const Operando *v2) const
  { return *Lex().NuevoLiteral(v1->GetValor() - v2->GetValor()); }

Rama OpProd::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpProd::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *ap= diferencia(Copia(r->getLeft()),v);
//     Rama *bp= diferencia(Copia(r->getRight()),v);
//     Rama *sum1= Lex().prodt(ap,r->getRight());
//     Rama *sum2= Lex().prodt(r->getLeft(),bp);
//     r= Lex().suma(sum1,sum2);
//     return r;
  }
Rama *OpProd::Simplifica(Rama *r) const
  {
    if(*(r->getLeft()) == 1.0)
      r= ContraeIzdo(r);
    else
      if(*(r->getRight()) == 1.0)
        r= ContraeDcho(r);
      else
        if(*(r->getLeft()) == 0.0)
          r= ContraeDcho(r);
        else
          if(*(r->getRight()) == 0.0)
            r= ContraeIzdo(r);
    return r;
  }

//! @brief Aplica la propiedad distributiva del producto respecto a la suma cuando
//! la suma está a la derecha.
Rama *OpProd::DistribDcha(Rama *raiz) const
  {
    Rama *ri= raiz->getLeft(); //Factor que multiplica a la suma.
    Rama *rd= raiz->getRight(); //Suma.
    Rama *si= new Rama(&(Lex().prodt),ri->getCopy(),rd->getLeft()->getCopy());
    Rama *sd= new Rama(&(Lex().prodt),ri->getCopy(),rd->getRight()->getCopy());
    delete raiz;
    raiz= new Rama(&(Lex().suma),si,sd);
    return raiz;
  }

//! @brief Aplica la propiedad distributiva del producto respecto a la suma cuando
//! la suma está a la derecha.
Rama *OpProd::DistribIzda(Rama *raiz) const
  {
    Rama *ri= raiz->getLeft(); //Suma.
    Rama *rd= raiz->getRight(); //Factor que multiplica a la suma.
    Rama *si= new Rama(&(Lex().prodt),ri->getLeft()->getCopy(),rd->getCopy());
    Rama *sd= new Rama(&(Lex().prodt),ri->getRight()->getCopy(),rd->getCopy());
    delete raiz;
    raiz= new Rama(&(Lex().suma),si,sd);
    return raiz;
  }

//! @brief Aplica la propiedad distributiva del producto respecto a la suma.
Rama *OpProd::Distrib(Rama *r) const
  {
    const Operando *data= r->DatoDcho();
    if(data)
      {
        const std::string nmbD= data->getName();
        if((nmbD=="+") || (nmbD=="-")) //Hay suma a la derecha.
          r= DistribDcha(r);
      }
    data= r->DatoIzdo();
    if(data)
      {
        const std::string nmbI= data->getName();
        if((nmbI=="+") || (nmbI=="-")) //Hay suma a la izquierda.
          r= DistribIzda(r);
      }
    return r;
  }

const Operando &OpProd::Opera(const Operando *v1,const Operando *v2) const
  { return *Lex().NuevoLiteral(v1->GetValor() * v2->GetValor()); }

Rama OpDiv::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "OpDiv::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     Rama *up= diferencia(Copia(r->getLeft()),v);
//     Rama *vp= diferencia(Copia(r->getRight()),v);
//     Rama *sum1= Lex().prodt(r->getRight(),up);
//     Rama *sum2= Lex().prodt(r->getLeft(),vp);
//     r= Lex().divn(Lex().resta(sum1,sum2),Lex().sqr(Copia(r->getRight())));
//     return r;
  }
const Operando &OpDiv::Opera(const Operando *v1,const Operando *v2) const
  { return *Lex().NuevoLiteral(v1->GetValor() / v2->GetValor()); }

const Operando &OpDivEnt::Opera(const Operando *v1,const Operando *v2) const
  { 
    ldiv_t dt= ldiv((long)v1->GetValor(),(long)v2->GetValor());
    return *Lex().NuevoLiteral(dt.quot);
  }

const Operando &OpMod::Opera(const Operando *v1,const Operando *v2) const
  { return *Lex().NuevoLiteral(long(v1->GetValor()) % long(v2->GetValor())); }

const Operando &OpPot::Opera(const Operando *v1,const Operando *v2) const
  { return *Lex().NuevoLiteral(pow(v1->GetValor(),v2->GetValor())); }

const Operando &OpMax::Opera(const Operando *v1,const Operando *v2) const
  {
    const double &vv1= v1->GetValor();
    const double &vv2= v2->GetValor();
    if(vv1>=vv2)
      return *Lex().NuevoLiteral(vv1);
    else
      return *Lex().NuevoLiteral(vv2);
  }

const Operando &OpMin::Opera(const Operando *v1,const Operando *v2) const
  {
    const double &vv1= v1->GetValor();
    const double &vv2= v2->GetValor();
    if(vv1<=vv2)
      return *Lex().NuevoLiteral(vv1);
    else
      return *Lex().NuevoLiteral(vv2);
  }
