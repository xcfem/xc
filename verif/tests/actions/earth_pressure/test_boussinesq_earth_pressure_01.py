# -*- coding: utf-8 -*-
''' Verify boussinesq earth pressure computation routines. Based on a 
    handwritten design example.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases
from geotechnics import boussinesq
from misc_utils import log_messages as lmsg

foot2Meter= .3048
psf2Pa= 47.8803

# =============================================================================
# =============================================================================
#  ___  _        _  _                    _                         _   
# | __|(_) _ _  (_)| |_  ___        ___ | | ___  _ __   ___  _ _  | |_ 
# | _| | || ' \ | ||  _|/ -_)      / -_)| |/ -_)| '  \ / -_)| ' \ |  _|
# |_|  |_||_||_||_| \__|\___|      \___||_|\___||_|_|_|\___||_||_| \__|
#                 _       _ 
#  _ __   ___  __| | ___ | |
# | '  \ / _ \/ _` |/ -_)| |
# |_|_|_|\___/\__/_|\___||_|
#
# =============================================================================
# =============================================================================
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
concrete= EC2_materials.C30
steel= EC2_materials.S500B
## Geometry
h= 0.40
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='WallSection', width= 1.0, depth= h, concrType= concrete, reinfSteelType= steel)
dummySection= rcSection.defElasticMembranePlateSection(preprocessor) # Elastic membrane plate section.

# Problem geometry.
wallHeight= 8*foot2Meter
wallLength= 40*foot2Meter
## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(wallLength,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(wallLength,0.0,wallHeight))
pt4= points.newPoint(geom.Pos3d(0.0,0.0,wallHeight))
## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 30
s.nDivJ= 10

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= rcSection.name
elem= seedElemHandler.newElement("ShellMITC4")
s.genMesh(xc.meshDir.I)

# Constraints.
fixedNodes= list()
for n in s.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.z)<1e-3):
        fixedNodes.append(n)
for n in fixedNodes:
    modelSpace.fixNode000_0FF(n.tag)
    
# Actions
# =============================================================================
# =============================================================================
#  ___             _    _           _ __                                   
# | __| __ _  _ _ | |_ | |_        | '_ \ _ _  ___  ___ ___ _  _  _ _  ___ 
# | _| / _` || '_||  _||   \       | .__/| '_|/ -_)(_-/(_-/| || || '_|/ -_)
# |___|\__/_||_|  \__||_||_|       |_|   |_|  \___|/__//__/ \_._||_|  \___|
#
# =============================================================================
# =============================================================================
# Define load case.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['earth_pressure']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
cLC= loadCaseManager.setCurrentLoadCase('earth_pressure')

# Define concentrated load
pos= geom.Pos3d(6.096,2.286, 2.4384) # Load position.
load= -11.15*1900*psf2Pa # Load value
loadedPoint= boussinesq.ConcentratedLoad(p= pos, Q=load)

# Compute loads on elements.
loadedPoint.appendLoadToCurrentLoadPattern(elements= s.elements, eta= 1.0, delta= math.radians(10.0))

modelSpace.addLoadCaseToDomain(cLC.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error('Can\'t solve.')

reaction= xc.Vector(6*[0.0])
halfXReaction= 0.0 
for n in fixedNodes:
    nReac= n.getReaction
    reaction+= nReac
    xNode= n.getInitialPos3d.x
    if(xNode<=wallLength/2.0):
        halfXReaction+= nReac[0]

leverArm= -reaction[3]/reaction[1]
avgPressureOnWall= reaction[1]/wallLength/wallHeight

ratio1= abs(reaction[0])
ratio2= math.sqrt(reaction[4]**2+reaction[5]**2)
ratio3= abs(avgPressureOnWall-8.845997924980255e3)/8.845997924980255e3
ratio4= abs(leverArm-1.1563959373748223)/1.1563959373748223
ratio5= abs(halfXReaction-7.031238438182229e3)/7.031238438182229e3

'''
print(reaction)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('lever arm: ', leverArm, 'm')
print('ratio4= ', ratio4)
print('average pressure on wall: ', avgPressureOnWall/1e3, 'kPa')
print('ratio3= ', ratio3)
print('halfXReaction= ', halfXReaction/1e3, 'kN')
print('ratio5= ', ratio5)
'''

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6 and abs(ratio4)<1e-6 and abs(ratio5)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
