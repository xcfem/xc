# -*- coding: utf-8 -*-
# home made test
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Thickness.
dens= 1.33 # Density kg/m2.

q= -2

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,1,0,0)
nodes.newNodeIDXYZ(3,1,1,0)
nodes.newNodeIDXYZ(4,0,1,0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elements= preprocessor.getElementHandler
elements.defaultMaterial= "memb1"
elem= elements.newElement("ShellMITC4",xc.ID([1,2,3,4]))


# Constraints
constraints= preprocessor.getBoundaryCondHandler

modelSpace.fixNode000_FFF(1)
modelSpace.fixNode000_FFF(2)
modelSpace.fixNode000_FFF(3)
modelSpace.fixNode000_FFF(4)

# Loads definition
cargas= preprocessor.getLoadHandler

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
eleLoad= lp0.newElementalLoad("shell_uniform_load")
eleLoad.elementTags= xc.ID([0]) 
eleLoad.transComponent= q
#We add the load case to domain.
casos.addToDomain("0")

import os
os.system("rm -f /tmp/test07.db")
db= feProblem.newDatabase("SQLite","/tmp/test07.db")
db.save(100)
feProblem.clearAll()
db.restore(100)

# Solution
analisis= predefined_solutions.simple_static_linear(feProblem)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True,1e-7)

RN1= nodes.getNode(1).getReaction[2] 
RN2= nodes.getNode(2).getReaction[2] 
RN3= nodes.getNode(3).getReaction[2] 
RN4= nodes.getNode(4).getReaction[2] 



ratio1= (abs((RN1+(q/4.0))/(q/4.0)))
ratio2= (abs((RN2+(q/4.0))/(q/4.0)))
ratio3= (abs((RN3+(q/4.0))/(q/4.0)))
ratio4= (abs((RN4+(q/4.0))/(q/4.0)))


''' 
elements= preprocessor.getElementHandler
        \for_each

            print "G3= ",getCoordTransf.getG3Vector


nodes= preprocessor.getNodeHandler

             \node[1]{print "reac= ",reac} 
             \node[2]{print "reac= ",reac} 
             \node[3]{print "reac= ",reac} 
             \node[4]{print "reac= ",reac} 
'''



'''
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
'''


from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
os.system("rm -f /tmp/test07.db") # Your garbage you clean it
