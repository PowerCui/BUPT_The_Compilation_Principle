#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int siganl,state; //移进规约压入栈的符号，以及转移到的状态
int tstack[50]; //栈
int top=-1; //栈顶元素下标
char buffer[50]; //输入缓冲区
int number[50]; //将输入字符串转化为编码
int pos=0; //number数组下标
int X; //当前获取到的输入符号的编码
int flag=1; //循环标志

typedef struct Gra
{
    int left; //产生式左侧非终结符的编码
    int right[4]; //产生式右侧的编码串
    int len; //产生式右侧的长度
}Gra;

Gra pro[10]={
    //所有产生式
    //下标从1开始
    //SETF 101-104
    //$()+-*/num 01234567
    {0},
    {101,{0,102},1},/*S -> E*/
    {102,{0,102,3,103},3},/*E -> E+T*/
    {102,{0,102,4,103},3},/*E -> E-T*/
    {102,{0,103},1},/*E -> T*/
    {103,{0,103,5,104},3},/*T -> T*F*/
    {103,{0,103,6,104},3},/*T -> T/F*/
    {103,{0,104},1},/*T -> F*/
    {104,{0,1,101,2},3},/*F -> (E)*/
    {104,{0,7},1},/*F -> num*/
};

int GOTO[16][5]={/*LR分析表goto*/
    //多构造一列为了下标从1开始 SETF
    {0,0,1,2,3},/*0*/
    {0,0,0,0,0},/*1*/
    {0,0,0,0,0},/*2*/
    {0,0,0,0,0},/*3*/
    {0,0,10,2,3},/*4*/
    {0,0,0,0,0},/*5*/
    {0,0,0,11,3},/*6*/
    {0,0,0,12,3},/*7*/
    {0,0,0,0,13},/*8*/
    {0,0,0,0,14},/*9*/
    {0,0,0,0,0},/*10*/
    {0,0,0,0,0},/*11*/
    {0,0,0,0,0},/*12*/
    {0,0,0,0,0},/*13*/
    {0,0,0,0,0},/*14*/
    {0,0,0,0,0},/*15*/
};

int ACTION[16][8]={/*LR分析表action*/
    //acc为999
    //51期待(或运算对象首字符，但出现运算符或者$
    //52括号不匹配，删掉右括号
    //53期待运算符号，但出现(或运算对象
    //S为正数，R为负数
    //$()+-*/num 01234567
    {51,4,52,51,51,51,51,5},//0
    {999,53,52,6,7,0,0,53},//1
    {-4,53,-4,-4,-4,8,9,53},//2
    {-7,0,-7,-7,-7,-7,-7,0},//3
    {0,4,52,51,51,51,51,5},//4
    {-9,0,-9,-9,-9,-9,-9,0},//5
    {51,4,52,51,51,51,51,5},//6
    {51,4,52,51,51,51,51,5},//7
    {51,4,52,51,51,51,51,5},//8
    {51,4,52,51,51,51,51,5},//9
    {0,53,15,6,7,0,0,53},//10
    {-2,53,-2,-2,-2,8,9,53},//11
    {-3,53,-3,-3,-3,8,9,53},//12
    {-5,0,-5,-5,-5,-5,-5,0},//13
    {-6,0,-6,-6,-6,-6,-6,0},//14
    {-8,0,-8,-8,-8,-8,-8,0}//15
};

void Transform(); //将读入缓冲区的字符数组转换为相应的编码
void GetNumber(); //获取当前输入符号串的元素
void Push(int A,int s); //符号、状态入栈
void Pop(); //出栈
void Shift(); //移进
void Reduce(); //规约
void Acc(); //accept
//错误处理
//51期待(或运算对象首字符，但出现运算符或者$
//52括号不匹配，删掉右括号
//53期待运算符号，但出现(或运算对象
void Err1();
void Err2();
void Err3();

int main()
{
    cout<<"请输入一个待分析串，以$结束"<<endl;
    scanf("%s",buffer);
    Transform(); //将字符转化为编码
    Push(0,0); //状态S0入栈
    GetNumber(); //从已经变成编码的buffer读取一个字符
    while(flag)
    {
        //cout<<tstack[top]<<" "<<X<<endl;
        if(ACTION[tstack[top]][X]>0 && ACTION[tstack[top]][X]<50)
        {
            //进行移进操作
            Shift();
            GetNumber();
        }
        else if(ACTION[tstack[top]][X]<0)
        {
            //进行规约操作
            Reduce();
        }
        else if(ACTION[tstack[top]][X]==999)
        {
            //accept
            Acc();
        }
        else
        {
            switch(ACTION[tstack[top]][X])
            {
                case 51:
                    
                    Err1();
                    break;
                case 52:
                    Err2();
                    GetNumber();
                    break;
                case 53:
                    Err3();
                    GetNumber();
                    break;
            }
        }  
    }
    system("pause");
    return 0;
}

void Transform() //将读入缓冲区的字符数组转换为相应的编码
{
    //把读入的字符串转化为编码存在number这个编码数组
    int i=0,j=0; //i为buffer下标，j为number下标
    while(buffer[i]!='$')
    {
        if(buffer[i]>=48 && buffer[i]<=57)
        {
            //当前字符为数字，num
            while(buffer[i]>=48 && buffer[i]<=57)
                i++;
            number[j++]=7;
        }
        else
        {
            switch (buffer[i++])
            {
            //$()+-*/num 01234567
            case '&':
                number[j++]=0;
                break;
            case '(':
                number[j++]=1;
                break;
            case ')':
                number[j++]=2;
                break;
            case '+':
                number[j++]=3;
                break;
            case '-':
                number[j++]=4;
                break;
            case '*':
                number[j++]=5;
                break;
            case '/':
                number[j++]=6;
                break;
            }
        }
        
    }
    number[j]=0;
}
void GetNumber() //获取当前输入符号串的元素
{
    X=number[pos++];
}
void Push(int A,int s) //符号、状态入栈
{
    top++;
    tstack[top]=A; //符号A入栈
    top++;
    tstack[top]=s; //状态S入栈
}
void Pop() //出栈
{
    top--;
    top--;
}
void Shift() //移进
{
    int temp;
    temp=ACTION[tstack[top]][X]; //查找表，确定需要移入的状态
    Push(X,temp); //当前读入字符与状态入栈
    printf("S%d\t移进.\n",temp);
}
void Reduce() //规约
{
    int x,y,z;
    x=-ACTION[tstack[top]][X];
    printf("R%d\t规约\t",x);
    switch (pro[x].left)
    {
    case 101:
        cout<<"S -> ";
        break;
    case 102:
        cout<<"E -> ";
        break;
    case 103:
        cout<<"T -> ";
        break;
    case 104:
        cout<<"F -> ";
        break;
    }
    for(int i=1;i<=pro[x].len;i++)
    {
        Pop(); //将栈中元素，按照产生式右边的长度依次弹出
        switch (pro[x].right[i])
        {
            case 0: 
                printf("$");
                break;
            case 1: 
                printf("(");
                break;
            case 2: 
                printf(")");
                break;
            case 3: 
                printf("+");
                break;
            case 4: 
                printf("-");
                break;
            case 5: 
                printf("*");
                break;
            case 6: 
                printf("/");
                break;
            case 7: 
                printf("num");
                break;
            case 101: 
                printf("S");
                break;
            case 102: 
                printf("E");
                break;
            case 103: 
                printf("T");
                break;
            case 104: 
                printf("F");
                break;
        }
    }
    cout<<endl;
    y=GOTO[tstack[top]][pro[x].left-100];
    //cout<<pro[x].left<<y<<endl;
    Push(pro[x].left,y);
    //将规约产生式的左部压入栈，并将转移状态也压栈
}
void Acc() //accept
{
    flag=0; //不再循环
    cout<<"ACC!"<<endl;
}
void Err1()
{
    //期待(或运算对象首字符，但出现运算符或者$
    printf("error1\t\t\t缺少运算符，将num入栈.\n");
    Push(7,5);
}
void Err2()
{
    //括号不匹配，删掉右括号 
    printf("error2\t\t\t括号不匹配，请删除右括号.\n");
}
void Err3()
{
    int pointer=pos;
    switch(tstack[top])
    {
        case 1:
            Push(3,6);
            printf("error3\t\t\t缺少操作符，请添加操作符到栈里.");
            break;
        case 2: 
        case 12:
        case 13:
            Push(5,8);
            printf("error3\t\t\t缺少操作符，请添加操作符到栈里.");
            break;
        case 11:
            if(number[pointer]>=3 && number[pointer]<=6)
            {
                //期待运算符
                Push(2,15);
                printf("e3\t\t\t缺少右括号，请添加右括号到栈.");
            }
            else if(number[pointer]==7)
            {
                Push(3,6);
                printf("error3\t\t\t缺少操作符，请添加操作符到栈里.");
            }
            break;
    }
}