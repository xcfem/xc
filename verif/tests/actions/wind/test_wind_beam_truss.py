# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

''' Test distribution of wind pressures in the cylindrical body of a tank'''

import os
import math
import xc_base
import geom
import xc
from model import predefined_spaces
from model.mesh import finit_el_model as fem
from model.geometry import grid_model as gm
from actions.wind import base_wind as bw
from materials import typical_materials as tm
from materials.ec3 import EC3_materials
from model.sets import sets_mng as sets
from actions import loads
from actions import load_cases as lcases

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega.ort@gmail.com l.pereztato@gmail.com"

#units
kmh2ms=1/3.6
#   Data
#Wind data
v=155*kmh2ms #basic speed wind [m/s]
Kd=0.95      #wind directionality factor
Kzt=1.0      #topographic factor
I=1.15       #importance factor
alpha=9.5    #terrain exposure constant (exposure C)
zg=275       #terrain exposure constant (exposure C)
Cp=1.2       #pressure coefficient

#Geometry data
xGridCoo=[0,4]
yGridCoo=[0]
zGridCoo=[0,5,10]

zGround=0


#End data
#             *** GEOMETRIC model (points, lines, surfaces) - SETS ***
FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #Defines the
# dimension of the space: nodes by three coordinates (x,y,z) and 
# six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

gridGeom=gm.GridModel(prep,xGridCoo,yGridCoo,zGridCoo)
gridGeom.generatePoints()

#Lines generation
columns=gridGeom.genLinMultiXYZRegion([
    ((0,0,0),(0,0,10)),
    ((4,0,0),(4,0,10))],'columns')
beams=gridGeom.genLinMultiXYZRegion([
    ((0,0,5),(4,0,5)),
    ((0,0,10),(4,0,10))],'beams')
diagonals=gridGeom.genSetLinFromMultiLstXYZPnt([
    ((0,0,5),(4,0,0)),
    ((0,0,5),(4,0,10))],'diagonals')

#Materials
steel=EC3_materials.S235JR
steel.gammaM=1.0
steel_prop=tm.MaterialData(name='steel',E=steel.E,nu=steel.nu,rho=steel.rho)
column_mat=EC3_materials.HEShape(steel,'HE_140_B')
column_mat.defElasticShearSection3d(prep)
beam_mat=EC3_materials.UPNShape(steel,'UPN_80')
beam_mat.defElasticShearSection3d(prep)
diag_mat=EC3_materials.SHSShape(steel,'SHS50x50x2_5')
xcDiagSteel=steel.defElasticMaterial(prep)

#                         ***FE model - MESH***
#Meshing
#Steel elements: local Z-axis corresponds to weak axis of the steel shape

columns_mesh=fem.LinSetToMesh(columns,column_mat,0.25,xc.Vector([1,0,0]))
beams_mesh=fem.LinSetToMesh(beams,beam_mat,0.25,xc.Vector([0,0,1]))
fem.multi_mesh(prep,[columns_mesh,beams_mesh],sectGeom='Y')  #mesh these sets and creates property 'sectionGeometry' for each element)

diagonals_mesh=fem.LinSetToMesh(diagonals,xcDiagSteel,100,xc.Vector([1,0,0]),'Truss',3,None)
diagonals_mesh.generateMesh(prep)
for e in diagonals.getElements:
    e.sectionArea=diag_mat.A()

#Boundary conditions
nodBase=sets.get_set_nodes_plane_XY(setName='nodBase', setBusq=columns, zCoord=zGround, tol=0.001)
for n in nodBase.nodes:
    modelSpace.fixNode('000_000',n.tag)

    
# Wind action
windParams=bw.windParams(v,Kd,Kzt,I,alpha,zg)
wind_columns=loads.WindLoadOnBeams('wind_columns',columns,windParams,Cp,xc.Vector([0,1,0]),column_mat.h())
wind_beams=loads.WindLoadOnBeams('wind_beams',beams,windParams,Cp,xc.Vector([0,1,0]))
wind_diagonals=loads.WindLoadOnTrusses('wind_diagonals',diagonals,windParams,Cp,xc.Vector([0,1,0]),diag_mat.h())

#    ***LOAD CASES***
LC1=lcases.LoadCase(preprocessor=prep,name="LC1",loadPType="default",timeSType="constant_ts")
LC1.create()
LC1.addLstLoads([wind_columns])

LC2=lcases.LoadCase(preprocessor=prep,name="LC2",loadPType="default",timeSType="constant_ts")
LC2.create()
LC2.addLstLoads([wind_beams])

LC3=lcases.LoadCase(preprocessor=prep,name="LC3",loadPType="default",timeSType="constant_ts")
LC3.create()
LC3.addLstLoads([wind_diagonals])

nReac1=gridGeom.getPntXYZ((0,0,0)).getNode()
nReac2=gridGeom.getPntXYZ((4,0,0)).getNode()

from solution import predefined_solutions
analysis= predefined_solutions.simple_static_linear(FEcase)

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC1')
result= analysis.analyze(1)
result= modelSpace.analyze(calculateNodalReactions= True)
R1a=nReac1.getReaction[1]
R1b=nReac2.getReaction[1]
R1_comp=-1880.48

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC2')
result= analysis.analyze(1)
result= modelSpace.analyze(calculateNodalReactions= True)
R2a=nReac1.getReaction[1]
R2b=nReac2.getReaction[1]
R2_comp=-444.547

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('LC3')
result= analysis.analyze(1)
result= modelSpace.analyze(calculateNodalReactions= True)
R3a=nReac1.getReaction[1]
R3b=nReac2.getReaction[1]
R3a_comp=-431.403717
R3b_comp=-430.08335587125214


OK=abs(R1a-R1_comp)<1e-2 and abs(R1b-R1_comp)<1e-2 and abs(R2a-R2_comp)<1e-2 and abs(R2b-R2_comp)<1e-2 and abs(R3a-R3a_comp)<1e-2 and abs(R3b-R3b_comp)<1e-2
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (OK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')



'''
#Graphics
from postprocess.config import default_config
from postprocess import output_styles as outSty
from postprocess import output_handler as outHndl
sty=outSty.OutputStyle() 
out=outHndl.OutputHandler(modelSpace,sty)
out.displayBlocks()
out.displayFEMesh()
modelSpace.addLoadCaseToDomain("LC1")
out.displayLoads()
modelSpace.removeLoadCaseFromDomain("LC1")
modelSpace.addLoadCaseToDomain("LC2")
out.displayLoads()
modelSpace.removeLoadCaseFromDomain("LC2")
modelSpace.addLoadCaseToDomain("LC3")
out.displayLoads()
modelSpace.removeLoadCaseFromDomain("LC3")
'''
