# -*- coding: utf-8 -*-
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
from import_export import dxf_reader
from import_export import neutral_mesh_description as nmd
import logging

layerNamesToImport= ['.*']

def getRelativeCoo(pt):
    return [pt[0],pt[1],pt[2]] #No modification.

pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
dxfFilePath= pth+'/../../aux/dxf/dxf_group_test.dxf' 
logger= logging.getLogger('ezdxf')
logger.setLevel(level=logging.WARNING) #Avoid logging info messages.
dxfImport= dxf_reader.DXFImport(dxfFilePath, layerNamesToImport,getRelativeCoo, importLines= True, polylinesAsSurfaces= False, threshold= 0.001, tolerance= .001)

ok= True
referenceEntities= [u'7E', u'7D', u'7C', u'7B', u'7A', u'77', u'79', u'78']
entitiesWithGroup= (dxfImport.entitiesGroups.keys())
for label in referenceEntities:
    if not label in entitiesWithGroup:
        ok= False
        break

# #Block topology
# blocks= dxfImport.exportBlockTopology('test')
# fileName= 'xc_model_blocks'
# ieData= nmd.XCImportExportData()
# ieData.outputFileName= fileName
# ieData.problemName= 'FEcase'
# ieData.blockData= blocks

# ieData.writeToXCFile()

'''
print(entitiesWithGroup)
print('ok: ', ok)
'''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ok):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
