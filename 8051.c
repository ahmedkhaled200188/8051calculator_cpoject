#include <reg51.h> // include 8051 header file
#define row P0 // define row as Port 0
#define col P3 // define col as Port 3

#define ldata P1 // define ldata as Port 1
sbit rs=P2^0; // define rs as bit 0 of Port 2
sbit rw=P2^1; // define rw as bit 1 of Port 2
sbit en=P2^2; // define en as bit 2 of Port 2



unsigned char keypad[4][4]={'7','8','9','/','4','5','6','*','1','2','3','-','c','0','=','+'};												
unsigned char result;			 // variable to hold the result of a key press
unsigned char operation=' ';  // variable to hold the current operation (+, -, *, /)
unsigned char i=0; // loop counter
unsigned char count=0;  // counter for storing result digits in an array
unsigned char arr_result[10]; // array to hold the digits of the result
int num1=0; // variable to hold the first number in an operation
int num2=0; // variable to hold the second number in an operation

//variables to Handle user Errors  
unsigned char is_operation = 1; // flag to indicate if an operation was pressed
unsigned char is_second_number = 0; // flag to indicate if the second number in an operation has been entered
unsigned char is_result = 0; // flag to indicate if a result has been displayed

// function for delay 
void msdelay(unsigned int time)
{
	unsigned int i ,z;
	for(i=0;i<time;i++)
	{
		for(z=0;z<1275;z++);
	
	}
}
	

// function to send a command to the LCD display
void lcdcmd(unsigned char value)
{

	ldata=value;
	rs=0; // set rs to 0 to indicate a command
	rw=0;
	en=1; // pulse the enable bit to latch the command
	msdelay(1);
	en=0;
	return;
}

// function to send data to the LCD display
void lcddata(unsigned char value)
{

	ldata=value;
	rs=1; // set rs to 1 to indicate data
	rw=0;
	en=1; // pulse the enable bit to latch the data
	msdelay(1);
	en=0;
	return;
}





//function to scan keypad_check and return the pressed key
char keypad_check(unsigned char colloc,unsigned char rowloc )
{
		do{
		row=0x00;
		colloc=col;
		colloc&=0x0F;
	}while(colloc!=0x0f);
	do{
	do{
	
			msdelay(10);
			colloc =col&0x0f; //0111
	}while(colloc == 0x0f);
			msdelay(10);
			colloc =col&0x0f;
	}while(colloc == 0x0f);
	
	while(1)
	{
	
		row=0xFE;
		colloc=col&0x0f;
		if(colloc != 0x0f)
		{
			rowloc=0;
			break;
		}
		row=0xFD;
		colloc=col&0x0f;
		if(colloc != 0x0f)
		{
			rowloc=1;
			break;
		}
		row=0xFB;
		colloc=col&0x0f;
		if(colloc != 0x0f)
		{
			rowloc=2;
			break;
		}
		row=0xF7;
		colloc=col&0x0f;
		if(colloc != 0x0f)
		{
			rowloc=3;
			break;
		}
	}
	
	
	if(colloc == 0x0E)
	{
	
		result = keypad[rowloc][0];
	}
	else if(colloc == 0x0D)
	{
	
		result = keypad[rowloc][1];
	}
	else if(colloc == 0x0B)
	{
	
		result = keypad[rowloc][2];
	}
	else if(colloc == 0x07)
	{	
	
		result = keypad[rowloc][3];
	}

	return result;

}





// function to perform an arithmetic operation and return the result
int calculate (int num1 , int num2, char operation)
{
	int result;
	int temp;

	switch(operation)
	{
		case '+': 
			result = num1+num2;
		break;
		case '-': 
			result = num1-num2;
		break;
		case '/': 
			result = num1/num2;
		break;
		case '*': 
			result = num1*num2;
		break;
	}
	temp=result;
	//count=0;

	// check if result is negative
	if (result < 0) {
		lcddata('-'); // display negative sign
		result *= -1; // convert result to positive for display
	} else if(result==0){
		return result;
	}
	temp=result;
	while(result !=0)
	{
		arr_result[count]=result%10;
		result=result/10;
		count++;
	}  
	
	
	return temp;

}






void main()
{
	unsigned char colloc,rowloc;  // variables to hold the column and row values of the pressed key
	col=0xff;
	
	lcdcmd(0x38); // initialize the LCD display
	msdelay(30);
	lcdcmd(0x0C);
	msdelay(30);
	lcdcmd(0x01);
	msdelay(30);
	lcdcmd(0x06);
	msdelay(30);
	lcdcmd(0xc0);
	msdelay(30);
	
	while(1)
	{
		 // scan the keypad and get the pressed key
		result = keypad_check( colloc, rowloc ); 
		// if the press is an number.
		if(result != '/' && result != '*'  && result != '+' && result != '-' && result != '=' && result != 'c'	)
		{	
			 // clear the display if a result has already been displayed
				if(is_result==1)
				{
				  result = 'c';
				
				}else{
			    num1=num1*10;
	    		num1 = num1 + (int)(result-'0');
		    	lcddata(result);
		    	msdelay(35);	
		    	is_operation=0;
		    	is_second_number=1;
				
				}
			
		}// if the press is an operation.
		else if(result == '/' || result == '*'  ||result == '+'|| result == '-' )
		{		
			//check if user enter first press an operation so clear 
			if(is_operation ==1)
			{
				result = 'c';
			}else {
			operation=result;
			num2=num1;
			num1=0; // to start storing the second number after operation
			lcddata(result);
			msdelay(35);	
			//to make sure that user doesn't enter 2 opeations in row
			is_operation=1;
			is_second_number=0;
			}
		}  
		else if (result == 'c') // if the clear key was pressed, reset all variables and clear the display
		{
			num1=0;
			num2=0;
			count=0;
			is_operation=1;
			is_second_number=0;
			is_result=0;
			lcdcmd(0x01);
			msdelay(30);
			lcdcmd(0x06);
			msdelay(30);
			lcdcmd(0xc0);
			msdelay(30);
			lcdcmd(0x38);
			msdelay(30);
		}
		 // if the equals key was pressed, calculate and display the result
		else if(result == '=')
		{
			
			//to make sure that operation and second number is enter to get result 
			if((operation != ' ') && (is_second_number != 0)) 
			{
			lcddata(result); // display '='
			msdelay(35);
			result = (char)calculate(num2,num1,operation ) +'0'; // display result of operation
				if(result =='0')
				{
					lcddata('0');
					msdelay(35);
				}else{
					count--;
		    	while(count !=0)
					{
						result = (char) arr_result[count] +'0';
						arr_result[count]=0; // zero the array for the any next operation
						count--;
						lcddata(result);
			
					}
						result = (char) arr_result[0] +'0'; //display index 0 
						lcddata(result);
						msdelay(35);	
				}
			
			is_operation=1;
			operation=' ';
			is_result=1;
			
			}else {
			
			}
			
		}
	}
}


