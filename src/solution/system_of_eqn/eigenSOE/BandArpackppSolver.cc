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
//BandArpackppSolver.cpp

#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSOE.h>
#include "arpack++/arbnsmat.h"
#include "arpack++/arbgnsym.h"

//! @brief Constructor.
XC::BandArpackppSolver::BandArpackppSolver(void)
:EigenSolver(EigenSOLVER_TAGS_BandArpackppSolver),
 theSOE(nullptr), autovalores(1), autovectores(1,Vector()) {}

//! @brief Constructor.
XC::BandArpackppSolver::BandArpackppSolver(const int &nModes)
 :EigenSolver(EigenSOLVER_TAGS_BandArpackppSolver,nModes),
 theSOE(nullptr), autovalores(nModes), autovectores(nModes,Vector()) {}

//! @brief Resuelve para todos los autovalores del problema.
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
    if(autovalores.size()!=nmodos)
      autovalores.resize(nmodos);
    if(autovectores.size()!=nmodos)
      autovectores.resize(nmodos);
    for(size_t i=0;i<nmodos;i++)
      autovectores[i].resize(n);
  }

ARluNonSymGenEig<double> getEigenProblem(const int &nmodes,ARbdNonSymMatrix<double, double> &ak,ARbdNonSymMatrix<double, double> &am,const double &shift)
  {
    if(shift<0.0)
      return ARluNonSymGenEig<double>(nmodes, ak, am); //Los autovalores más grandes.
    else
      return ARluNonSymGenEig<double>(nmodes, ak, am,shift); //Los autovalores más proximos a shift.
  }


int XC::BandArpackppSolver::solve(int nModes)
  {
    int retval= 0;
    if(!theSOE)
      {
        std::cerr << "BandArpackppSolver::solve(); aún no se ha asignado el problema (EigenSOE).\n";
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
	    std::cerr << "BandArpackppSolver::solve(); el número de modos ha de"
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
                  autovalores[i]= dprob.EigenvalueReal(i);

                for(size_t i=0; i<nconv; i++)
                  for(int j=0; j<n;j++)
                    autovectores[i](j)= dprob.RawEigenvector(i)[j];
               }
            else
              retval= -3;
          }
      }
    return retval;
  }

//! @brief Asigna el problema de autovalores a resolver.
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
      std::cerr << nombre_clase() << "::setEigenSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

//! @brief Asigna el problema de autovalores a resolver.
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
      retval= autovectores[mode-1];
    return retval;
  }

const double &XC::BandArpackppSolver::getEigenvalue(int mode) const
  {
    static double retval= 0.0;
    if(mode < 1 || mode > numModes)
        std::cerr << "BandArpackppSolver::getEigenvalue() -- mode " 
                  << mode << " is out of range (1 - " << numModes << ")\n";
    else
      retval= autovalores[mode-1];
    return retval;
  }

int XC::BandArpackppSolver::setSize(void)
  {
    setup_autos(numModes,theSOE->size);
    return 0;
  }

//! @brief Devuelve la dimensión de los autovectores.
const int &XC::BandArpackppSolver::getSize(void) const
  { return theSOE->size; }

int XC::BandArpackppSolver::sendSelf(CommParameters &cp)
  { return 0; }

int XC::BandArpackppSolver::recvSelf(const CommParameters &cp)
  { return 0; }
