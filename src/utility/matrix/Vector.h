//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.9 $
// $Date: 2005/12/14 23:49:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/matrix/Vector.h,v $

                                                                        
// File: ~/utility/matrix/Vector.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for Vector.
// Vector is a concrete class implementing the vector abstraction.
//
// What: "@(#) Vector.h, revA"

#ifndef Vector_h
#define Vector_h 

#include "xc_utils/src/base/CmdVectorBase.h"
#include "xc_basic/src/matrices/m_double.h"

class Vector2d;
class Vector3d;

namespace XC {
class ID;

class Matrix; 
class Message;
class SystemOfEqn;

class Vector: public CmdVectorBase
  {
  private:
    static double VECTOR_NOT_VALID_ENTRY;
    int sz;
    double *theData;
    int fromFree;
    void libera(void);
    void alloc(const size_t &sz);
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors and destructor
    Vector();
    explicit Vector(const int &, const double &valor= 0.0);
    explicit Vector(const std::vector<double> &v);
    explicit Vector(const Vector2d &v);
    explicit Vector(const Vector3d &v);
    Vector(const double &,const double &,const double &);
    Vector(const Vector &);    
    Vector(double *data, int size);
    Vector(const boost::python::list &);
    ~Vector(void);

    // utility methods
    int setData(double *newData, int size);
    const double *getDataPtr(void) const;
    double *getDataPtr(void);
    bool Nulo(void) const;
    int Assemble(const Vector &V, const ID &l, double fact = 1.0);
    void from_string(const std::string &str);
    double Norm2(void) const;
    double Norm(void) const;
    double pNorm(int p) const;
    double NormInf(void) const;
    int Size(void) const;
    int getNumBytes(void) const;
    int resize(int newSize);
    void Zero(void);
    bool isnan(void) const;
    int reset(const int &newSize);
    int Normalize(void);
    int NormalizeInf(void);
    Vector Normalized(void) const;
    Vector NormalizedInf(void) const;
    
    int addVector(double factThis, const Vector &other, double factOther);
    int addMatrixVector(double factThis, const Matrix &m, const Vector &v, double factOther); 
    int addMatrixTransposeVector(double factThis, const Matrix &m, const Vector &v, double factOther);

    
    //! @brief Devuelve el elemento cuya fila se pasa como parámetro.
    virtual double &at(const size_t &f);
    //! @brief Devuelve el elemento cuya fila se pasa como parámetro.
    virtual const double &at(const size_t &f) const;
    //! @brief Chequea el índice que se le pasa como parámetro.
    virtual bool CheckIndice0(const size_t &i) const;

    // overloaded operators
    const double &operator()(int x) const;
    double &operator()(int x);
    const double &operator[](int x) const;  // these two operator do bounds checks
    double &operator[](int x);
    Vector operator()(const ID &rows) const;
    Vector &operator=(const Vector  &V);

    template <class TNSR>
    Vector &operator=(const TNSR &T);
    
    Vector &operator+=(double fact);
    Vector &operator-=(double fact);
    Vector &operator*=(double fact);
    Vector &operator/=(double fact); 

    Vector operator+(double fact) const;
    Vector operator-(double fact) const;
    Vector operator*(double fact) const;
    Vector operator/(double fact) const;
    
    Vector &operator+=(const Vector &V);
    Vector &operator-=(const Vector &V);
    
    Vector operator+(const Vector &V) const;
    Vector operator-(const Vector &V) const;
    double operator^(const Vector &V) const;
    Vector operator/(const Matrix &M) const;

    double dot(const Vector &) const;    

    // methods added by Remo
    int Assemble(const Vector &V, int init_row, double fact = 1.0);
    int Extract(const Vector &V, int init_row, double fact = 1.0); 

    Vector getComponents(const ID &) const; 
    void putComponents(const Vector &,const ID &); 
    void addComponents(const Vector &,const ID &); 
  
    virtual any_const_ptr GetProp(const std::string &cod) const;
    void write(std::ofstream &);
    void read(std::ifstream &);
    friend std::ostream &operator<<(std::ostream &s, const Vector &V);
    // friend istream &operator>>(istream &s, Vector &V);    
    friend Vector operator*(double a, const Vector &V);
    
    friend class Message;
    friend class SystemOfEqn;
    friend class Matrix;
    friend class TCP_SocketNoDelay;    
    friend class TCP_Socket;
    friend class UDP_Socket;
    friend class MPI_Channel;
  };

std::vector<double> vector_to_std_vector(const Vector &);
m_double vector_to_m_double(const Vector &);
Vector interpreta_xc_vector(const std::string &str);
Vector convert_to_vector(const boost::any &);

double dot(const Vector &a,const Vector &b);
Matrix prod_tensor(const Vector &,const Vector &);
Matrix operator&(const Vector &u,const Vector &v);

Vector normalize(const Vector &);
Vector normalize_inf(const Vector &);



/********* INLINED VECTOR FUNCTIONS ***********/
inline int Vector::Size(void) const 
  { return sz; }

inline int Vector::getNumBytes(void) const
  { return Size()*sizeof(double); }

inline const double *Vector::getDataPtr(void) const
  { return theData; }

inline double *Vector::getDataPtr(void)
  { return theData; }

inline bool Vector::Nulo(void) const
  { return (theData==NULL); }

inline void Vector::Zero(void)
  {
    for(register int i=0; i<sz; i++)
      theData[i] = 0.0;
  }

inline bool Vector::isnan(void) const
  {
    bool retval= false;
    for(register int i=0; i<sz; i++)
      if(::isnan(theData[i]))
        {
          retval= true;
          break;
        }
    return retval;
  }

inline int Vector::reset(const int &newSize)
  {
    const int retval= resize(newSize);
    Zero();
    return retval;
  }

inline const double &Vector::operator()(int x) const
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(x < 0 || x >= sz)
      {
	std::cerr << "XC::Vector::(loc) - loc " << x << " outside range [0, " << sz-1 << endln;
        return VECTOR_NOT_VALID_ENTRY;
      }
#endif
    return theData[x];
  }


inline double &Vector::operator()(int x)
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if (x < 0 || x >= sz)
      {
	std::cerr << "XC::Vector::(loc) - loc " << x << " outside range [0, " << sz-1 << std::endln;
        return VECTOR_NOT_VALID_ENTRY;
      }
#endif
    return theData[x];
  }

template <class TNSR>
Vector & Vector::operator=(const TNSR &V) 
  {
    int rank= V.rank();
    if(rank != 2)
      {
        std::cerr << "XC::Vector::operator=() - BJtensor must be of rank 2\n";
        return *this;
      }
    int dim= V.dim(1);
    if(dim != V.dim(2))
      {
        std::cerr << "XC::Vector::operator=() - BJtensor must have square dimensions\n";
        return *this;
      }
  
    if(dim != 2 || dim != 3 || dim != 1)
      {
        std::cerr << "XC::Vector::operator=() - BJtensor must be of dimension 2 or 3\n";
        return *this;
      }      
  
    if(dim == 1)
      {
        if(sz != 1)
           {
             std::cerr << "XC::Vector::operator=() - Vector size must be 1\n"; 
             return *this;
           }
        theData[0]= V.cval(1,1);
      }
    else if(dim == 2)
      {
        if(sz != 3)
          {
            std::cerr << "XC::Vector::operator=() - Vector size must be 3\n"; 
            return *this;
          }
        theData[0]= V.cval(1,1);
        theData[1]= V.cval(2,2);
        theData[2]= V.cval(1,2);
      }
    else
      {
        if(sz != 6)
          {
            std::cerr << "XC::Vector::operator=() - Vector size must be 6\n"; 
            return *this;
          }      
        theData[0]= V.cval(1,1);
        theData[1]= V.cval(2,2);
        theData[2]= V.cval(3,3);
        theData[3]= V.cval(1,2);
        theData[4]= V.cval(1,3);
        theData[5]= V.cval(2,3);
      }
    return *this;
  }

} // fin namespace XC


#endif

