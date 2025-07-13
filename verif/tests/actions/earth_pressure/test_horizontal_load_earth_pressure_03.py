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
from geotechnics import horizontal_surcharge as hs
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
# |___|\__/_||_|   \__||_||_|      |_|   |_|  \___|/__//__/ \_._||_|  \___|
#
# =============================================================================
# =============================================================================
# Define load case.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['earth_pressure']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
cLC= loadCaseManager.setCurrentLoadCase('earth_pressure')

# Define concentrated load
## Position of the concentrated load.
pos= geom.Pos3d(wallLength/2.0, wallHeight/2.0, wallHeight) # Centroid.
### Corners:
posA= pos+geom.Vector3d(-0.5,-0.5,0.0)
posB= pos+geom.Vector3d(-0.5, 0.5,0.0)
posC= pos+geom.Vector3d( 0.5, 0.5,0.0)
posD= pos+geom.Vector3d( 0.5,-0.5,0.0)
# Horizontal load.
H= geom.Vector3d(-5e3,-10e3,0)
loadedArea= hs.HorizontalLoadedAreaOnBackfill3D(contour= [posA, posB, posC, posD], H= H)

# Compute loads on elements.
phi= math.radians(30) # effective friction angle of soil.
delta= math.radians(10.0) # friction angle between the soil and the element material.
loadedAreaRatio= loadedArea.appendLoadToCurrentLoadPattern(elements= s.elements, phi= phi, delta= delta)

# Loaded area ratio is the quotient between the actually loaded portion of the
# retaining structure and the area of the intersection of the "prism" that
# carries the load towards the retaining structure and the plane containing the
# retaining structure 

modelSpace.addLoadCaseToDomain('earth_pressure')

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
action= xc.Vector([loadedAreaRatio*H.y*math.tan(delta), loadedAreaRatio*H.y, loadedAreaRatio*H.z, -loadedAreaRatio*leverArm*H.y, 0.0, 0.0])
ratio1= (reaction+action).Norm()/action.Norm()
ratio2= math.sqrt(reaction[4]**2+reaction[5]**2)
ratio3= abs(leverArm-1.0099054769954645)/1.0099054769954645

'''
print('loaded area ratio: ', loadedAreaRatio)
print('action: ', action/1e3, 'kN, m')
print('reaction: ', reaction/1e3, 'kN, m')
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('lever arm: ', leverArm, 'm')
print('ratio3= ', ratio3)
'''

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-2 and abs(ratio2)<1e-6 and abs(ratio3)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
