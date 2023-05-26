# -*- coding: utf-8 -*-
''' Verification test of a rectangular 2D fiber section with an
    elasto plastic material model.
    written from "Nociones de cálculo plástico." Carlos Benito Hernández.
    page 26 and following.
    url={https://books.google.es/books?id=v7bbMwEACAAJ},
'''
from __future__ import print_function

from materials.sections import section_properties
from misc import scc2d_testing_bench
from solution import predefined_solutions


from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Rectangular cross-section definition
b= 10 # Cross section width  [cm]
h= 20 # Cross section depth [cm]
scc10x20= section_properties.RectangularSection('scc10x20',b,h)
scc10x20.nDivIJ= 1 # 2D discretization
scc10x20.nDivJK= 32 # number of cells in JK direction

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
sys.path.append(pth+"/../../../../../../aux/")
import fiber_section_test_macros

fy= 2600 # Yield stress of the material expressed in kp/cm2.
E= 2.1e6 # Young’s modulus of the material (kp/cm2).

feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
# Materials definition
epp= typical_materials.defElasticPPMaterial(preprocessor, "epp",E,fy,-fy) # elastic perfectly-plastic uniaxial material
respVy= typical_materials.defElasticMaterial(preprocessor, "respVy",1e6) # Shear response in y direction.
# Section geometry
# creation
geomRectang= preprocessor.getMaterialHandler.newSectionGeometry("geomRectang")
# generation of a quadrilateral region of the scc10x20 sizes and number of
# divisions made of material nmbMat
reg= scc10x20.getRegion(gm=geomRectang, nmbMat= epp.name, twoDimensionalMember= True)
sa= preprocessor.getMaterialHandler.newMaterial("fiberSectionShear2d","sa")

fiberSectionRepr= sa.getFiberSectionRepr() # Create fiber representation of the section.
fiberSectionRepr.setGeomNamed(geomRectang.name) # Assign the geometry.
sa.setupFibers()
fiber_section_test_macros.extractFiberSectionProperties(sa, scc10x20, fy)
sa.setRespVyByName(respVy.name)

zlElement, nodA, nodB= scc2d_testing_bench.sectionModel(preprocessor, sa.name)

# Constraints
modelSpace= predefined_spaces.getStructuralMechanics2DSpace(preprocessor)
modelSpace.fixNode000(nodA.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')

loadVy= 2e4
loadMz= 0.999*scc10x20.getPlasticMomentZ(fy)
lp0.newNodalLoad(nodB.tag,xc.Vector([0,loadVy,loadMz]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_newton_raphson(feProblem)
analOk= analysis.analyze(1)
if(analOk!=0):
    print("Error!; failed to converge.")
    exit()

nodes= preprocessor.getNodeHandler
nodes.calculateNodalReactions(True,1e-7)
n1= nodA
RVy= n1.getReaction[1] 
RMz= n1.getReaction[2] 

scc= zlElement.getSection()
esfVy= scc.getStressResultantComponent("Vy")
esfMz= scc.getStressResultantComponent("Mz")



referenceCenterOfMassY= 0.0
referenceCenterOfMassZ= 0.0
ratio1= (sumAreas-scc10x20.A())/scc10x20.A()
ratio2= centerOfMassY-referenceCenterOfMassY
ratio3= (I1-scc10x20.Iz())/scc10x20.Iz()
ratio4= (i1-scc10x20.iz())/scc10x20.iz()
ratio5= (Me1-scc10x20.getYieldMomentZ(fy))/scc10x20.getYieldMomentZ(fy)
ratio6= (SzPosG-scc10x20.getPlasticSectionModulusZ())/scc10x20.getPlasticSectionModulusZ()
ratio7= (RMz+loadMz)/loadMz
ratio8= (esfMz-loadMz)/loadMz

ratio9= (RVy+loadVy)/loadVy
ratio10= (esfVy-loadVy)/loadVy

'''
print("RVy= ",(RVy))
print("esfVy= ",(esfVy))
print("loadVy= ",(loadVy))

print("RMz= ",(RMz))
print("esfMz= ",(esfMz))
print("loadMz= ",(loadMz))
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('ratio5= ', ratio5)
print('ratio6= ', ratio6)
print('ratio7= ', ratio7)
print('ratio8= ', ratio8)
print('ratio9= ', ratio9)
print('ratio10= ', ratio10)
   '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-5) & (abs(ratio3)<5e-3) & (abs(ratio4)<1e-3) & (abs(ratio5)<5e-3) & (abs(ratio6)<1e-5) & (abs(ratio7)<1e-5) & (abs(ratio8)<1e-5) & (abs(ratio9)<1e-5) & (abs(ratio10)<1e-5) & (analOk == 0.0) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
