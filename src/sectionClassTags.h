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
//sectionClassTags.h

#ifndef sectionClassTags_h
#define sectionClassTags_h

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
