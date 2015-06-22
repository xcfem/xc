\c{Test de comprobación de orejeta según EC-3}
\path{"/usr/local/lib/macros_xc"}
\incluye{"uniones/orejetaEC3.xcm"}
\incluye{"materiales/ec3/acerosEC3.xcm"}

\c{Geometría}
\expr{gM0= 1.05}
\expr{gM2= 1.25}
\expr{t= .05}
\expr{F= 1744e3}\c{Tracción en la barra.}
\expr{dBulon= 76e-3} \c{Diámetro del bulón.}
\expr{d0= dBulon+2e-3} \c{Diámetro del taladro.}
\expr{ABulon= @PI()*(dBulon/2.0)^2}
\expr{WBulon= @PI()*(dBulon/2.0)^3/4.0}
\expr{fyBulon= 460e6} \c{Límite elástico del acero del bulón}
\expr{fuBulon= 610e6} \c{Resistencia última a tracción del acero del bulón}
\expr{fy= @fyEC3(355,t)} \c{Límite elástico del acero}
\expr{fu= @fuEC3(355,t)} \c{Resistencia última a tracción}
\expr{A= 148e-3} \c{Distancia entre caras exteriores de los brazos de la horquilla}
\expr{T= 76e-3} \c{Huelgo entre los brazos de la horquilla}

\expr{aMin= @getAMinOrejetaEC3(F,gM0,t,d0,fy)}
\expr{cMin= @getCMinOrejetaEC3(F,gM0,t,d0,fy)}
\expr{RCBulon= @getResistCortanteBulonEC3(ABulon,gM2,fuBulon)}
\expr{ROjal= @getFAplastamientoOjalEC3(t,d0,fy,gM0)}
\expr{MSBulon= @getMSolicBulonEC3(F,t,(A-T)/2,(T-t)/2)}
\expr{MRBulon= @getMResistBulonEC3(WBulon,fyBulon,gM0)}
\expr{FBulon= @getFactorCapacidadBulonEC3(MSBulon,MRBulon,F/2,RCBulon)}

\print{"fy= ",@tonum(fy/1e6)," MPa\n"}
\print{"fu= ",@tonum(fu/1e6)," MPa\n"}
\print{"aMin= ",@tonum(aMin*1e3)," mm\n"}
\print{"cMin= ",@tonum(cMin*1e3)," mm\n"}
\print{"ABulon= ",@tonum(ABulon*1e4)," cm2\n"}
\print{"WBulon= ",@tonum(WBulon*1e6)," cm3\n"}
\print{"Resistencia a cortante del bulon; RBulon= ",@tonum(RCBulon/1e3)," kN\n"}
\print{"Resistencia al aplastamiento del ojal; ROjal= ",@tonum(ROjal/1e3)," kN\n"}
\print{"Momento que solicita al bulón; MSBulon= ",@tonum(MSBulon/1e3)," kN m\n"}
\print{"Momento que puede resistir el bulón; MRBulon= ",@tonum(MRBulon/1e3)," kN m\n"}
\print{"Factor de capacidad del bulón; FBulon= ",@tonum(FBulon)," kN m\n"}

\fin{}

\if
  {
    \cond{(@abs(ratio0)<1e-10) & (@abs(ratio1)<1e-12)}
    \then{\print{"test beam3dUniformLoad 01: pasa.\n"}}
    \else{\print{"test beam3dUniformLoad 01: falla.\n"}}
  }
