# -*- coding: utf-8 -*-
'''Calculation of displacements and internal forces of a pile taken from project "Reordenación del Enlace de la Pañoleta y Accesos a Camas (Sevilla)", annex nº 13 Structures, pages 93-96. Study in 2D.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import xc
from model import predefined_spaces
from model.geometry import grid_model as gm
from model.mesh import finit_el_model as fem
from materials.ehe import EHE_materials
from materials import typical_materials as tm
from geotechnics.foundations import guia_cimentaciones_oc as guia



# Data
fiPile= 1.5  # pile diameter [m]
Emat=2.8e6    # elastic modulus of pile material [Pa]
bearCap=450e4 # total bearing capacity of the pile [N]
pType='endBearing' # type of pile
zGround=0  # ground elevation
Lpile=36
soils=[[-1,'sandy',8025e3],
       [-1.85,'clay',8025e3/75.],
       [-6.55,'clay',122e3],
       [-16.45,'clay',18.75e3],
       [-26.25,'clay',4500e4/75.],
       [-36.0,'clay',253e3]] # Properties of the soils:
# sandy soils [(zBottom,type, nh), ...]  where 'zBottom' is the global Z 
#  coordinate of the bottom level of the soil and 'nh' [Pa/m] is the 
#  coefficient corresponding to the compactness of the sandy soil.
# clay soils [(zBottom,type, s), ...]  where 'zBottom' is the global Z 
#  coordinate of the bottom level of the soil and 'su' [Pa/m] is the shear  
#  strength of the saturated cohesive soil.
soilLayers= guia.SoilLayers(soilProfile= soils, groundLevel= zGround)


# Loads
Fh=706.9e3+572.5e3
N=-1532e3-1470.1e3
Fh=706.9e3
N=-1532e3

# Materials
concrete=EHE_materials.HA30

# Define FE problem.
FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes) # Defines the
## Problem geometry
pTop= modelSpace.newKPoint(0,0)
pBottom= modelSpace.newKPoint(0,-Lpile)
l= modelSpace.newLine(pTop, pBottom)
eSize= 0.24     # length of elements
l.setElemSize(eSize)
pileBC= guia.PileFoundation(pileSet= None, pileDiam= fiPile, E= concrete.Ecm(), pileType='endBearing', pileBearingCapacity=bearCap, soilLayers= soilLayers)

## Create mesh.
### Finite element material
xcSection= pileBC.defElasticShearSection2d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)

### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
### Set element type.
modelSpace.newSeedElement('ElasticBeam2d')
### Generate mesh.
l.genMesh(xc.meshDir.I)

### Constraints.
pileBC.pileSet= l
pileBC.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
nTop= pTop.getNode()
nBase= pBottom.getNode()
modelSpace.fixNode('FF0',nTop.tag)  # no rotation around z axis.

pileSet= modelSpace.defSet('pileSet')
for e in l.elements:
    pileSet.elements.append(e)
pileSet.fillDownwards()

springs= pileBC.springs
springSet= modelSpace.defSet('springSet')
for e in springs:
    springSet.getElements.append(e)
    # print('z:',e.getCooCentroid(True)[2], ' Kx (t/m):',round(e.getMaterials()[0].E*1e-4,2))
springSet.fillDownwards()
allSets= modelSpace.setSum('allSets', [pileSet,springSet])

### Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nTop.tag, xc.Vector([Fh,N,0]))
modelSpace.addLoadCaseToDomain(lp0.name) # add the load case to the domain.

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

# Get computed results.
y_node_check=[-0.48,-1.44,-4.32,-12.72,-21.36,-35.28]
ux_ref=[0.0095,0.0092,0.0075,0.002,-0.0001,0.00002]
ux_calc= list()
Kx_ref= [138.67e4,288.9e4,329.4e4,50.62e4,1620e4,683.1e4]
Kx_calc= list()
V_ref= [-68.71e4,-58.77e4,-26.07e4,-1.52e4,5.35e4,-0.06e4]
V_calc= list()
M_ref= [-210.51e4,-147.87e4,-23e4,36.57e4,18.75e4,-0.03e4]
M_calc= list()

for y in y_node_check:
    p= geom.Pos3d(0,y,0)
    n= pileSet.nodes.getNearestNode(p)
    ux= n.getDisp[0]
    ux_calc.append(ux)
    e_s= springSet.elements.getNearestElement(p)
    Kx=e_s.getMaterials()[0].E
    Kx_calc.append(Kx)
    p= geom.Pos3d(0,y-0.24/2.,0)
    e_p= pileSet.elements.getNearestElement(p)
    V= e_p.getV2
    V_calc.append(V)
    M= e_p.getM2
    M_calc.append(M)

# Check results.
nVal=len(y_node_check)
ac_ux=0
for uxc, uxr in zip(ux_calc, ux_ref):
    ac_ux+=((uxr-uxc)/uxr)**2
err_ux= math.sqrt(ac_ux/nVal)
ac_Kx=0
for kxr, kxc in zip(Kx_ref, Kx_calc):
    ac_Kx+=((kxr-kxc)/kxr)**2
err_Kx= math.sqrt(ac_Kx/nVal)
ac_V=0
for vr, vc in zip(V_ref, V_calc):
    ac_V+=((vr-vc)/vr)**2
err_V= math.sqrt(ac_V/nVal)
ac_M=0
for mr, mc in zip(M_ref, M_calc):
    ac_M+= ((mr-mc)/mr)**2
err_M=math.sqrt(ac_M/nVal)

'''
print('err_ux= ', err_ux)
print('err_Kx= ', err_Kx)
print('err_V= ', err_V)
print('err_M= ', err_M)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err_ux)<0.1 and abs(err_Kx)<5e-5 and abs(err_V)<0.5 and abs(err_M)<0.5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX',defFScale= 100.0,setToDisplay=pileSet)
# # oh.displayDispRot(itemToDisp='uY',defFScale= 2.0,setToDisplay=pileSet)
# # oh.displayIntForcDiag(itemToDisp='N',setToDisplay=pileSet)
# oh.displayIntForcDiag(itemToDisp='V',setToDisplay=pileSet)
# oh.displayIntForcDiag(itemToDisp='M',setToDisplay=pileSet)
