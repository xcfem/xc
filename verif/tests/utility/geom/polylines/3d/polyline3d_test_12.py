# -*- coding: utf-8 -*-
''' Verification of the getNormalVectorAtVertices method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2025, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom

# Define polyline.
points= [geom.Pos3d(4290.52, 5333.42, 11.809), geom.Pos3d(4305.5, 5320.17, 12.047), geom.Pos3d(4320.69, 5307.15, 12.258), geom.Pos3d(4336.07, 5294.38, 12.444), geom.Pos3d(4351.66, 5281.84, 12.624), geom.Pos3d(4367.44, 5269.55, 12.804), geom.Pos3d(4383.41, 5257.51, 12.984), geom.Pos3d(4399.56, 5245.72, 13.164), geom.Pos3d(4415.9, 5234.19, 13.344), geom.Pos3d(4432.41, 5222.91, 13.524), geom.Pos3d(4449.1, 5211.89, 13.704)]
pline= geom.Polyline3d(points)

# Compute vertex normals.
normals= pline.getNormalVectorAtVertices()

# Check results.
half_sqrt_2= math.sqrt(2)/2.0
ref_normals= [geom.Vector3d(-0.660816, -0.745541, 0.086552), geom.Vector3d(-0.654945, -0.750694, 0.0866424), geom.Vector3d(-0.643404, -0.761444, 0.0789586), geom.Vector3d(-0.632821, -0.774045, 0.0197903), geom.Vector3d(-0.62063, -0.784103, -0.000177072), geom.Vector3d(-0.608247, -0.793748, -3.58222e-05), geom.Vector3d(-0.59583, -0.803111, -0.000128504), geom.Vector3d(-0.583106, -0.812396, 7.68069e-05), geom.Vector3d(-0.570353, -0.8214, -8.78436e-05), geom.Vector3d(-0.557584, -0.830121, 0.000126681), geom.Vector3d(-0.551004, -0.834503, 0.000197834)]

err= 0.0
for n, n_ref in zip(normals, ref_normals):
    err+= (n-n_ref).getModulus2()
err= math.sqrt(err)

'''
print(normals)
print(err)
'''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-4):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Graphic output
# from postprocess.xcVtk import vtk_geom
# from postprocess.xcVtk import vtk_graphic_base
# from vtk.vtkCommonColor import vtkNamedColors

# colors= vtkNamedColors()

# vtkPlineActor= vtk_geom.get_vtk_polyline_actor(pline, color= colors.GetColor3d('Tomato'))
# vtkVectorFieldActor= vtk_geom.get_vtk_vector_field_actor(positions= pline.getVertices(), vectors= normals, scaleFactor= 10.0) 
# vtk_graphic_base.setup_render_window(windowName= 'PolyLine', actors= [vtkPlineActor, vtkVectorFieldActor], backgroundColor= colors.GetColor3d('DarkOliveGreen'), xSize= 800, ySize= 600)
