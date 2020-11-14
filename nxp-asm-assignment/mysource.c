#include <MKL25Z4.H>



extern  void Fxn1(void);  //if-else,for-loop,while loop
extern int Fxn(int a,int b, int c);  //returns maxvalue in params
extern int specialFxn(int n);		//returns sum of 1-10

extern int Fxn5();		//uses initialized data/array. returns the max value
extern int Fxn4(void);

		int a = 7;
		int b = 5;
		int c = 3;
int main(){
	

		Fxn1();

		int max = Fxn(a,b,c);

    int m=Fxn5();
	  int summ=Fxn4();



}

