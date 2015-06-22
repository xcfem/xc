\def_var["alpha","double"]{@deg2rad(45)}
\def_var["R","double"]{5*@sqrt(2)}
\def_var["r","double"]{4*@sqrt(2)}
\def_var["t","double"]{@R-@r}
\def_var["n1","double"]{1}
\def_var["n2","double"]{100}
\def_var["G","vector"]{}

\mdlr
  {
    \materiales
       {
         \elastic_material["elast"]{ \E{1.0} }
         \geom_secc["geomPrueba"]
           {
             \regiones{\reg_circ["elast"]
               {
                 \nDivRad{@n1}
                 \nDivCirc{@n2}
                 \centro{0.0,0.0}
                 \extRad{@R}
                 \intRad{@r}
                 \initAng{@PI/2-@alpha}
                 \finalAng{@PI/2+@alpha}

                 \expr{area= @getArea}
                 \set{G= @getCdg}
                 \expr{Iz= @tonum(@getIz+area*@sqr(@G[1]))}
                 \expr{Iy= @getIy}
                 \expr{Pyz= @getPyz}
               }}
           }
       }
  }

\expr{areaTeor= @alpha*(@sqr(@R)-@sqr(@r))}
\expr{areaAcad= 14.1372}
\expr{yGAcad= 5.7532}
\expr{yGTeor= @R*((2*@sin(@alpha)/(3*@alpha)*(1-@t/@R+1/(2-@t/@R))))}
\expr{zGAcad= 0.0}
\expr{zGTeor= 0.0}
\expr{iyAcad= 105.3119}
\expr{F1= @tonum(1-3*@t/2/@R+@sqr(@t/@R)-(@t/@R)^3/4)}
\expr{F2= @tonum(@alpha+@sin(@alpha)*@cos(@alpha)-2*@sqr(@sin(@alpha))/@alpha)}
\expr{F3= @tonum(@sqr(@t)*@sqr(@sin(@alpha))/(3*@sqr(@R)*@alpha*(2-@t/@R)))}
\expr{F4= @tonum(1-@t/@R+@sqr(@t)/(6*@sqr(@R)))}
\expr{F5= @tonum(@R^3*@t*(F1*F2+F3*F4))}
\expr{izTeor= @tonum(F5+areaTeor*@sqr(yGTeor))}
\expr{izAcad= 474.3119}
\expr{iyTeor= @tonum(@R^3*@t*F1*(@alpha-@sin(@alpha)*@cos(@alpha)))}


\expr{ratio1= @tonum((area-areaTeor)/areaTeor)}
\expr{ratio2= @tonum((@G[1]-yGTeor)/yGTeor)}
\expr{ratio3= @tonum(@G[0]-zGTeor)}
\expr{ratio4= @tonum((Iy-iyTeor)/iyTeor)}
\expr{ratio5= @tonum((Iz-izTeor)/izTeor)}

\print{"area= ",area,"\n"}
\print{"areaTeor= ",areaTeor,"\n"}
\print{"areaAcad= ",areaAcad,"\n"}
\print{"G= ",@G,"\n"}
\print{"yGAcad= ",yGAcad,"\n"}
\print{"yGTeor= ",yGTeor,"\n"}
\print{"zGAcad= ",zGAcad,"\n"}
\print{"zGTeor= ",zGTeor,"\n"}
\print{"izAcad= ",izAcad,"\n"}
\print{"izTeor= ",izTeor,"\n"}
\print{"Iz= ",Iz,"\n"}
\print{"iyAcad= ",iyAcad,"\n"}
\print{"iyTeor= ",iyTeor,"\n"}
\print{"Iy= ",Iy,"\n"}
\print{"Pyz= ",Pyz,"\n"}
\print{"ratio1= ",ratio1,"\n"}
\print{"ratio2= ",ratio2,"\n"}
\print{"ratio3= ",ratio3,"\n"}
\print{"ratio4= ",ratio4,"\n"}
\print{"ratio5= ",ratio5,"\n"}
\c{
}

\if
  {
    \cond{(@abs(ratio1)<1e-2) & (@abs(ratio2)<1e-2) & (@abs(ratio3)<1e-14)
           & (@abs(ratio4)<1e-15) & (@abs(ratio5)<1e-15) }
    \then{\print{"test_rg_cuadrilatera_02: pasa.\n"}}
    \else{\print{"test_rg_cuadrilatera_02: falla.\n"}}
  }