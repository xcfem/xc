# -*- coding: utf-8 -*-
''' Compute micropiles buckling length.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import geom
import xc

import math
from model import predefined_spaces
from solution import predefined_solutions
from materials.api import API_materials
from materials.ec3 import EC3_materials
from materials.ehe import EHE_materials
from geotechnics.foundations import guia_micropilotes_oc as guia


# Material
## Steel pipe.
pipeSteel= API_materials.M80
gammaM0= 1.10
pipeSteel.gammaM= gammaM0
pipeShape= EC3_materials.MicropileTubeShape(steel= pipeSteel, name= 'MP_88.9x7.5')
pipeShape.sectionClass= 1
## Mortar
mortar= EHE_materials.HA25

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
## Fuc: reduction factor of the cross-section area due to the type of the
##      union taken from table 3.4.
Fuc= 0.5
## Define soil horizons.
soilHorizons= guia.SandySoilLayers(soilProfile= [[-100.0, 'dense']], waterTableZ= -100.0, groundLevel= 0.0)
## Define micropile.
micropile= guia.Micropile(pileSet= None, pileDiam= 225e-3, soilLayers= soilHorizons, mortarMaterial= mortar, pipeSection= pipeShape, axialRebar= None, pileType= 'endBearing', soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)

micropileLength= 8.0

# Finite element model.
## Problem type
feProblem= xc.FEProblem()
feProblem.title= 'Micropile buckling length.'
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Finite element material
xcSection= micropile.defElasticShearSection3d(preprocessor)
## Problem geometry
pTop= modelSpace.newKPoint(0,0,0)
pBottom= modelSpace.newKPoint(0,0,-micropileLength)
l= modelSpace.newLine(pTop,pBottom)
l.nDiv= 20
### Geometric transformations
corot= modelSpace.newCorotCrdTransf("corot",xc.Vector([0,1,0]))
## Mesh generation
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 3 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= corot.name
seedElem= seedElemHandler.newElement("ElasticBeam3d",xc.ID([0,0]))
l.genMesh(xc.meshDir.I)

### Constraints.
micropile.pileSet= l
micropile.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
bottomTipNode= pBottom.getNode()
modelSpace.fixNode('FFF_FF0',bottomTipNode.tag)

### Load factors
# Loads definition
lp0= modelSpace.newLoadPattern(name= 'lp0')
P= -1
lp0.newNodalLoad(pTop.getNode().tag, xc.Vector([0,0,P,0,0,0]))
modelSpace.addLoadCaseToDomain(lp0.name)

# Perform linear buckling analysis.
linearBucklingAnalysis= predefined_solutions.LinearBucklingAnalysis(prb= feProblem, numModes= 2, constraintHandlerType= 'transformation', numberingMethod= 'rcm', convTestType= "norm_disp_incr_conv_test", convergenceTestTol= 1e-8, maxNumIter= 1000, soeType= "band_gen_lin_soe", solverType= "band_gen_lin_lapack_solver", solutionAlgorithmType= 'krylov_newton_soln_algo', eigenSOEType= "band_arpackpp_soe", eigenSolverType= "band_arpackpp_solver")
linearBucklingAnalysis.setup()
analOk= linearBucklingAnalysis.solve()

eulerBucklingLoad= linearBucklingAnalysis.analysis.getEigenvalue(1)
k= math.pi/micropileLength*math.sqrt(micropile.pipeSection.EIz()/eulerBucklingLoad)
# Compute effective length.
effectiveLength= k*micropileLength
# Buckling reduction factor (equal for both axis).
chiN= micropile.pipeSection.getBucklingReductionFactorZ(Leq= effectiveLength)
ratio1= abs(chiN-0.41829303076129865)

'''
print("Euler buckling load: ",eulerBucklingLoad/1e3, 'kN')
print("effective length factor; k= ",k, 'm')
print("effective length = ",k*micropileLength, 'm')
print('buckling reduction factor: ', chiN)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayEigenvectors(1)
# oh.displayEigenResult(1, defFScale= 10.0)
# oh.displayLocalAxes()
# oh.displayReactions()
