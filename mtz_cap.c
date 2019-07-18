/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                        	TCC2	                              *
*                          UTFPR                                  *
*         DESENVOLVIDO POR Anderson Felipe Schumacher         	  *
*   VERS√ÉO: 1.0                           DATA: 06/09/2018        *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                     DESCRI√á√ÉO DO ARQUIVO                        *
*  Programa que inicializa corretamente o display LCD, efetua a
*constante leitura de tr√™s bot√µes. O primeiro e o segundo bot„o,
*aumenta e diminui, respectivamente, o valor de capacit‚ncia que 
*È mostrado no LCD. O terceiro bot„o (enter), seleciona a capaci- *
*t‚ncia que est· sendo mostrada no LCD. Assim que a capacit√¢ncia È
*selecionada os capacitores necess·rios s„o ativados.
*
*-----------------------------------------------------------------*
*   PIC 16F877A                          					      *
*                                                                 *
*                                                                 *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                CONFIGURA«√O DE GRAVA«√O (FUSES)		  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                     ARQUIVOS DE DEFINI«’ES                      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

#include <xc.h>

#define _XTAL_FREQ 4000000

void PULSE();
void ESCREVE_8vias(int DADO);
void ESCREVE_4vias(int DADO);
void INICIALIZANDO_LCD();
void espera_tecla();
void ATIVA_CAPACITOR(int num);
void INICIA_TXT_LCD();
void MOSTRA_OPCAO_CAPACITANCIA(int cap_op);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                         CONSTANTES                              *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                     VARI¡VEIS GLOBAIS                   	  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

int POSICAOCURSOR = 0;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                        FLAGS INTERNOS                           *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                           ENTRADAS                              *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

// botıes conectados aos bits 0, 1 e 2 da PORTB
#define SETA_CIMA   PORTCbits.RC0
#define SETA_BAIXO  PORTCbits.RC1
#define ENTER       PORTCbits.RC2


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                           SAÕDAS                                *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

// barramento de dados do LCD conectado ‡ PORTD

// RelÈs conectadps aos bits 0,1,2 e 3 da PORTA

#define DISPLAY_LCD PORTD
#define RS          PORTEbits.RE0	/* INDICA P/ O DISPLAY UM DADO OU COMANDO
                            1 -> DADO
                            0 -> COMANDO            */
#define ENABLE      PORTEbits.RE1 	/* SINAL DE ENABLE P/ DISPLAY
			  ATIVO NA BORDA DE DESCIDA */

#define RELE_A1     PORTAbits.RA0 //
#define RELE_A2     PORTAbits.RA1
#define RELE_B1     PORTAbits.RA2
#define RELE_B2     PORTAbits.RA3
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*               FUN«’ES DE INTERRUP«√O                             *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*void interrupt isr (void)
{
}*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*	            	FUN«’ES ORDIN¡RIAS             		  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

//-------------- INÕCIO da funÁ„o ESCREVE_8vias ---------------
void PULSE()
{
    ENABLE = 1;
    __delay_us(1);
    ENABLE = 0;
}
//-------------- FIM da funÁ„o PULSE ------------------


//-------------- INÕCIO da funÁ„o ESCREVE_8vias ---------------
void ESCREVE_8vias(int DADO)
{
    DISPLAY_LCD = DADO;
    PULSE();
    __delay_us(100);
}
//-------------- FIM da funÁ„o ESCREVE_8vias ------------------


//-------------- INÕCIO da funÁ„o ESCREVE_4vias ---------------
void ESCREVE_4vias(int DADO)
{
//----- ENVIANDO A PARTE ALTA ---------
    int aux = (DADO & 0b11110000);
/*
    DISPLAY_LCD = aux;
    PULSE();
    __delay_ms(1);
 */
    PORTDbits.RD4 = aux & 0b00010000;
    PORTDbits.RD5 = aux & 0b00100000;
    PORTDbits.RD6 = aux & 0b01000000;
    PORTDbits.RD7 = aux & 0b10000000;
    PULSE();
    __delay_ms(1);
//----- PARTE ALTA ENVIADA ------------


//----- ENVIANDO A PARTE BAIXA --------
    aux = (DADO & 0b00001111);
    aux = aux << 4;

    /*DISPLAY_LCD = aux;
    PULSE();
    __delay_ms(1);
     */
    
    PORTDbits.RD4 = aux & 0b00010000;
    PORTDbits.RD5 = aux & 0b00100000;
    PORTDbits.RD6 = aux & 0b01000000;
    PORTDbits.RD7 = aux & 0b10000000;
    PULSE();
    __delay_ms(1);
    
//----- PARTE BAIXA ENVIADA ---------
}
//-------------- FIM da funÁ„o ESCREVE_4vias ------------------

//-------------- INÕCIO da funÁ„o INICIALIZANDO_LCD ---------------
void INICIALIZANDO_LCD()
{
// ---- * ----- * estabiliza√ß√£o do display * ------ * ----- * ------
    __delay_ms(20);

    RS = 0;
    ENABLE = 0;

    ESCREVE_8vias(0x30);
    __delay_ms(5);

    ESCREVE_8vias(0x30);
    __delay_us(200);

    ESCREVE_8vias(0x30);
    __delay_us(80);

    ESCREVE_8vias(0x20);
    __delay_us(80);
// ---- * ----- * estabiliza√ß√£o do display * ------ * ----- * ------

// **** a partir daqui o display est· configurado para comunicaÁ„o em 4 vias ****

    ESCREVE_4vias(0x28);  /*Estabelece as condiÁıes de utilizaÁ„o. Neste caso, comunicaÁ„o em quatro
vias, display de duas linhas e matriz de 7x5.*/
    __delay_us(80);

	ESCREVE_4vias(0x08);
    __delay_us(80);

    ESCREVE_4vias(0x01);  /*Comando para limpar o display e posicionar o cursor na primeira linha,
primeira coluna (esquerda).*/
    __delay_ms(4);

    ESCREVE_4vias(0x0C);//comando para ligar o display sem cursor.
    __delay_us(80);

    ESCREVE_4vias(0x0C); //comando para ligar o display sem cursor.
    __delay_us(80);
    
    ESCREVE_4vias(0x06); //comando para estabelecer o modo de entrada. Deslocamento autom·tico do cursor para a direita.
    __delay_ms(20);
}
//-------------- FIM da funÁ„o INICIALIZANDO_LCD ------------------

//-------------- INÕCIO da funÁ„o espera_tecla -----------------
void espera_tecla()
{
    __delay_ms(40); // debounce

    while( (SETA_CIMA == 0) || (SETA_BAIXO == 0) ||
           (ENTER == 0) )
    {

    };
    __delay_ms(40); // debounce
}
//-------------- FIM da funÁ„o espera_tecla ---------------------
 

//-------------- INÕCIO da funÁ„o INICIA_TXT_LCD -----------------
void INICIA_TXT_LCD()
{
	RS = 0;
	ESCREVE_4vias(0x01); // apaga todo o conte√∫do no LCD, posiciona o cursor no in√≠cio
	__delay_ms(20);
    
	RS = 1; // para o LCD reconhecer como letras o c√≥digo passado
    ESCREVE_4vias(' ');
    ESCREVE_4vias(' ');
	ESCREVE_4vias('C');
	ESCREVE_4vias('a');
	ESCREVE_4vias('p');
	ESCREVE_4vias('a');
	ESCREVE_4vias('C');
	ESCREVE_4vias('i');
	ESCREVE_4vias('t');
	ESCREVE_4vias('a');
	ESCREVE_4vias('n');
	ESCREVE_4vias('c');
	ESCREVE_4vias('i');
	ESCREVE_4vias('a');
}
//-------------- FIM da funÁ„o INICIA_TXT_LCD --------------------

//-------------- INÕCIO da funÁ„o MOSTRA_OPCAO_CAPACITANCIA -----------------
void MOSTRA_OPCAO_CAPACITANCIA(int cap_op)
{
	INICIA_TXT_LCD();
	
    RS = 0; 
	ESCREVE_4vias(0xC0); // posiciona cursor no inÌcio da segunda linha
	
	RS = 1;
	if(cap_op == 1)
    {
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias('0');
        ESCREVE_4vias(',');
        ESCREVE_4vias('5');
        ESCREVE_4vias('0');
        ESCREVE_4vias('0');
        ESCREVE_4vias('x');
        ESCREVE_4vias('C');
    }else if(cap_op == 2)
    {
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias('1');
        ESCREVE_4vias(',');
        ESCREVE_4vias('0');
        ESCREVE_4vias('0');
        ESCREVE_4vias('0');
        ESCREVE_4vias('x');
        ESCREVE_4vias('C');
    }else if(cap_op == 3)
    {
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias('1');
        ESCREVE_4vias(',');
        ESCREVE_4vias('5');
        ESCREVE_4vias('0');
        ESCREVE_4vias('0');
        ESCREVE_4vias('x');
        ESCREVE_4vias('C');
    }else if(cap_op == 4)
    {
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias(' ');
        ESCREVE_4vias('2');
        ESCREVE_4vias(',');
        ESCREVE_4vias('0');
        ESCREVE_4vias('0');
        ESCREVE_4vias('0');
        ESCREVE_4vias('x');
        ESCREVE_4vias('C');
    }else if(cap_op == 5)
    { 
		RS = 0;
		ESCREVE_4vias(0x01); // apaga todo o conte√∫do no LCD, posiciona o cursor no in√≠cio
		__delay_ms(20);
		ESCREVE_4vias(0xC0); // posiciona cursor no in√≠cio da segunda linha

		RS = 1;

		ESCREVE_4vias(' ');
		ESCREVE_4vias(' ');
		ESCREVE_4vias(' ');
		ESCREVE_4vias(' ');
		ESCREVE_4vias(' ');
		ESCREVE_4vias('A');
		ESCREVE_4vias('B');
		ESCREVE_4vias('E');
		ESCREVE_4vias('R');
		ESCREVE_4vias('T');
		ESCREVE_4vias('O');
    }
}
//-------------- FIM da funÁ„o MOSTRA_OPCAO_CAPACITANCIA --------------------

//-------------- INÕCIO da funÁ„o SELECIONA_CAPACITANCIA -----------------
void SELECIONA_CAPACITANCIA(int cap_op)
{
    // 0 -> rele ligado
    // 1 -> rele desligado
    if(cap_op == 1) // capacit‚ncia em 0,5*C
    {
        RELE_A1 = 0; // somente A1 e A2 ligados
        RELE_A2 = 0;
        RELE_B1 = 1;
        RELE_B2 = 1;
    }else if(cap_op == 2) // capacit‚ncia em 1,0*C
    {
        RELE_A1 = 0; // somente A1 ligado
        RELE_A2 = 1;
        RELE_B1 = 1;
        RELE_B2 = 1;
    }else if(cap_op == 3) // capacit‚ncia em 1,5*C
    {
        RELE_A1 = 0; // somente A1, B1 e B2 ligados
        RELE_A2 = 1;
        RELE_B1 = 0;
        RELE_B2 = 0;
    }else if(cap_op == 4) // capacit„Ø£ia em 2,0*C
    {
        RELE_A1 = 0; // somente A1 e B1 ligados
        RELE_A2 = 1;
        RELE_B1 = 0;
        RELE_B2 = 1;
    }else if(cap_op == 5) // circuito aberto
    {
        RELE_A1 = 1; // todos os rel√©s desligados
        RELE_A2 = 1;
        RELE_B1 = 1;
        RELE_B2 = 1;
    }
}
//-------------- FIM da funÁ„o SELECIONA_CAPACITANCIA --------------------

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*        	FUN«√O DE INICIALIZA«√O DAS VARI¡VEIS E SFRs          *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  ATRIBUI√á√ÉO DE VALORES INICIAIS √ÄS VARI√ÅVEIS CRIADAS NA RAM. */
void inic_regs(void)
{
    TRISA = 0x00;
    TRISB = 0x00; // TRIS configura as portas como entrada ou saÌda.
                        // 0 - saÌda
                        // 1 - entrada
    TRISC = 0b00000111; // bit 0,1 e 2 configurados como entrada para os botıes
    TRISD = 0x00; // porta D respons·vel por carregar os 4 bits de dados para o LCD
    TRISE = 0x00;
    ADCON1 = 0x07;
    OPTION_REG = 0x00;
    INTCON = 0x00;
    PORTA = 0b11111111;   //os relÈs iniciam desligados     
    PORTB = 0b11111111;   //o bot„o de seta para cima, baixo e enter est„o ligados,
                          //respectivamente aos bit 0, 1 e 2 da porta B.
    PORTC = 0b11111111;    
    DISPLAY_LCD = 0x00;        
    PORTE = 0b11111111;         //os bits 0,1 respons·veis pelo RS e ENABLE respectivamente

    INICIALIZANDO_LCD();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
*                     FUN«√O PRINCIPAL                            *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void main(void)
{
	__delay_ms(100);
    int cap_selecionada = 1;
    
    inic_regs();
    INICIA_TXT_LCD();
    MOSTRA_OPCAO_CAPACITANCIA(cap_selecionada); // inicia com capacit‚ncia em 0,5*C
    
    
    while(1)
    {
        __delay_ms(2); //estabiliza√ß√£o

		if(SETA_CIMA == 0)// seta pra cima pressionada - aumenta 0,5*C a capacit‚ncia no LCD
		{
            if(cap_selecionada == 5)
            {
                cap_selecionada = 1;
            }else
            {
                cap_selecionada = cap_selecionada + 1;
            }
            
            MOSTRA_OPCAO_CAPACITANCIA(cap_selecionada);
            espera_tecla(); // debounce e espera soltar a tecla pra voltar a executar o programa
		}else if(SETA_BAIXO == 0)// seta pra baixo pressionada - diminui 0,5*C a capacit√¢ncia no LCD
		{ 
            
			if(cap_selecionada == 1)
            {
                cap_selecionada = 5;
            }else
            {
                cap_selecionada = cap_selecionada - 1;
            }
            MOSTRA_OPCAO_CAPACITANCIA(cap_selecionada);
            espera_tecla(); // debounce e espera soltar a tecla pra voltar a executar o programa
		}else if(ENTER == 0)// tecla enter pressionado entrega na saÌda do circuito o valor de capacit‚ncia selecionado
		{
            SELECIONA_CAPACITANCIA(cap_selecionada);
            espera_tecla(); // debounce e espera soltar a tecla pra voltar a executar o programa
		}
    };
}