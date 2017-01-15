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
** file 'COPYRIGHT'  in main directory for information on usage and   **
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
