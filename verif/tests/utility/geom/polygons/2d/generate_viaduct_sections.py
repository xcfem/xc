# -*- coding: utf-8 -*-
''' Generate viaduct variable depth concrete box cross-sections and compute
their area and moments of inertia.

'''

import os
import ezdxf
import geom
from geom_utils import dxf_document
from geom_utils import parabola as pb
from geom_utils import mass_properties

def get_section_polygons(depth):
    ''' Return the interior and exterior polygons of the cross-section.

    :param depth: cross-section depth.
    '''
    rightSideVector= geom.Vector2d(-0.7,-2.62)
    bottomTapersVector= geom.Vector2d(-1.04, -1.25)
    # External contour.
    p0= geom.Pos2d(0,0)
    p1= p0+geom.Vector2d(7, -7*.02)
    p2= p1+geom.Vector2d(0,-.2)
    p3= p2+geom.Vector2d(-3.3, -.14)
    extRightSideLine= geom.Line2d(p3, rightSideVector)
    extBottomLine= geom.Line2d(geom.Pos2d(-10, -depth), geom.Pos2d(10, -depth))
    p4= extBottomLine.getIntersection(extRightSideLine)[0]
    p5= geom.Pos2d(0.0, p4.y)
    p6= geom.Pos2d(-p4.x, p4.y)
    p7= geom.Pos2d(-p3.x, p3.y)
    p8= geom.Pos2d(-p2.x, p2.y)
    p9= geom.Pos2d(-p1.x, p1.y)
    extPlg= geom.Polygon2d([p0, p1, p2, p3, p4, p5, p6, p7, p8, p9])
    # Internal contour.
    p100= p0+geom.Vector2d(0, -0.35)
    p101= p100+geom.Vector2d(1.5, 0.0)
    p102= p101+geom.Vector2d(1.54, -(0.54-0.35))
    intRightSideLine= geom.Line2d(p102, rightSideVector)
    topOfBottomTapersLine= geom.Line2d(geom.Pos2d(-10, -depth+0.35+0.25), geom.Pos2d(10, -depth+0.35+0.25))
    p103= topOfBottomTapersLine.getIntersection(intRightSideLine)[0]
    rightBottomTaperVector= geom.Line2d(p103, bottomTapersVector)
    intBottomLine= geom.Line2d(geom.Pos2d(-10, -depth+0.35), geom.Pos2d(10, -depth+0.35))
    p104= intBottomLine.getIntersection(rightBottomTaperVector)[0]
    p105= geom.Pos2d(0, p104.y)
    p106= geom.Pos2d(-p104.x, p104.y)
    p107= geom.Pos2d(-p103.x, p103.y)
    p108= geom.Pos2d(-p102.x, p102.y)
    p109= geom.Pos2d(-p101.x, p101.y)
    intPlg= geom.Polygon2d([p100, p101, p102, p103, p104, p105, p106, p107, p108, p109])
    return extPlg, intPlg

halfLength= 265.0/2.0
# Position of piers.
xP5= -halfLength # Pier 5
xP6= xP5+50 # Pier 6
xP7= xP6+50 # Pier 7
xP8= xP7+65 # Pier 8
xP9= xP8+50 # Pier 9
xP10= xP9+50 # Pier 10

# Bottom face parabolas
regularDeckDepth= 3.06
minDeckDepth= 2.16
maxDeckDepth= 4.26
## 65 m span.
p65a= geom.Pos2d(xP7+1.25, -maxDeckDepth)
p65b= geom.Pos2d(0.0, -minDeckDepth)
p65c= geom.Pos2d(xP8-1.25, -maxDeckDepth)
span65Parabola= pb.Parabola(p65a,p65b,p65c)
## 50 m span
p50a= geom.Pos2d(xP5+1.25, -maxDeckDepth)
p50b= geom.Pos2d(xP6, -regularDeckDepth)
p50c= geom.Pos2d(xP7-1.25, -maxDeckDepth)
span50Parabola= pb.Parabola(p50a,p50b,p50c)

x= xP5
xi= [x]
yi= [-regularDeckDepth]
while (x<0):
    x+= 0.5
    if(x<(xP6+1.25)):
        y= -regularDeckDepth
    elif(x<xP7-1.25):
        y= span50Parabola.y(x)
    elif(x<xP7+1.25):
        y= -maxDeckDepth
    else:
        y= span65Parabola.y(x)
    xi.append(x)
    yi.append(y)

xi_tmp= list(reversed(xi))
yi_tmp= list(reversed(yi))
xi.extend([-x for x in xi_tmp[1:]])
yi.extend(yi_tmp[1:])

crossSections= dict()
crossSectionsDict= dict()
for i, (x, y) in enumerate(zip(xi,yi)):
    depth= -y
    if(not depth in crossSections):
        extPlg, intPlg= get_section_polygons(depth= depth)
        area= extPlg.getArea()-intPlg.getArea()
        objList= [(1.0, extPlg), (-1.0, intPlg)]
        centerOfMass= mass_properties.get_center_of_mass_2d(objList)
        inertia= mass_properties.get_inertia_2d(objList)
        crossSectionData= {'ext_plg':extPlg, 'int_plg':intPlg, 'depth':depth}
        crossSectionData['area']= area
        crossSectionData['x0']= centerOfMass.x
        crossSectionData['y0']= centerOfMass.y
        crossSectionData['Ix']= inertia[0]
        crossSectionData['Iy']= inertia[1]
        crossSectionData['Pxy']= inertia[2]
        crossSections[depth]= crossSectionData
        
    crossSectionsDict[x]= crossSections[depth]

avgArea= 0.0
avgIx= 0.0
avgIy= 0.0
avgPxy= 0.0
sz= len(crossSectionsDict)
for x in crossSectionsDict:
    sectionData= crossSectionsDict[x]
    avgArea+= sectionData['area']
    avgIx+= sectionData['Ix']
    avgIy+= sectionData['Iy']
    avgPxy+= sectionData['Pxy']
avgArea/=sz
avgIx/=sz
avgIy/=sz
avgPxy/=sz

# print(avgArea)
# print(avgIx)
# print(avgIy)
# print(avgPxy)

# Check results
err= (avgArea-9.96352235130664)**2
err+= (avgIx-14.386404642788735)**2
err+= (avgIy-103.49923929052757)**2
err+= (avgPxy)**2

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-12:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

# # Display deck depths.
# import matplotlib.pyplot as plt
# plt.plot(xi, yi, '-')
# plt.grid()
# # plt.axis('equal')
# plt.show()
    
# # Create DXF document.
# doc = ezdxf.new("R2000")
# dxfDocument= dxf_document.DXFDocument(ezdxfDoc= doc)

# for i, depth in enumerate(crossSections.keys()):
#     crossSectionData= crossSections[depth]
#     extPlg= crossSectionData['ext_plg']
#     intPlg= crossSectionData['int_plg']
#     # Add it to the dxf model.
#     j= int(i/10)
#     extPlg.move(geom.Vector2d((i%10)*20, -j*10))
#     intPlg.move(geom.Vector2d((i%10)*20, -j*10))
#     dxfDocument.addPolygon2d(extPlg)
#     dxfDocument.addPolygon2d(intPlg)
    
# fname= os.path.basename(__file__)
# outputFileName= './'+fname.replace('.py', '.dxf')
# doc.saveas(outputFileName)

