# -*- coding: utf-8 -*-

'''Test sets and grid model. Generation/selection of geometric and FE entities using grid model, boolean operations with the created sets.'''

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
from materials import section_properties as sectpr
from model.mesh import finit_el_model as fem
from model.sets import sets_mng as sets


# *** PROBLEM
FEcase= xc.ProblemaEF()
prep=FEcase.getPreprocessor
nodes= prep.getNodeLoader
elements= prep.getElementLoader
elements.dimElem= 3
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #defines dimension of
                  #the space: nodes by three coordinates (x,y,z) and 
                  #three DOF for each node (Ux,Uy,Uz)

# *** GEOMETRY ***

# coordinates in global X,Y,Z axes for the generation of grid 
xList=[i for i in range(11)]
yList=[i for i in range(21)]
zList=[i for i in range(6)]

#Points' generation
gridGeom=gm.GridModel(prep,xList,yList,zList)
gridGeom.generatePoints()

# Parameters

# Ranges to be used lately
floor1_rg=gm.IJKRange((2,5,2),(8,15,2))
floor2_rg=[gm.IJKRange((0,0,4),(10,20,4))]
wall_rg=gm.IJKRange((2,10,2),(8,10,5))
columns_rg=gm.IJKRange((2,5,0),(8,15,4)).extractIncludedKranges(stepI=6,stepJ=10)

#Surfaces' generation
floor1=gridGeom.genSurfOneRegion(ijkRange=floor1_rg,nameSet='floor1')
floor1.description='Floor 1'
floor2=gridGeom.genSurfMultiRegion(lstIJKRange=floor2_rg,nameSet='floor2')
floor2.description='Floor 2'
wall=gridGeom.genSurfOneRegion(ijkRange=wall_rg,nameSet='wall')
wall.description='Wall'

#Lines' generation
columns=gridGeom.genLinMultiRegion(lstIJKRange=columns_rg,nameSet='columns')
columns.description='Columns'

# Plot
# from postprocess.xcVtk.CAD_model import vtk_CAD_graphic
# defDisplay= vtk_CAD_graphic.RecordDefDisplayCAD()
# totalSet= prep.getSets.getSet('total')
# defDisplay.displayBlocks(xcSet=totalSet,fName= None,caption= 'Model grid')


#                         *** MATERIALS *** 
mat_slabs=tm.MaterialData(name='mat_slabs',E=2.55e15,nu=0.2,rho=2500)
mat_walls=tm.MaterialData(name='mat_walls',E=2.55e15,nu=0.2,rho=2500)
mat_cols=tm.MaterialData(name='mat_cols',E=2.55e15,nu=0.2,rho=2500)

# Isotropic elastic section-material appropiate for plate and shell analysis
deck_mat=tm.DeckMaterialData(name='deck_mat',thickness= 0.25,material=mat_slabs)
deck_mat.setupElasticSection(preprocessor=prep)   #creates de section-material
wall_mat=tm.DeckMaterialData(name='wall_mat',thickness= 0.15,material=mat_walls)
wall_mat.setupElasticSection(preprocessor=prep)   #creates de section-material

#Geometric sections
#rectangular sections
geomSectColumns=sectpr.RectangularSection(name='geomSectColumns',b=0.25,h=0.25)
columns_mat= tm.BeamMaterialData(name= 'columns_mat', section=geomSectColumns, material=mat_cols)
columns_mat.setupElasticShear3DSection(preprocessor=prep)


#                         ***FE model - MESH***
eSize=1
floor1_mesh=fem.SurfSetToMesh(surfSet=floor1,matSect=deck_mat,elemSize=eSize,elemType='ShellMITC4')
floor2_mesh=fem.SurfSetToMesh(surfSet=floor2,matSect=deck_mat,elemSize=eSize,elemType='ShellMITC4')
wall_mesh=fem.SurfSetToMesh(surfSet=wall,matSect=wall_mat,elemSize=eSize,elemType='ShellMITC4')
columns_mesh=fem.LinSetToMesh(linSet=columns,matSect=columns_mat,elemSize=eSize,vDirLAxZ=xc.Vector([1,0,0]),elemType='elastic_beam_3d',dimElemSpace=3,coordTransfType='linear')

fem.multi_mesh(preprocessor=prep,lstMeshSets=[columns_mesh,floor1_mesh,floor2_mesh,wall_mesh])     #mesh these sets

#      *** SETS ***
wall_nel=wall.getNumElements
wall_nel_targ=(wall_rg.getIMax()-wall_rg.getIMin())*(wall_rg.getKMax()-wall_rg.getKMin())
floor1_nel=floor1.getNumElements
floor1_nel_targ=(floor1_rg.getIMax()-floor1_rg.getIMin())*(floor1_rg.getJMax()-floor1_rg.getJMin())
floor2_nel=floor2.getNumElements
floor2_nel_targ=(floor2_rg[0].getIMax()-floor2_rg[0].getIMin())*(floor2_rg[0].getJMax()-floor2_rg[0].getJMin())
columns_nel=columns.getNumElements
columns_nel_targ=4*4
ratio1=(wall_nel-wall_nel_targ)+(floor1_nel-floor1_nel_targ)+(floor2_nel-floor2_nel_targ)+(columns_nel-columns_nel_targ)

# *** Boolean operations with sets
# Union and difference
overallSet=wall+floor1+floor2+columns
ratio2=overallSet.getNumElements-(wall_nel+floor1_nel+floor2_nel+columns_nel)

allButWall=overallSet-wall
ratio3=allButWall.getNumElements-(floor1_nel+floor2_nel+columns_nel)

onlyColumns=allButWall-(floor1+floor2)
ratio4=onlyColumns.getNumElements-columns_nel

# Intersection
int1=overallSet*onlyColumns
int2=overallSet*allButWall*onlyColumns
ratio5=int1.getNumElements-int2.getNumElements
int4=(floor1+floor2)*onlyColumns
ratio6=int4.getNumElements

# **** Subsets
pxmax=5
pymax=13
hexaedr=geom.BND3d(geom.Pos3d(0,0,0),geom.Pos3d(xList[pxmax],yList[pymax],zList[5]))

wallInHex= sets.getSubsetInside(geomObj=hexaedr,fromSet=wall,toSetName='wallInHex',tol=0.0)
wallInHex_nel_targ=(pxmax-wall_rg.getIMin())*(wall_rg.getKMax()-wall_rg.getKMin())

floor1InHex= sets.getSubsetInside(geomObj=hexaedr,fromSet=floor1,toSetName='floor1InHex',tol=0.0)
floor1InHex_nel_targ=(pxmax-floor1_rg.getIMin())*(pymax-floor1_rg.getJMin())

floor2InHex= sets.getSubsetInside(geomObj=hexaedr,fromSet=floor2,toSetName='floor2InHex',tol=0.0)
floor2InHex_nel_targ=(pxmax-floor2_rg[0].getIMin())*(pymax-floor2_rg[0].getJMin())

columnsInHex= sets.getSubsetInside(geomObj=hexaedr,fromSet=columns,toSetName='columnsInHex',tol=0.0)
columnsInHex_nel_targ=4

ratio7=(wallInHex.elements.size-wallInHex_nel_targ)+(floor1InHex.elements.size-floor1InHex_nel_targ)+(floor2InHex.elements.size-floor2InHex_nel_targ)+(columnsInHex.elements.size-columnsInHex_nel_targ)

overallInHex= sets.getSubsetInside(geomObj=hexaedr,fromSet=overallSet,toSetName='overallInHex',tol=0.0)

ratio8=overallInHex.elements.size-wallInHex.elements.size-floor1InHex.elements.size-floor2InHex.elements.size-columnsInHex.elements.size

ratio9=overallInHex.surfaces.size-wallInHex.surfaces.size-floor1InHex.surfaces.size-floor2InHex.surfaces.size-columnsInHex.surfaces.size

'''
# ***** this doesn't work for the moment TAKE A LOOK!!!
shells=sets.getSubsetElemOfType(elemType='ShellMITC4',fromSet=overallSet,toSetName='shells')

beams=sets.getSubsetElemOfType(elemType='elastic_beam_3d',fromSet=overallSet,toSetName='beams')

columnsmat=sets.getSubsetElemOfMat(matType='columns_mat',fromSet=overallSet,toSetName='columnsmat')

# this works but yields lots of warning messages
wallmat=sets.getSubsetElemOfMat(matType='wall_mat',fromSet=overallSet,toSetName='wallmat')
deckmat=sets.getSubsetElemOfMat(matType='deck_mat',fromSet=overallSet,toSetName='deckmat')
'''
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if(ratio1==0) & (ratio2==0) & (ratio3==0) & (ratio4==0) & (ratio5==0) & (ratio6==0) & (ratio7==0) & (ratio8==0) & (ratio9==0):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

