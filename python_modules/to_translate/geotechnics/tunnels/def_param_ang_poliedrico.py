# -*- coding: utf-8 -*-

def defParamAngPoliedrico(jp): 
    JPvacio= jp.Vacio
    if(not(JPvacio)):
        containsTunnelAxis= or(jp.esInterior(tunnelAxisVector),jp.esInterior(-tunnelAxisVector))  
        I1= jp.getVectorExterno1(tunnelAxis)
        I2= jp.getVectorExterno2(tunnelAxis)
        angI1= I1(angI1= getXAxisAngle)
        angI2= I2(angI2= getXAxisAngle)
        angI1I2= angI2-angI1
        ptsTang= None
        if(angI1I2<PI):
            ptsTang= computePointsOfTangency("tunnelSection",I1,I2)
            p1= ptsTang.at(1)
            r1= puntoyVector([p1,I1])
            p2= ptsTang.at(2)
            r2= puntoyVector([p2,I2])
            p3= intersection_point(r1,r2)
