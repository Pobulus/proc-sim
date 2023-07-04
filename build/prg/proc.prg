// test procedury
		sob start
//dane wejściowe
a: 		rst 6
b:		rst 57				
start:	pob a
		dns
		sdp mnozX
		lad wynik
		stp

wynik: 	rpa

mnozX:	lad temp //w AK przekazana wartosc
		pob b
		lad count
		pob zero
		lad mnozXw
		
mnozXl:	pob mnozXw
		dod temp
		lad mnozXw
		pob count
		ode jeden
		soz mnozXk
		lad count
		sob mnozXl
mnozXk:	pob mnozXw
		pwr
mnozXw:	rpa		
temp: 	rpa
adr: 	rpa
count:	rpa
zero: 	rst 0
jeden: 	rst 1

