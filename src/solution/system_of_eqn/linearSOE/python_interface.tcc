//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//python_interface.tcc

class_<XC::LinearSOE, bases<XC::SystemOfEqn>, boost::noncopyable >("LinearSOE", no_init)
  .def("newSolver", &XC::LinearSOE::newSolver,return_internal_reference<>(),"Solver to use.")
  ;

class_<XC::LinearSOEData, bases<XC::LinearSOE>, boost::noncopyable >("LinearSOEData", no_init);

class_<XC::FactoredSOEBase, bases<XC::LinearSOEData>, boost::noncopyable >("FactoredSOEBase", no_init);

class_<XC::BandGenLinSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("BandGenLinSOE", no_init)
    ;


class_<XC::BandSPDLinSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("BandSPDLinSOE", no_init)
    ;


class_<XC::DiagonalSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("DiagonalSOE", no_init)
    ;



class_<XC::FullGenLinSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("FullGenLinSOE", no_init)
    ;

#ifdef _PETSC
class_<XC::PetscSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("PetscSOE", no_init)
    ;
class_<XC::PetscSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("PetscSolver", no_init);
class_<XC::PetscSparseSeqSolver, bases<XC::SparseGenRowLinSolver>, boost::noncopyable >("PetscSparseSeqSolver", no_init);
#endif

class_<XC::ProfileSPDLinSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("ProfileSPDLinSOE", no_init)
    ;

class_<XC::SparseSOEBase, bases<XC::FactoredSOEBase>, boost::noncopyable >("SparseSOEBase", no_init)
    ;

class_<XC::SparseGenSOEBase, bases<XC::SparseSOEBase>, boost::noncopyable >("SparseGenSOEBase", no_init)
    ;


class_<XC::SparseGenColLinSOE, bases<XC::SparseGenSOEBase>, boost::noncopyable >("SparseGenColLinSOE", no_init)
    ;

#ifdef _PARALLEL_PROCESSING
class_<XC::DistributedBandGenLinSOE, bases<XC::BandGenLinSOE>, boost::noncopyable >("DistributedBandGenLinSOE", no_init)
    ;

class_<XC::DistributedBandPDLinSOE, bases<XC::BandSPDLinSOE>, boost::noncopyable >("DistributedBandSPDLinSOE", no_init)
    ;

class_<XC::DistributedProfileSPDLinSOE, bases<XC::ProfileSPDLinSOE>, boost::noncopyable >("DistributedProfileSPDLinSOE", no_init)
    ;
class_<XC::DistributedDiagonalSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("DistributedDiagonalSOE", no_init)
    ;

class_<XC::DistributedSparseGenColLinSOE, bases<XC::SparseGenColLinSOE>, boost::noncopyable >("DistributedSparseGenColLinSOE", no_init)
    ;

class_<XC::DistributedSparseGenRowLinSOE, bases<XC::SparseGenSOEBase>, boost::noncopyable >("DistributedSparseGenRowLinSOE", no_init)
    ;

class_<XC::DistributedDiagonalSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("DistributedDiagonalSolver", no_init);

class_<XC::DistributedSparseGerRowLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("DistributedSparseGerRowLinSolver", no_init);

class_<XC::DistributedSuperLU, bases<XC::SparseGenColLinSolver>, boost::noncopyable >("DistributedSuperLU", no_init);

#endif

class_<XC::SparseGenRowLinSOE, bases<XC::SparseGenSOEBase>, boost::noncopyable >("SparseGenRowLinSOE", no_init)
    ;


class_<XC::SymSparseLinSOE, bases<XC::SparseSOEBase>, boost::noncopyable >("SymSparseLinSOE", no_init)
    ;

// class_<XC::UmfpackGenLinSOE, bases<XC::FactoredSOEBase>, boost::noncopyable >("UmfpackGenLinSOE", no_init)
//     ;

class_<XC::LinearSOESolver, bases<XC::Solver>, boost::noncopyable >("LinearSOESolver", no_init);

class_<XC::BandGenLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("BandGenLinSolver", no_init);

class_<XC::BandGenLinLapackSolver, bases<XC::BandGenLinSolver>, boost::noncopyable >("BandGenLinLapackSolver", no_init);

class_<XC::BandSPDLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("BandSPDLinSolver", no_init);

class_<XC::BandSPDLinLapackSolver, bases<XC::BandSPDLinSolver>, boost::noncopyable >("BandSPDLinLapackSolver", no_init);

// class_<XC::BandSPDLinThreadSolver, bases<XC::BandSPDLinSolver>, boost::noncopyable >("BandSPDLinThreadSolver", no_init);

class_<XC::ConjugateGradientSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("ConjugateGradientSolver", no_init);

class_<XC::DiagonalSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("DiagonalSolver", no_init);

class_<XC::DiagonalDirectSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("DiagonalDirectSolver", no_init);

class_<XC::DomainSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("DomainSolver", no_init);

class_<XC::FullGenLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("FullGenLinSolver", no_init);

class_<XC::FullGenLinLapackSolver, bases<XC::FullGenLinSolver>, boost::noncopyable >("FullGenLinLapackSolver", no_init);

// class_<XC::ItPackLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("ItPackLinSolver", no_init);

class_<XC::ProfileSPDLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("ProfileSPDLinSolver", no_init);

class_<XC::ProfileSPDLinDirectBase, bases<XC::ProfileSPDLinSolver>, boost::noncopyable >("ProfileSPDLinDirectBase", no_init);

class_<XC::ProfileSPDLinDirectBlockSolver, bases<XC::ProfileSPDLinDirectBase>, boost::noncopyable >("ProfileSPDLinDirectBlockSolver", no_init);

class_<XC::ProfileSPDLinDirectSolver, bases<XC::ProfileSPDLinDirectBase>, boost::noncopyable >("ProfileSPDLinDirectSolver", no_init);

// class_<XC::ProfileSPDLinDirectThreadSolver, bases<XC::ProfileSPDLinDirectBase>, boost::noncopyable >("ProfileSPDLinDirectThreadSolver", no_init);

class_<XC::ProfileSPDLinSubstrSolver, bases<XC::ProfileSPDLinDirectBase,XC::DomainSolver>, boost::noncopyable >("ProfileSPDLinSubstrSolver", no_init);

// class_<XC::ProfileSPDLinDirectSkypackSolver, bases<XC::ProfileSPDLinSolver>, boost::noncopyable >("ProfileSPDLinDirectSkypackSolver", no_init);

class_<XC::SparseGenColLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("SparseGenColLinSolver", no_init);

class_<XC::SuperLU, bases<XC::SparseGenColLinSolver>, boost::noncopyable >("SuperLU", no_init);

// class_<XC::ThreadSuperLU, bases<XC::SparseGenColLinSolver>, boost::noncopyable >("ThreadSuperLU", no_init);

class_<XC::SparseGenRowLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("SparseGenRowLinSolver", no_init);

class_<XC::SymSparseLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("SymSparseLinSolver", no_init);

// class_<XC::UmfpackGenLinSolver, bases<XC::LinearSOESolver>, boost::noncopyable >("UmfpackGenLinSolver", no_init);



