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
//vectorZ.h
/*
Template para vectores cuyos elementos forman estructura de anillo
con las operaciones de composicion interna suma y producto.
*/

#ifndef VECTORZ_H
#define VECTORZ_H

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <numeric>
#include <list>
#include <cstring>

template <class numero>
class vectorZ : public std::vector<numero>
  {
  protected:
    typedef std::vector<numero> vector_numero;
    typedef vectorZ<numero> vectorZ_numero;

  public:
    //typedef vector_allocator vectorZ_allocator;
    typedef typename std::vector<numero>::size_type size_type;
    typedef std::list<numero> lst_numero;

    vectorZ(void) : vector_numero() {}
    vectorZ(size_type n,const numero &value = numero()) : vector_numero(n,value) {}
    vectorZ(const lst_numero &ln);
    vectorZ(const vectorZ<numero> &otro) : vector_numero(otro) {}
    vectorZ(const size_t &sz,const numero arr[]) : vector_numero(arr,arr+sz){}
    template <class InputIterator>
    vectorZ(InputIterator b,InputIterator e);
    vectorZ<numero> &operator=(const vectorZ<numero> &otro)
      {
        vector_numero::operator=(otro);
        return *this;
      }
    vectorZ<numero> &operator+=(const vectorZ<numero> &otro)
      {
        Suma(otro);
        return *this;
      }
    vectorZ<numero> &operator-=(const vectorZ<numero> &otro)
      {
        Resta(otro);
        return *this;
      }
    vectorZ<numero> &operator*=(const numero &n)
      {
        Prod(n);
        return *this;
      }
    void Suma(const vectorZ<numero> &v);
    void Resta(const vectorZ<numero> &v);
    void Suma(const vectorZ<numero> &v1,const vectorZ<numero> &v2)
      {
        vectorZ::operator =(v1);
        Suma(v2);
      }
    void Resta(const vectorZ<numero> &v1,const vectorZ<numero> &v2)
      {
        vectorZ::operator=(v1);
        Resta(v2);
      }    
    void Prod(const numero &n);
    void PutSuma(size_type i,const numero &n)
      { (*this)[i]+= n; }
    void PutResta(size_type i,const numero &n)
      { (*this)[i]-= n; }
    void PutProd(size_type i,const numero &n)
      { (*this)[i]*= n; }
    void swap(size_type n1,size_type n2)
      { std::swap((*this)[n1],(*this)[n2]); }
    long Busca(const numero &n) const;
    void Neg(void);
    bool Nulos(const numero &tol= numero());
    void Con(size_type n1,size_type n2,const numero &n)
      {
        fill(&(vector_numero::operator [](n1)),&(vector_numero::operator [](n2)),n);
        vector_numero::operator [](n2)= n;
      }
    void Con(size_type n1,const numero &n)
      { fill(&(vector_numero::operator [](n1)),this->end(),n); }      
    void Con(const numero &n)
      { fill(this->begin(),this->end(),n); }
    numero Sumatorio( size_type i, size_type j) const
    //devuelve la suma de los elementos [i,j)
      { return accumulate(this->begin()+ i,this->begin() + j,numero()); }
    numero Productorio(size_type i, size_type j) const
    //devuelve el producto de los elementos [i,j)
      { return accumulate(this->begin()+ i+1,this->begin() + j,*(this->begin()+i),std::multiplies<numero>()); }
    vectorZ<numero> Left(size_t j) const;
    vectorZ<numero> Right(size_t j) const;
    vectorZ<numero> Mid(size_t i,size_t j) const;
    vectorZ<numero> GetMenor(size_t j) const;
    vectorZ<numero> Sustituye(size_t j,const vectorZ<numero> &v) const;
    unsigned long Distintos(const vectorZ<numero> &v) const;
    friend vectorZ<numero> operator+( const vectorZ<numero> &v1,
                                      const vectorZ<numero> &v2)
      {
        typename vectorZ<numero>::size_type mx= std::max(v1.size(),v2.size());
        vectorZ<numero> suma(mx);
        suma.Suma(v1,v2);
        return suma;
      }                                       
    friend vectorZ<numero> operator-( const vectorZ<numero> &v1,
                                      const vectorZ<numero> &v2)
      {
        typename vectorZ<numero>::size_type mx= std::max(v1.size(),v2.size());
        vectorZ<numero> resta(mx);
        resta.Resta(v1,v2);
        return resta;      
      }                                       
    friend vectorZ<numero> operator*( const numero &d,
                                       const vectorZ<numero> &m)
      { return m*d; }
    friend vectorZ<numero> operator*( const vectorZ<numero> &m,
                                       const numero &d)
      {
        vectorZ<numero> producto(m);
        producto.Prod(d);
        return producto;
      }                                       
    friend bool operator ==(const vectorZ<numero> &x,const vectorZ<numero> &y)
      { return ((const vector_numero &) x == (const vector_numero &) y); }
  };

//! @brief Constructor con lista inicialización.
template <class numero> template<class InputIterator>
vectorZ<numero>::vectorZ(InputIterator b,InputIterator e)
  : vector_numero(b,e) {}

template <class numero>
vectorZ<numero>::vectorZ(const lst_numero &ln) : vector_numero(ln.size())
  {
    size_type j= 0;
    typedef typename lst_numero::const_iterator c_iterator;
    for(c_iterator i=ln.begin();i!=ln.end();i++)
      { (*this)[j]= *i; j++; }
  }

template <class numero>
vectorZ<numero> operator -(const vectorZ<numero> &m)
  {
    vectorZ<numero> neg(m);
    neg.Neg();
    return neg;
  }

template <class numero>
long vectorZ<numero>::Busca(const numero &n) const
  {
    size_type sz= this->size();
    for(size_type i=0;i<sz;i++)
      if ((*this)[i] == n) return i;
    return -1;
  }
  
template <class numero>
void vectorZ<numero>::Neg(void)
  {
    size_type sz= this->size();
    for(size_type i= 0;i<sz;i++)
      (*this)[i] = -(*this)[i];
  }
template <class numero>
bool vectorZ<numero>::Nulos(const numero &tol)
  {
    size_type sz= this->size();
    for(size_type i= 0;i<sz;i++)
      if(Abs((*this)[i])>tol) return false;
    return true;
  }

template <class numero>
void vectorZ<numero>::Suma(const vectorZ<numero> &v)
  {
    size_type sz= v.size();  
    for(size_type i=0;i<sz;i++) PutSuma(i,v[i]);
  }

template <class numero>
void vectorZ<numero>::Resta(const vectorZ<numero> &v)
  {
    size_type sz= v.size();
    for(size_type i=0;i<sz;i++) PutResta(i,v[i]);
  }

template <class numero>
void vectorZ<numero>::Prod(const numero &n)
  {
    size_type sz= this->size();      
    for(size_type i= 0;i<sz;i++)
      PutProd(i,n);
  }
  
template <class numero>
vectorZ<numero> vectorZ<numero>::Left(size_t j) const
//Devuelve un vector con los elementos a la izquierda de j.
  {
    assert(j>0);
    vectorZ<numero> left(j);
    for(size_t i=0;i<j;i++) left[i]= (*this)[i];
    return left;
  }

template <class numero>
vectorZ<numero> vectorZ<numero>::Right(size_t j) const
//Devuelve un vector con los elementos a la derecha de j.
  {
    size_type sz= this->size();
    assert(j<sz);
    size_type szr= sz-j-1;
    vectorZ<numero> right(szr);
    for(size_type i=0;i<szr;i++) right[i]= (*this)[i+j+1];
    return right;
  }

template <class numero>
vectorZ<numero> vectorZ<numero>::Mid(size_t i,size_t j) const
//Devuelve un vector con los elementos entre i y j.
  {
    assert(j>i);  
    size_type sz= j-i-1;
    vectorZ<numero> mid(sz);
    for(size_type k=0;k<sz;k++) mid[k]= (*this)[k+i+1];
    return mid;
  }
  
template <class numero>
vectorZ<numero> vectorZ<numero>::GetMenor(size_t j) const
  {
    if (j == 0) return Right(j);
    if (j == (this->size() - 1)) return Left(j);
    return Cat(Left(j),Right(j));    
  }

template <class numero>
vectorZ<numero> vectorZ<numero>::Sustituye(size_t j,const vectorZ<numero> &v) const
  {
    vectorZ<numero> s;
    if (j>0) 
      s= Cat(Left(j),v);
    else
      s= v;
    if (j<this->size()) s= Cat(s,Right(j));
    return s;    
  }

template <class numero>
unsigned long vectorZ<numero>::Distintos(const vectorZ<numero> &v) const
//Cuenta el número de elementos distintos entre los dos vectores
  {
    unsigned long c= this->size(),sz=v.size();
    for(unsigned long i=0;i<sz;i++)
      if (Busca(v[i]) < 0) c++;
    return c;
  }

template <class numero>
std::ostream &operator << (std::ostream &os,const vectorZ<numero> &n)
  {
    os << '[';
    typedef typename vectorZ<numero>::size_type sz_type;
    sz_type sz= n.size();
    if(sz>0) os << n[0];
    for(sz_type i= 1;i<sz;i++) os << ',' << n[i];
    os << ']';
    return os;
  }

template <class numero>
std::istream &operator >> (std::istream &is, vectorZ<numero> &n)
  {
    char c;
    typename vectorZ<numero>::lst_numero ln;
    numero ni;
    is >> c;
    if(c != '[') is.putback(c);
    while(!is.eof())
      {
        is >> ni >> c;
        ln.push_back(ni);
        if(c == ']')
	  {
            n= vectorZ<numero>(ln);
            return is;
          }
      }
    n= vectorZ<numero>(ln);
    return is;
  }

template <class numero>
vectorZ<numero> Cat(const vectorZ<numero> &v1,const vectorZ<numero> &v2)
  {
    size_t sz1= v1.size(), sz2= v2.size();
    size_t szc= sz1 + sz2;
    vectorZ<numero> cat(szc);
    typedef typename vectorZ<numero>::size_type sz_type;
    for(sz_type i=0;i<sz1;i++)
      cat[i]= v1[i];
    for(sz_type i=0;i<sz2;i++)
      cat[i+sz1]= v2[i];
    return cat;
  }

#endif
