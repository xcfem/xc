# -*- coding: utf-8 -*-
JPvacio= Vacio
if(not(JPvacio))
    \then
      {
        containsTunnelAxis= or(esInterior(tunnelAxisVector),esInterior(-tunnelAxisVector))  
        I1= getVectorExterno1(tunnelAxis)
        I2= getVectorExterno2(tunnelAxis)
        angI1= 
        \I1{angI1= getXAxisAngle}
        angI2= 
        \I2{angI2= getXAxisAngle}
        angI1I2= angI2-angI1
        ptsTang= 
        p1= 
        p2= 
        p3= 
        if(angI1I2<PI)
            \then
              {
                ptsTang= computePointsOfTangency("tunnelSection",I1,I2)
                r1= 
                p1= ptsTang.at(1)
                \r1{\puntoyVector{[p1,I1]}}
                r2= 
                p2= ptsTang.at(2)
                \r2{\puntoyVector{[p2,I2]}}
                p3= intersection_point(r1,r2)
              }
          }
      }
  }
