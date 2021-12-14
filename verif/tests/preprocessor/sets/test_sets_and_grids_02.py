# -*- coding: utf-8 -*-
'''Test sets and grid model. Selection of subsets'''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import os
import xc_base
import geom
import xc
from model import predefined_spaces
from model.geometry import grid_model as gm
from materials import typical_materials as tm
from materials.sections import section_properties as sectpr
from model.mesh import finit_el_model as fem
from model.sets import sets_mng as sets
# Default configuration of environment variables.
# from postprocess import output_styles as outSty
# from postprocess import output_handler as outHndl



# *** PROBLEM
FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #defines dimension of
                  #the space: nodes by three coordinates (x,y,z) and 
                  #three DOF for each node (Ux,Uy,Uz)
#sty=outSty.OutputStyle() 
#out=outHndl.OutputHandler(modelSpace,sty)

# *** GEOMETRY ***

# coordinates in global X,Y,Z axes for the generation of grid 
xList=[i for i in range(11)]
yList=[i for i in range(21)]
zList=[i for i in range(6)]

#Points' generation
gridGeom=gm.GridModel(prep,xList,yList,zList)
gridGeom.generatePoints()

floor1=gridGeom.genSurfOneXYZRegion(xyzRange=((2,5,2),(8,15,2)), setName='floor1')
#out.displayBlocks()
floor2=gridGeom.genSurfOneXYZRegion(xyzRange=((0,0,4),(10,20,4)), setName='floor2')
#out.displayBlocks()
columns_rg=gm.IJKRange((2,5,0),(8,15,4)).extractIncludedKranges(stepI=2,stepJ=3)
columns=gridGeom.genLinMultiRegion(lstIJKRange=columns_rg,setName='columns')
#out.displayBlocks()
#                         *** MATERIALS (constitutive models) *** 
mat_slabs= tm.MaterialData(name='mat_slabs',E=2.55e15,nu=0.2,rho=2500)
mat_walls= tm.MaterialData(name='mat_walls',E=2.55e15,nu=0.2,rho=2500)
mat_cols= tm.MaterialData(name='mat_cols',E=2.55e15,nu=0.2,rho=2500)

# Isotropic elastic section-material appropiate for plate and shell analysis
deck_mat=tm.DeckMaterialData(name='deck_mat',thickness= 0.25,material=mat_slabs)
deck_mat.setupElasticSection(preprocessor=prep)   #creates de section-material
wall_mat=tm.DeckMaterialData(name='wall_mat',thickness= 0.15,material=mat_walls)
wall_mat.setupElasticSection(preprocessor=prep)   #creates de section-material

#Sections geometry
#rectangular sections
geomSectColumns=sectpr.RectangularSection(name='geomSectColumns',b=0.25,h=0.25)
columns_mat= tm.BeamMaterialData(name= 'columns_mat', section=geomSectColumns, material=mat_cols)
columns_mat.setupElasticShear3DSection(preprocessor=prep)


#                         ***FE model - MESH***
eSize=0.5
floor1_mesh=fem.SurfSetToMesh(surfSet=floor1,matSect=deck_mat,elemSize=eSize,elemType='ShellMITC4')
floor2_mesh=fem.SurfSetToMesh(surfSet=floor2,matSect=deck_mat,elemSize=eSize,elemType='ShellMITC4')
#wall_mesh=fem.SurfSetToMesh(surfSet=wall,matSect=wall_mat,elemSize=eSize,elemType='ShellMITC4')
columns_mesh=fem.LinSetToMesh(linSet=columns,matSect=columns_mat,elemSize=eSize,vDirLAxZ=xc.Vector([1,0,0]),elemType='ElasticBeam3d',dimElemSpace=3,coordTransfType='linear')

fem.multi_mesh(preprocessor=prep,lstMeshSets=[columns_mesh,floor1_mesh,floor2_mesh])     #mesh these sets

surfElInters=floor1.getElements.size

subset1_rg=gm.IJKRange((2,5,2),(8,15,4))
subset1=gridGeom.getSubsetSurfOneRegion(superset=floor2, ijkRange=subset1_rg, nameSubset='subset1')
ratio1=subset1.getElements.size-surfElInters


subset2=gridGeom.getSubsetSurfOneXYZRegion(superset=floor2, xyzRange=((2,5,2),(8,15,4)), nameSubset='subset2')
ratio2=subset2.getElements.size-surfElInters

subset3_rg=[gm.IJKRange((2,5,2),(8,10,4)),gm.IJKRange((2,10,2),(8,15,4))]
subset3=gridGeom.getSubsetSurfMultiRegion(superset=floor2, lstIJKRange=subset3_rg, nameSubset='subset3')
ratio3=subset3.getElements.size-surfElInters

subset4_xyzrg=[((2,5,2),(8,10,4)),((2,10,2),(8,15,4))]
subset4=gridGeom.getSubsetSurfMultiXYZRegion(superset=floor2, lstXYZRange=subset4_xyzrg, nameSubset='subset4')
ratio4=subset4.getElements.size-surfElInters


linElInters=columns.getElements.size/len(columns_rg)
subset5_rg=columns_rg[0]
subset5=gridGeom.getSubsetLinOneRegion(superset=columns,ijkRange=subset5_rg,nameSubset='subset5')
ratio5=subset5.getElements.size-linElInters

linElInters=columns.getElements.size/2.0
subset6_rg=gm.IJKRange((2,5,0),(8,15,2)).extractIncludedKranges(stepI=2,stepJ=3)
subset6=gridGeom.getSubsetLinMultiRegion(superset=columns,lstIJKRange=subset6_rg,nameSubset='subset6')
ratio6=subset6.getElements.size-linElInters

linElInters=columns.getElements.size
subset7=gridGeom.getSubsetLinOneXYZRegion(superset=columns,xyzRange=((0,0,0),(10,20,5)),nameSubset='subset7')
ratio7=subset7.getElements.size-linElInters

linElInters=columns.getElements.size
subset8=gridGeom.getSubsetLinMultiXYZRegion(superset=columns,lstXYZRange=[((0,0,0),(5,20,5)),((5,0,0),(10,20,5))],nameSubset='subset8')
ratio8=subset8.getElements.size-linElInters

'''
print(ratio1)
print(ratio2)
print(ratio3)
print(ratio4)
print(ratio5)
print(ratio6)
print(ratio7)
print(ratio8)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1==0) & (ratio2==0) & (ratio3==0) & (ratio4==0) & (ratio5==0) & (ratio6==0) & (ratio7==0) & (ratio8==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


