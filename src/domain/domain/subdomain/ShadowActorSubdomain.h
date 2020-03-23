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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
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
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/30 23:47:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/subdomain/ShadowActorSubdomain.h,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the integer codes used in ShadowSubdomain
// and the ActorSubdomain classes.
//
// What: "@(#) ShadowSubdomain.h, revA"
namespace XC {
//! @ingroup SubDom
//
static const int ShadowActorSubdomain_setTag = 72;
static const int ShadowActorSubdomain_newStep = 73;
static const int ShadowActorSubdomain_buildSubdomain = 70;
static const int ShadowActorSubdomain_getRemoteData = 71;
static const int ShadowActorSubdomain_addElement = 1;
static const int ShadowActorSubdomain_addNode = 2;
static const int ShadowActorSubdomain_addExternalNode =  3;
static const int ShadowActorSubdomain_addSFreedom_Constraint = 4;
static const int ShadowActorSubdomain_addMFreedom_Constraint = 5;
static const int ShadowActorSubdomain_addLoadPattern = 6;
static const int ShadowActorSubdomain_addNodalLoadToPattern  = 7;
static const int ShadowActorSubdomain_addElementalLoadToPattern = 8;
static const int ShadowActorSubdomain_addSFreedom_ConstraintToPattern  = 9;
static const int ShadowActorSubdomain_removeElement = 10;
static const int ShadowActorSubdomain_removeNode = 11;
static const int ShadowActorSubdomain_removeSFreedom_Constraint = 12;
static const int ShadowActorSubdomain_removeMFreedom_Constraint = 13;
static const int ShadowActorSubdomain_removeLoadPattern = 14;
static const int ShadowActorSubdomain_removeNodalLoadFromPattern = 15;
static const int ShadowActorSubdomain_removeElementalLoadFromPattern = 16;
static const int ShadowActorSubdomain_removeSFreedom_ConstraintFromPattern = 17;
static const int ShadowActorSubdomain_getElement = 20;
static const int ShadowActorSubdomain_getNode = 21;
static const int ShadowActorSubdomain_hasElement = 22;
static const int ShadowActorSubdomain_hasNode = 23;
static const int ShadowActorSubdomain_applyLoad = 32;
static const int ShadowActorSubdomain_setLoadConstant = 31;
static const int ShadowActorSubdomain_update = 33;
static const int ShadowActorSubdomain_updateTimeDt = 38;
static const int ShadowActorSubdomain_computeNodalResponse = 37;
static const int ShadowActorSubdomain_commit = 34;
static const int ShadowActorSubdomain_revertToLastCommit = 35;
static const int ShadowActorSubdomain_revertToStart = 36;
static const int ShadowActorSubdomain_setDomainDecompAnalysis = 40;
static const int ShadowActorSubdomain_domainChange = 41;
static const int ShadowActorSubdomain_clearAnalysis = 42;
static const int ShadowActorSubdomain_setAnalysisAlgorithm = 43;
static const int ShadowActorSubdomain_setAnalysisIntegrator = 44;
static const int ShadowActorSubdomain_setAnalysisLinearSOE = 45;
static const int ShadowActorSubdomain_setAnalysisConvergenceTest = 46;
static const int ShadowActorSubdomain_getTang = 53;
static const int ShadowActorSubdomain_getResistingForce = 54;
static const int ShadowActorSubdomain_computeTang = 55;
static const int ShadowActorSubdomain_computeResidual = 56;
static const int ShadowActorSubdomain_getCost = 60;
static const int ShadowActorSubdomain_setCommitTag = 25;
static const int ShadowActorSubdomain_setCurrentTime = 26;
static const int ShadowActorSubdomain_setCommittedTime = 27;
static const int ShadowActorSubdomain_DIE = 0;
static const int ShadowActorSubdomain_getElementPtr = 90;
static const int ShadowActorSubdomain_getNodePtr = 91;
static const int ShadowActorSubdomain_Print = 150;
static const int ShadowActorSubdomain_addRecorder = 151;
static const int ShadowActorSubdomain_removeRecorders = 152;
static const int ShadowActorSubdomain_getNodeDisp = 92;
static const int ShadowActorSubdomain_setMass = 93;
static const int ShadowActorSubdomain_setRayleighDampingFactors = 94;
static const int ShadowActorSubdomain_wipeAnalysis = 95;
static const int ShadowActorSubdomain_clearAll = 96;

} // end of XC namespace
