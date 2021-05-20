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
//Polynomial.h
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "function.h"
#include "IndPol.h"
#include <map>
#include "utility/utils/misc_utils/matem.h"
#include "utility/matrices/ZMatrix.h"


size_t GetNumCoefs(char m,short unsigned int n);
inline size_t GetNumCoefs(char m,const char *vars)
  { return GetNumCoefs(m,strlen(vars)); }


typedef std::map< IndPol, double, std::less<IndPol> > mapPol;
typedef vectorZ<double> vZ_double;
typedef ZMatrix<double> mZ_double;

class Polynomial : public Function, public mapPol
  {
  protected:
    inline void init(const double &d)
      {
        IndPol v= GetIndPol();
        mapPol::operator[](v)= d;
      }
    void PutSuma(const NmbVars &vars,const IndPol &v,const double &d)
    //Le suma al coeficiente que corresponde a IndoPol la cantidad d.
      {
        IndPol v2= Conv(this->Vars,vars,v);
        mapPol::iterator i= find(v2);
        if (i == end())
          mapPol::operator[](v2) = d;
        else
          mapPol::operator[](v2) += d;
      }
    void PutSuma(const IndPol &v,const Polynomial &p);
    void PutProd(const IndPol &v,const double &d)
    //Multiplica el coeficiente que corresponde a IndPol por la cantidad d.
      {
        mapPol::iterator i= find(v);
        if (i != end()) mapPol::operator[](v) *= d;
      }
    void OutSigno(std::ostream &stream,mapPol::const_iterator &i) const
    //Devuelve el signo del elemento al que se refiere i.
      {
        if ( (*i).second < 0 )
          stream << " - ";
        else
          stream << " + ";
      }
    void OutVars(std::ostream &stream,mapPol::const_iterator &i) const;
    Polynomial Reduce(short unsigned int j,const double &val) const;
  public:
    Polynomial(const double &d= 0.0): Function(), mapPol() 
      { init(d); }
    Polynomial(const NmbVars &vars): Function(vars), mapPol() 
      { init(0.0); }      
    Polynomial(const char *vars): Function(vars), mapPol() 
      { init(0.0); }
    Polynomial(const char var): Function(var), mapPol() 
      { init(0.0); }
    Polynomial(const Polynomial &otro): Function(otro), mapPol(otro) {}
    Polynomial &operator=(const Polynomial &p)
      {
        Function::operator=(p);
        mapPol::operator=(p);
        return *this;
      }
    //! @brief Return a vector of indexes suited to
    //! the polynomial.
    IndPol GetIndPol(void) const
      {
        IndPol i(GetDim());
        return i;
      }
    int Grado(short unsigned int j) const;
    IndPol Grados(void) const;
    //! @brief Return the degree of the polynomial.
    int Grado(void) const
      {
        IndPol gs= Grados();
        return *max_element(gs.begin(),gs.end());  
      }
    void Neg(void);
    static Polynomial neutro_suma(void)
      { return Polynomial(0.0); }
    static Polynomial neutro_producto(void)
      { return Polynomial(1.0); }
    Polynomial Reduce(short unsigned int j,const Polynomial &val) const;
    //! @brief Return the polynomial that results from the substitution of
    //! the j-th variable with the value being passed as parameter.
    Polynomial Eval(short unsigned int j,const double &val) const;
    //! @brief Return the polynomial that results from the substitution of
    //! the j-th variable with the value being passed as parameter.
    Polynomial Eval(short unsigned int j,const Polynomial &val) const;
    //! @brief Return the value of the polynomial at point v.
    double Eval(const vZ_double &v) const;
    //! @brief Return the value of the polynomial at point v.
    double Eval(const mZ_double &v) const;
    Polynomial CompactaVar(unsigned short int j) const;
    //! @brief Evaluates the polynomial in point x.
    double operator()(const vZ_double &x) const
      { return Eval(x); }
    friend bool operator ==(const Polynomial &p1,const Polynomial &p2)
      {
        if ((const Function &) p1 != (const Function &) p2) return 0;
        if ((const mapPol &) p1 != (const mapPol &) p2) return 0;
        return 1;
      }
    friend Polynomial operator -(const Polynomial &p)
      {
        Polynomial neg(p);
        neg.Neg();
        return neg;
      }
    Polynomial& operator+=(const Polynomial &p);
    Polynomial &operator-=(const Polynomial &m)
      {
        Polynomial p(*this);
        p+= -m;
        (*this)= p;
        return *this;
      }
    Polynomial &operator +=(const double &d)
      {
        IndPol v= GetIndPol();
        PutSuma(Vars,v,d);
        return *this;        
      }
    friend Polynomial operator +(const Polynomial &p1,const Polynomial &p2)
      {
        Polynomial p(p1);
        p+= p2;
        return p;
      }
    friend Polynomial operator -(const Polynomial &p1,const Polynomial &p2)
      { return p1 + (-p2); }
    friend Polynomial operator +(const Polynomial &p,const double &d)
      {
        Polynomial q(p);
        q+=d;
        return q;
      }
    friend Polynomial operator +(const double &d,const Polynomial &p)
      { return p+d; }
    friend Polynomial operator -(const Polynomial &p,const double &d)
      { return p + (-d); }
    friend Polynomial operator -(const double d,const Polynomial &p)
      { return d+(-p); }
    friend Polynomial operator *(const Polynomial &p1,const Polynomial &p2);
    Polynomial &operator*=(const double &d);
    Polynomial &operator*=(const Polynomial &p);
    inline friend Polynomial operator *(const Polynomial &p,const double &d)
      {
        Polynomial q(p);
        q*= d;
        return q;
      }
    inline friend Polynomial operator *(const double &d,const Polynomial &p)
      { return p*d; }
    Polynomial Parcial(short unsigned int j) const;
    Polynomial Derivada(short unsigned int j,const double &x) const
      {
        Polynomial p= Parcial(j);
        return p.Eval(j,x);
      }
    Polynomial Primitiva(short unsigned int j) const;
    Polynomial Integral(short unsigned int j,const double &x0,const double &x1) const
      {
        Polynomial p= Primitiva(j);
        return p.Eval(j,x1)-p.Eval(j,x0);
      }
    double Integral(const vZ_double &x0,const vZ_double &x1) const;
    friend std::ostream &operator <<(std::ostream &stream,const Polynomial &p);
  };

Polynomial operator *(const Polynomial &p1,const Polynomial &p2);
std::ostream &operator<<(std::ostream &stream,const Polynomial &p);
std::istream &operator>>(std::istream &stream,const Polynomial &p);

//! @brief Computes the square of the polynomial
inline Polynomial sqr(const Polynomial &p)
  { return p*p; }

//! @brief Return p^n
Polynomial pow(const Polynomial &p,unsigned int n);

inline Polynomial neutro_suma(const Polynomial &)
  { return Polynomial::neutro_suma(); }
inline Polynomial neutro_producto(const Polynomial &)
  { return Polynomial::neutro_producto(); }

#endif








