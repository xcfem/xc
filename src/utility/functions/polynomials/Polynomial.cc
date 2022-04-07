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
//Polynomial.cc

#include "Polynomial.h"
#include "utility/functions/statistics/combinatorics.h"

const Polynomial one_polynomial= Polynomial::neutro_producto();
const Polynomial zero_polynomial= Polynomial::neutro_suma();


size_t GetNumCoefs(char m,short unsigned int n)
  {
    if (m<0) return 0;
    short unsigned int i;
    size_t t= 1;
    for(i=1;i<=n;i++)
      t= t * (m + i);
    t= t / factorial(n);
    return t;
  }

void Polynomial::PutSuma(const IndPol &v,const Polynomial &p)
//Le suma al coeficiente que corresponde a IndoPol la cantidad d.
  {
    mapPol::const_iterator i;
    IndPol ip1= p.GetIndPol();
    for (i= p.begin();i != p.end();i++)
      {
        ip1= Conv(this->Vars,p.Vars,(*i).first);
        PutSuma(this->Vars,v+ip1,(*i).second);
      }
  }

//! @brief Return the polynomial degrees for each of its variables.
IndPol Polynomial::Grados(void) const
  {
    IndPol gs= GetIndPol();
    size_t i,sz= gs.size();
    for (i= 0; i < sz;i++)
      gs[i]= Grado(i+1);
    return gs;
  }

//! @brief Return the polynomial degree for the j-th variable.
int Polynomial::Grado(short unsigned int j) const
  {
    int g= 0;
    mapPol::const_iterator i;
    for (i= begin();i != end();i++)
      g= std::max((int)(*i).first.Grado(j-1),g);
    return g;
  }

//! @brief Changes the arithmetic inverse of the polynomial.
void Polynomial::Neg(void)
  {
    mapPol::iterator i;
    for (i= begin();i != end();i++) (*i).second= -(*i).second;
  }

//! @brief Return the polynomial that results from the substitution of
//! the j-th variable with the value being passed as parameter.
Polynomial Polynomial::Eval(short unsigned int j,const double &val) const
  {
    Polynomial q= *this;
    while (q.Grado(j) > 0) q= q.Reduce(j,val);
    q= q.CompactaVar(j); //The polynomial doesn't depends on this variable anymore.
    return q;
  }

//! @brief Return the polynomial that results from the substitution of
//! the j-th variable with the value being passed as parameter.
Polynomial Polynomial::Eval(short unsigned int j,const Polynomial &val) const
  {
    Polynomial q= *this;
    Polynomial q2= val;
    while (q.Grado(j) > 0) q= q.Reduce(j,q2);
    q= q.CompactaVar(j);  //The polynomial doesn't depends on this variable anymore.
    return q;
  }

//! @brief Return the value of the polynomial at point v.
double Polynomial::Eval(const vZ_double &v) const
  {
    Polynomial q= *this;
    size_t i,sz= v.size();
    for (i=0;i<sz;i++) q= q.Eval(i+1,v[i]);
    return (*q.begin()).second;
  }

//! @brief Return the value of the polynomial at point v.
double Polynomial::Eval(const mZ_double &v) const
  {
    Polynomial q= *this;
    size_t i,sz= v.size();
    for (i=1;i<=sz;i++) q= q.Eval(i,v(i,1));
    return (*q.begin()).second;
  }

//! @brief Return the partial derivative of the polynomial with respect
//! to the j-th variable.
Polynomial Polynomial::Parcial(short unsigned int j) const
  {
    j--; //El indice de IndPol empieza en 0;
    Polynomial q(Vars);
    if (Grado() == 0) return q;
    IndPol ip1= GetIndPol();
    IndPol ip2= GetIndPol();
    ip1[j]=1;    
    for ( mapPol::const_iterator i= begin(); i != end(); i++)
      {
        ip2= (*i).first;
        if (ip2[j] > 0) q[ip2-ip1]= ip2[j] * (*i).second;
      }
    return q;
  }

//! @brief Returns the primitive of the polynomial with respect
//! to the j-th variable.
Polynomial Polynomial::Primitiva(short unsigned int j) const
  {
    j--; //El indice de las variables empieza en 0;
    Polynomial q(Vars);
    if (Grado() == 0) return q;
    IndPol ip1= GetIndPol();
    IndPol ip2= GetIndPol();
    ip1[j]=1;    
    for ( mapPol::const_iterator i= begin(); i != end(); i++)
      {
        ip2= (*i).first;
        q[ip2+ip1]= (1.0/(ip2[j]+1)) * (*i).second;
      }
    return q;
  }

//! @brief Return the polynomial that result of substituting once the
//! j-th variable by the argument. The name of the routine is due to the
//! fact that the returned polynomial has a degree less that this one in
//! the j-th variable.
Polynomial Polynomial::Reduce(short unsigned int j,const double &val) const
  {
    j--; //El indice de IndPol empieza en 0;
    Polynomial result(Vars);
    IndPol ip1= GetIndPol();
    if (Grado() == 0) return result;
    IndPol ip2= ip1;
    ip1[j]=1;
    for ( mapPol::const_iterator i= begin();i != end() ;i++)
      {
        ip2= (*i).first;
        if (ip2[j] > 0)
          result.PutSuma(result.Vars,ip2-ip1,val * (*i).second);
        else
          result.PutSuma(result.Vars,ip2,(*i).second);
      }
    return result;    
  }

//! @brief Return the polynomial that result of substituting once the
//! j-th variable by the argument. The name of the routine is due to the
//! fact that the returned polynomial has a degree less that this one in
//! the j-th variable.
Polynomial Polynomial::Reduce(short unsigned int j,const Polynomial &val) const
  {
    j--; //El indice de IndPol empieza en 0;
    //The polynomial will have as variables the union of the variables
    // of both polynomials.
    Polynomial result(this->Vars + val.Vars);
    if (Grado() == 0) return result;
    IndPol ip1= result.GetIndPol();
    IndPol ip2= ip1;
    ip1[j]=1;
    for ( mapPol::const_iterator i= begin();i != end();i++)
      {
        ip2= Conv(result.Vars,this->Vars,(*i).first);
        if (ip2[j] > 0)
          result.PutSuma(ip2-ip1,val * (*i).second);
        else
          result.PutSuma(result.Vars,ip2,(*i).second);
      }
    return result;    
  }

//! @brief Eliminates the j-th variable. If the variable to eliminate
//! has not zero degree in the polynomial the result is a disaster.
Polynomial Polynomial::CompactaVar(unsigned short int j) const
  {
    NmbVars Vars= GetVars();
    //Vars.erase(&Vars.at(j-1)); Doesn't works.
    NmbVars::iterator ij= Vars.begin() + (j-1);
    Vars.erase(ij);
    Polynomial result(Vars);
    IndPol ip1= GetIndPol();
    for ( mapPol::const_iterator i= begin();i != end();i++)
      {
        ip1= (*i).first;
        //ip1.erase(&ip1[j-1]); Doesn't works.
        IndPol::iterator ij= ip1.begin() + (j-1);
        ip1.erase(ij);
        result[ip1] = (*i).second;
      }
    return result;
  }

//! @brief Returns the integral of the polynomial between x0 and x1.
double Polynomial::Integral(const vZ_double &x0,const vZ_double &x1) const
  {
    Polynomial q= *this;
    size_t i,sz= x0.size();
    for (i=0;i<sz;i++) q= q.Integral(i+1,x0[i],x1[i]);
    return (*q.begin()).second;  
  }

void Polynomial::OutVars(std::ostream &stream,mapPol::const_iterator &i) const
//Imprime las variebales elevadas a sus correspondientes potencias.
  {
    int j,d= GetDim();  
    for (j=1;j<=d;j++)
      {
        int pot= (*i).first[j-1];
        if (pot > 0)
          stream << GetNmbVar(j);
        if (pot > 1)
          stream << '^' << pot;
      }
  }

Polynomial& Polynomial::operator+=(const Polynomial &p)
  {
    Polynomial s(Vars + p.Vars);
    mapPol::const_iterator i;
    for (i= begin();i != end();i++) 
      s.PutSuma(Vars,(*i).first,(*i).second);    
    for (i= p.begin();i != p.end();i++) 
      s.PutSuma(p.Vars,(*i).first,(*i).second);
    (*this)= s;
    return *this;
  }

std::ostream &operator<<(std::ostream &stream,const Polynomial &p)
  {
    for ( mapPol::const_iterator i= p.begin() ; 
          i != p.end() ; 
          i++)
      { 
        if (i==p.begin()) 
          stream << (*i).second << ' ';
        else
          {
            p.OutSigno(stream,i);
            if (fabs((*i).second) != 1) stream << fabs((*i).second) << ' ';
          }
        p.OutVars(stream,i);
      }
    return stream;
  }

std::istream &operator>>(std::istream &stream,const Polynomial &p)
  {
    std::cerr << "Polynomial; read from stream not implemented yet." << std::endl;
    return stream;
  }

Polynomial &Polynomial::operator*=(const double &d)
  {
    if(d == 1) return *this;
    if(d == 0) 
      {
        (*this)= zero_polynomial;
        return *this;
      }
    for ( mapPol::iterator i= begin() ; i != end() ; i++)
       (*i).second*= d;
    return *this;
  }
Polynomial &Polynomial::operator*=(const Polynomial &p)
  {
    if(p == one_polynomial) return *this;
    if(p == zero_polynomial) 
      {
        (*this)= zero_polynomial;
        return *this;
      }
    (*this)= (*this) * p;
    return *this;
  }
Polynomial operator *(const Polynomial &p1,const Polynomial &p2)
  {
    if(p1 == one_polynomial) return p2;
    if(p2 == one_polynomial) return p1;
    if( (p1 == zero_polynomial) || (p2 == zero_polynomial) )
      return zero_polynomial;
    Polynomial p(p1.Vars + p2.Vars);
    mapPol::const_iterator i,j;
    for(i=p1.begin();i!= p1.end();i++)
      for(j=p2.begin();j!= p2.end();j++)
        {
          IndPol ip= p.GetIndPol();
          ip= Conv(p.Vars,p1.Vars,(*i).first) + Conv(p.Vars,p2.Vars,(*j).first);
          p.PutSuma(p.Vars,ip,(*i).second * (*j).second);
        }
    return p;
  }

//! @brief Return p^n.
Polynomial pow(const Polynomial &p,unsigned int n)
  {
    Polynomial q= p;
    unsigned int i;
    for(i=1;i<n;i++) q= q*p;
    return q;
  }
