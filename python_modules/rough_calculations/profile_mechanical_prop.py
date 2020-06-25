from __future__ import division
import math


# Rough calculation of mechanical properties of profiles

def Iprofile(h,b,tw,tf,r):
    '''print(mechanical properties of an I-shaped profile)

    - Z: strong axis
    - Y: weak axis

    :param h: height
    :param b: widht
    :param tw: web thickness
    :param tf: flange thickness
    :param r: radius
    '''
    hi=h-2*tf
    d=hi-2*r
    A=2*b*tf+tw*hi+(2*r)**2-math.pi*r**2
    Iz=2*(1/12.*b*tf**3+b*tf*(h/2.-tf/2.)**2)+(1/12*tw*hi**3)
    Iy=2*(1/12*tf*b**3)+1/12*hi*tw**3
    Wz=Iz/(h/2)
    Wy=Iy/(b/2)
    iz=math.sqrt(Iz/A)
    iy=math.sqrt(Iy/A)
    Avz=A-2*b*tf+(tw+2*r)*tf
    print('hi=',hi)
    print('d=', d)
    print('A=', A)
    print('Iy=',Iy)
    print('Iz=',Iz)
    print('Wy=',Wy)
    print('Wz=',Wz)
    print('iy=',iy)
    print('iz=',iz)
    print('Avz=',Avz )
    return
