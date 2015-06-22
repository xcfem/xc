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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                    
// $Revision: 1.7 $
// $Date: 2005/03/21 21:35:56 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/randomNumber/CStdLibRandGenerator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/analysis/randomNumber/RandomNumberGenerator.h>
#include <reliability/analysis/randomNumber/CStdLibRandGenerator.h>
#include <reliability/domain/distributions/NormalRV.h>
#include <utility/matrix/Vector.h>


XC::CStdLibRandGenerator::CStdLibRandGenerator()
  :RandomNumberGenerator(), generatedNumbers(nullptr) {}

XC::CStdLibRandGenerator::~CStdLibRandGenerator()
  {
    if(generatedNumbers)
      delete generatedNumbers;
  }





int XC::CStdLibRandGenerator::generate_nIndependentUniformNumbers(int n, double lower, double upper, int seedIn)
  {
    // Initial declarations
    int randomNumberBetween0AndRAND_MAX= 0;
    double randomNumberBetween0And1;
    Vector randomArray(n);


    // Create array of standard normal random numbers
    if(seedIn!=0)
      { srand(seedIn); }
    for(int j=0; j<n; j++)
      {
        // Generate a number between 0 and RAND_MAX
        randomNumberBetween0AndRAND_MAX = rand();

        // Modify it so that the value lies between 0 and 1
        randomNumberBetween0And1 = (double)randomNumberBetween0AndRAND_MAX/RAND_MAX;

        // Transform according to uniform distribution
        randomArray(j) = (upper-lower)*randomNumberBetween0And1 + lower;
      }

    seed= randomNumberBetween0AndRAND_MAX;
      
    if(generatedNumbers == 0)
      { generatedNumbers = new Vector(n); }
    else if(generatedNumbers->Size()!=n)
      {
        delete generatedNumbers;
        generatedNumbers = new XC::Vector(n);
      }
    (*generatedNumbers) = randomArray;
    return 0;
  }




int XC::CStdLibRandGenerator::generate_nIndependentStdNormalNumbers(int n, int seedIn)
{
    // Initial declarations
    int j;
    int randomNumberBetween0AndRAND_MAX= 0;
    double randomNumberBetween0And1;
    Vector randomArray(n);
    NormalRV *aStdNormRV = 0;
    aStdNormRV = new XC::NormalRV(1,0.0,1.0,0.0);


    // Check if computer ran out of memory
    if (aStdNormRV==0) {
            std::cerr << "XC::CStdLibRandGenerator::generate_nIndependentStdNormalNumbers() - " << std::endl
                    << " out of memory while instantiating internal objects." << std::endl;
            return -1;
    }


    // Create array of standard normal random numbers
    if (seedIn != 0) {
            srand(seedIn);
    }
    for ( j=0; j<n; j++)
    {
            // Generate a number between 0 and RAND_MAX
            randomNumberBetween0AndRAND_MAX = rand();

            // Modify it so that the value lies between 0 and 1
            randomNumberBetween0And1 = (double)randomNumberBetween0AndRAND_MAX/RAND_MAX;


            // Treat two special cases
            if (randomNumberBetween0And1 == 0.0) {
                    randomNumberBetween0And1 = 0.0000001;
            }
            if (randomNumberBetween0And1 == 1.0) {
                    randomNumberBetween0And1 = 0.9999999;
            }


            // Transform that number into a standard normal variable
            //    Phi(z) = F(x)
            //    z = invPhi( F(x) )
            //       where F(x) for the uniform distribution 
            //       from 0 to 1 in fact is equal to x itself.
            randomArray(j) = aStdNormRV->getInverseCDFvalue(randomNumberBetween0And1); 
    }
    seed = randomNumberBetween0AndRAND_MAX;

    if (generatedNumbers == 0) {
            generatedNumbers = new XC::Vector(n);
    }
    else if (generatedNumbers->Size() != n) {
            delete generatedNumbers;
            generatedNumbers = new XC::Vector(n);
    }
    (*generatedNumbers) = randomArray;

    delete aStdNormRV;

    return 0;
}



XC::Vector XC::CStdLibRandGenerator::getGeneratedNumbers()
  { return (*generatedNumbers); }


int XC::CStdLibRandGenerator::getSeed()
  { return seed; }
