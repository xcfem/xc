# -*- coding: utf-8 -*-
''' Computation of the interaction diagram. Home made test. '''
from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc

from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

width= 0.2 # Section width expressed in meters.
depth= 0.4 # Section width expressed in meters.
cover= 0.05 # Concrete cover expressed in meters.
diam= 16e-3 # Bar diameter expressed in meters.
areaFi16= 2.01e-4 # Rebar area expressed in square meters.


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Define materials
concr= EHE_materials.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
concrMatTag25= concr.defDiagD(preprocessor)
Ec= concr.getDiagD(preprocessor).getTangent
tagB500S= EHE_materials.B500S.defDiagD(preprocessor)
Es= EHE_materials.B500S.getDiagD(preprocessor).getTangent

geomSecHA= preprocessor.getMaterialHandler.newSectionGeometry("geomSecHA")
regions= geomSecHA.getRegions
concrete= regions.newQuadRegion(EHE_materials.HA25.nmbDiagD)
concrete.nDivIJ= 10
concrete.nDivJK= 10
concrete.pMin= geom.Pos2d(-depth/2.0,-width/2.0)
concrete.pMax= geom.Pos2d(depth/2.0,width/2.0)
reinforcement= geomSecHA.getReinfLayers
reinforcementInf= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
reinforcementInf.numReinfBars= 2
reinforcementInf.barArea= areaFi16
reinforcementInf.p1= geom.Pos2d(cover-depth/2.0,width/2.0-cover) # bottom layer.
reinforcementInf.p2= geom.Pos2d(cover-depth/2.0,cover-width/2.0)
reinforcementSup= reinforcement.newStraightReinfLayer(EHE_materials.B500S.nmbDiagD)
reinforcementSup.numReinfBars= 2
reinforcementSup.barArea= areaFi16
reinforcementSup.p1= geom.Pos2d(depth/2.0-cover,width/2.0-cover) # top layer.
reinforcementSup.p2= geom.Pos2d(depth/2.0-cover,cover-width/2.0)

materialHandler= preprocessor.getMaterialHandler
secHA= materialHandler.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecHA")
secHA.setupFibers()
fibras= secHA.getFibers()

param= xc.InteractionDiagramParameters()
param.concreteTag= EHE_materials.HA25.matTagD
param.reinforcementTag= EHE_materials.B500S.matTagD
diagIntsecHA= materialHandler.calcInteractionDiagram("secHA",param)

diagIntsecHA.writeTo("/tmp/interaction_diagram_test_01.dat") # Used in interaction_diagram_test_03
ratio1= diagIntsecHA.getCapacityFactor(geom.Pos3d(352877,0,0))-1
ratio2= diagIntsecHA.getCapacityFactor(geom.Pos3d(352877/2.0,0,0))-0.5
ratio3= diagIntsecHA.getCapacityFactor(geom.Pos3d(-574457,41505.4,2.00089e-11))-1.0
ratio4= diagIntsecHA.getCapacityFactor(geom.Pos3d(-978599,-10679.4,62804.3))-1.0

''' 
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5) & (abs(ratio4)<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
