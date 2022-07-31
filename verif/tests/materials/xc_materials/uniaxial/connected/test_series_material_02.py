# -*- coding: utf-8 -*-
'''SeriesMaterial trivial test.''' 

from __future__ import print_function

import math
import xc
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
mat1= typical_materials.defElastNoTensMaterial(preprocessor, "mat1",E= 1e12)
mat2= typical_materials.defElasticMaterial(preprocessor, "mat2", E= 1e3)
materialsToConnect= [mat1.name, mat2.name]
seriesMaterial= typical_materials.defSeriesMaterial(preprocessor, 'seriesMaterial', materialsToConnect)

ratio= abs(seriesMaterial.numMaterials-len(materialsToConnect))

#print(seriesMaterial.numMaterials)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
