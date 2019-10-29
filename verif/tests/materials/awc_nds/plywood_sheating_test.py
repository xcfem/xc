# -*- coding: utf-8 -*-
''' Test based on Plywood Roof Design Example Calculations Using ASD
    from document Design Capacities for Structural Plywood. Allowable 
    Stress Design (ASD) of TECO company (TECO_design_capacities.pdf)
'''
from __future__ import division
from __future__ import print_function

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials.awc_nds import AWCNDS_materials
from materials import typical_materials

# Loads
from actions import load_cases as lcm
from actions import combinations as combs

# Problem type
sheathingBeam= xc.FEProblem()
sheathingBeam.title= 'Sheating design'
preprocessor= sheathingBeam.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Materials
# Mechanical properties taken from:
# http://www.pfsteco.com/techtips/pdf/tt_plywooddesigncapacities
structuralPanelGeom= AWCNDS_materials.PlywoodPanel('TECO 32/16', b=1.0, h= 0.594*0.0254, shear_constant= 3)
plywood= typical_materials.MaterialData(name='TECO 32/16',E=4161501119.15,nu=0.2,rho=500)
section= structuralPanelGeom.defElasticShearSection2d(preprocessor,plywood)
EI= section.sectionProperties.EI()
EIRef= 126500*4.44822*(0.0254)**2/0.3048

ratio0= abs((EI-EIRef)/EIRef)

thickness= structuralPanelGeom.h

deflectionSpan= (32-1.5+0.25)*0.0254
internalForcesSpan= 32*0.0254 # 32
span= internalForcesSpan 
pointHandler= preprocessor.getMultiBlockTopology.getPoints
pt1= pointHandler.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= pointHandler.newPntFromPos3d(geom.Pos3d(span,0.0,0.0))
pt3= pointHandler.newPntFromPos3d(geom.Pos3d(2.0*span,0.0,0.0))
pt4= pointHandler.newPntFromPos3d(geom.Pos3d(3.0*span,0.0,0.0))

lines= preprocessor.getMultiBlockTopology.getLines
l1= lines.newLine(pt1.tag,pt2.tag)
l2= lines.newLine(pt2.tag,pt3.tag)
l3= lines.newLine(pt3.tag,pt4.tag)

infSet= preprocessor.getSets.defSet("inf")
infSet.getLines.append(l1)
infSet.getLines.append(l2)
infSet.getLines.append(l3)

# Mesh
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
nodes.newSeedNode()
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf2d("lin")
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= section.name
seedElemHandler.defaultTransformation= "lin"
elem= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))

xcTotalSet= preprocessor.getSets.getSet("total")
mesh= infSet.genMesh(xc.meshDir.I)
infSet.fillDownwards()

# Constraints
for p in [pt1,pt2,pt3,pt4]:
    n= p.getNode()
    modelSpace.fixNode00F(n.tag)

for n in infSet.getNodes:
    pos= n.getInitialPos3d
    nInf= infSet.getNearestNode(pos)
    modelSpace.constraints.newEqualDOF(nInf.tag,n.tag,xc.ID([1]))

# Actions
bendingStiffnessLoad= 32.88108719285194*47.88026 # Live load N/m2
bendingStrengthLoad= 120*407*1.15/32**2*47.88026 # Bending strength load
shearStrengthLoad= 20.0*198.0*1.15/(32-1.5)*47.88026 # Shear strength load

load= bendingStrengthLoad
loadCaseManager= lcm.LoadCaseManager(preprocessor)
loadCaseNames= ['load']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)

# load.
cLC= loadCaseManager.setCurrentLoadCase('load')
for e in infSet.getElements:
    e.vector2dUniformLoadGlobal(xc.Vector([0.0,-load]))

#We add the load case to domain.
preprocessor.getLoadHandler.addToDomain("load")

# Solution
# Linear static analysis.
analisis= predefined_solutions.simple_static_linear(sheathingBeam)
result= analisis.analyze(1)

uYMax= -1e6
for n in infSet.getNodes:
    uY= -n.getDisp[1]
    uYMax= max(uY,uYMax)

uYMax*= bendingStiffnessLoad/bendingStrengthLoad
r= span/uYMax

DeltaLL= 12*bendingStiffnessLoad*span**4/1743.0/section.sectionProperties.EI()
r= span/DeltaLL
ratio1= abs(uYMax-DeltaLL)/DeltaLL


# Bending and shear strength
C_D= 1.15 # Snow load duration factor.
Ft= 2800*4.44822/0.3048/section.sectionProperties.A
Fb= C_D*407.0/structuralPanelGeom.Wzel()*4.44822*0.0254/0.3048
Fv= C_D*198*4.44822/0.3048/structuralPanelGeom.h

sgMax= -1e6
tauMax= -1e6
for e in infSet.getElements:
    e.getResistingForce()
    m1= e.getM1
    sg1= abs(m1/section.sectionProperties.I*structuralPanelGeom.h/2)
    tau1= abs(e.getV1/section.sectionProperties.A)
    sgMax= max(sgMax,sg1)
    tauMax= max(tauMax,tau1)
    m2= e.getM2
    sg2= abs(m2/section.sectionProperties.I*structuralPanelGeom.h/2)
    tau2= abs(e.getV2/section.sectionProperties.A)
    sgMax= max(sgMax,sg2)
    tauMax= max(tauMax,tau2)

ratio2= abs(Fb-sgMax)/Fb
tauMax*= shearStrengthLoad/bendingStrengthLoad
ratio3= abs(Fv-tauMax)/Fv

'''
print('uYMax= ', uYMax*1e3, ' mm (L/'+str(r)+'), DeltaLL= ', DeltaLL*1e3, ' mm (L/'+str(r)+') ratio1= ', ratio1)
print('Fb= ', Fb/1e6,' MPa, sgMax= ', sgMax/1e6,' MPa, ratio2= ', ratio2)
print('Fv= ', Fv/1e6,' MPa, tauMax= ', tauMax/1e6,' MPa, ratio3= ', ratio3)
'''

import os
fname= os.path.basename(__file__)
if (ratio0<2e-12) and (ratio1<0.02) and (ratio2<1e-6) and(ratio3<0.06):
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")
