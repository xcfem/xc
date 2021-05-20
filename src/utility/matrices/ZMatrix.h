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
//ZMatrix.h

#ifndef ZMATRIX_H
#define ZMATRIX_H

#include <iostream>
#include "vectorZ.h"
#include "TMatrix.h"
#include <cmath>

template <class MATR>
MATR prod_vectorial(const MATR &v1,const MATR &v2);

//! @brief Matrix wich element type has estructura de anillo
//! respecto a las operaciones + y *.
template <class numero>
class ZMatrix : public TMatrix<numero,vectorZ<numero> >
  {
  public:
    typedef vectorZ<numero> vectorZ_numero;
    typedef TMatrix<numero,vectorZ<numero> > mT_numero;  
    //typedef vectorZ_allocator ZMatrix_allocator;
    typedef typename vectorZ_numero::reference reference;
    typedef typename vectorZ_numero::const_reference const_reference;
    typedef typename vectorZ_numero::value_type value_type;    
    typedef typename vectorZ_numero::size_type size_type;
    typedef typename mT_numero::lst_T lst_numero;

  private:
    void TSRGT(numero eps,size_t *it,ZMatrix<numero> &C,ZMatrix<size_t> &Kp,ZMatrix<size_t> &Lp) const;
    numero DMGT(const numero &eps) const;

  protected:
    numero row_sum(size_type i) const;
    numero column_sum(size_type j) const;
  public:
    ZMatrix(void) : mT_numero(1,1) {}
    ZMatrix(size_type n_rows,size_type n_columns) : mT_numero(n_rows,n_columns) {}
    ZMatrix(size_type n_rows,size_type n_columns,numero val) : mT_numero(n_rows,n_columns,val) {}
    ZMatrix(size_type n_rows,size_type n_columns,const lst_numero &ln) : mT_numero(n_rows,n_columns)
      { vectorZ_numero::operator=(vectorZ_numero(ln)); }
    ZMatrix(const ZMatrix<numero> &other) : mT_numero(other) {}
    template <class InputIterator>
    ZMatrix(const size_t &n_rows,const size_t &n_columns,InputIterator b,InputIterator e);
    ZMatrix<numero> &operator=(const ZMatrix<numero> &m)
      {
	mT_numero::operator=(m);
        return *this;
      }
    ZMatrix<numero>& operator=(const numero &n)
      {
        mT_numero::Con(n);
        return *this;
      }
    ZMatrix<numero>& operator+=(const ZMatrix<numero> &m)
      {
        this->check_sto_sum(m);
        this->Suma(m);
        return *this;
      }
    ZMatrix<numero>& operator-=(const ZMatrix<numero> &m)
      {
        this->check_sto_dif(m);
        this->Resta(m);
        return *this;
      }
    ZMatrix<numero> &operator*=(const numero &n)
      {
        this->Prod(n);
        return *this;
      }
    ZMatrix<numero> &operator*=(const ZMatrix<numero> &m)
      {
        *this= (*this)*m;
        return *this;
      }
    numero row_maximum(size_type i) const;
    numero column_maximum(size_type j) const;
    void PutSuma(size_type i,size_type j,const numero &n)
    //Le suma n a la posiciOn i,j.
      { vectorZ_numero::PutSuma(Indice(i,j),n); }
    void PutResta(size_type i,size_type j,const numero &n)
    //Le resta n a la posici'on i,j.
      { vectorZ_numero::PutResta(Indice(i,j),n); }    
    void PutProd(size_type i,size_type j,const numero &n)
    //Multiplica la posici'on i por n.
      { vectorZ_numero::PutProd(Indice(i,j),n); }
    void swap(size_type f1,size_type c1,size_type f2, size_type c2)
      { vectorZ_numero::swap(Indice(f1,c1),Indice(f2,c2)); }    
    ZMatrix<numero> &Trn(void)
      {
	mT_numero::Trn();
        return *this;
      }
    ZMatrix<numero> GetTrn(void) const
      {
        ZMatrix<numero> m(*this);
        m.Trn();
        return m;
      }
    ZMatrix<numero> getBox(size_t f1, size_t c1, size_t f2, size_t c2) const;
    ZMatrix<numero> getRow(size_type iRow) const
      { return getBox(iRow,1,iRow,this->n_columns); }
    ZMatrix<numero> getColumn(size_type col) const
      { return getBox(1,col,this->n_rows,col); }
    ZMatrix<numero> GetMenor(size_t f,size_t c) const;
    void Idn(void);
    void sumBox(size_t f,size_t c,const ZMatrix<numero> &box);
    numero GetDetLento(void) const;
    numero GetDet(const numero &eps= 1e-10) const;
    numero Traza(void) const;
    inline bool Nulo(const numero &tol= numero()) const
      { return (this->Abs2()<(tol*tol)); }
    numero getRowNorm(void) const;
    numero getColumnNorm(void) const;
    inline void Neg(void)
      { return vectorZ<numero>::Neg(); }

    virtual void Input(std::istream &);
    virtual void Input(const std::string &);

    friend bool operator==(const ZMatrix<numero> &m1,const ZMatrix<numero> &m2)
      { return ((const mT_numero &) m1 == (const mT_numero &) m2); }
/*     friend ZMatrix<numero> operator+(const ZMatrix<numero> &m1,const ZMatrix<numero> &m2) return suma(m1.n_rows,m1.n_columns); */
/*       { */
/*         check_sum(m1,m2); */
/*         //ZMatrix<numero> suma(m1.n_rows,m1.n_columns); */
/*         suma.Suma(m1,m2); */
/*         return; */
/*       } */
    friend ZMatrix<numero> operator-(const ZMatrix<numero> &m1,const ZMatrix<numero> &m2)
      {
        check_dif(m1,m2);
        ZMatrix<numero> resta(m1.n_rows,m1.n_columns);
        resta.Resta(m1,m2);
        return resta;
      }
    numero dot(const ZMatrix<numero> &v2) const;
    //Producto escalar de este por v2.
    //v2: column vector.

    numero Abs2(void) const;
    numero Abs(void) const;

    inline friend numero dot(const ZMatrix<numero> &v1,const ZMatrix<numero> &v2)
    //Producto escalar de dos vectores.
    //v1: row vector.
    //v2: column vector.
      { return v1.dot(v2); }

/*     friend ZMatrix<numero> operator*(const ZMatrix<numero> &m1,const ZMatrix<numero> &m2) */
/*       { */
/*         check_prod(m1,m2); */
/*         ZMatrix<numero> producto(m1.n_rows,m2.n_columns); */
/*         for(size_type i=1;i<=m1.n_rows;i++) */
/*           for(size_type j=1;j<=m2.n_columns;j++) */
/*  	    { */
/*               numero t=numero(); */
/*               for(size_type k=1;k<= m1.n_columns;k++) */
/*                 t+= m1(i,k) * m2(k,j); */
/*               producto(i,j)= t; */
/* 	    } */
/*         return producto; */
/*       } */
    friend ZMatrix<numero> operator*(const ZMatrix<numero> &m,const numero &p)
      {
        ZMatrix<numero> producto(m);
        producto.Prod(p);
        return producto;
      }
    friend ZMatrix<numero> operator*(const numero &p,const ZMatrix<numero> &m)
      { return m*p; }
    friend ZMatrix<numero> operator^(const ZMatrix<numero> &v1,const ZMatrix<numero> &v2)
    //¡Ojo! está escrito para vectores de dimensión 3 xxx.
      { return prod_vectorial(v1,v2); }
  };

template <class numero> template<class InputIterator>
ZMatrix<numero>::ZMatrix(const size_t &n_rows,const size_t &n_columns,InputIterator b,InputIterator e)
  : mT_numero(n_rows,n_columns,b,e) {}

template <class numero>
ZMatrix<numero> operator-(const ZMatrix<numero> &m)
  {
    ZMatrix<numero> retval(m);
    retval.Neg();
    return retval;
  }

template <class numero>
ZMatrix<numero> ZMatrix<numero>::GetMenor(size_t f,size_t c) const
  { return ::GetMenor(*this,f,c); }
  
template <class numero>  
void ZMatrix<numero>::sumBox(size_t f,size_t c,const ZMatrix<numero> &box)
  {
    size_type i,j;
    for(i=1;i<=box.n_rows;i++)
      for(j=1;j<=box.n_columns;j++)
        PutSuma(i+f-1,j+c-1,box(i,j));
  }

template <class numero>
void ZMatrix<numero>::Idn(void)
  {
    size_type i;
    if(this->Cuadrada())
      {
	vectorZ_numero::Con(numero());
	for(i= 1;i<=this->n_rows;i++) (*this)(i,i)= neutro_producto(numero()); //Neutro del producto
      }
  }

//!  @brief Cross product. ¡Warning! written for column vectors
//! of dimension 3 xxx.
template <class MATR>
MATR prod_vectorial(const MATR &v1,const MATR &v2)
  {
    MATR v(v1.getNumberOfRows(),v1.getNumberOfColumns());
    MATR m(v1.getNumberOfRows(),3);
    m.PutCol(2,v1);
    m.PutCol(3,v2);
    MATR menor(2,2);
    menor= GetMenor(m,1,1);
    v(1,1)= menor.GetDet();
    menor= GetMenor(m,2,1);
    v(2,1)= -menor.GetDet();
    menor= GetMenor(m,3,1);
    v(3,1)= menor.GetDet();
    return v;
  }

//! @brief Return el determinante.
//! 
//! Return el determinante empleando un algoritmo recursivo lentísimo 
//! pero que no necesita divisiones.
template <class numero>
numero ZMatrix<numero>::GetDetLento(void) const
  {
    this->check_traza();
    numero p= neutro_suma(numero());
    const numero cero= neutro_suma(p);
    size_type n= this->getNumberOfRows();
    switch(n)
      {
        case 0:
          break;
        case 1:
          p= (*this)(1,1);
          break;
        case 2:
          p= (*this)(1,1)*(*this)(2,2)-(*this)(1,2)*(*this)(2,1);
          break;
        case 3:
          p= (*this)(1,1)*(*this)(2,2)*(*this)(3,3);
          p+= (*this)(1,2)*(*this)(2,3)*(*this)(3,1);
          p+= (*this)(1,3)*(*this)(2,1)*(*this)(3,2);
          p-= (*this)(1,3)*(*this)(2,2)*(*this)(3,1);
          p-= (*this)(1,2)*(*this)(2,1)*(*this)(3,3);
          p-= (*this)(1,1)*(*this)(2,3)*(*this)(3,2);
          break;
        default:
          {
            for(size_type i= 1;i<= n;i++)
	      {
                ZMatrix<numero> menor= (*this).GetMenor(i,1);
                numero n= (*this)(i,1)*menor.GetDetLento();
                if ((i+1)%2 == cero)
                  p+= n;
                else
                  p-= n;
              }
          }
          break;
      }
    return p;
  }

//! @brief Return el determinante.
//! 
template <class numero>
numero ZMatrix<numero>::GetDet(const numero &eps) const
  { return DMGT(eps); }

template <class numero>
std::ostream &operator<<(std::ostream &os,const ZMatrix<numero> &m)
  {
    os << '[';
    typename ZMatrix<numero>::size_type i,j;
    typename ZMatrix<numero>::size_type n_rows= m.getNumberOfRows(),n_columns= m.getNumberOfColumns();
    for(i= 1;i<=n_rows;i++)
      {
	os << '[';
        if(n_columns > 0) os << m(i,1);
	for(j= 2;j<=n_columns;j++)
	  os << ',' << m(i,j);
	os << ']';
      }
    os << ']';
    return os;
  }

template <class numero>
std::istream &operator>>(std::istream &is,ZMatrix<numero> &m)
  {
    m.Input(is);
    return is;
  }

template<class numero>
inline ZMatrix<numero> operator+(const ZMatrix<numero> &m1,const ZMatrix<numero> &m2)
  {
    ZMatrix<numero> suma(m1);
    check_sum(m1,m2);
    suma.Suma(m2);
    return suma;
  }

template<class numero>
inline ZMatrix<numero> operator*(const ZMatrix<numero> &m1,const ZMatrix<numero> &m2)
  {
    check_prod(m1,m2);
    ZMatrix<numero> producto(m1.getNumberOfRows(),m2.getNumberOfColumns());
    size_t n_rows= m1.getNumberOfRows();
    size_t ncols_m1= m1.getNumberOfColumns();
    size_t ncols= m2.getNumberOfColumns();
    typedef typename ZMatrix<numero>::size_type sz_type;
    for(sz_type i=1;i<=n_rows;i++)
      for(sz_type j=1;j<=ncols;j++)
        {
          numero t= m1(i,1) * m2(1,j); //Initialize (they can be matrices of boxes).
          for(sz_type k=2;k<= ncols_m1;k++)
            t+= m1(i,k) * m2(k,j);
          producto(i,j)= t;
        }
    return producto;
  }

template<class numero>
void ZMatrix<numero>::TSRGT(numero eps,size_t *it,ZMatrix<numero> &C,ZMatrix<size_t> &Kp,ZMatrix<size_t> &Lp) const
/*The function TSRGT applies to input real square matrix A(n,n) the upper
triangularization algorithm of Gauss method with full pivoting and keeps
trace of successive transformations done in integer vectors KP and LP.
----------------------------------------------------------------------------
  Input parameters:
  eps        precision (real)
  n          size of A matrix (integer)
  A          pointer to input real square matrix (double**)
  Output parameters:
  it         flag=1 if A matrix ok, =0 if A matrix is singular (integer)
  C          pointer to table storing main diagonal elements and supra-
             diagonal elements of upper triangular matrix and the multi-
             plying coefficients used during triangularization process (double**)
  KP         table storing informations concerning the column exchanges
             during process (int*)
  LP         table storing informations concerning the line exchanges
             during process (int*)
-----------------------------------------------------------------------------
The table C is first initialized to A matrix, then receives at each step k
of the triangularization process, usefull elements of A matrix at step k for
k=1,2,...n.
The variables po(real), lo and ko(integer) store respectively pivot at step k,
its line number and its column number.
-----------------------------------------------------------------------------*/
  {
    const size_t n= this->n_rows;
    size_t i,j,k;
    size_t ko,lo; numero po,t0;
    //Copy this matrix into C
    C= *this;
    *it=1; k=1;
    while (*it==1 && k<n)
      {
        po=C(k,k); lo=k; ko=k;
        for(i=k; i<=n; i++)
          for(j=k; j<=n; j++)
            if(fabs(C(i,j))>fabs(po))
              { po=C(i,j); lo=i; ko=j; }
        Lp(k)=lo; Kp(k)=ko;
        if(fabs(po)<eps)
          {
            *it=0;
	    std::cerr << std::endl
		      << "Error: pivot too small!" << std::endl;
          }
        else
          {
            if(lo!=k)
              for(j=k; j<=n; j++)
                { t0=C(k,j); C(k,j)=C(lo,j); C(lo,j)=t0; }
            if(ko!=k)
              for(i=1; i<=n; i++)
                { t0=C(i,k); C(i,k)=C(i,ko); C(i,ko)=t0; }
	    for(i=k+1; i<=n; i++)
              {
                C(i,k)/= po;
                for(j=k+1; j<=n; j++)
                  C(i,j)-= C(i,k)*C(k,j);
	      }
            k++;
          }
      }
    if(*it==1 && fabs(C(n,n))<eps) *it=0;
  } //TSRGT()

template <class numero>
numero ZMatrix<numero>::DMGT(const numero &eps) const
/*The function DMGT returns the determinant of a real square matrix
A(n,n) by Gauss method with full pivoting.
----------------------------------------------------------------------------
  Input parameters:
  eps        precision (double)
  n          size of A matrix (int)
  A          pointer to input real square matrix (double**)
  Output parameters:
  None
-----------------------------------------------------------------------------
The procedure TSRGT is used to reduce A matrix to an upper triangular matrix.
Output variables are it(integer), C(pMat), Kp and Lp(pVecI).
If it=0, matrix A is singular, if it=1, matrix A is regular. Table C contains
at location i,j (j>=i) the corresponding element of the upper triangular matrix.
Tables Lp and Kp contain informations relative to exchanges of line or column
that occured during the process. For instance, the element number k of Lp is
an integer <> k if an exchange of line has been made at step k (k=1,2,...,n).
The number of exchanges of lines and columns is stored in L(integer). the
determinant of A matrix is stored in d0(real).
Note: types pMat and pVecI must be declared and allocated for by main program,
except local variables C,Kp,Lp allocated (and disposed of) here.
-----------------------------------------------------------------------------*/
  {
    const size_t n= this->n_rows;
    size_t it;
    size_t k,l;
    numero d0;
    const numero cero= neutro_suma(d0);
    const numero uno= neutro_producto(d0);
    ZMatrix<numero> C(n,n);
    ZMatrix<size_t> Kp(n,1,0), Lp(n,1,0);

    TSRGT(eps,&it,C,Kp,Lp);  //call triangularization procedure

    if(it==0) //singular matrix, det=0
      return cero;  
    else //regular matrix, det<>0
      {         
        d0=uno;
        for(k=1; k<=n; k++)  d0 *= C(k,k);
        l=0;
        for(k=1; k<n; k++)
          {
            if (Lp(k)!=k)  l++;
            if (Kp(k)!=k)  l++;
          }
        if ((l%2)!=0)  d0=-d0;
      }
    return d0;  //return el determinante
  } //DMGT

//! @brief Return the box between the indices being passed as parameter.
template <class numero>
ZMatrix<numero> ZMatrix<numero>::getBox(size_t f1, size_t c1, size_t f2, size_t c2) const
  {
    this->check_get_box(f1,c1,f2,c2);
    ZMatrix<numero> box(f2-f1+1,c2-c1+1);
    for(size_type i=1;i<=box.n_rows;i++)
      for(size_type j=1;j<=box.n_columns;j++)
        box(i,j)= ZMatrix<numero>::operator()(i+f1-1,j+c1-1);
    return box;
  }

//! @brief Return the trace of the matrix.
template <class numero>
numero ZMatrix<numero>::Traza(void) const
  {
    this->check_traza();
    numero n= numero();
    for(size_type i= 1;i<=this->n_rows;i++) n+= (*this)(i,i);
    return n;
  }
template <class numero>
numero ZMatrix<numero>::row_sum(size_type i) const
  {
    numero sumaf= (*this)(i,1);
    for(size_t j=2;j<=this->n_columns;j++)
      sumaf+= (*this)(i,j);
    return sumaf;
  }
template <class numero>
numero ZMatrix<numero>::column_sum(size_type j) const
  {
    numero sumac= (*this)(1,j);
    for(size_t i=2;i<=this->n_columns;i++)
      sumac+= (*this)(i,j);
    return sumac;
  }
template <class numero>
numero ZMatrix<numero>::row_maximum(size_type i) const
  {
    numero maxf= (*this)(i,1);
    for(size_t j=2;j<=this->n_columns;j++)
      maxf= std::max(maxf,(*this)(i,j));
    return maxf;
  }
template <class numero>
numero ZMatrix<numero>::column_maximum(size_type j) const
  {
    numero maxc= (*this)(1,j);
    for(size_t i=2;i<=this->n_columns;i++)
      maxc+= std::max(maxc,(*this)(i,j));
    return maxc;
  }
//! @brief Return el valor máximo de los elementos del vector
//! que resulta de sumar los rows elements.
template <class numero>
numero ZMatrix<numero>::getRowNorm(void) const
  {
    numero maximo= row_sum(1);
    for(size_t i=2;i<=this->n_rows;i++) maximo= max(maximo,row_sum(i));
    return maximo;
  }
//! @brief Return the maximum value of the components of the vector obtained by
//! adding the components of the columns.
template <class numero>
numero ZMatrix<numero>::getColumnNorm(void) const
  {
    numero maximo= column_sum(1);
    for(size_t j=2;j<=this->n_columns;j++) maximo= max(maximo,column_sum(j));
    return maximo;
  }
template <class numero>
numero ZMatrix<numero>::dot(const ZMatrix<numero> &v2) const
//Producto escalar de este por v2.
//v2: column vector.
  {
    check_dot(*this,v2);
    numero retval((*this)(1,1)*v2(1,1));
    typename ZMatrix<numero>::size_type i;
    for(i=2;i<=this->n_columns;i++)
      { retval+= (*this)(1,i) * v2(i,1); }
    return retval;
  }

//! @brief Return the squared norm (euclidean norm) of the matrix.
template <class numero>
numero ZMatrix<numero>::Abs2(void) const
  {
    numero r= numero();
    const ZMatrix<numero> trn= this->GetTrn();
    if(this->isRow()) //is a row matrix.
      r= dot(trn);
    else
      if(this->isColumn()) //is a column matrix.
        r= trn.dot(*this);
      else //Es cualquiera
        {
          const ZMatrix<numero> prod= (*this)*trn;
          r= prod.Traza();
        }
    return r;
  }

//! @brief Return the norm (euclidean norm) of the matrix.
template <class numero>
numero ZMatrix<numero>::Abs(void) const
  { return sqrt(Abs2()); }

//! @brief Lectura desde istream.
template <class numero>
void ZMatrix<numero>::Input(std::istream &is)
  {
    char c;
    typename ZMatrix<numero>::lst_numero ln;
    size_t n_rows=0,n_columns=1;
    int sigue= 1;
    numero ni;
    is >> c;
    if(c != '[') is.putback(c);
    std::istream::sentry ipfx(is);
    while(ipfx)
      {
        is >> c;
        switch(c)
          {
	  case '[':
            n_rows++;
            break;
          case ']':
            sigue= 0;
            is >> c;
            if(c != ']')
              {
                is.putback(c);
                continue;
              }
            else
              {
                (*this)= ZMatrix<numero>(n_rows,n_columns,ln.begin(),ln.end());
                return;
              }
            break;
          case ',':
            if(sigue) n_columns++;
            break;
          default:
            break;
          }
        is >> ni;
        ln.push_back(ni);
      }
    (*this)= ZMatrix<numero>(n_rows,n_columns,ln.begin(),ln.end());
    return;
  }

//! @brief Lectura desde string.
template <class numero>
void ZMatrix<numero>::Input(const std::string &str)
  {
    std::istringstream iss(str);
    Input(iss);
  }

//! @brief Return the matrix norm.
template <class treal>  
inline treal Abs(const ZMatrix<treal> &m)
  { return m.Abs(); }

#endif

