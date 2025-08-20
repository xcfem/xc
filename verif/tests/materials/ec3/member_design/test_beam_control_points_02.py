# -*- coding: utf-8 -*-
'''     Test that checks the method setControlPoints() of EC3Beam, that
        sets the five equally spaced points in the beam where the moment Mz 
        will be evaluated in order to obtain the moment gradient factor 
        involved in the calculation of the lateral-torsional buckling reduction 
        factor. That moment gradient factor will be calculated following   
        the general expression proposed by A. López, D. J. Yong, 
        M. A. Serna.
        An attribute of EC3Beam is created, named 'contrPnt' that contains 
        a list of five tuples (elem,relativDist), each of which contains the
        element of the beam nearest to one control-point and the relative 
        distance from this control point to the first node of the element.
'''

from __future__ import print_function
__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2018, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es"

import geom
import xc
from materials.ec3 import EC3_materials
from model.geometry import grid_model as gm
from model.mesh import finit_el_model as fem
from model import predefined_spaces
from materials.ec3 import EC3Beam as ec3b

FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor
prep=preprocessor   #short name
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #Defines the
# dimension of the space: nodes by three coordinates (x,y,z) and 
# six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

# coordinates in global X,Y,Z axes for the grid generation
xList=[0]
yList=[0,1,3,6,7]
zList=[0]
lastYpos=len(yList)-1

eSize= 0.2     #length of elements

# grid model definition
gridGeom= gm.GridModel(prep,xList,yList,zList)

# Grid geometric entities definition (points, lines, surfaces)
# Points' generation
gridGeom.generatePoints()

gridGeom.movePointsRange(gm.IJKRange((0,1,0),(0,1,0)),xc.Vector([0.0,0,1.0]))
gridGeom.movePointsRange(gm.IJKRange((0,2,0),(0,3,0)),xc.Vector([0.0,0,1.5]))

beamY_rg=gm.IJKRange((0,0,0),(0,lastYpos,0))
beamY=gridGeom.genLinOneRegion(ijkRange=beamY_rg,setName='beamY')
#                         *** MATERIALS *** 
S235JR= EC3_materials.S235JR
S235JR.gammaM= 1.00


# Steel material-section appropriate for 3D beam analysis, including shear
  # deformations.
  # Attributes:
  #   steel:         steel material (
  #   name: name of the standard steel profile. Types: IPEShape, HEShape,
  #         UPNShape, AUShape, CHSShape
  #      (defined in materials.sections.structural_shapes.arcelor_metric_shapes)
beamY_mat= EC3_materials.IPEShape(steel=S235JR,name='IPE_A_450')
beamY_mat.defElasticShearSection3d(preprocessor)

#                         ***FE model - MESH***

beamY_mesh= fem.LinSetToMesh(linSet=beamY,matSect=beamY_mat,elemSize=eSize,vDirLAxZ=xc.Vector([1,0,0]),elemType='ElasticBeam3d',coordTransfType='linear')
beamY_mesh.generateMesh(preprocessor)

# EC3beam definition
lstPntBeam= gridGeom.getLstPntRange(beamY_rg)

ec3beam= ec3b.EC3Beam(name='ec3bm', ec3Shape=None, lstPoints= lstPntBeam)

ec3beam.setControlPoints()

# calculated control points
e1S= ec3beam.contrPnt[0][0].getLineSegment(0)
v1CP= e1S.getFromPoint()+(e1S.getToPoint()-e1S.getFromPoint())*ec3beam.contrPnt[0][1]

e2S= ec3beam.contrPnt[1][0].getLineSegment(0)
v2CP= e2S.getFromPoint()+(e2S.getToPoint()-e2S.getFromPoint())*ec3beam.contrPnt[1][1]

e3S= ec3beam.contrPnt[2][0].getLineSegment(0)
v3CP= e3S.getFromPoint()+(e3S.getToPoint()-e3S.getFromPoint())*ec3beam.contrPnt[2][1]

e4S= ec3beam.contrPnt[3][0].getLineSegment(0)
v4CP= e4S.getFromPoint()+(e4S.getToPoint()-e4S.getFromPoint())*ec3beam.contrPnt[3][1]

e5S= ec3beam.contrPnt[4][0].getLineSegment(0)
v5CP= e5S.getFromPoint()+(e5S.getToPoint()-e5S.getFromPoint())*ec3beam.contrPnt[4][1]

#Target control points 
p1=geom.Pos3d(0,0,0)
p2=geom.Pos3d(0.0 , 1.63585268035 , 1.15896317009)
p3=geom.Pos3d(0.0 , 3.66350463128 , 1.5)
p4=geom.Pos3d(0.0 , 5.7331401345 , 1.5)
p5=geom.Pos3d(0,7,0)

ratio1=v1CP.dist2(p1)
ratio2=v2CP.dist2(p2)
ratio3=v3CP.dist2(p3)
ratio4=v4CP.dist2(p4)
ratio5=v5CP.dist2(p5)

'''
print('p1= ', p1)
print('v1CP= ', v1CP)
print('ratio1= ', ratio1)
print('p2= ', p2)
print('v2CP= ', v2CP)
print('ratio2= ', ratio2)
print('p3= ', p3)
print('v3CP= ', v3CP)
print('ratio3= ', ratio3)
print('p4= ', p4)
print('v4CP= ', v4CP)
print('ratio4= ', ratio4)
print('p5= ', p5)
print('v5CP= ', v5CP)
print('ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5)  and (ratio3<1e-5) and (ratio4<1e-5) and (ratio5<1e-5) ):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
