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
//solution_algorithms.h

//Headers for the solution algorithms.
#include "solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h"
#include <solution/analysis/algorithm/equiSolnAlgo/BFGS.h>
#include <solution/analysis/algorithm/equiSolnAlgo/Broyden.h>
#include <solution/analysis/algorithm/equiSolnAlgo/KrylovNewton.h>
#include <solution/analysis/algorithm/equiSolnAlgo/Linear.h>
#include <solution/analysis/algorithm/equiSolnAlgo/ModifiedNewton.h>
#include <solution/analysis/algorithm/equiSolnAlgo/NewtonLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/NewtonRaphson.h>
#include <solution/analysis/algorithm/equiSolnAlgo/PeriodicNewton.h>
#include <solution/analysis/algorithm/eigenAlgo/EigenAlgorithm.h>
#include <solution/analysis/algorithm/eigenAlgo/FrequencyAlgo.h>
#include <solution/analysis/algorithm/eigenAlgo/StandardEigenAlgo.h>
#include <solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo.h>
#include <solution/analysis/algorithm/eigenAlgo/KEigenAlgo.h>
#include <solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h>
//#include "solution/analysis/algorithm/equiSolnAlgo/AcceleratedNewton.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/BisectionLineSearch.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/SecantLineSearch.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/InitialInterpolatedLineSearch.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/RegulaFalsiLineSearch.h"
