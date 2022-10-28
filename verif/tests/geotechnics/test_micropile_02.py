# -*- coding: utf-8 -*-
''' Home made test.'''
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
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Finite element material
xcSection= micropile.defElasticShearSection3d(preprocessor)
## Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
pTop= points.newPoint(geom.Pos3d(0,0,0))
pBottom= points.newPoint(geom.Pos3d(0,0,-10.0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pTop.tag,pBottom.tag)
l.nDiv= 20
### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
## Mesh generation
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 3 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
seedElem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))
l.genMesh(xc.meshDir.I)

### Constraints.
micropile.pileSet= l
micropile.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
bottomTipNode= pBottom.getNode()
modelSpace.fixNode('FFF_FF0',bottomTipNode.tag)

### Loads
Fh= 10e3
N= -0.85*micropile.getNcRd()
topTipNode= pTop.getNode()
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(topTipNode.tag,xc.Vector([Fh,0,N,0,0,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution
result= modelSpace.analyze(calculateNodalReactions= True)

# Get bottom tip displacement.
bottomTipDisp= bottomTipNode.getDisp[2]
ratio1= abs(bottomTipDisp+0.010075333675765406)/0.010075333675765406
topTipDisp= topTipNode.getDisp
ratio2= (topTipDisp[0]-0.177892)**2+topTipDisp[1]**2+(topTipDisp[2]+0.018579)**2

'''
print(bottomTipDisp)
print(ratio1)
print(topTipDisp)
print(ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-8 and abs(ratio2)<1e-8:
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
oh.displayDispRot(itemToDisp='uX',defFScale= 2.0,setToDisplay=dispSet)
oh.displayDispRot(itemToDisp='uZ',defFScale= 2.0,setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='N',setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='Vy',setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='Mz',setToDisplay=dispSet)
'''
