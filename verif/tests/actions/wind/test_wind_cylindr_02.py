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
from model.geometry import grid_model as gm
from model.mesh import finit_el_model as fem
from materials import typical_materials as tm
from materials.astm_aisc import ASTM_materials as astm
from actions.wind import base_wind as bw
from actions import load_cases as lcases
from model.sets import sets_mng as sets


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
Gf=0.85   #gust effect factor

#Tanks data
R=1      # radius
height=2    # height of the cylindrical tank
thickness=5e-3 # thickness of the wall

#Tank data
windComp=[1,0] # components [x,y] of a vector in wind direction 
zGround=0      # ground level
zBase=4        # level of the tank base 
xCent=2        # x-coordinate of the cylinder axis
yCent=1        # y-coordinate of the cylinder axis

#End data

FEcase= xc.FEProblem()
FEcase.title= 'Cylindrical tank'
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
elements.dimElem= 3
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) #Defines the

# grid model definition
Rcoo=[R]
Angcoo=[i*360/25 for i in range(25)]
zTop=zBase+height
Zcoo=[zBase,zTop]
grid=gm.GridModel(prep,Rcoo,Angcoo,Zcoo,xCentCoo=xCent,yCentCoo=yCent)
grid.generateCylZPoints()

tank=grid.genSurfOneXYZRegion(xyzRange=((R,0,zBase),(R,Angcoo[-1],zTop)), setName='tank',closeCyl='Y')

steel=astm.A36
steel_prop=tm.MaterialData(name='steel',E=steel.E,nu=steel.nu,rho=steel.rho)
tank_mat=tm.DeckMaterialData(name='tank_mat',thickness= thickness,material=steel_prop)
tank_mat.setupElasticSection(preprocessor=prep)   #creates the section-materia
tank_mesh=fem.SurfSetToMesh(surfSet=tank,matSect=tank_mat,elemSize=0.25,elemType='ShellMITC4')
tank_mesh.generateMesh(prep)

# Boundary condition
nodBase=sets.get_set_nodes_plane_XY(setName='nodBase', setBusq=tank, zCoord=zBase, tol=0.001)
for n in nodBase.nodes:
    modelSpace.fixNode('000_FFF',n.tag)

# Loading
windParams=bw.windParams(v,Kd,Kzt,I,alpha,zg)
tankWind=bw.cylindrWind(2*R,height,windParams,windComp,zGround,xCent,yCent)

WX=lcases.LoadCase(preprocessor=prep,name="WX",loadPType="default",timeSType="constant_ts")
WX.create()
for e in tank.elements:
    vCoo=e.getCooCentroid(0)
    pres=Gf*tankWind.getWindPress(vCoo[0],vCoo[1],vCoo[2])
    loadVector=xc.Vector([0,0,-pres])
    e.vector3dUniformLoadLocal(loadVector)

modelSpace.removeAllLoadPatternsFromDomain()
modelSpace.addLoadCaseToDomain('WX')
modelSpace.analyze(calculateNodalReactions=True)

def getResultReactForces(dof,nodSet):
    '''dof=0 for Rx, dof=1 for Ry, dof=2 for Rz
    '''
    result=0.0
    for n in nodSet.nodes:
        result+=n.getReaction[dof]
    return result
Fx=-getResultReactForces(0,nodBase)
Fy=-getResultReactForces(1,nodBase)
Fz=-getResultReactForces(2,nodBase)
Mx=0
for n in nodBase.nodes:
    Mx+=n.getReaction[3]+(n.get3dCoo[1]-yCent)*n.getReaction[2]
My=0
for n in nodBase.nodes:
    My+=n.getReaction[4]+(n.get3dCoo[0]-xCent)*n.getReaction[2]

ratio1=Fx-2305.2953
ratio2=Fy
ratio3=Mx
ratio4=My-2320.22049

OK=abs(ratio1)<1e-4 and abs(ratio2)<1e-4 and abs(ratio3)<1e-4 and abs(ratio4)<1e-4 

'''
print(OK)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (OK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

'''
from postprocess import output_styles as outSty
from postprocess import output_handler as outHndl
sty=outSty.OutputStyle() 
out=outHndl.OutputHandler(modelSpace,sty)
out.displayFEMesh()
out.displayLoadVectors()
'''
