#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

using namespace std;

#define LEN 255
bool flaP=true;
char src[LEN];
int i;
void E();
void T();
void P();
void F();
void Q();

int main()
{
    printf("请输入待检测语句串，以$结束）:\n");
    
    scanf("%s",&src);

    i=0;
    E();
    if(src[i]=='$'&&flaP==true)
    {
        printf("语句合法\n");
    }
    else
    {
        printf("不合法\n");
    } 
    system("pause");
    return 0;
}

void E()
{
    printf("E->TP\n");
    T();
    P();
    
}

void T()
{
    printf("T->FQ\n");
    F();
    Q();
}
void P()
{
    if(src[i]=='+')
    {
        i++;
        printf("P->+TP\n");
        T();
        P();
    }
    else if(src[i]=='-')
    {
        printf("P->-TP\n");
        i++;
        T();
        P();
    }
    
}

void F()
{
    if(src[i]=='(')
    {
        i++;
        E();
        if(src[i]==')')
        {   
            i++;
            printf("F->(E)\n");
        }
        else 
            flaP=false;
    }
    else if(src[i]>='0'&&src[i]<='9')
    {   
        printf("F->num\n");
        i++;
    }
    else 
        flaP=false;
}

void Q(){
    
    if(src[i]=='*')
    {
        i++;
        F();
        Q();
    }
    else if(src[i]=='/')
    {
        i++;
        F();
        Q();
    }

}

