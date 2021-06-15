# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
'''Test for checking the deflections in a prestressed concrete beam.
Data for the problem and approximate calculation are taken from 
Example 4.1 of the topic 4 of course "Prestressed Concrete Design 
(SAB 4323) by Baderul Hisham Ahmad 
ocw.utm.my

Problem statement:
Determine the midspan deflection of a beam: 
(i) at transfer with an inertial prestress force of 6800kN; 
(ii) under an imposed load of 30 kN/m when the prestress force has been 
reduced to 4500 kN. 
Take self weight of beam = 11.26 kN/m; I =0.06396m4 ; E = 28 x 10^6 kN/m2
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@xcengineering.xyz"

import math
import xc_base
import geom
import xc
from materials import typical_materials as tm
from materials.prestressing import prestressed_concrete as presconc
from model import predefined_spaces
from solution import predefined_solutions
from model.mesh import finit_el_model as fem

#DATA
#Geometry
span=24      #span of the beam [m]
hBeam=1.305406 #height of the cross-section [m]. Parallel to local z-axis 
wBeam= 0.3450267 #width of the cross-section [m]. Parallel to local y-axis
Abeam=hBeam*wBeam   #cross-section area of the beam[m2]
Iybeam=1/12.*hBeam*wBeam**3 #moment of inertia of the beam cross-section [m4]
Izbeam=1/12.*wBeam*hBeam**3 #moment of inertia of the beam cross-section [m4]
deltaTendon=0.26
nDivLines=8      #number of elements in each line

#Material properties
Ec=28e6      #modulus of elasticity of concrete [kPa]
Ep=Ec*1e-3   #modulus of elasticity of prestressing steel (allow big deflection)
nuc=0.2      #coefficient of Poisson of concrete
densc= 2.5   #specific mass of concrete (t/m3)

fy= 1171e3 # Yield stress of the steel expressed in kPa.
Aps=1  #area of tendon cross-section [m2]
#Prestress
fpi=6800/Aps       #initial stress in the tendon [kPa]
fps=4500/Aps       #stress in the tendon in service [kPa]
#Loads
Wsw=Abeam*densc*10   #self-weight uniform load on beam [kN/m]
q_service=30         #service load [kN/m]
#END DATA

#DIRECT CALCULATION
#Short-term deflection due to prestress force at mid-span
a=span/3.
e=0
uZ_prestr=fpi*Aps*span**2/(8*Ec*Izbeam)*(e+deltaTendon-4*deltaTendon*a**2/3/span**2)
#Short-term deflection due to beam self-weight
uZ_Wsw=-5*Wsw*span**4/(384*Ec*Izbeam)
#Short-term deflection under full service load
uZ_prestr_serv=fps*Aps*span**2/(8*Ec*Izbeam)*(e+deltaTendon-4*deltaTendon*a**2/3/span**2)
#Short-term deflection due to  service load
uZ_qserv=-5*q_service*span**4/(384*Ec*Izbeam)
# XC model of the beam
# Problem type
FEcase= xc.FEProblem()
prep=  FEcase.getPreprocessor
points=prep.getMultiBlockTopology.getPoints
lines=prep.getMultiBlockTopology.getLines
sets=prep.getSets 
nodes= prep.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

#Points and lines beam
beamSet=sets.defSet('beamSet')
beamPoints= beamSet.getPoints
beamLines= beamSet.getLines
for i in range(4):
    p=points.newPntFromPos3d(geom.Pos3d(0,i*span/3,0))
    beamPoints.append(p)
    if(i>0):
        l= lines.newLine(beamPoints[i-1].tag,beamPoints[i].tag)
        beamLines.append(l)
#Points and lines tendon
tendonSet=sets.defSet('tendonSet')
tendonPoints= tendonSet.getPoints
tendonLines= tendonSet.getLines
for i in range(1,3):
    p= points.newPntFromPos3d(geom.Pos3d(0,i*span/3,-deltaTendon))
    tendonPoints.append(p)
tendonLines.append(lines.newLine(beamPoints[0].tag,tendonPoints[0].tag))
tendonLines.append(lines.newLine(tendonPoints[0].tag,tendonPoints[1].tag))
tendonLines.append(lines.newLine(tendonPoints[1].tag,beamPoints[3].tag))

#BEAM
#Geometric section
from materials.sections import section_properties as sectpr
geomSectBeam=sectpr.RectangularSection(name='geomSectBeam',b=wBeam,h=hBeam)

# Material definition
concrete=tm.MaterialData(name='concrete',E=Ec,nu=nuc,rho=densc)
beamMat=tm.BeamMaterialData(name= 'beamMat', section=geomSectBeam, material=concrete)
beamMat.setupElasticShear3DSection(prep)

#Meshing
for l in beamSet.getLines:
    l.nDiv=nDivLines
beam_mesh=fem.LinSetToMesh(linSet=beamSet,matSect=beamMat,elemSize=None,vDirLAxZ=xc.Vector([1,0,0]),elemType='ElasticBeam3d',dimElemSpace=3,coordTransfType='linear')
beam_mesh.generateMesh(prep)

#Boundary conditions
modelSpace.fixNode000_FFF(0)
endnode=beamSet.getNodes.getNearestNode(geom.Pos3d(0,span,0))
modelSpace.fixNode000_FFF(endnode.tag)

#TENDON
#Material
prestressingSteel= tm.defCableMaterial(preprocessor=prep, name="prestressingSteel",E=Ep,prestress=fpi,rho=0.0)

#Meshing
for l in tendonSet.getLines:
    l.nDiv=nDivLines
tendon_mesh=fem.LinSetToMesh(linSet=tendonSet,matSect=prestressingSteel,elemSize=None,vDirLAxZ=xc.Vector([1,0,0]),elemType='Truss',dimElemSpace=3,coordTransfType=None)
tendon_mesh.generateMesh(prep)
for e in tendonSet.getElements:
    e.sectionArea=Aps

# Connection between tendon and beam
gluedDOFs= [0,1,2,3,4,5]
for n1,n2 in zip(beamLines[0].nodes,tendonLines[0].nodes):
    modelSpace.constraints.newEqualDOF(n1.tag,n2.tag,xc.ID(gluedDOFs))
for n1,n2 in zip(beamLines[1].nodes,tendonLines[1].nodes):
    modelSpace.constraints.newEqualDOF(n1.tag,n2.tag,xc.ID(gluedDOFs))
for n1,n2 in zip(beamLines[2].nodes,tendonLines[2].nodes):
    modelSpace.constraints.newEqualDOF(n1.tag,n2.tag,xc.ID(gluedDOFs))

# mid-span node
msNod=beamSet.getNodes.getNearestNode(geom.Pos3d(0,span/2.,0))
    
'''
#Plot
from postprocess.xcVtk.FE_model import vtk_FE_graphic
displaySettings= vtk_FE_graphic.DisplaySettingsFE()
displaySettings.displayMesh(xcSets=[beamSet,tendonSet],fName= None,caption='Mesh',nodeSize=0.0010,scaleConstr=0.30)
'''
# Loads definition
cargas= prep.getLoadHandler
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
casos.currentLoadPattern='0'
#We add the load case to domain.
casos.addToDomain('0')

# Phase 1: prestressing of tendon
# Solution procedure
analysis= predefined_solutions.simple_static_linear(FEcase)
analOk= analysis.analyze(1)
uz_ph1=msNod.getDisp[2]

# Phase 2: self weight
loadVector=xc.Vector([0,0,-Wsw])
for e in beamSet.getElements:
    e.vector3dUniformLoadGlobal(loadVector)
analOk= analysis.analyze(1)
uz_ph2=msNod.getDisp[2]

# Phase 3: prestress loss in service
for e in tendonSet.getElements:
    tendonMat= e.getMaterial()
    tendonMat.prestress= fps
    e.update() #Update element state according with the new prestress
analOk= analysis.analyze(1)
uz_ph3=msNod.getDisp[2]

# Phase 4: load in servide
loadVector=xc.Vector([0,0,-q_service])
for e in beamSet.getElements:
    e.vector3dUniformLoadGlobal(loadVector)
analOk= analysis.analyze(1)
uz_ph4=msNod.getDisp[2]

ratio1=uz_ph1-uZ_prestr
ratio2=uz_ph2-(uZ_prestr+uZ_Wsw)
ratio3=uz_ph3-(uZ_prestr_serv+uZ_Wsw)
ratio4=uz_ph4-(uZ_prestr_serv+uZ_Wsw+uZ_qserv)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-4 and abs(ratio2)<1e-4 and abs(ratio3)<1e-4 and abs(ratio4)<1e-4:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

''' 
#PLOT

from postprocess.xcVtk.FE_model import quick_graphics as QGrph
from postprocess.xcVtk import vtk_graphic_base
lcs=QGrph.QuickGraphics(FEcase)
lcs.displayDispRot(itemToDisp='uZ',setToDisplay=beamSet+tendonSet,fConvUnits=1e3,unitDescription='beam [mm]. Phase 1: prestressing of tendon',viewDef= vtk_graphic_base.CameraParameters("XNeg",1),fileName=None,defFScale=2e2)

arg='N'

lcs.displayIntForcDiag(itemToDisp=arg,setToDisplay=beamSet,fConvUnits= 1,scaleFactor=1,unitDescription='[kN,m]',viewDef=vtk_graphic_base.CameraParameters("XNeg",1),fileName=None,defFScale=1)

lcs.displayIntForcDiag(itemToDisp=arg,setToDisplay=tendonSet,fConvUnits= 1,scaleFactor=1,unitDescription='[kN,m]',viewDef=vtk_graphic_base.CameraParameters("ZNeg",1),fileName=None,defFScale=1)
'''


