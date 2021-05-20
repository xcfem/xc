      INTEGER FUNCTION SKYPCI (DGPNT,ILAST,JFIRST,JLAST,JMIN)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPCI identifies the first modified column in the submatrix     *
C*    (from the left of from the right)                                *
C*                                                                     *
C*  - PARAMETERS :                                                     *
C*    DGPNT  (aii) : addresses of the diagonal coefficients            *
C*    ILAST  (sii) : last  row    of the    diagonal submatrix         *
C*    JFIRST (sii) : first column of the nondiagonal submatrix         *
C*    JLAST  (sii) : last  column of the nondiagonal submatrix         *
C*    JMIN   (sii) : index of the first column of the skyline block    *
C*                                                                     *
C*  - SUBPROGRAM :                                                     *
C*    SKYPRI                                                           *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER DGPNT(*),ILAST,JFIRST,JLAST,JMIN
C
C==== local variables ==================================================
C
      INTEGER INCR,J,JFROW
C
C==== subprogram =======================================================
C
      INTEGER SKYPRI
C
C**** executable statements ********************************************
C
      SKYPCI = 0
C
      IF ( JFIRST .LE. JLAST ) THEN
         INCR = +1
      ELSE 
         INCR = -1
      END IF
C
      DO 010 J = JFIRST,JLAST,INCR
         JFROW = SKYPRI(DGPNT,J,JMIN)
         IF ( JFROW .LE. ILAST ) THEN
            SKYPCI = J
            RETURN
         END IF
  010 CONTINUE
C
      RETURN
C
C**** end of SKYPCI ****************************************************
C
      END
