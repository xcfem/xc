      SUBROUTINE SKYPSB (DGPNT,IFIRST,ILAST,INDEX,JFIRST,JLAST,JMIN,
     &                   LIMIT,MCOLS,MROWS,NCOLS,A,WORK,SHAPE)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPSB copies a submatrix of the skyline stored array            *
C*    into a two dimensional array                                     *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    DGPNT  (aii) : addresses of the diagonal coefficients            *
C*    IFIRST (sii) : index of the first row    in the submatrix        *
C*    ILAST  (sii) : index of the last  row    in the submatrix        *
C*    INDEX  (aio) : indexes of the columns copied into (WORK)         *
C*    JFIRST (sii) : index of the first column in the submatrix        *
C*    JLAST  (sii) : index of the last  column in the submatrix        *
C*    JMIN   (sii) : index of the first column of (A)                  *
C*    LIMIT  (sii) : maximum row index                                 *
C*    MCOLS  (sii) : maximum number of columns stored in (WORK)        *
C*    MROWS  (sii) : maximum number of rows    stored in (WORK)        *
C*    NCOLS  (sio) : number of columns copied  to (WORK)               *
C*    A      (ari) : block of the skyline matrix                       *
C*    WORK   (arw) : two dimensional work array                        *
C*    SHAPE  (sci) : type of the submatrix to be copied                *
C*                   'R' = rectangular                                 *
C*                   'T' = triangular                                  *
C*                                                                     *
C*  - SUBPROGRAM :                                                     *
C*    SKYPRI                                                           *
C*                                                                     *
C*  - INTRINSIC FUNCTIONS :                                            *
C*    MAX0,MIN0                                                        *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          DGPNT(*),IFIRST,ILAST,INDEX(*),JFIRST,
     &                 JLAST,JMIN,LIMIT,MCOLS,MROWS,NCOLS
      DOUBLE PRECISION A(*),WORK(MROWS,MCOLS)
      CHARACTER        SHAPE*1
C
C==== local variables ==================================================
C
      INTEGER          I,IABOVE,IBELOW,IEND,J,JA,JFROW,NINDEX
C
C==== subprogram =======================================================
C
      INTEGER          SKYPRI
C
C==== intrinsic functions ==============================================
C
      INTEGER          MAX0,MIN0
C
C**** executable statements ********************************************
C
      NINDEX = 0    
C
      DO 010 J = JFIRST,JLAST
C
C....... copy a piece of the j-th column of (A) into (WORK) ............
C
         JA     = DGPNT(J) - J
         JFROW  = SKYPRI(DGPNT,J,JMIN)
         IABOVE = MIN0(JFROW-1,ILAST)
         IBELOW = MAX0(IABOVE+1,IFIRST)
C
         IF ( IBELOW .LE. LIMIT ) THEN
C
            IF      ( SHAPE .EQ. 'R' ) THEN
                    IEND = ILAST
            ELSE IF ( SHAPE .EQ. 'T' ) THEN
                    IEND = J
            END IF
C
            NINDEX = NINDEX + 1
            INDEX(NINDEX) = J
C
            DO 020 I = IFIRST,IABOVE
               WORK(I-IFIRST+1,NINDEX) = 0.0D0           
  020       CONTINUE
C
            DO 030 I = IBELOW,IEND 
               WORK(I-IFIRST+1,NINDEX) = A(JA+I)
  030       CONTINUE
C
            IF ( IBELOW .LE. IEND  ) NCOLS = NINDEX
            IF ( NINDEX .EQ. MCOLS ) GO TO 040
C
         END IF
C
  010 CONTINUE
C
  040 CONTINUE
C
C.... if allowed, shift JFIRST to the right ............................
C
      IF ( SHAPE .EQ. 'R' ) JFIRST = INDEX(NINDEX) + 1
C
      RETURN
C
C**** end of SKYPSB ****************************************************
C
      END
