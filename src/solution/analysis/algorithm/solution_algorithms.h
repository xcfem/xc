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
//solution_algorithms.h

//Archivos de cabecera de los algoritmos de solución.
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
#include <solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h>
//#include "solution/analysis/algorithm/equiSolnAlgo/AcceleratedNewton.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/BisectionLineSearch.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/SecantLineSearch.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/InitialInterpolatedLineSearch.h"
#include "solution/analysis/algorithm/equiSolnAlgo/lineSearch/RegulaFalsiLineSearch.h"
