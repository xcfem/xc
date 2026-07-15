# -*- coding: utf-8 -*-
from __future__ import print_function


# Test for checking the interaction diagram

import geom
import xc

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

nmbHorm= "HA25"
from materials.ehe import EHE_materials

width= 0.5  # Cross-section width [m]
depth= 0.75 # Cross-section depth [m]
cover= 0.06 # Cover [m]
diam= 20e-3 # Diameter of rebars [m]
areaFi20= 3.14e-4 # Rebars cross-section area [m2]


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concr= EHE_materials.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
concrMatTag25= concr.defDiagD(preprocessor)
Ec= concr.getDiagD(preprocessor).getTangent
tagB500S= EHE_materials.B500S.defDiagD(preprocessor)
Es= EHE_materials.B500S.getDiagD(preprocessor).getTangent

# Section geometry
# setting up
geomSecHA= preprocessor.getMaterialHandler.newSectionGeometry("geomSecHA")
#filling with regions
regions= geomSecHA.getRegions

#generation of a quadrilateral region with the specified sizes and number of
#divisions for the cells (fibers) generation
concrete= regions.newQuadRegion(EHE_materials.HA25.getDDiagName())  #name of the region: EHE_materials.HA25.getDDiagName()
concrete.nDivIJ= 10
concrete.nDivJK= 10
concrete.pMin= geom.Pos2d(-depth/2.0,-width/2.0)
concrete.pMax= geom.Pos2d(depth/2.0,width/2.0)

#generation of reinforcement layers 
reinforcement= geomSecHA.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.getDDiagName())
reinforcementInf.numReinfBars= 6
reinforcementInf.barArea= areaFi20
reinforcementInf.setP1P2(geom.Pos2d(cover-depth/2.0,width/2.0-cover), geom.Pos2d(cover-depth/2.0,cover-width/2.0)) # bottom layer.

reinforcementPielInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.getDDiagName())
reinforcementPielInf.numReinfBars= 2
reinforcementPielInf.barArea= areaFi20
y= (depth-2*cover)/3.0/2.0
reinforcementPielInf.setP1P2(geom.Pos2d(-y,width/2-cover), geom.Pos2d(-y,cover-width/2)) # Bottom skin reinforcement.

reinforcementPielSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.getDDiagName())
reinforcementPielSup.numReinfBars= 2
reinforcementPielSup.barArea= areaFi20
y= (depth-2*cover)/3.0/2.0
reinforcementPielSup.setP1P2(geom.Pos2d(y,width/2-cover), geom.Pos2d(y,cover-width/2)) # Top skin reinforcement.

reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.getDDiagName())
reinforcementSup.numReinfBars= 6
reinforcementSup.barArea= areaFi20
reinforcementSup.setP1P2(geom.Pos2d(depth/2.0-cover,width/2.0-cover), geom.Pos2d(depth/2.0-cover,cover-width/2.0)) # top layer.

# Create fiber section.
secHA= geomSecHA.getFiberSection3d('secHA')

param= xc.InteractionDiagramParameters()
param.concreteTag= EHE_materials.HA25.getMatTagD()
param.reinforcementTag= EHE_materials.B500S.getMatTagD()
materialHandler= preprocessor.getMaterialHandler
diagIntsecHA= materialHandler.calcInteractionDiagram(secHA.name, param)

diagIntsecHA.writeTo("/tmp/interaction_diagram_test_02.dat")  # Used in test_interaction_diagram04
FCs= []
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(2185.5e3,0,0)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(1595.8e3,0,186.5e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(1006.2e3,0,380.2e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(416.6e3,0,565.9e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-173e3,0,739.3e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-762.6e3,0,870.8e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-1352.2e3,0,962.2e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-1941.9e3,0,1004.1e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-2531.5e3,0,1021.9e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-3121.1e3,0,950.7e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-3710.7e3,0,872.1e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-4300.3e3,0,782.8e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-4889.9e3,0,678.1e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-5479.6e3,0,551.8e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-6069.2e3,0,394.6e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-6658.8e3,0,230.7e3)))
FCs.append(diagIntsecHA.getCapacityFactor(geom.Pos3d(-7248.4e3,0,0.0)))

szFCs= float(len(FCs))
sumQuad= 0
for f in FCs:
  sumQuad+= f**2


ratio1= abs(sumQuad-szFCs)/szFCs

''' 
print("FCs= ",FCs)
print("sumQuad= ",sumQuad)
print("ratio1= ",ratio1)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<0.02):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Section output
# import matplotlib.pyplot as plt
# from materials.sections.fiber_section import plot_fiber_section
# fig = plt.figure()
# ax= fig.add_subplot(111)
# preprocessor.getMaterialHandler
# plot_fiber_section.mplot_section_geometry(ax, sectionGeometry= geomSecHA)
# plt.show()
