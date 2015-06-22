\expr{NumDiv= 2}
\expr{ApertBase= 3.3} \c{Apertura de la columna en la base.}
\expr{AltColumna= 24.77444}
\expr{AngPuntal= atan(AltColumna/(ApertBase/2))} \c{Ángulo del puntal con la horizontal}
\expr{LT1Puntal= 10.674} \c{Longitud del primer tramo del puntal.}
\expr{LT2Puntal= 10.142-0.501} \c{Longitud del segundo tramo del puntal (hasta el eje de la unión viga-columna).}
\expr{LT3Puntal= 0.501} \c{Longitud del tercer tramo del puntal (hasta el extremo superior del perfil).}

\expr{xp2= LT1Puntal*cos(AngPuntal)}
\expr{zp2= LT1Puntal*sin(AngPuntal)}
\expr{xp3= LT2Puntal*cos(AngPuntal)+xp2}
\expr{zp3= LT2Puntal*sin(AngPuntal)+zp2}
\expr{xp4= LT3Puntal*cos(AngPuntal)+xp3}
\expr{zp4= LT3Puntal*sin(AngPuntal)+zp3}
\expr{xp5= ApertBase/2}
\expr{zp5= AltColumna}

\expr{xp6= ApertBase}
\expr{zp6= zp1}
\expr{xp7= ApertBase-xp2}
\expr{zp7= zp2}
\expr{xp8= ApertBase-xp3}
\expr{zp8= zp3}
\expr{xp9= ApertBase-xp4}
\expr{zp9= zp4}

\c{Puntal izquierdo}
\pnt[p1]{ \pos{ \x{0.0} \y{0.0} \z{0.0}} } \c{Base puntal izquierdo}
\pnt[p2]{ \pos{ \x{$xp2} \y{0.0} \z{$zp2}} } \c{Unión cubrejuntas puntal izquierdo}
\pnt[p3]{ \pos{\x{xp3} \y{0.0} \z{$zp3}} } \c{Union viga columna}
\pnt[p4]{ \pos{\x{xp4} \y{0.0} \z{$zp4}} } \c{Extremo superior puntal}
\pnt[p5]{ \pos{\x{xp5} \y{0.0} \z{$zp5}} } \c{Extremo superior puntal}

\c{Puntal derecho}
\pnt[p6]{ \pos{\x{xp6} \y{0.0} \z{$zp6}} } \c{Extremo superior puntal}
\pnt[p7]{ \pos{\x{xp7} \y{0.0} \z{$zp7}} } \c{Extremo superior puntal}
\pnt[p8]{ \pos{\x{xp8} \y{0.0} \z{$zp8}} } \c{Extremo superior puntal}
\pnt[p9]{ \pos{\x{xp9} \y{0.0} \z{$zp9}} } \c{Extremo superior puntal}

\c{Puntal izquierdo}
\linea[l1] { \p1{p1} \p2{p2} \ndiv{$NumDiv} }
\linea[l2] { \p1{p2} \p2{p3} \ndiv{$NumDiv} }
\linea[l3] { \p1{p3} \p2{p4} \ndiv{$NumDiv} }
\linea[l4] { \p1{p4} \p2{p5} \ndiv{$NumDiv} }

\c{Puntal derecho}
\linea[l5] { \p1{p6} \p2{p7} \ndiv{$NumDiv} }
\linea[l6] { \p1{p7} \p2{p8} \ndiv{$NumDiv} }
\linea[l7] { \p1{p8} \p2{p9} \ndiv{$NumDiv} }
\linea[l8] { \p1{p9} \p2{p5} \ndiv{$NumDiv} }