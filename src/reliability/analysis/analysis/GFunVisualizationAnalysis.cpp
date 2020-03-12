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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/10/27 23:45:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/GFunVisualizationAnalysis.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include "GFunVisualizationAnalysis.h"
#include <reliability/analysis/gFunction/GFunEvaluator.h>
#include <reliability/domain/components/ReliabilityDomain.h>
#include <reliability/analysis/analysis/ReliabilityAnalysis.h>
#include <reliability/analysis/sensitivity/GradGEvaluator.h>
#include <reliability/analysis/meritFunction/MeritFunctionCheck.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <reliability/analysis/misc/MatrixOperations.h>
#include <reliability/domain/distributions/NormalRV.h>
#include <reliability/domain/components/RandomVariable.h>
#include <cmath>
//#include <fstream>
#include <iomanip>
using std::ios;
#include <reliability/analysis/transformation/ProbabilityTransformation.h>
#include <reliability/analysis/convergenceCheck/ReliabilityConvergenceCheck.h>
#include <reliability/analysis/rootFinding/RootFinding.h>

//! @brief Constructor.
XC::GFunVisualizationAnalysis::GFunVisualizationAnalysis(ReliabilityDomain *passedReliabilityDomain,
                                        GFunEvaluator *passedGFunEvaluator,
                                        ProbabilityTransformation *passedProbabilityTransformation,
                                        const std::string &passedOutputFileName,
                                        const std::string &passedConvFileName,
                                        int passedConvResults,
                                        int passedSpace,
                                        int passedFunSurf,
                                        int passedAxes,
                                        int passedDir)
:ReliabilityAnalysis()
{
        theReliabilityDomain = passedReliabilityDomain;
        theGFunEvaluator = passedGFunEvaluator;
        theProbabilityTransformation = passedProbabilityTransformation;
        theMeritFunctionCheck = 0;
        theGradGEvaluator = 0;
        theReliabilityConvergenceCheck = 0;
        theStartPoint = 0;

        outputFileName= passedOutputFileName;

        convFileName= passedConvFileName;

        convResults = passedConvResults;
        space = passedSpace;
        funSurf = passedFunSurf;
        axes = passedAxes;
        dir = passedDir;

        nrv = theReliabilityDomain->getNumberOfRandomVariables();

        scaleValue = 1.0;

        if (convResults==1) {
          static std::ofstream convFile(convFileName.c_str(), ios::out );
        }


}

//! @brief Lanza the analysis.
int XC::GFunVisualizationAnalysis::analyze(void)
  {
        // Meaning of the tags
        // convResults [ 0:no,                   1:yes                 ]
        // space       [ 0:error,                1:X,        2:Y       ]
        // funSurf     [ 0:error,                1:function, 2:surface ]
        // dir         [ 0:(needed for surface), 1:rv,       2:file    ] (pass rvDir or theDirectionVector)
        // axes        [ 0:error,   1:coords1,   2:coords2,  3:file    ] (pass axesVector... or theMatrix+numPts)


        // Alert the user that the visualization analysis has started
        std::cerr << "Visualization Analysis is running ... " << std::endl;;


        // Open output file
        std::ofstream outputFile( outputFileName.c_str(), ios::out );

        
        // Initial declarations
        int i,j;
        Vector thePoint;
        double result= 0;


        // Determine number of points to be plotted
        int numPoints1= 0, numPoints2= 0;
        if(axes==1)
          {
            numPoints1 = numPts1;
            numPoints2 = 1;
          }
        else if(axes==2)
          {
            numPoints1 = numPts1;
            numPoints2 = numPts2;
          }
        else if(axes==3)
          {
            // Should have a warning here if theMatrix isn't set
            numPoints1 = theMatrix.noCols()-1;
            numPoints2 = numLinePts;
          }

        int counter = 0; 
        for(i=1; i<=numPoints1; i++) {
                for (j=1; j<=numPoints2; j++) {

                        counter++;
                        std::cerr << counter << " ";

                        // Get the current point in relevant space
                        if (axes==1 || axes==2) {
                                thePoint = this->getCurrentAxes12Point(i,j);
                        }
                        else if (axes==3) {
                                thePoint = this->getCurrentAxes3Point(i,j);
                        }

                        // Evaluate G or find the surface
                        if (funSurf==1) {

                                // Transform the point into x-space (1-space) if the user has specified in 2-space
                                if (space==2) {
                                        result = theProbabilityTransformation->set_u(thePoint);
                                        if (result < 0) {
                                                std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                                                        << " could not set u in the xu-transformation." << std::endl;
                                                return -1;
                                        }

                                        result = theProbabilityTransformation->transform_u_to_x();
                                        if (result < 0) {
                                                std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                                                        << " could not transform from u to x and compute Jacobian." << std::endl;
                                                return -1;
                                        }
                                        thePoint = theProbabilityTransformation->get_x();
                                }

                                // Evaluate g
                                if (j==1) {
                                        result = this->evaluateGFunction(thePoint,true);
                                }
                                else {
                                        result = this->evaluateGFunction(thePoint,false);
                                }
                        }
                        else if (funSurf==2) {

                                // Find surface in relevant space
                                result = this->findGSurface(thePoint);
                        }

                        // Print the result (g or distance) to file
                        outputFile << result << " ";
                } 
                outputFile << std::endl;
        }

        // Clean up
        outputFile.close();

        // Print summary of results to screen (more here!!!)
        std::cerr << std::endl << "GFunVisualizationAnalysis completed." << std::endl;

        return 0;
  }


XC::Vector XC::GFunVisualizationAnalysis::getCurrentAxes12Point(int i, int j)
  {
    // Initial declarations
    Vector thePoint;
    int result;

    // Find the start point in the space which the user 
    // wants to visualize in. 
    if(theStartPoint == 0)
      {
                
        // This indicates the origin in the standard normal space
        Vector dummy(nrv);
        dummy.Zero();
        thePoint = dummy;


        // If the user wants to visualize in the x-space; transform it into the x-space
        if(space==1)
          {
            result = theProbabilityTransformation->set_u(thePoint);
            if(result < 0)
              {
                 std::cerr << "GFunVisualizationAnalysis::analyze() - " << std::endl
                           << " could not set u in the xu-transformation." << std::endl;
                 //return -1; Comentado LCPT.
              }
            result = theProbabilityTransformation->transform_u_to_x();
            if(result < 0)
              {
                std::cerr << "GFunVisualizationAnalysis::analyze() - " << std::endl
                          << " could not transform from u to x and compute Jacobian." << std::endl;
                //return -1; Comentado LCPT.
              }
            thePoint = theProbabilityTransformation->get_x();
          }
      }
    else
      {
        // Here the start point is actually given in the original space
        thePoint = (*theStartPoint);

        // Transform it into the u-space if that's where the user wants to be
        if(space==2)
          {
            result = theProbabilityTransformation->set_x(thePoint);
            if(result < 0)
              {
                std::cerr << "SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
                          << " could not set x in the xu-transformation." << std::endl;
                //return -1; Comentado LCPT.
              }
            result = theProbabilityTransformation->transform_x_to_u();
            if(result < 0)
              {
                std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
                          << " could not transform from x to u." << std::endl;
                //return -1; Comentado LCPT.
              }
            thePoint = theProbabilityTransformation->get_u();
          }
      }
    // Now we have the point in the space we want it, 
    // So, set the random variables to be 'ranged'
    thePoint(rv1-1) = from1+(i-1)*interval1; 
    if(axes==2)
      { thePoint(rv2-1) = from2+(j-1)*interval2; }
    return thePoint;
  }


XC::Vector XC::GFunVisualizationAnalysis::getCurrentAxes3Point(int i, int j)
  {
        // j is the index that runs along the line
        // i is to say which line we are on


        // Initial declarations
        Vector vector1(nrv);
        Vector vector2(nrv);


        // Extract end vectors
        for (int k=0; k<nrv; k++) {
                vector1(k) = theMatrix(k,i-1);
                vector2(k) = theMatrix(k,i);
        }


        // Compute vector between the points
        Vector vector = vector2-vector1;


        // The point to be returned
        Vector thePoint;
        double vectorFactor = ((double)(j-1))/((double)(numLinePts-1));
        thePoint = ( vector1 + vector*vectorFactor );


        return thePoint;
}


int XC::GFunVisualizationAnalysis::setDirection(int prvDir)
  {
    rvDir = prvDir;
    return 0;
  }

int XC::GFunVisualizationAnalysis::setDirection(Vector pDirectionVector)
  {
    theDirectionVector = pDirectionVector;
    return 0;
  }
        
int XC::GFunVisualizationAnalysis::setAxes(Vector axesVector)
  {
    // Deschipher the content of the vector
    rv1         = (int)axesVector(0);
    from1       =      axesVector(1);
    double to1  =      axesVector(2);
    numPts1     = (int)axesVector(3);
    double to2= 0;
    if(axesVector.Size() > 4 )
      {
        rv2         = (int)axesVector(4);
        from2       =      axesVector(5);
        to2  =      axesVector(6);
        numPts2     = (int)axesVector(7);
      }
    interval1   = (to1-from1)/(numPts1-1);
    interval2   = (to2-from2)/(numPts2-1);
    return 0;
  }

int XC::GFunVisualizationAnalysis::setAxes(Matrix pMatrix)
  {
    theMatrix = pMatrix;
    return 0;
  }

int XC::GFunVisualizationAnalysis::setNumLinePts(int pNumLinePts)
  {
    numLinePts = pNumLinePts;
    return 0;
  }

int XC::GFunVisualizationAnalysis::setRootFindingAlgorithm(RootFinding *pRootFinder)
  {
    theRootFindingAlgorithm = pRootFinder;
    return 0;
  }


int XC::GFunVisualizationAnalysis::setStartPoint(Vector *pStartPoint)
  {
    theStartPoint = pStartPoint;
    return 0;
  }

int XC::GFunVisualizationAnalysis::setGradGEvaluator(GradGEvaluator *pGradGEvaluator)
  {
    theGradGEvaluator = pGradGEvaluator;
    return 0;
  }

int XC::GFunVisualizationAnalysis::setMeritFunctionCheck(MeritFunctionCheck *pMeritFunctionCheck)
  {
    theMeritFunctionCheck = pMeritFunctionCheck;
    return 0;
  }

int XC::GFunVisualizationAnalysis::setReliabilityConvergenceCheck(ReliabilityConvergenceCheck *pReliabilityConvergenceCheck)
  {
    theReliabilityConvergenceCheck = pReliabilityConvergenceCheck;
    return 0;
  }



double XC::GFunVisualizationAnalysis::findGSurface(Vector thePoint)
  {
    // Initial declarations
    int result;
    double g;
    Vector surfacePoint(nrv);
    Vector Direction(nrv);
        Vector theTempPoint(nrv);
        int i;
        Vector distance;
        double scalarDist;
        double a;


        // Find direction; in whichever space user wants
        Direction.Zero();
        if (dir==1) {
                Direction(rvDir-1) = 1.0;
        }
        else if (dir==2) {
                if (nrv != theDirectionVector.Size()) {
                        std::cerr << "ERROR: There is something wrong with the size of the direction" << std::endl
                                << " vector of the visualization analysis object." << std::endl;
                }
                Direction = theDirectionVector;
        }

        // Transform the point into x-space if the user has given it in 2-space
        if (space==2) {
                result = theProbabilityTransformation->set_u(thePoint);
                if (result < 0) {
                        std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                                << " could not set u in the xu-transformation." << std::endl;
                        return -1;
                }

                result = theProbabilityTransformation->transform_u_to_x();
                if (result < 0) {
                        std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                                << " could not transform from u to x and compute Jacobian." << std::endl;
                        return -1;
                }
                theTempPoint = theProbabilityTransformation->get_x();
        }
        else {
                theTempPoint = thePoint;
        }


        // Evaluate limit-state function
        result = theGFunEvaluator->runGFunAnalysis(theTempPoint);
        if (result < 0) {
                std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                        << " could not run analysis to evaluate limit-state function. " << std::endl;
                return -1;
        }
        result = theGFunEvaluator->evaluateG(theTempPoint);
        if (result < 0) {
                std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                        << " could not tokenize limit-state function. " << std::endl;
                return -1;
        }
        g = theGFunEvaluator->getG();


        // FIND THE POINT IN WHICHEVER SPACE USER HAS SPECIFIED
        surfacePoint = theRootFindingAlgorithm->findLimitStateSurface(space,g,Direction,thePoint);


        // POST-PROCESSING: FIND THE DISTANCE IN THE RELEVANT SPACE

        // Determine scaling factor 'a' in: surfacePoint = thePoint + NewtonDirection*a;
        i=0;
        while (Direction(i)==0.0) {
                i++;
        }
        a = ( surfacePoint(i) - thePoint(i) ) / Direction(i);
        distance = surfacePoint-thePoint;

        // Then the distance is:
        scalarDist = (a/fabs(a)*distance.Norm());


        return scalarDist;
}

double XC::GFunVisualizationAnalysis::evaluateGFunction(Vector thePoint, bool isFirstPoint)
  {
        // Initial declarations
        double g;
        int result;


        // Evaluate limit-state function
        result = theGFunEvaluator->runGFunAnalysis(thePoint);
        if (result < 0) {
                std::cerr << "GFunVisualizationAnalysis::analyze() - " << std::endl
                        << " could not run analysis to evaluate limit-state function. " << std::endl;
                return -1;
        }
        result = theGFunEvaluator->evaluateG(thePoint);
        if (result < 0) {
                std::cerr << "XC::GFunVisualizationAnalysis::analyze() - " << std::endl
                        << " could not tokenize limit-state function. " << std::endl;
                return -1;
        }
        g = theGFunEvaluator->getG();



        // Possibly compute and print out more comprehensive results
        if (convResults==1) {

                // Open file for these results
          static std::ofstream convFile(convFileName.c_str(), ios::out );


                // Initial declarations
                char myString[300];
                Vector u;
                double meritFunctionValue;
                int numCrit = theReliabilityConvergenceCheck->getNumberOfCriteria();
                double criteriumValue;


                // Print a division if this is the first point on the vector between two points
                if (isFirstPoint) {
                        double dummy = 0.0;
                        sprintf(myString,"%20.14e  %20.14e  ", dummy,dummy);
                        convFile << myString;
                        sprintf(myString,"%20.14e  ", dummy);
                        convFile << myString;
                        for (int crit=1; crit<=numCrit; crit++) {
                                sprintf(myString,"%20.14e  ", dummy);
                                convFile << myString;
                        }
                        convFile << std::endl;        
                }

                // Set scale value of convergence criteria
                if (scaleValue == 1.0 && convResults == 1) {
                        scaleValue = g;
                        theReliabilityConvergenceCheck->setScaleValue(g);
                }


                // Transform the x-point into u-space
                result = theProbabilityTransformation->set_x(thePoint);
                if (result < 0) {
                        std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
                                << " could not set x in the xu-transformation." << std::endl;
                        return -1;
                }

                result = theProbabilityTransformation->transform_x_to_u();
                if (result < 0) {
                        std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
                                << " could not transform from x to u." << std::endl;
                        return -1;
                }
                u = theProbabilityTransformation->get_u();

        
                // And back to the x-space again, just to get the jacobian...
                result = theProbabilityTransformation->set_u(u);
                if (result < 0)
		  {
		    std::cerr << "GFunVisualizationAnalysis::" << __FUNCTION__
		              << " - could not set " << std::endl
                              << " vector u in the xu-transformation. "
			      << std::endl;
                        return -1;
                  }
                result = theProbabilityTransformation->transform_u_to_x_andComputeJacobian();
                if (result < 0)
		  {
                        std::cerr << "GFunVisualizationAnalysis::" << __FUNCTION__
				  << " - could not  " << std::endl
                                  << " transform u to x. " << std::endl;
                        return -1;
                  }
                theProbabilityTransformation->get_x();
                Matrix jacobian_x_u = theProbabilityTransformation->getJacobian_x_u();


                // Print limit-state fnc value and distance to origin to file
                sprintf(myString,"%20.14e  %20.14e  ", g,u.Norm());
                convFile << myString;

                // Compute gradients
                result = theGradGEvaluator->computeGradG(g,thePoint);
                if (result < 0) {
                        std::cerr << "XC::SearchWithStepSizeAndStepDirection::doTheActualSearch() - " << std::endl
                                << " could not compute gradients of the limit-state function. " << std::endl;
                        return -1;
                }
                Vector gradientOfgFunction = theGradGEvaluator->getGradG();
                gradientOfgFunction = jacobian_x_u ^ gradientOfgFunction;

                
                // Evaluate the merit function
                if (isFirstPoint) {
                        meritFunctionValue = theMeritFunctionCheck->updateMeritParameters(u, g, gradientOfgFunction);
                }
                meritFunctionValue = theMeritFunctionCheck->getMeritFunctionValue(u, g, gradientOfgFunction);
                sprintf(myString,"%20.14e  ", meritFunctionValue);
                convFile << myString;

                // Get value of convergence criteria
                theReliabilityConvergenceCheck->check(u,g,gradientOfgFunction);
                for (int crit=1; crit<=numCrit; crit++) {
                        criteriumValue = theReliabilityConvergenceCheck->getCriteriaValue(crit);
                        sprintf(myString,"%20.14e  ", criteriumValue);
                        convFile << myString;
                }

                convFile << std::endl;

        }


        return g;
}

