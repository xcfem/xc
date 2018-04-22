# -*- coding: utf-8 -*-
''' Reinforced concrete section verification test.
   results are compared with those of the prontuario.
   informático del hormigón estructural (Cátedra de hormigón de la ETSICCP-IECA
   UPM). '''


import xc_base
import geom
import xc
from misc import scc3d_testing_bench
from solution import predefined_solutions

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

areaFi16= 2.01e-4 # Rebar area expressed in square meters.
brazo= 0.5 # Rebar lever arm with respect to the section axis.


from materials.ehe import EHE_materials
from model import predefined_spaces

gammas= 1.15 # Partial safety factor for steel.


MzDato= 10e3
NDato= 0.0

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
tag= EHE_materials.B500S.defDiagD(preprocessor)
dgDB500S= EHE_materials.B500S.getDiagD(preprocessor)

import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print "pth= ", pth
execfile(pth+"/geomSeccBarras.py")

secBarras= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","secBarras")
fiberSectionRepr= secBarras.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomSecBarras")
secBarras.setupFibers()

scc3d_testing_bench.sectionModel(preprocessor, "secBarras")

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics3DSpace(preprocessor)
modelSpace.fixNode000_000(1)
modelSpace.fixNodeF00_00F(2)

# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,0,0,0,0,MzDato]))

#We add the load case to domain.
casos.addToDomain("0")


# Solution procedure
analisis= predefined_solutions.simple_newton_raphson(feProblem)
analOk= analisis.analyze(1)

nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)

RN= nodes.getNode(1).getReaction[0] 
RM= nodes.getNode(1).getReaction[5] 
RN2= nodes.getNode(2).getReaction[0] 

elements= preprocessor.getElementHandler
ele1= elements.getElement(1)
scc= ele1.getSection()
esfN= scc.getFibers().getResultant()
esfMy= scc.getFibers().getMy(0.0)
esfMz= scc.getFibers().getMz(0.0)
defMz= scc.getSectionDeformationByName("defMz")
defN= scc.getSectionDeformationByName("defN")


ratio1= (RM+MzDato)/MzDato
ratio2= (esfMz-MzDato)/MzDato
ratio3= (esfN-NDato)

''' 
print "RN= ",(RN/1e3)," kN \n"
print "RN2= ",(RN2/1e3)," kN \n"
print "N= ",(esfN/1e3)," kN \n"
print "My= ",(esfMy/1e3)," kN m\n"
print "Mz= ",(esfMz/1e3)," kN m\n"
print "defMz= ",(defMz)
print "defN= ",(defN)
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
'''

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-10) & (abs(ratio2)<1e-10) & (abs(ratio3)<1e-10) & (abs(RN)<1e-10) & (abs(RN2)<1e-10) & (abs(esfMy)<1e-10) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
