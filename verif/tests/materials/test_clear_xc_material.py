# -*- coding: utf-8 -*-
''' Check clearXCMaterial method.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import xc
from materials import typical_materials as tm
from model import predefined_spaces
from materials.sections import section_properties as sectpr

#Geometry
hBeam=1.305406 #height of the cross-section [m]. Parallel to local z-axis 
wBeam= 0.3450267 #width of the cross-section [m]. Parallel to local y-axis
Abeam=hBeam*wBeam   #cross-section area of the beam[m2]
Iybeam=1/12.*hBeam*wBeam**3 #moment of inertia of the beam cross-section [m4]
Izbeam=1/12.*wBeam*hBeam**3 #moment of inertia of the beam cross-section [m4]

#Material properties
Ec= 28e6  # modulus of elasticity of concrete [kPa]
nuc= 0.2      #coefficient of Poisson of concrete

# Problem type
FEcase= xc.FEProblem()
prep=  FEcase.getPreprocessor
nodes= prep.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

#BEAM
#Geometric section
geomSectBeam= sectpr.RectangularSection(name='geomSectBeam',b=wBeam,h=hBeam)

# Material definition
concrete= tm.MaterialData(name='concrete',E=Ec,nu=nuc,rho= 0.0)
beamMat= tm.BeamMaterialData(name= 'beamMat', section=geomSectBeam, material=concrete)
beamMat.defElasticShearSection3d(prep)

materialNames= modelSpace.getMaterialNames()
testOK= (materialNames==['beamMat'])
beamMat.clearXCMaterial(preprocessor= prep)
materialNames= modelSpace.getMaterialNames()
testOK= testOK and (len(materialNames)==0)

# print(testOK)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
