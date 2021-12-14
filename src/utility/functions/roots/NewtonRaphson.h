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
//NewtonRaphson.h

#ifndef NEWTONRAPHSON_H
#define NEWTONRAPHSON_H


#include "utility/utils/misc_utils/mchne_eps.h"
#include "utility/matrices/m_double.h"
#include "utility/matrices/m_func.h"
#include "utility/matrices/m_int.h"
#include "utility/matrices/solvers/solver_lu.h"
#include "utility/functions/op_dif/op_dif.h"
#include "utility/functions/max_min/max_min.h"

bool norma_inf_nula(const m_double &x,const double &tolerance);

double dot_trn(const m_double &v1,const m_double &v2);
    //Producto escalar de dos vectores COLUMNA
     //Sirve para evitar calcular la traspuesta de v1.

template<class MF>
class Function_f
  {
    const MF &F;
  public:
    Function_f(const MF &F_)
      : F(F_) {}
    inline double operator()(const m_double &x) const
      { 
        m_double val= F.Eval(x);
        return 0.5*dot_trn(val,val);
      }
  };

template<class MF>
class Function_g: public Function_f<MF>
  {
    const m_double &x;
    const m_double &dx;
  public:
    Function_g(const MF &F_,const m_double &x_,const m_double &dx_)
      : Function_f<MF>(F_),x(x_),dx(dx_) {}
    inline double operator()(const double &lambda) const
      { return Function_f<MF>::operator()(x+lambda*dx); }
  };

template <class F>
double baja_hasta( const double &a,const double &b,const double &umbral,
                   const F &f,const double &dx= sqrt_mchne_eps_dbl)
  {
    const bool minimo= true;
    bool crece, cambia;

    double ab = b - a;
    double valor= 0;
    m_double x= extremo_calcx(a,b,ab);
    m_double y= matrix_argument(f,x);

    while (ab > dx)
      {
        crece = (y(ant+1,(long)interior) < y(post+1,(long)interior));
        cambia= (crece == minimo);   /* extremo que cambia */
        x= extremo_cx(x,cambia,(long)interior,(long)exterior);
        y= extremo_cy(f,y,x,cambia,(long)interior,(long)exterior);
        ab = x(post+1,(long)exterior) - x(ant+1,(long)exterior);
        valor= 0.5*(y(post+1,(long)exterior) - y(ant+1,(long)exterior));
        if(valor<umbral)
	  break;
      }
    return (0.5 * (x(1,(long)exterior) + x(2,(long)exterior)));
  }

template<class MF>
double busqueda_direccional(const MF &F,const m_double &x,const m_double &dx,const size_t &verbosity= 1)
  {
    Function_f<MF> f(F);
    const m_double nabla_f= gradiente_cdf(x,f);
    const double prod= dot_trn(nabla_f,dx);
    double lambda= 1.0; //Valor por defecto.
    if(prod>0.0) //Problemas de estabilidad numérica.
      {
        if(verbosity)
          {
            std::cerr << "Problemas de estabilidad numérica en búsqueda direccional." << std::endl;
            std::cerr << "  nabla_f= " << nabla_f << std::endl;
            std::cerr << "  dx= " << dx << std::endl;
            std::cerr << "  nabla_f . dx= " << prod << std::endl;
          }
      }
    else
      {
        const double f_nuevo= f(x+lambda*dx);
        const double f_viejo= f(x);
        const double umbral= f_viejo+1e-4*prod;
        if(f_nuevo>umbral)
          {
            Function_g<MF> g(F,x,dx);
            lambda= baja_hasta(0.0, 1.0, umbral, g);
          }
      }
    //lambda= max(lambda,0.01); //xxx ???
    return lambda;
  }

template<class MF>
class NewtonRaphson
  {
    size_t iter; //Contador de iteraciones.
    int error; //Flag de error. 0: Se produjo un error.
    size_t maxIter; //Número máximo de iteraciones.
    size_t verbosity;
    m_double actualiza_delta_X(SolverLU<m_double,m_double> &solver,m_double &J,const m_double &residuo,const m_double &X)
      {
        m_double retval(residuo);
        // solve for the guess refinement vector
        solver.putMatrix(J);
        if(!solver.Decomp()) //Jacobiano singular.
          {
            error= 0;
            if(verbosity)
              {
                std::cerr << "No se pudo hacer la descomposición LU para el jacobiano:" << std::endl;
                std::cerr << "  X= " << X << std::endl;
                std::cerr << "  F= " << residuo << std::endl;
                std::cerr << "  J= " << J << std::endl;
              }
          }
        else //Jacobiano no singular.
          retval= solver.BackSub(-residuo);
        return retval;
      }
    bool actualiza_moreIter(const m_double &residuo,const double &tol_dx) const
      {
        bool retval= true;
        if(norma_inf_nula(residuo,tol_dx)) retval= false; //Si residuo es grande continua.
        if(retval) retval = (iter < maxIter) ? true : false; //Si número de iteraciones menor que máximo continúa.
        return retval;
      }
  public:
    NewtonRaphson(const size_t &vrbrr= 1)
      : iter(0),error(1),maxIter(30), verbosity(vrbrr) {}
    const size_t &GetNumIter(void) const
      //Devuelve el número de iteraciones empleadas para el cálculo.
      { return iter; }
    const int &GetError(void) const
      { return error; }
    m_double Itera( const MF &F,const m_double &Xguess,
                    const double &tol_f= sqrt_mchne_eps_dbl,
                    const double &tol_dx= sqrt_mchne_eps_dbl);
  };

template<class MF>
m_double NewtonRaphson<MF>::Itera( const MF &F,const m_double &Xguess,
                                   const double &tol_f,
                                   const double &tol_dx)
  {
    bool moreIter;
    iter= 0;
    m_double X(Xguess);
    m_double X_buena(X); //Última X para la que no se produjo error.
    m_double delta_X(X); //Incremento de X.
    size_t numEqns= X.getNumberOfRows();

    m_double J(numEqns,numEqns); //Matrix for the jacobian.
    m_double residuo(numEqns,1); //Residual vector.
    double lambda= 1.0;
    SolverLU<m_double,m_double> solver(verbosity);
    do
      {
        iter++;
        residuo= F.Eval(X); //Calcula la imagen de X.
        if(norma_inf_nula(residuo,tol_f)) //Si es ya muy pequeño salimos.
          {
            moreIter= 0;
            X_buena= X;
            break; //Solución obtenida, hemos terminado.
          }
        
        J= jacobiano_cdf(X,F); // Actualiza el jacobiano de F.
        delta_X= actualiza_delta_X(solver,J,residuo,X); //Calcula el incremento de X.
        if(error==0)
          {
            break; //Si se produjo un error hemos terminado.
          }
        else
          X_buena= X;

        // update guess and test for convergence
        lambda= busqueda_direccional(F,X,delta_X,verbosity);
        X+= lambda*delta_X;
        // clear the more-iteration flag
        moreIter = actualiza_moreIter(delta_X,tol_dx);
        if(!moreIter) break;
      }
    while(true);
    error = 1 - moreIter;
    return X_buena;
  }

#endif




