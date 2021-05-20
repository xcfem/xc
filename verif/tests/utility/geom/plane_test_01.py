from __future__ import print_function
import xc_base
import geom
o=geom.Pos3d(0,0,0)
p1=geom.Pos3d(1,0,0)
p2=geom.Pos3d(0,1,0)

plane=geom.Plane3d(o,p1,p2)
normal=plane.getNormal()
base1=plane.getBase1()
base2=plane.getBase2()
tp=plane.tipo()

import os
fname= os.path.basename(__file__)
if normal.x==0 and normal.y==0 and normal.z==1:
  print("test ",fname,": ok.")
else:
  print("test ",fname,": ERROR.")


