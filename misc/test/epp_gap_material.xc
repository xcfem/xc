\c{Test de elaboración propia}
\archivo_log{"/tmp/borrar.log"} \c{Para no imprimir mensajes de advertencia.}
\expr{fy= 600} \c{Tensión de cedencia del material.}
\expr{E= 2.1e5} \c{Módulo de Young del material.}
\expr{epsy= fy/E} \c{Deformación para la que se produce la cedencia}
\expr{Nsteps= 10} \c{Número de pasos para el análisis.}

\c{Definición del modelo}
\mdlr
  {
    \c{Definimos materiales}
    \materiales
       {
         \epp_gap_material["eppGap"]{ \fy{fy} \E{E} }
         \eppGap
           {
             \expr{A= 1.5*@getEpsy}
             \expr{fin= 54}
             \expr{incr= @PI/24.0}
             \nuevo_archivo_salida["ley"]{"./verif/ley.dat"}
             \for
               {
                 \inicio{\expr{j=0}} \continua{'j<=fin'} \incremento{\expr{j=@tonum(j+1)}}
                 \bucle
                   {
                     \set_trial_strain{A*@sin(j*incr)};
                     \commit{}
                     \print["ley"]{@strain," ",@stress,"\n"}
                   }
               }
             \cierra_archivo_salida{"ley"} \c{Si no cerramos falla la comparacion}
           }
       }

  }