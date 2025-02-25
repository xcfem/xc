# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials

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
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(1,0,0)
n3= nodes.newNodeXYZ(1,1,0)
n4= nodes.newNodeXYZ(0,1,0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))


# Constraints
constraints= preprocessor.getBoundaryCondHandler

modelSpace.fixNode000_FFF(n1.tag)
modelSpace.fixNode000_FFF(n2.tag)
modelSpace.fixNode000_FFF(n3.tag)
modelSpace.fixNode000_FFF(n4.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("shell_uniform_load")
eleLoad.elementTags= xc.ID([elem.tag]) 
eleLoad.transComponent= q
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

import os
os.system("rm -f /tmp/test07.db")
db= feProblem.newDatabase("SQLite","/tmp/test07.db")
db.save(100)
feProblem.clearAll()
db.restore(100)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

RN1= n1.getReaction[2] 
RN2= n2.getReaction[2] 
RN3= n3.getReaction[2] 
RN4= n4.getReaction[2] 



ratio1= (abs((RN1+(q/4.0))/(q/4.0)))
ratio2= (abs((RN2+(q/4.0))/(q/4.0)))
ratio3= (abs((RN3+(q/4.0))/(q/4.0)))
ratio4= (abs((RN4+(q/4.0))/(q/4.0)))


''' 
elements= preprocessor.getElementHandler
        \for_each

            print("G3= ",getCoordTransf.getG3Vector)


nodes= preprocessor.getNodeHandler

             \node[1]{print("reac= ",reac} )
             \node[2]{print("reac= ",reac} )
             \node[3]{print("reac= ",reac} )
             \node[4]{print("reac= ",reac} )
'''



'''
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
print("ratio4= ",ratio4)
'''


from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
os.system("rm -f /tmp/test07.db") # Your garbage you clean it
