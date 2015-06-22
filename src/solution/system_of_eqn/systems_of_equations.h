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

