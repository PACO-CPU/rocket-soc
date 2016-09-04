double root(int a, int b)
{
    int j;
    double i,k=1;
    double incre = 0.0000001;
    for(i=1; i<=a; i = i+incre)
    {
        for(j=0;j<b;j++)
        {
            k=k*i;
        }
        if(a<k)
        {
            return(i-incre);
            break;
        }
        else
            k=1;
    }
}


float pow(int x,int y)
{
    int i; /* Variable used in loop counter */
    float number = 1.0;

    for (i = 0; i < y; ++i)
        number *= x;

    return(number);
}


unsigned int absolute(signed int y)
{

  return y * ( (y<0) * (-1) + (y>0));
  // simpler: y * ((y>0) - (y<0))   

}

int main()
{
    int number,answer1;
    signed int exponent_n,exponent_d;
    float answer;
    int reciprocal,fraction;



    printf("Enter base (x in x^[n/d]): ");
    scanf("%d",&number);

    printf("Enter exponent_n (n in x^[n/d]): ");
    scanf("%d",&exponent_n);

    printf("Enter exponent_d (d in x^[n/d]): ");
    scanf("%d",&exponent_d);



printf("expo: %d/%d \n", exponent_n,exponent_d);

	reciprocal = (exponent_n<0)? 1 : 0;
	
	fraction =  (exponent_d!=1)? 1 : 0;



//calculating and printing answer


    	if(reciprocal==1 && fraction==1){
		
		exponent_n = absolute(exponent_n);
		answer1 = pow(number,exponent_n);
		answer =  root(answer1,exponent_d);	
		printf("x^y : %f\n",1/answer);   
                printf("\nflag1\n");
	
	}else if(reciprocal==1 && fraction==0){

		exponent_n = absolute(exponent_n);	
		answer = pow(number,exponent_n); 	
		printf("x^y : %f\n",1/answer);  printf("\nflag2\n");

	}else if(reciprocal==0 && fraction==1){
		
		answer1 = pow(number,exponent_n);
		answer =  root(answer1,exponent_d);
		printf("x^y : %f\n",answer);  printf("\nflag3\n");

	}else{

		answer = pow(number,exponent_n); 	
		printf("x^y : %f\n",answer);  printf("\nflag4\n");
    
	}

    return 0;
}

