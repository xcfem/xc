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
from geom_utils import dxf_model_space



# Create DXF document.
doc = ezdxf.new("R2000")
msp = doc.modelspace()

dxfModelSpace= dxf_model_space.DXFModelSpace(msp)

# Define 2D polyline.
pline2d= geom.Polyline2d([geom.Pos2d(1,0), geom.Pos2d(1,1), geom.Pos2d(0,1)])
## Add it to the dxf model.
dxfModelSpace.addPolyline2d(pline2d)

# Define 3D polyline.
pline3d= geom.Polyline3d([geom.Pos3d(1,0,1), geom.Pos3d(1,1,1), geom.Pos3d(0,1,1), geom.Pos3d(0,1,2)])
## Add it to the dxf model.
dxfModelSpace.addPolyline3d(pline3d)



fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.dxf')
doc.saveas(outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

from misc_utils import log_messages as lmsg
if(testOK) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.

