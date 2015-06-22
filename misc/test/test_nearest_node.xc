\c{Test para comprobar el funcionamiento de getNearestNode}

\def_prop["tg","double"]{0}
\def_prop["lapso","double"]{0}
\mdlr
  {    
    \nodos
      {
        \def_var["i","double"]{1}
        \for(i=0;@i<2000;i=@i+1)
          {\bucle{\nod{\coo{@i,0,0}}}}
      }
    \sets
      {
        \total
         {
           \c{\print{"creados ",@nnod," nodos.\n"}}
           \def_prop["crono","timer"]{1}
           \crono{\start{}}
           \set{tg= @getTagNearestNode(50.51,0,0)}
           \crono{\stop{} \set{lapso= @userTime}}
         }
      }
  }

\print{"tag nodo: ",@tg,"\n"}
\print{"lapso: ",@lapso,"\n"}
\c{
  }

\if((@tg==51) & (@lapso<100))
  {
    \then{\print{"prueba nearest node 01: pasa.\n"}}
    \else{\print{"prueba nearest node 01: falla.\n"}}
  }
