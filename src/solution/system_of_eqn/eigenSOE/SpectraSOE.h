// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//SpectraSOE.h

#ifndef SpectraSOE_h
#define SpectraSOE_h

#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <Eigen/Core>
#include <Eigen/SparseCore>


namespace XC {
class SpectraSolver;

//! @ingroup EigenSOE
//
//! @brief <a href="http://www.caam.rice.edu/software/ARPACK/" target="_new"> Arpack++</a> based band matrix eigenvalue SOE solver.
class SpectraSOE: public EigenSOE
  {
  private:
    typedef Eigen::Triplet<double> T;
    std::deque<T> tripletListA;
    std::deque<T> tripletListM;
    
    Eigen::SparseMatrix<double> A;
    Eigen::SparseMatrix<double> M;
    
    int addToMatrix(std::deque<T> &,const Matrix &, const ID &,const double &);
    void store_mass_matrix(void);
  protected:
    bool setSolver(EigenSolver *);
    void assembleMatrices(void);

    friend class SolutionStrategy;
    friend class FEM_ObjectBroker;
    SpectraSOE(SolutionStrategy *);
    SystemOfEqn *getCopy(void) const;
  public:
    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);    
   
    virtual void zeroA(void);
    virtual void zeroM(void);
    virtual void identityM(void);

    inline const Eigen::SparseMatrix<double> &getA(void) const
      { return A; }
    inline const Eigen::SparseMatrix<double> &getM(void) const
      { return M; }
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);    

    friend class SpectraSolver;
  };
inline SystemOfEqn *SpectraSOE::getCopy(void) const
  { return new SpectraSOE(*this); }
} // end of XC namespace

#endif
