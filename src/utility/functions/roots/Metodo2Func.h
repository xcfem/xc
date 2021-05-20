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
//Metodo2Func.h

#ifndef METODO2FUNC_H
#define METODO2FUNC_H


//! @brief Converts a method into a function in order to
//! allow calling the root computation templates with it.
template<class T,class metodo>
class Metodo2Func
  {
  protected:
    T *t_;
    metodo f_;
  public:
    Metodo2Func(void)
      : t_(NULL), f_(NULL) {}
    Metodo2Func(T &t,metodo f)
      : t_(&t), f_(f) {}
    Metodo2Func(const Metodo2Func<T,metodo> &otro)
      {
        t_= otro.t_;
        f_= otro.f_;
      }
    Metodo2Func &operator=(const Metodo2Func<T,metodo> &otro)
      {
        t_= otro.t_;
        f_= otro.f_;
        return *this;
      }
  };

template<class T,class metodo>
class Metodo2FuncDDP: public Metodo2Func<T,metodo>
  {
    typedef Metodo2Func<T,metodo> metodo_2_func_t_metodo;

    inline double eval(const double &d,const double *p) const
      { return (this->t_ && this->f_) ? (this->t_->*this->f_)(d,p) : 0.0; }
  public:
    Metodo2FuncDDP(T &t,metodo f)
      : metodo_2_func_t_metodo(t,f) {}
    inline double operator()(const double &d,const double *p) const
      { return eval(d,p); }
  };

#endif
