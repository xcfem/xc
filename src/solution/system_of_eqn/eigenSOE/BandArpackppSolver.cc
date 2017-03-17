//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//BandArpackppSolver.cpp

#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSOE.h>
#include "arpack++/arbnsmat.h"
#include "arpack++/arbgnsym.h"

//! @brief Constructor.
XC::BandArpackppSolver::BandArpackppSolver(void)
:EigenSolver(EigenSOLVER_TAGS_BandArpackppSolver),
 theSOE(nullptr), eigenvalues(1), eigenvectors(1,Vector()) {}

//! @brief Constructor.
XC::BandArpackppSolver::BandArpackppSolver(const int &nModes)
 :EigenSolver(EigenSOLVER_TAGS_BandArpackppSolver,nModes),
 theSOE(nullptr), eigenvalues(nModes), eigenvectors(nModes,Vector()) {}

//! @brief Solves para todos los eigenvalues del problema.
int XC::BandArpackppSolver::solve(void)
  {return solve(theSOE->size);}

ARbdNonSymMatrix<double, double> creaARbdNonSymMatrix(const msp_double &m,XC::Vector &v)
  {
    const size_t ndiagL= m.ndiagL();
    const size_t ndiagU= m.ndiagU();
    const size_t ncols= m.getNumCols();
    assert(m.getNumFilas()==ncols);
    const size_t sizeCol= (ndiagL+ndiagU+1);
    const size_t sizeV= sizeCol*ncols;
    v.resize(sizeV);
    v.Zero();
    
    m.FillVectorBanda(v.getDataPtr());
    return ARbdNonSymMatrix<double, double>(ncols,ndiagL,ndiagU,v.getDataPtr());
  }

void XC::BandArpackppSolver::setup_autos(const size_t &nmodos,const size_t &n)
  {
    if(eigenvalues.size()!=nmodos)
      eigenvalues.resize(nmodos);
    if(eigenvectors.size()!=nmodos)
      eigenvectors.resize(nmodos);
    for(size_t i=0;i<nmodos;i++)
      eigenvectors[i].resize(n);
  }

ARluNonSymGenEig<double> getEigenProblem(const int &nmodes,ARbdNonSymMatrix<double, double> &ak,ARbdNonSymMatrix<double, double> &am,const double &shift)
  {
    if(shift<0.0)
      return ARluNonSymGenEig<double>(nmodes, ak, am); //Greatest eigenvalues.
    else
      return ARluNonSymGenEig<double>(nmodes, ak, am,shift); //Eigenvalues nearest to shift.
  }


int XC::BandArpackppSolver::solve(int nModes)
  {
    int retval= 0;
    if(!theSOE)
      {
        std::cerr << "BandArpackppSolver::solve(); aún no se ha asignado the problema (EigenSOE).\n";
        retval= -1;
      }
    else
      {
        numModes = nModes; // Set number of modes
        const int n= theSOE->size; // Number of equations

        // Check for quick return
        if((numModes <= 1) || (numModes>=(n-1)))
          {
            numModes= 0;
	    std::cerr << "BandArpackppSolver::solve(); the número de modos ha de"
                      << "estar comprendido entre: " << 2 << " y " << n-2 << ".\n";
            retval= -2;
          }
        else
          {
            Vector vk;
            ARbdNonSymMatrix<double, double> ak=  creaARbdNonSymMatrix(theSOE->A,vk);
            Vector vm;
            ARbdNonSymMatrix<double, double> am=  creaARbdNonSymMatrix(theSOE->M,vm);
            ARluNonSymGenEig<double> dprob= getEigenProblem(numModes, ak, am,theSOE->shift);

            if(verborrea>5)
              dprob.Trace();

            // Finding eigenvalues and eigenvectors.
            dprob.FindEigenvectors();

            //dprob.NoTrace();
            // Almacenamos la solucion.
            const size_t nconv= dprob.ConvergedEigenvalues();
            if(nconv>0)
              {
                setup_autos(nconv,n);
                for(size_t i=0; i<nconv; i++)
                  eigenvalues[i]= dprob.EigenvalueReal(i);

                for(size_t i=0; i<nconv; i++)
                  for(int j=0; j<n;j++)
                    eigenvectors[i](j)= dprob.RawEigenvector(i)[j];
               }
            else
              retval= -3;
          }
      }
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::BandArpackppSolver::setEigenSOE(EigenSOE *soe)
  {
    bool retval= false;
    BandArpackppSOE *tmp= dynamic_cast<BandArpackppSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr << nombre_clase() << "::setEigenSOE: the system of equations no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

//! @brief Sets the eigenproblem to solve.
bool XC::BandArpackppSolver::setEigenSOE(BandArpackppSOE &theBandSOE)
  { return setEigenSOE(&theBandSOE); }

const XC::Vector &XC::BandArpackppSolver::getEigenvector(int mode) const
  {
    static Vector retval(1);
    if(mode < 1 || mode > numModes)
      {
        std::cerr << "BandArpackppSolver::getEigenVector() -- mode " << mode << " is out of range (1 - "
	          << numModes << ")\n";
      }
    else
      retval= eigenvectors[mode-1];
    return retval;
  }

const double &XC::BandArpackppSolver::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    if(mode < 1 || mode > numModes)
        std::cerr << "BandArpackppSolver::getEigenvalue() -- mode " 
                  << mode << " is out of range (1 - " << numModes << ")\n";
    else
      retval= eigenvalues[mode-1];
    return retval;
  }

int XC::BandArpackppSolver::setSize(void)
  {
    setup_autos(numModes,theSOE->size);
    return 0;
  }

//! @brief Return the dimensión de los eigenvectors.
const int &XC::BandArpackppSolver::getSize(void) const
  { return theSOE->size; }

int XC::BandArpackppSolver::sendSelf(CommParameters &cp)
  { return 0; }

int XC::BandArpackppSolver::recvSelf(const CommParameters &cp)
  { return 0; }
