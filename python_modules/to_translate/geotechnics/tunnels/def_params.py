# -*- coding: utf-8 -*-

def defParamAngPoliedrico(jp): 
    JPvacio= jp.Vacio()
    if(not(JPvacio)):
        containsTunnelAxis= jp.esInterior(tunnelAxisVector) or jp.esInterior(-tunnelAxisVector)
        I1= jp.getVectorExterno1(tunnelAxis)
        I2= jp.getVectorExterno2(tunnelAxis)
        angI1= I1.getXAxisAngle()
        angI2= I2.getXAxisAngle()
        angI1I2= angI2-angI1
        if(angI1I2<PI):
            ptsTang= computePointsOfTangency("tunnelSection",I1,I2)
            p1= ptsTang.at(1)
            r1= puntoyVector([p1,I1])
            p2= ptsTang.at(2)
            r2= puntoyVector([p2,I2])
            p3= intersection_point(r1,r2)

def def_params_jps():
    defParamAngPoliedrico(JP0000)
    defParamAngPoliedrico(JP0001)
    defParamAngPoliedrico(JP0010)
    defParamAngPoliedrico(JP0011)
    defParamAngPoliedrico(JP0100)
    defParamAngPoliedrico(JP0101)
    defParamAngPoliedrico(JP0110)
    defParamAngPoliedrico(JP0111)
    defParamAngPoliedrico(JP1000)
    defParamAngPoliedrico(JP1001)
    defParamAngPoliedrico(JP1010)
    defParamAngPoliedrico(JP1011)
    defParamAngPoliedrico(JP1100)
    defParamAngPoliedrico(JP1101)
    defParamAngPoliedrico(JP1110)
    defParamAngPoliedrico(JP1111)
    
def def_params_jps_4planes():
    defParamAngPoliedrico(JP0000)
    defParamAngPoliedrico(JP0001)
    defParamAngPoliedrico(JP0010)
    defParamAngPoliedrico(JP0011)
    defParamAngPoliedrico(JP0100)
    defParamAngPoliedrico(JP0101)
    defParamAngPoliedrico(JP0110)
    defParamAngPoliedrico(JP0111)
    defParamAngPoliedrico(JP1000)
    defParamAngPoliedrico(JP1001)
    defParamAngPoliedrico(JP1010)
    defParamAngPoliedrico(JP1011)
    defParamAngPoliedrico(JP1100)
    defParamAngPoliedrico(JP1101)
    defParamAngPoliedrico(JP1110)
    defParamAngPoliedrico(JP1111)
    
def def_params_jps_3planes():
    defParamAngPoliedrico(JP000)
    defParamAngPoliedrico(JP001)
    defParamAngPoliedrico(JP010)
    defParamAngPoliedrico(JP011)
    defParamAngPoliedrico(JP100)
    defParamAngPoliedrico(JP101)
    defParamAngPoliedrico(JP110)
    defParamAngPoliedrico(JP111)
