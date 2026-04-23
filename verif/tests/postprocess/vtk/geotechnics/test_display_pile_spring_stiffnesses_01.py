# -*- coding: utf-8 -*-
''' Home made test. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
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

# Check that the properties are set.
k_x_ref_values= {4: 190500.0, 5: 381000.0, 6: 571500.0, 7: 762000.0, 8: 952500.0, 9: 1143000.0, 10: 1333500.0, 11: 1524000.0, 12: 1714500.0, 13: 1905000.0, 14: 2095500.0, 15: 2286000.0, 16: 2476500.0, 17: 2667000.0, 18: 2857500.0, 19: 3048000.0, 20: 3238500.0, 1: 1714500.0}
err= 0.0
for n in micropile.pileSet.nodes:
    if(n.hasProp('k_x')):
        nTag= n.tag
        k_x= n.getProp('k_x')
        k_x_ref= k_x_ref_values[nTag]
        err+= (k_x-k_x_ref)**2
err= math.sqrt(err)
testOK= (err<1e-6)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0, fileName= outputFileName)

# Check that file exists
testOK= testOK and os.path.isfile(outputFileName)

from misc_utils import log_messages as lmsg
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


