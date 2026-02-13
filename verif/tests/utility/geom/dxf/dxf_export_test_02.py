# -*- coding: utf-8 -*-
''' Check DXF export of 2D polylines.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import geom
import ezdxf
from geom_utils import dxf_document



# Create DXF document.
doc = ezdxf.new("R2000")

dxfDocument= dxf_document.DXFDocument(ezdxfDoc= doc)

layers= list()
p02d= geom.Pos2d(0,0)
p03d= geom.Pos3d(0,0,0)
for i in range(0, 10):
    layerName= 'test_'+str(i)
    layers.append(dxfDocument.newLayer(layerName= layerName, color= i))
    p12d= geom.Pos2d(1+i, 1+2*i)
    s2d= geom.Segment2d(p02d, p12d)
    dxfDocument.addSegment2d(segment2d= s2d, layerName= layerName, color= 2*i)
    p13d= geom.Pos3d(1+i, 0, 1+2*i)
    s3d= geom.Segment3d(p03d, p13d)
    dxfDocument.addSegment3d(segment3d= s3d, layerName= layerName, color= 3*i)
 
testOK= (len(layers)==10)



fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.dxf')
doc.saveas(outputFileName)

# Check that file exists
testOK= testOK and os.path.isfile(outputFileName)

# print(len(layers))

from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.

