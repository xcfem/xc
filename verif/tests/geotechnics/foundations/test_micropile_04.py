# -*- coding: utf-8 -*-
''' Home made test. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec3 import EC3_materials
from materials.api import API_materials
from materials.ehe import EHE_materials
from geotechnics.foundations import guia_micropilotes_oc as guia
import xc
import geom
from model import predefined_spaces

# Material
## Steel pipe.
pipeSteel= API_materials.M80
gammaM0= 1.10
pipeSteel.gammaM= gammaM0
pipeShape= EC3_materials.MicropileTubeShape(steel= pipeSteel, name= 'MP_88.9x7.5')
## Mortar
mortar= EHE_materials.HA25
## Rebar
rebar= EHE_materials.rebar32_B500S

# Micropile section.
# (ascii art from ascii.co.uk)
# 
#              =-=   
#           =   _   = 
#          =  /   \  =
#          |    o    |
#          =  \ _ /  =
#           =       = 
#              = =    
#      
## soilAggressivity: soil aggresivity descriptor
##                   ('unalteredNatural', 'pollutedNatural', 'aggresiveNatural',
##                    'backfill', 'aggresiveBackfill') according to table 2.4.
soilAggressivity= 'unalteredNatural'
## designLife: design service life of the micropile (in years).
designLife= 50
## Fe: influence of the execution technique taken from table 3.5.
Fe= 1.5
## Fuc: reduction factor of the cross-section area due to the  type of the
##      union taken from table 3.4.
Fuc= 0.5
## Define soil horizons.
soilHorizons= guia.SandySoilLayers(soilProfile= [[-100.0, 'medium']], waterTableZ= -100.0, groundLevel= -1.0)
## Define micropile.
micropile= guia.Micropile(pileSet= None, pileDiam= 152.4e-3, soilLayers= soilHorizons, mortarMaterial= mortar, pipeSection= pipeShape, axialRebar= rebar, pileType= 'endBearing', soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)

# Problem definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Finite element material
xcSection= micropile.defElasticShearSection2d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)
## Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
pTop= points.newPoint(geom.Pos3d(0,0,0))
pBottom= points.newPoint(geom.Pos3d(0,-10.0,0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pTop.tag,pBottom.tag)
l.nDiv= 20
### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
## Mesh generation
modelSpace.newSeedElement('ElasticBeam2d')
l.genMesh(xc.meshDir.I)

### Constraints.
micropile.pileSet= l
micropile.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
bottomTipNode= pBottom.getNode()

### Loads
Fh= 10e3
N= -0.85*micropile.getNcRd()
topTipNode= pTop.getNode()
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(topTipNode.tag, xc.Vector([Fh,N,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution
result= modelSpace.analyze(calculateNodalReactions= True)

# Get bottom tip displacement.
## Reference values taken from the results of test_micropile_02.py except for
## the vertical displacement of the top node which is different:
## 3D: -0.018579 vs. 2D: -0.0159323 => TODO: investigate this.
bottomTipDisp= bottomTipNode.getDisp[1]
ratio1= abs(bottomTipDisp+0.010138959626292113)/0.010138959626292113
topTipDisp= topTipNode.getDisp
ratio2= abs(topTipDisp[0]-0.177892)/0.177892
ratio3= abs(topTipDisp[1]+0.0159323)/0.0159323

'''
print('bottom tip displacement: ', bottomTipDisp)
print('ratio1= ', ratio1)
print('top tip displacement: ', topTipDisp)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-4 and abs(ratio3)<1e-4:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


'''
# Graphic stuff.
dispSet= modelSpace.defSet('dispSet')
dispSet.lines.append(l)
dispSet.fillDownwards()
    
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
oh.displayFEMesh()
oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
oh.displayDispRot(itemToDisp='uX',defFScale= 10.0,setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='N',setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='V',setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='M',setToDisplay=dispSet)
'''
