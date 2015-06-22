\nverborrea{0}
\mdlr
  {
    \c{Definimos materiales}
    \materiales
       {
         \py_simple1[1,py1]{\clay{} \Cd{0.1} \pult{10} \y50{0.05}}
         \qz_simple1[1,qz1]{\clay{} \qult{10} \z50{0.05}}
         \for_each
           {
             \print{"Material: ",@tag," tipo suelo: ",@soilType," Cd: ",@Cd," pult: ",@pult," y50: ",@y50,"\n"}
           }
       }
  }