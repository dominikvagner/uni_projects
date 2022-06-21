; Vernamova sifra na architekture DLX
; Dominik Vágner xvagne10

        .data 0x04          ; zacatek data segmentu v pameti
login:  .asciiz "xvagne10"  ; <-- nahradte vasim loginem
cipher: .space 9 ; sem ukladejte sifrovane znaky (za posledni nezapomente dat 0)

        .align 2            ; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login         ; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher        ; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40          ; adresa zacatku programu v pameti
        .global main        ; 

main:   add r16, r0, r0;	znak counter
	
	start:	

	; KONTROLA DALSIHO ZNAKU
	lb r6, login(r16);	nacteni dalsiho znaku
	addi r9, r0, 97;	znak 'a'	
	sgt r9, r9, r6;		r9 = 1 if r9 > r6 else r9 = 0
	bnez r9, finish;
	nop;
	nop;

	; PRVNI ZNAK
	addi r9, r0, 123;	konec abecedy
	addi r5, r0, 118;	vlozeni znaku 'v' do r5
	subi r5, r5, 96;	prevod na pozici v abecede
	
	lb r15, login(r16);	nacteni sifrovaneho znaku
	add r15, r15, r5;	posuv + 
	sgt r9, r9, r15;	r9 = 1 if r9 > r15 else r9 = 0
	beqz r9, overflow; 	if r9 == 0 jump to overflow
	nop;
	nop;
	j endoverflow;
	nop;
	nop;
	overflow:	
		subi r15, r15, 26;	odecteni poctu znaku v abecede
	endoverflow:
		addi r9, r16, cipher;	pozice pro zapis zasifrovaneho znaku do r9
		sb 0(r9), r15;		zapsani zasifrovaneho znaku
		sb 1(r9), r0;		zapsani null na ukonceni stringu

	addi r16, r16, 1;	inkrementace counteru

	; KONTROLA DALSIHO ZNAKU
	lb r6, login(r16);	nacteni dalsiho znaku
	addi r9, r0, 97;	znak 'a'	
	sgt r9, r9, r6;		r9 = 1 if r9 > r6 else r9 = 0
	bnez r9, finish;
	nop;
	nop;

	; DRUHY ZNAK
	addi r9, r0, 97;	zacatek abecedy
	addi r5, r0, 97;	vlozeni znaku 'a' do r5
	subi r5, r5, 96;	prevod na pozici v abecede

	lb r15, login(r16);	nacteni sifrovaneho znaku
	sub r15, r15, r5;	posuv -
	sgt r9, r9, r15;	r9 = 1 if r9 > r15 else r9 = 0
	bnez r9, underflow;
	nop;
	nop;
	j endunderflow;
	nop;
	nop;
	underflow:
		addi r15, r15, 26;	pricteni poctu znaku v abecede
	endunderflow:
		addi r9, r16, cipher;	poice pro zapis sifrovaneho znaku
		sb 0(r9), r15;		zapsani sifrovaneho znaku
		sb 1(r9), r0;		zapsani null na ukonceni stringu

	addi r16, r16, 1;	inkrementace counteru
	
	j start;
	nop;
	nop:
	
	finish:

end:    addi r14, r0, caddr ; <-- pro vypis sifry nahradte laddr adresou caddr
        trap 5  ; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0  ; ukonceni simulace
