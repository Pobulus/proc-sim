// to jest komentarz
        pob zero 
        dod c//wyzeruj komórkę c
loop:   pob c
        ode jeden
        lad c
        ode test
        soz koniec   
        sob loop
koniec: pob 12

        stp

jeden:  rst 1
zero:   rst 0
c:      rpa
test:   rst 10
