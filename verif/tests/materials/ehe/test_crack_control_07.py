# -*- coding: utf-8 -*-
''' Test for checking the shear-strength verificacion of a circular
    reinforced concrete section.

    The results seem reasonable but no tests have been found with which to compare them.
'''

from __future__ import division
from __future__ import print_function

from misc_utils import log_messages as lmsg
import xc_base
import geom
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions
from model import predefined_spaces
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section

from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking

from materials import typical_materials

import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"

# Materials definition
concr= EHE_materials.HA40
steel= EHE_materials.B500S

# Section geometry
diameter= 350e-3 # Cross-section diameter [m]
cover= 0.025 # Cover [m]
section= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)
section.nDivIJ= 10

# Longitudinal reinforcement
rebarDiam= 16e-3
rebarArea= EHE_materials.Fi16
numOfRebars= 12


# # Shear reinforcement
# shearReinfArea= EHE_materials.Fi6
# shearReinfDiam= 6e-3
# numRamas= 2
# shearReinf= def_simple_RC_section.ShearReinforcement(familyName= "sh",nShReinfBranches= numRamas, areaShReinfBranch= shearReinfArea, shReinfSpacing= 0.2, angAlphaShReinf= math.pi/2.0,angThetaConcrStruts= math.pi/4.0)

nCover= cover

mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= numOfRebars, width= math.pi*(diameter-2*cover), nominalCover= nCover)])


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
materialHandler= preprocessor.getMaterialHandler

# Section geometry
section.mainReinf= mainReinf

section.defRCSection(preprocessor,matDiagType= 'k')
scc3d_testing_bench.sectionModel(preprocessor, section.sectionName)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)

# Loads definition
Nd= -550e3/1.5 # Axial force when checking crack width.
Myd= 190e3/1.5 # # Y bending moment when checking crack width.
Mzd= 0.0 # Z bending moment value when checking shear.
# Vd= 148.7e3 # Shear value.

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')

lp0.newNodalLoad(2,xc.Vector([Nd,0,0,0,Myd,Mzd]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem, mxNumIter= 10)
analOk= analysis.analyze(10)

import os
fname= os.path.basename(__file__)
if(analOk<0):
    lmsg.error(fname+' ERROR. Failed to converge.')
    quit()

secHAParamsFis= EHE_limit_state_checking.CrackControl('SLS_crack')

elements= preprocessor.getElementHandler
scc= elements.getElement(1).getSection()
secHAParamsFis.computeWk(scc,concr.matTagK,steel.matTagK,concr.fctm())
ratio1= ((secHAParamsFis.Wk-0.383585469616e-3)/0.383585469616e-3)


'''
print("Wk= ", secHAParamsFis.Wk*1e3,"mm")
print("ratio1= ", ratio1)
secHAParamsFis.printParams()
'''

if (abs(ratio1)<1e-10):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

# Graphic stuff
# fibers= scc.getFibers()
# xC= list()
# yC= list()
# zC= list()
# xS= list()
# yS= list()
# zS= list()
# U= list()
# V= list()
# W= list()
# for f in fibers:
#     matName= f.getMaterial().name
#     value= f.getStress()/1e6
#     if('HA40' in matName): # Concrete
#         xC.append(f.getLocY())
#         yC.append(f.getLocZ())
#         zC.append(value)
#     else: # steel reinforcement
#         xS.append(f.getLocY())
#         yS.append(f.getLocZ())
#         zS.append(0.0)
#         U.append(0.0)
#         V.append(0.0)
#         W.append(value/10)
#         print('steel value: ', matName, value)
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
# fig = plt.figure()
# ax = fig.gca(projection='3d')
# surf= ax.plot_trisurf(xC, yC, zC, linewidth=0.2, antialiased=False)
# vectors= ax.quiver(xS, yS, zS, U, V, W, arrow_length_ratio = 0.001, color= 'r')
# plt.show()
