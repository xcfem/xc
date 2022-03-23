# -*- coding: utf-8 -*-
''' Response spectrum modal analysis test
taken from the publication from Andrés Sáez Pérez: «Estructuras III»
 E.T.S. de Arquitectura de Sevilla (España).'''

from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc

from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

storeyMass= 134.4e3
nodeMassMatrix= xc.Matrix([[storeyMass,0,0],[0,storeyMass,0],[0,0,0]])
Ehorm= 200000*1e5 # Concrete elastic modulus.

Bbaja= 0.45 # Columns size.
Ibaja= 1/12.0*Bbaja**4 # Cross section moment of inertia.
Hbaja= 4 # First floor height.
B1a= 0.40 # Columns size.
I1a= 1/12.0*B1a**4 # Cross section moment of inertia.
H= 3 # Height of the rest of the floors.
B3a= 0.35 # Columns size.
I3a= 1/12.0*B3a**4 # Cross section moment of inertia.


kPlBaja= 20*12*Ehorm*Ibaja/(Hbaja**3)
kPl1a= 20*12*Ehorm*I1a/(H**3)
kPl2a= kPl1a
kPl3a= 20*12*Ehorm*I3a/(H**3)
kPl4a= kPl3a

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

nodes= preprocessor.getNodeHandler
nod0= nodes.newNodeXY(0,0) 
nod0.mass= nodeMassMatrix
nod0.setProp("gdlsCoartados",xc.ID([0,1,2]))
nod1= nodes.newNodeXY(0,4)
nod1.mass= nodeMassMatrix
nod1.setProp("gdlsCoartados",xc.ID([1,2]))
nod2= nodes.newNodeXY(0,4+3)
nod2.mass= nodeMassMatrix
nod2.setProp("gdlsCoartados",xc.ID([1,2]))
nod3= nodes.newNodeXY(0,4+3+3)
nod3.mass= nodeMassMatrix
nod3.setProp("gdlsCoartados",xc.ID([1,2]))
nod4= nodes.newNodeXY(0,4+3+3+3)
nod4.mass= nodeMassMatrix
nod4.setProp("gdlsCoartados",xc.ID([1,2]))
nod5= nodes.newNodeXY(0,4+3+3+3+3)
nod5.mass= nodeMassMatrix
nod5.setProp("gdlsCoartados",xc.ID([1,2]))

setTotal= preprocessor.getSets.getSet("total")
nodes= setTotal.getNodes
for n in nodes:
  n.fix(n.getProp("gdlsCoartados"),xc.Vector([0,0,0]))

# Materials definition
materialHandler= preprocessor.getMaterialHandler
#scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)

sccPlBaja= typical_materials.defElasticSection2d(preprocessor, "sccPlBaja",20*Bbaja*Bbaja,Ehorm,20*Ibaja)
sccPl1a= typical_materials.defElasticSection2d(preprocessor, "sccPl1a",20*B1a*B1a,Ehorm,20*I1a) 
sccPl2a= typical_materials.defElasticSection2d(preprocessor, "sccPl2a",20*B1a*B1a,Ehorm,20*I1a) 
sccPl3a= typical_materials.defElasticSection2d(preprocessor, "sccPl3a",20*B3a*B3a,Ehorm,20*I3a) 
sccPl4a= typical_materials.defElasticSection2d(preprocessor, "sccPl4a",20*B3a*B3a,Ehorm,20*I3a)


# Geometric transformation(s)
lin= modelSpace.newLinearCrdTransf("lin")

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= sccPlBaja.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([nod0.tag, nod1.tag]))
beam2d.h= Bbaja
elements.defaultMaterial= sccPl1a.name 
beam2d= elements.newElement("ElasticBeam2d",xc.ID([nod1.tag, nod2.tag]))
beam2d.h= B1a
elements.defaultMaterial= sccPl2a.name 
beam2d= elements.newElement("ElasticBeam2d",xc.ID([nod2.tag, nod3.tag]))
beam2d.h= B1a
elements.defaultMaterial= sccPl3a.name 
beam2d= elements.newElement("ElasticBeam2d",xc.ID([nod3.tag, nod4.tag]))
beam2d.h= B3a
elements.defaultMaterial= sccPl4a.name 
beam2d= elements.newElement("ElasticBeam2d",xc.ID([nod4.tag, nod5.tag]))
beam2d.h= B3a



# Solution procedure
analysis= predefined_solutions.frequency_analysis(feProblem, systemPrefix= 'sym_band')
analOk= analysis.analyze(5)


periods= analysis.getPeriods()
modos= analysis.getNormalizedEigenvectors()
modalParticipationFactors= analysis.getModalParticipationFactors()
effectiveModalMasses= analysis.getEffectiveModalMasses()
totalMass= analysis.getTotalMass()
distributionFactors= analysis.getDistributionFactors()



targetPeriods= xc.Vector([0.468,0.177,0.105,0.084,0.065])
ratio1= (periods-targetPeriods).Norm()
exempleModes= xc.Matrix([[0.323,-0.764,0.946,0.897,-0.623],[0.521,-0.941,0.378,-0.251,1.000],[0.685,-0.700,-0.672,-0.907,-0.658],[0.891,0.241,-1.000,1.000,0.195],[1.000,1.000,0.849,-0.427,-0.042]])
diff= (modos-exempleModes)
ratio2= diff.Norm()
ratio3= abs(totalMass-5*storeyMass)/5/storeyMass
''' The values of the first three distribution factors values (fist 3 columns)
   were taken from the reference example. The two others (which are not given
in the example) are those obtained from the program (they can always get
wrong because of some error). ''' 
exampleDistribFactors= xc.Matrix([[0.419,0.295,0.148,0.0966714,0.0429946],
                         [0.676,0.363,0.059,-0.0270432,-0.0689994],
                         [0.889,0.27,-0.105,-0.0978747,0.0453662],
                         [1.157,-0.093,-0.156,0.1078,-0.0134259],
                         [1.298,-0.386,0.133,-0.0461473,0.00292086]])
diff= distributionFactors-exampleDistribFactors
ratio4= diff.Norm()

''' 
print("kPlBaja= ",kPlBaja)
print("kPl1a= ",kPl1a)
print("kPl3a= ",kPl3a)
print("periods: ",periods)
print("ratio1= ",ratio1)
print("modos: ",modos)
print("diff: ",diff)
print("ratio2= ",ratio2)
print("modalParticipationFactors: ",modalParticipationFactors)
print("effectiveModalMasses: ",effectiveModalMasses)
print("totalMass: ",totalMass)
print("ratio3= ",ratio3)
print("distributionFactors: ",distributionFactors)
print("ratio4= ",ratio4)
 '''
'''
#Display de deformed shape associated with mode 2
from postprocess.xcVtk.FE_model import quick_graphics as qg
qg.displayEigenResults(preprocessor,eigenMode=2, setToDisplay=setTotal,defShapeScale=1.0,equLoadVctScale=None,accelMode=None,unitsScale=1.0,viewNm="XYZPos",hCamFct=1.0,caption= 'Mode 2: deformed shape.',fileName=None)
'''
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-3) & (ratio2<5e-2) & (ratio3<1e-12) & (ratio4<5e-2)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

