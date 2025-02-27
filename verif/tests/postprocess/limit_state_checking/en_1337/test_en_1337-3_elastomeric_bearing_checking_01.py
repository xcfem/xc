# -*- coding: utf-8 -*-
''' Test checking of elastomeric bridge bearings according to EN 1337-3. It is only a dumb test to check that things work as expected. It serves also as a reference as how to use the checking routines.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import inspect
import math
import geom
import xc
from scipy.constants import g
import statistics

from solution import predefined_solutions
from model import predefined_spaces
from materials.ec3 import EC3_materials
from materials.en_1337 import en_1337_3 as en
from materials.en_1337 import en_1337_3_limit_state_checking as en_ls
from actions import loads
from actions import combinations as combs

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
## Beam material.
steel= EC3_materials.S275JR
steel.setGammaM0(1.0)
shape= EC3_materials.IPEShape(steel= steel, name= 'IPE_600')
xcSection= shape.defElasticShearSection3d(preprocessor)
## Elastomeric bearing.
# a: bearing length (see figure 2 of EN 1337-3:2005).
# b: bearing width (see figure 2 of EN 1337-3:2005).
# tb: total height (see figure 2 of EN 1337-3:2005).
# ti: thickness of individual elastomer layer (see figure 2 of EN 1337-3:2005).
# ts: thickness of steel reinforcing plate (see figure 2 of EN 1337-3:2005).
# tso: thickness of outer steel reinforcing plate.
# Te: total elastomer thickness.
# Tb: total nominal thickness of bearing.
# n: number of elastomer layers.
# C: side cover.
# ted: edge cover.
# G: (optional) nominal value of conventional shear modulus of elastomeric
#    bearing (see table 1 of EN 1337-3:2005).
# gammaM: (optional) partial safety factor which value may be chosen in the
#         National Annex. The recommended value is γm = 1.00.
# fy: (optional) yield stress of the reinforcing steel.
# bearing_type: (optional) string that identifies the type of the bearing in the#               problem.
bearing= en.RectangularLaminatedBearing(a= 0.25, b= 0.30, tb= 0.03, ti= 0.008, ts= 3e-3, Te= 0.021, tso= 0.0, Tb= 0.03, n= 2, C= 6e-3, ted= 8e-3, bearing_type= 'T0')

# Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
L= 5.0 # Bar length.
l= L*math.sqrt(2.0)/2.0
pt0= points.newPoint(geom.Pos3d(0,0,-0.35))
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(l,l,0))
pt3= points.newPoint(geom.Pos3d(l,l,-0.35))

lines= preprocessor.getMultiBlockTopology.getLines
l1= lines.newLine(pt1.tag,pt2.tag)
l1.nDiv= 10

l0= lines.newLine(pt0.tag, pt1.tag)
l0.nDiv= 1
l2= lines.newLine(pt2.tag, pt3.tag)
l2.nDiv= 1
# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

## Seed element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 3 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
beam3d= seedElemHandler.newElement("ElasticBeam3d")

xcTotalSet= preprocessor.getSets.getSet("total")
xcTotalSet.genMesh(xc.meshDir.I)

## Constraints.
bearingXAxis= xc.Vector(l1.getIVector) # axis along the beam.
bearingYAxis= xc.Vector([-bearingXAxis[1], bearingXAxis[0], bearingXAxis[2]])
orientation= [bearingXAxis, bearingYAxis]
bearingElements= list()
n0= pt0.getNode()
newNode, newElement= bearing.putAsSupport(modelSpace= modelSpace, iNod= n0.tag, orientation= orientation)
bearingElements.append(newElement)
n3= pt3.getNode()
newNode, newElement= bearing.putAsSupport(modelSpace= modelSpace, iNod= n3.tag, orientation= orientation)
bearingElements.append(newElement)

# Loads.
g1= modelSpace.newLoadPattern(name= 'G1', setCurrent= True) # self weight.
accel= xc.Vector([0,0,g])
for e in l1.elements:
    e.createInertiaLoad(accel)
g2= modelSpace.newLoadPattern(name= 'G2', setCurrent= True) # dead load. 
for e in l1.elements:
    e.vector3dUniformLoadGlobal(xc.Vector([0,0,-5e3]))

q1= modelSpace.newLoadPattern(name= 'Q1', setCurrent= True) # traffic load. 
for e in l1.elements:
    e.vector3dUniformLoadGlobal(xc.Vector([0,0,-10e3]))
q2= modelSpace.newLoadPattern(name= 'Q2', setCurrent= True) # wind.
windLoad= 1e3*xc.Vector(l1.getJVector)
for e in l1.elements:
    e.vector3dUniformLoadGlobal(windLoad)
q3a= modelSpace.newLoadPattern(name= 'Q3a', setCurrent= True) # thermal expansion.
thermalStrain= 1e-5*35
thermalMembers= modelSpace.defSet('thermalMembers')
for e in l1.elements:
    thermalMembers.elements.append(e)
loads.isotropic_strain_load_on_set(loadPattern= q3a, xcSet= thermalMembers, strain= thermalStrain)
q3b= modelSpace.newLoadPattern(name= 'Q3b', setCurrent= True) # thermal contraction.
thermalStrain= -1e-5*18
loads.isotropic_strain_load_on_set(loadPattern= q3a, xcSet= thermalMembers, strain= thermalStrain)
a1= modelSpace.newLoadPattern(name= 'a1', setCurrent= True) # accidental load. 
for e in l1.elements:
    e.vector3dUniformLoadGlobal(xc.Vector([0,0,-15e3]))

# Read the load combinations.
## Create a container for load combinations.
combContainer= combs.CombContainer()
## Read load combinations from file.
pth= os.path.dirname(inspect.getfile(inspect.currentframe()))
loadCombinationsFile= pth+'/../../../aux/reference_files/combinations_for_test_elastomeric_bearing_checking.json'
combContainer.readFromJSON(inputFileName= loadCombinationsFile)

# Extract combinations to check the bearings against.
bearingPermLoadCombinations= dict() # quasi permanent combinations.
for situation in [combContainer.SLS.qp]:
    for combName in situation:
        combExpr= situation[combName].expr
        bearingPermLoadCombinations[combName]= combExpr
bearingLoadCombinations= dict() # quasi permanent and frequent combinations.
for situation in [combContainer.SLS.qp, combContainer.SLS.freq]:
    for combName in situation:
        combExpr= situation[combName].expr
        bearingLoadCombinations[combName]= combExpr

# Compute permanent loads on bearings:

bearing_results= en_ls.compute_results_on_bearings(modelSpace= modelSpace, bearingElements= bearingElements, permLoadCombinations= bearingPermLoadCombinations, varLoadCombinations= bearingLoadCombinations)

check_results= en_ls.check_results_en_1337_3(analysisResults= bearing_results, bearingElements= bearingElements, bearingMatDict= {'T0':bearing})

efficiencies= list()
for k in check_results:
    results= check_results[k]
    for kk in results:
        chk_results= results[kk]
        tmp= list()
        for eTag in chk_results:
            e_results= chk_results[eTag]
            for var in e_results:
                if(var.endswith('_cf')):
                    tmp.append(e_results[var])
        efficiencies.append(statistics.mean(tmp))

refValues= [13.261990813230685, 0.013585942457707667, 0.14103068333596597, 0.6666666666666666, 0.007757881739994591, 14.036253412958153, 0.03448812294214598]

# Check that the results are always the same.
err= 0.0
for v, vRef in zip(efficiencies, refValues):
    err+= (v-vRef)**2

err= math.sqrt(err)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayBlocks()
# # oh.displayFEMesh()
# # oh.displayDispRot(itemToDisp='uZ', defFScale= 100.0)
# oh.displayLocalAxes()
# # oh.displayLoads()
# # oh.displayIntForcDiag('N')
# # oh.displayIntForcDiag('M')
# # oh.displayLocalAxes()
