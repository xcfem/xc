# -*- coding: utf-8 -*-
''' Test the computation of the Frenet trihedron along a helix.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import geom
import numpy as np

# Generate helix points.
theta_min= 0.0
theta_max= 4*math.pi
n = 1000
theta_i= np.linspace(0, theta_max, n)
points= list()
for theta in theta_i:
    x= math.cos(theta)
    y= math.sin(theta)
    z= theta/4.0
    points.append(geom.Pos3d(x, y, z))

helix= geom.Polyline3d(points)

ft= geom.FrenetTrihedron(helix)
length= ft.getPath().getLength()
si= np.linspace(0, length, 5)
refSystems= list()
for s in si:
    refSystems.append(ft.getRefSys(float(s)))

# Check results.
refSys2= refSystems[2]
iVector2= refSys2.getIVector()
jVector2= refSys2.getJVector()
kVector2= refSys2.getKVector()
iVector2Ref= geom.Vector3d(0,0.97014,0.242546)
jVector2Ref= geom.Vector3d(-1,0,0)
kVector2Ref= geom.Vector3d(0, -iVector2Ref.z, iVector2Ref.y)
err= math.sqrt((iVector2-iVector2Ref).getModulus2()+(jVector2-jVector2Ref).getModulus2()+(kVector2-kVector2Ref).getModulus2())

'''
print('iVector2= ', iVector2)
print('jVector2= ', jVector2)
print('kVector2= ', kVector2)
print(err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-4):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # VTK Graphic output
# from postprocess.xcVtk import vtk_geom
# from postprocess.xcVtk import vtk_graphic_base
# from vtk.vtkCommonColor import vtkNamedColors

# colors= vtkNamedColors()

# vtkPlineActor= vtk_geom.get_vtk_polyline_actor(helix, color= colors.GetColor3d('Tomato'))
# vtkRefSysActors= vtk_geom.get_ref_sys_actor(refSystems[2])
# actors= [vtkPlineActor]+ vtkRefSysActors
# vtk_graphic_base.setup_render_window(windowName= 'PolyLine', actors= actors, backgroundColor= colors.GetColor3d('DarkOliveGreen'), xSize= 800, ySize= 600)


