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
//systems_of_equations.h

//Archivos de cabecera de los sistemas de ecuaciones.
//Sistemas de ecuaciones
#include <solution/system_of_eqn/SystemOfEqn.h>

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
#include "solution/system_of_eqn/linearSOE/DomainSolver.h"
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h>
#include "solution/system_of_eqn/linearSOE/bandGEN/BandGenLinLapackSolver.h"
#include <solution/system_of_eqn/linearSOE/bandGEN/DistributedBandGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.h>
#include "solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinLapackSolver.h"
//#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinThreadSolver.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSOE.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSOE.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSolver.h>
#include "solution/system_of_eqn/linearSOE/fullGEN/FullGenLinLapackSolver.h"
//#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSOE.h>
//#include <solution/system_of_eqn/linearSOE/itpack/ItpackLinSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectBlockSolver.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h>
//#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectThreadSolver.h>
//#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSkypackSolver.h>
#include "solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinDirectSolver.h"
#include "solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSubstrSolver.h"
#include <solution/system_of_eqn/linearSOE/profileSPD/DistributedProfileSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenColLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenRowLinSOE.h>
#include <solution/system_of_eqn/linearSOE/cg/ConjugateGradientSolver.h>

#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackSOE.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSOE.h>
#include <solution/system_of_eqn/eigenSOE/SymArpackSOE.h>
#include <solution/system_of_eqn/eigenSOE/SymBandEigenSOE.h>
#include <solution/system_of_eqn/eigenSOE/FullGenEigenSOE.h>

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include <solution/system_of_eqn/eigenSOE/SymArpackSolver.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackSolver.h>
#include <solution/system_of_eqn/eigenSOE/FullGenEigenSolver.h>
#include <solution/system_of_eqn/eigenSOE/SymBandEigenSolver.h>





#ifdef _THREADS
#include <solution/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h>
#else
#include <solution/system_of_eqn/linearSOE/sparseGEN/SuperLU.h>
#endif
#ifdef _PETSC
#include "solution/system_of_eqn/linearSOE/petsc/PetscSOE.h"
#include "solution/system_of_eqn/linearSOE/petsc/PetscSolver.h"
#include "solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h"
#include "solution/system_of_eqn/linearSOE/petsc/PetscSparseSeqSolver.h"
#endif

#ifdef _MUMPS
#include <MumpsSOE.h>
#ifdef _PARALLEL_PROCESSING
#include <MumpsParallelSOE.h>
#endif
#endif
#include <solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseSYM/SymSparseLinSolver.h>

//#include <solution/system_of_eqn/linearSOE/umfGEN/UmfpackGenLinSOE.h>
#ifdef _PARALLEL_PROCESSING
#include "solution/system_of_eqn/linearSOE/bandSPD/DistributedBandSPDLinSOE.h"
#include "solution/system_of_eqn/linearSOE/profileSPD/DistributedProfileSPDLinSOE.h"
#include "solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenColLinSOE.h"
#include "solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSOE.h"
#include "solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenRowLinSolver.h"
#include "solution/system_of_eqn/linearSOE/bandGEN/DistributedBandGenLinSOE.h"
#include "solution/system_of_eqn/linearSOE/sparseGEN/DistributedSuperLU.h"
#endif

