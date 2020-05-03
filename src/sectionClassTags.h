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
//sectionClassTags.h

#ifndef sectionClassTags_h
#define sectionClassTags_h

#define SEC_TAG_Elastic1d                       1000
#define SEC_TAG_Elastic2d                       1001
#define SEC_TAG_Elastic3d                       1002
#define SEC_TAG_Generic1d	                1003
#define SEC_TAG_GenericNd                       1004   
#define SEC_TAG_Aggregator                      1005
#define SEC_TAG_Fiber                           1006
#define SEC_TAG_FiberSection2d                  1007
#define SEC_TAG_FiberSection3d		        1008
#define SEC_TAG_FiberSectionGJ		        1009
#define SEC_TAG_BeamFiberSection	        1010
#define SEC_TAG_ElasticPlateSection	        1011
#define SEC_TAG_ElasticMembranePlateSection	1012
#define SEC_TAG_MembranePlateFiberSection	1013 
#define SEC_TAG_Bidirectional	                1014
#define SEC_TAG_WSection2d	                1015
#define SEC_TAG_Isolator2spring                 1016
#define SEC_TAG_SoilFooting2d                   1017
#define SEC_TAG_YieldSurface2d                  1018
#define SEC_TAG_YieldSurface2D02                1019
#define SEC_TAG_YieldSurface2D01                1020
#define SEC_TAG_ElasticShear2d                  1021
#define SEC_TAG_ElasticShear3d                  1022
#define SEC_TAG_FiberSection2dInt               1023
#define SEC_TAG_FiberSectionShear3d	        1024

#endif
