// -*-c++-*-
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
                                                                        
#ifndef KEigenAlgo_h
#define KEigenAlgo_h

#include <solution/analysis/algorithm/eigenAlgo/EigenAlgorithm.h>

namespace XC {
class KEigenIntegrator;

//! @ingroup EigenAlgo
//
//! @brief Algorithm for ill-conditioning analysis.
class KEigenAlgo: public EigenAlgorithm
  {
  protected:
    int ns; //!< number of smallest eigenpairs
    int nl; //!< number of largest eigenpairs
    double condNumberThreshold; //!< condition number threshold for
                               //triggering analysis;
    mutable double rcond; //!< computed reciprocal condition number.
    mutable std::deque<Vector> eigenvectors;
    mutable std::deque<double> eigenvalues;

    KEigenIntegrator *getKEigenIntegrator(void);
    virtual void eigen_to_model(void);

    friend class AnalysisAggregation;
    KEigenAlgo(AnalysisAggregation *);
    virtual SolutionAlgorithm *getCopy(void) const;
    int form_matrices(void);
    int dump_modes(void);
    int compute_eigenvalues(int numEigen, const std::string &);
    int compute_smallest_eigenvalues(void);
    int compute_largest_eigenvalues(void);
  public:
    virtual int solveCurrentStep(int numModes);
    inline int getNs(void) const
      { return ns; }
    inline void setNs(int n)
      { ns= n; }
    inline int getNl(void) const
      { return nl; }
    inline void setNl(int n)
      { nl= n; }
    inline double getConditionNumberThreshold(void) const
      { return condNumberThreshold; }
    inline void setConditionNumberThreshold(double d)
      { condNumberThreshold= d; }
    inline double getRCond(void) const
      { return rcond; }
    inline void setRCond(double d)
      { rcond= d; }
    virtual void Print(std::ostream &s, int flag = 0) const;
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };
inline SolutionAlgorithm *KEigenAlgo::getCopy(void) const
  { return new KEigenAlgo(*this); }
} // end of XC namespace

#endif


