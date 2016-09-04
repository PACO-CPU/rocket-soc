#include<stdio.h>
#include <inttypes.h>
 
//#include <math.h>
double root1(int,int);
float power(double,int);
float powlib(int,int,int);
float powlib(int base, int exponent, int factor)
{
   float powerresult;
   if(base == 1)
     return (float)base;

   if(exponent == 0)
      return (float)1;

   if(exponent == 1)
     return (float)base;
  
   if(factor > 1 && exponent > 0) 
    {
       double root = root1(base,factor);
       //int expo = (exponent > 0) ? 1*exponent : (-1)*exponent;
       powerresult = power(root,exponent);
        
    }
   else if(exponent < 0 && factor > 1 )
    {
      double root = root1(base,factor);
      int expo = (exponent > 0) ? 1*exponent : (-1)*exponent;
      powerresult = power(root,expo);
      powerresult = 1/powerresult;
    }
     else if (factor == 1 && exponent <0)
     {
       int expo = (exponent > 0) ? 1*exponent : (-1)*exponent;
       powerresult = power((double)base, expo);
       powerresult = 1 / powerresult;
     }
     else if (factor == 1 && exponent >0)
     {
       powerresult = power((double)base, exponent);
     }
    
    return powerresult;

 
      
}



int main()
{
    int base;
    int exponent;
    int factor;
    printf("Enter base: ");
    scanf("%d",&base);
    printf("Enter the value for 'n'(the root to be calculated): ");
    scanf("%d",&exponent);
    printf("Enter the factor for: ");
    scanf("%d",&factor);
    float result = powlib(base,exponent,factor);
    printf("result is %f", result);

   //float number = power(num1,n);
   //printf("power is %f \n \n",number);
   //int base = (int)number;
   /*double root=  root1(num1,100);
   printf("root is %f \n \n",root);
   float number = power(root,n);
   printf("power is %f \n \n",number);*/
  
    return 0;
}
 
double root1(int a, int b)
{
    int j;
    double i,k=1.0;
    double incre = 0.000001;
 
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
            k=1.0;
    }
}
float power (double x,int n)
{
    int i;  /*Variable used in loop counter*/
    double val = 1.0;

    for (i = 0; i < n; i++)
        val *= x;

    return (float)(val);
   /*double value=1;
   while(n>0)
   {
    value*=x;
    --n;
   }
   return value;*/
}

/*int pow(int a, int e){
    if(e == 1) return a;
    return a * pow(a,e-1);
}*/
