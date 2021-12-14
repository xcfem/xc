//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//FunctionFromPointsR_R.cc

#include "FunctionFromPointsR_R.h"
#include <set>

//! @brief Operador asignación.
FunctionFromPointsR_R &FunctionFromPointsR_R::operator=(const FunctionFromPointsR_R &other)
  {
    FunctionFromPointsR_T<double>::operator=(other);
    return *this;
  }

//! @brief Assignment operator. Equal to a constant.
FunctionFromPointsR_R &FunctionFromPointsR_R::operator=(const double &c)
  {
    (*this)= asigna(*this,c);
    return *this;
  }

//! @brief Return the position of the maximum.
FunctionFromPointsR_R::const_iterator FunctionFromPointsR_R::PosMax(void) const
  {
    const_iterator i= begin();
    const_iterator retval= i;
    double max= (*i).second;
    for(;i!=end();i++)
      if(max < (*i).second)
        {
          max= (*i).second;
          retval= i;
        }
    return retval;
  }
//! @brief Return the position of the minimum.
FunctionFromPointsR_R::const_iterator FunctionFromPointsR_R::PosMin(void) const
  {
    const_iterator i= begin();
    const_iterator retval= i;
    double min= (*i).second;
    for(;i!=end();i++)
      if(min > (*i).second)
        {
          min= (*i).second;
          retval= i;
        }
    return retval;
  }

//! @brief Return the function domain.
dq_double FunctionFromPointsR_R::Dominio(void) const
  {
    dq_double retval(2,0.0);
    retval[0]= begin()->first;
    retval[1]= rbegin()->first;
    return retval;
  }

//! @brief Return the abcissae of the function definition points.
dq_double FunctionFromPointsR_R::Abcisas(void) const
  {
    dq_double retval;
    const_iterator i= begin();
    for(;i!=end();i++)
      retval.push_back((*i).first);
    return retval;
  }

//! @brief Return las ordenadas correspondientes a cada abcisa.
dq_double FunctionFromPointsR_R::Ordenadas(void) const
  {
    dq_double retval;
    const_iterator i= begin();
    for(;i!=end();i++)
      retval.push_back((*i).second);
    return retval;
  }

//! @brief Return the roots of the function.
dq_double FunctionFromPointsR_R::Ceros(void) const
  {
    dq_double retval;
    const_iterator i= begin();
    const_iterator j= i;
    j++;
    for(;j!=end();i++,j++)
      {
        const double y0= (*i).second;
        const double y1= (*j).second;
        if(y0*y1<=0.0)
          {
            double x0= (*i).first;
            double x1= (*j).first;
            double a= (y1-y0)/(x1-x0);
            double b= y1-a*x1;
            retval.push_back(-(b/a));
          }
      }
    return retval;
  }

double FunctionFromPointsR_R::firstX(void) const
  {
    double retval= 0.0;
    if(!empty())
      retval= begin()->first;
    else
      std::cerr << "Undefined function." << std::endl;
    return retval;
  }
double FunctionFromPointsR_R::firstY(void) const
  {
    double retval= 0.0;
    if(!empty())
      retval= begin()->second;
    else
      std::cerr << "Undefined function." << std::endl;
    return retval;
  }
double FunctionFromPointsR_R::lastX(void) const
  {
    double retval= 0.0;
    if(!empty())
      retval= rbegin()->first;
    else
      std::cerr << "Undefined function." << std::endl;
    return retval;
  }
double FunctionFromPointsR_R::lastY(void) const
  {
    double retval= 0.0;
    if(!empty())
      retval= rbegin()->second;
    else
      std::cerr << "Undefined function." << std::endl;
    return retval;
  }


//! @brief Return the area enclosed by the function.
double FunctionFromPointsR_R::getArea(void) const
  {
    double retval= 0.0;
    const_iterator i= begin();
    const_iterator j= i;
    j++;
    double x0,y0,x1,y1;
    for(;j!=end();i++,j++)
      {
        x0= (*i).first; y0= (*i).second;
        x1= (*j).first; y1= (*j).second;
        retval+= (y0+y1)/2*(x1-x0); //Trapecio.
      }
    return retval;
  }

//! @brief Return las abcisas entre x0 and x1 (ambas inclusive).
FunctionFromPointsR_R getSub(const FunctionFromPointsR_R &f1,const double &x0,const double &x1)
  {
    FunctionFromPointsR_R retval;
    double X0= x0;
    double X1= x1;
    if(X1<X0)
      std::swap(X1,X0);
    std::set<double> tmp;
    tmp.insert(X0);
    tmp.insert(X1);
    const dq_double a= f1.Abcisas();
    for(dq_double::const_iterator i= a.begin();i!=a.end();i++)
      if((X0<*i) && (*i<X1))
        tmp.insert(*i);
    for(std::set<double>::const_iterator i= tmp.begin();i!=tmp.end();i++)
        retval.insert(*i,f1.Valor(*i));
    return retval;
  }

//! @brief Return the part of the function between x0 and x1.
FunctionFromPointsR_R FunctionFromPointsR_R::sub(const double &x0,const double &x1) const
  { return getSub(*this,x0,x1); }


//! @brief Return the function with the opposite sign.
FunctionFromPointsR_R FunctionFromPointsR_R::getNeg(void) const
  { return (*this)*-1.0; }

//! @brief Change the sign of the function.
FunctionFromPointsR_R &FunctionFromPointsR_R::Neg(void)
  {
    (*this)= getNeg();
    return *this;
  }

FunctionFromPointsR_R &FunctionFromPointsR_R::operator+=(const double &c)
  {
    (*this)= (*this)+c;
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator+=(const FunctionFromPointsR_R &f)
  {
    (*this)= (*this)+f;
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator-=(const double &c)
  {
    (*this)= (*this)+(-c);
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator-=(const FunctionFromPointsR_R &f)
  {
    (*this)= (*this)-f;
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator*=(const double &c)
  {
    (*this)= (*this)*c;
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator*=(const FunctionFromPointsR_R &f)
  {
    (*this)= (*this)*f;
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator/=(const double &c)
  {
    (*this)= (*this)*(1.0/c);
    return *this;
  }
FunctionFromPointsR_R &FunctionFromPointsR_R::operator/=(const FunctionFromPointsR_R &f)
  {
    (*this)= (*this)/f;
    return *this;
  }

//! @brief Return the function that results from the integration
//! between x0 and x1.
FunctionFromPointsR_R FunctionFromPointsR_R::integra(const double &x0,const double &x1,const double &d) const
  {
    double delta(d);
    FunctionFromPointsR_R retval;
    const double longIntervalo= x1-x0;
    if(delta<0.0)
      delta= (x1-x0)/100;
    if(delta<=0.0)
      {
        std::cerr << "FunctionFromPointsR_R::integra() Attempting to integrate time step" 
                  << delta << "<= 0\n";
        return retval;
      }

    // Add one to get ceiling out of type cast
    const size_t numSteps = (int)(longIntervalo/delta + 1.0);

    retval.insert(x0,0.0);
    // Set the second point
    // Assuming initial condition is zero, i.e. F(0) = 0
    double previousValue= (*this)(x0); // Temporary storage to avoid accessing same value twice
	                             // through identical method calls
  
    double dummyX= x0+delta; // Dummy variable for integrating
    double currentValue= 0.0;
    double acum= 0.0;
    
    for(size_t i= 1;i<numSteps;i++,dummyX+=delta)
      {
        currentValue = (*this)(dummyX);
        // Apply the trapezoidal rule to update the integrated value
        acum+= delta*(currentValue+previousValue)/2.0;
        retval.insert(dummyX,acum);
        previousValue = currentValue;
      }
    return retval;
  }

//! @brief Assigns a constant to the function.
FunctionFromPointsR_R asigna(const FunctionFromPointsR_R &f,const double &c)
  {
    FunctionFromPointsR_R retval;
    for(FunctionFromPointsR_R::const_iterator i= f.begin();i!=f.end();i++)
      retval.insert((*i).first,c);
    return retval;
  }

//! @brief Sums a constant to the function.
FunctionFromPointsR_R operator+(const FunctionFromPointsR_R &f,const double &c)
  {
    FunctionFromPointsR_R retval;
    for(FunctionFromPointsR_R::const_iterator i= f.begin();i!=f.end();i++)
      retval.insert((*i).first,(*i).second+c);
    return retval;
  }

dq_double merge_abcisas(const FunctionFromPointsR_R &f1,const FunctionFromPointsR_R &f2)
  {
    dq_double retval;
    const dq_double a= f1.Abcisas();
    const dq_double b= f2.Abcisas();
    if(a==b)
      retval= a;
    else
      {
        std::set<double> tmp;
        tmp.insert(a.begin(),a.end());
        tmp.insert(b.begin(),b.end());
        for(std::set<double>::const_iterator i= tmp.begin();i!=tmp.end();i++)
          retval.push_back(*i);
      }
    return retval;
  }

//! @brief Function addition.
FunctionFromPointsR_R operator+(const FunctionFromPointsR_R &f1,const FunctionFromPointsR_R &f2)
  {
    FunctionFromPointsR_R retval;
    dq_double abcisas= merge_abcisas(f1,f2);
    for(dq_double::const_iterator i= abcisas.begin();i!=abcisas.end();i++)
      retval.insert((*i),f1.Valor(*i)+f2.Valor(*i));
    return retval;
  }

//! @brief Function substraction.
FunctionFromPointsR_R operator-(const FunctionFromPointsR_R &f1,const FunctionFromPointsR_R &f2)
  {
    FunctionFromPointsR_R retval;
    dq_double abcisas= merge_abcisas(f1,f2);
    for(dq_double::const_iterator i= abcisas.begin();i!=abcisas.end();i++)
      retval.insert((*i),f1.Valor(*i)-f2.Valor(*i));
    return retval;
  }

//! @brief Function product.
FunctionFromPointsR_R operator*(const FunctionFromPointsR_R &f1,const FunctionFromPointsR_R &f2)
  {
    FunctionFromPointsR_R retval;
    dq_double abcisas= merge_abcisas(f1,f2);
    for(dq_double::const_iterator i= abcisas.begin();i!=abcisas.end();i++)
      retval.insert((*i),f1.Valor(*i)*f2.Valor(*i));
    return retval;
  }

//! @brief Function division.
FunctionFromPointsR_R operator/(const FunctionFromPointsR_R &f1,const FunctionFromPointsR_R &f2)
  {
    FunctionFromPointsR_R retval;
    dq_double abcisas= merge_abcisas(f1,f2);
    for(dq_double::const_iterator i= abcisas.begin();i!=abcisas.end();i++)
      retval.insert((*i),f1.Valor(*i)/f2.Valor(*i));
    return retval;
  }

//! @brief Multiplies the function by a constant.
FunctionFromPointsR_R operator*(const double &c,const FunctionFromPointsR_R &f)
  {
    FunctionFromPointsR_R retval;
    for(FunctionFromPointsR_R::const_iterator i= f.begin();i!=f.end();i++)
      retval.insert((*i).first,(*i).second*c);
    return retval;
  }

