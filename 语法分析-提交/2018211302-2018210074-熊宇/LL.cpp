#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NumOfPro 10 //产生式个数
#define MaxLenOfPro 8 //产生式最大长度
#define NumOfEnd 8 //终结符个数
#define NumOfNonEnd 5 //非终结符个数
#define MaxOfF 8 //FIRST\FOLLOW集大小
#define MaxLenOfStr 100 //待输入的语句的最大长度
#define MaxLenOfNum 10 //待输入表达式中num的最大长度
#define ERROR -1 //分析表中error
#define SYNCH -2 //分析表中synch
#define StackInitialLen 10 //栈初始大小
#define StackInpleLen 5 //栈空间增量
#define NUM 1 //数字标记
#define OTHER 0 //其他字符标记
#define TRUE 1
#define FALSE 0
#define NOTFOUND -1
#define OVERFLOW -2

using namespace std;

typedef struct stack
{
    char* top; //栈顶指针
    char* bottom; //栈底指针
    int sizeofstack; //栈大小
}stack;

//一些变量
char grammar[NumOfPro][MaxLenOfPro]; //文法集
char End[NumOfEnd]; //终结符集
char nonend[NumOfNonEnd]; //非终结符表
char FIRST[NumOfNonEnd][MaxOfF]; //FIRST集
char FOLLOW[NumOfNonEnd][MaxOfF]; //FOLLOW集
int L[NumOfNonEnd][NumOfEnd]; //语法预测分析表
char str[MaxLenOfStr+1]; //语句输入缓冲区
stack s; //栈

//LL(1)文法分析器函数
void InitialLL(); //初始化
void ProduceAnalysis(); //生成预测分析表
void GetString(); //获取输入的语句串
void Analysis(); //使用预测分析表进行分析

//分析函数
int GetStrLen(); //获取输入语句的长度
int JudgeEnd(char ch); //判断字符是否为终结符
int JudgeNonEnd(char ch); //判断字符是否为非终结符
int GetEndNum(char ch); //获取终结符ch在终结符集合里的下标
int GetNonEndNum(char ch); //获取非终结符ch在非终结符集合里的下标
int JudgeFirst(char A, char ch); //判断ch是否在A的FIRST集合里
int JudgeFollow(char A, char ch); //判断ch是否在A的FOLLOW集合里面
void PrintPro(int i); //打印产生式
void PrintStack(); //打印当前栈中符号
void PrintBuffer(int num); //打印当前输入缓冲区中的内容
int JudgeNum(char ch); //判断是否为num
int JuegeOverflow(); //判断是否溢出

//栈操作
void InitStack(); //构造一个空栈
void Push(char ch); //将字符ch压入栈顶
void Pop(); //弹出栈顶字符
char GetTop(); //获取栈顶字符

int main()
{
    InitialLL(); //初始化LL
    ProduceAnalysis(); //构造预测分析表
    for(int i=0;i<NumOfNonEnd;i++)
    {
        for(int j=0;j<NumOfEnd;j++)
        {
            cout<<nonend[i]<<" "<<End[j]<<" ";
            PrintPro(L[i][j]);
            cout<<endl;
        }
    }
    GetString(); //获取输入
    Analysis(); //分析
    system("pause");
    return 0;
}

//栈操作
void InitStack() //构造一个空栈
{
    s.bottom=(char *)malloc(StackInitialLen*sizeof(char));
    if(!s.bottom)
        exit(OVERFLOW);
    s.top=s.bottom;
    s.sizeofstack=StackInitialLen;
}
void Push(char ch) //将字符ch压入栈顶
{
    if(s.top-s.bottom >= s.sizeofstack)
    {
        //栈不够了，需要增加空间
        s.bottom=(char *)realloc(s.bottom,(s.sizeofstack+StackInpleLen)*sizeof(char));
        if(!s.bottom)
            exit(OVERFLOW);
        s.top=s.bottom+s.sizeofstack;
        s.sizeofstack+=StackInpleLen;
    }
    *s.top++=ch; //压栈
}
void Pop() //弹出栈顶字符
{
    if(s.bottom!=s.top)
        s.top--;
}
char GetTop() //获取栈顶字符
{
    char ch;
    if(s.bottom!=s.top)
        ch=*(s.top-1);
    return ch;
}

//LL(1)文法分析器函数
void InitialLL() //初始化
{
    int i, j;   
    InitStack();
    Push('$');
    Push('E');  //初始化栈：构造空栈，并压入'$'与'E' 

    //设置文法表，存储P代表E'，Q代表T'，e代表ε,n代表num 
    strcpy(grammar[0], "E#TP#");  //E →TE'
    strcpy(grammar[1], "P#+TP#");  //E' →+TE'
    strcpy(grammar[2], "P#-TP#");  //E' →-TE'
    strcpy(grammar[3], "P#e#");  //E' →ε
    strcpy(grammar[4], "T#FQ#");  //T →FT'
    strcpy(grammar[5], "Q#*FQ#");  //T' →*FT'
    strcpy(grammar[6], "Q#/FQ#");  //T' →/FT' 
    strcpy(grammar[7], "Q#e#");  //T' →ε
    strcpy(grammar[8], "F#(E)#");  //F →(E)
    strcpy(grammar[9], "F#n#");  //F →num
    
    for (i = 0; i < NumOfNonEnd; i++)  //LL(1)语法预测分析表初始化：所有表项置为错误ERROR
        for (j = 0; j < NumOfEnd; j++)
            L[i][j] = ERROR;   
    strcpy(End, "+-*/()n$");  //初始化终结符表  
    strcpy(nonend, "EPTQF");  //初始化非终结符表   
    
    //初始化FIRST集
    strcpy(FIRST[0], "(n#");
    strcpy(FIRST[1], "+-e#");
    strcpy(FIRST[2], "(n#");
    strcpy(FIRST[3], "*/e#");
    strcpy(FIRST[4], "(n#");
    
    //初始化FOLLOW集 
    strcpy(FOLLOW[0], ")$#");
    strcpy(FOLLOW[1], ")$#");  
    strcpy(FOLLOW[2], "+-)$#");  
    strcpy(FOLLOW[3], "+-)$#");  
    strcpy(FOLLOW[4], "+-*/)$#");
}
int GetStrLen() //获取输入语句的长度
{
    int count=0;
    for(count=0;str[count]!='\0';count++);
    return count;
}
int JudgeEnd(char ch) //判断字符是否为终结符
{
    if (ch=='+' || ch=='-' || ch=='*' || ch=='/' || ch=='(' || ch==')' ||  ch=='n' || ch=='$')
        return TRUE;
    else
        return FALSE; 
}
int JudgeNonEnd(char ch) //判断字符是否为非终结符
{
    if (ch=='E' || ch=='P' || ch=='T' || ch=='Q' || ch=='F')
        return TRUE;
    else
        return FALSE;
}
int GetEndNum(char ch) //获取终结符ch在终结符集合里的下标
{
    int i=0;
    for(i=0;i<NumOfEnd;i++)
    {
        if(ch==End[i])
            return i;
    }
    return NOTFOUND;
}
int GetNonEndNum(char ch) //获取非终结符ch在非终结符集合里的下标
{
    int i=0;
    for(i=0;i<NumOfNonEnd;i++)
    {
        if(ch==nonend[i])
            return i;
    }
    return NOTFOUND;
}
int JudgeFirst(char A, char ch) //判断ch是否在A的FIRST集合里
{
    int i,j;
    i=GetNonEndNum(A);
    for(j=0;FIRST[i][j]!='#';j++)
    {
        if(ch==FIRST[i][j])
            return TRUE;
    }
    return FALSE;
}
int JudgeFollow(char A, char ch) //判断ch是否在A的FOLLOW集合里面
{
    int i,j;
    i=GetNonEndNum(A);
    for(j=0;FOLLOW[i][j]!='#';j++)
    {
        if(ch==FOLLOW[i][j])
            return TRUE;
    }
    return FALSE;
}
void PrintPro(int i) //打印产生式
{
    printf ("  当前产生式为 :   "); 
    switch (i)  //根据产生式的标号打印出对应是输出产生式 
	{
        case 0:
            printf ("E → TE'\n");
            break;
        case 1:
            printf ("E' → +TE'\n");
            break;
        case 2:
            printf ("E' → -TE'\n");
            break;
        case 3:
            printf ("E' → ε\n");
            break;
        case 4:
            printf ("T → FT'\n");
            break;
        case 5:
            printf ("T' → *FT'\n");
            break;
        case 6:
            printf ("T' → /FT'\n");
            break;
        case 7:
            printf ("T' → ε\n");
            break;
        case 8:
            printf ("F → (E)\n");
            break;
        case 9:
            printf ("F → num\n");
            break;
        default:
            break; 
    }             
}
void PrintStack() //打印当前栈中符号
{
    char *sp = NULL;  
    printf ("  栈： ");
    for (sp = s.bottom; sp != s.top; sp++) 
	{
        switch (*sp) 
		{
            case 'P':  //将P转换为E'输出 
                printf ("E'");
                break;
            case 'Q':  //将Q转换为T'输出  
                printf ("T'");
                break;
            case 'n':  //将n转换为num输出 
                printf ("num");
                break;                 
            default:  //其他情况直接输出 
                printf ("%c", *sp);
                break;
        }
    }
    printf ("\n"); 
}
void PrintBuffer(int num) //打印当前输入缓冲区中的内容
{
    int i;
    printf ("  输入： ");
    for (i = num; str[i] != '$'; i++)
        printf ("%c", str[i]);
    printf ("$\n"); 
}
int JudgeNum(char ch) //判断是否为num
{
    if (ch >= '0' && ch <= '9')
        return TRUE;
    else
        return FALSE; 
}
int JuegeOverflow() //判断是否溢出
{
    int ip, bp, width;
    char a;
    int flag1 = OTHER, flag2 = OTHER;  //字符类型标记
    int toolong = FALSE;  //过长标记 
    for (ip = 0; (str[ip] != '\0') && (! toolong); ip += width)
	{
        a = str[ip];
        if (a=='+' || a=='-' || a=='*' || a=='/' || a=='(' || a==')' || a=='$')
            width = 1;
        else  //对输入表达式中代表num的子串的长度进行判断 
		{
            bp = ip; 
            width = 0;
            if (JudgeNum(a))
                flag1 = flag2 = NUM;
            bp ++;
            do  //获取输入表达式中代表num的子串的长度 
			{
                a = str[bp];
                width ++;
                if (width > MaxLenOfNum)  //若大于子串的最大长度，标记过长并跳出循环
				{ 
                    toolong = TRUE;
                    break;
                }
                if (JudgeNum (a))
                    flag2 = NUM;
                else
                    flag2 = OTHER;
                bp ++;
            } while (flag1 == flag2);  //两个标记相等，即子串没有结束，继续循环 
        }
    } 
    return toolong;  //返回过长标记
}
void ProduceAnalysis() //生成预测分析表
{
    int i, j;
    int n1, n2;
    char ch, A;
    for (i = 0; i < NumOfPro; i++)   //对于每个产生式A →α 
	{
        A = grammar[i][0];
        ch = grammar[i][2];
        if (JudgeNonEnd (ch))  //若a∈ FIRST(α)，M[A,a]中应放入产生式A →α
		{
            for (j = 0; j < NumOfEnd; j++)
			{
                if (JudgeFirst (ch, End[j])) 
				{
                    n1 = GetNonEndNum(A);
                    L[n1][j] = i; 
                }
            }
        }
        else if (JudgeEnd(ch)) 
		{
            n1 = GetNonEndNum(A);
            n2 = GetEndNum(ch); 
            L[n1][n2] = i; 
        }
        else if (ch == 'e')  //若ε∈FIRST(α)，且b∈FOLLOW(A)，M[A,b]中应放入产生式A →α 
		{
            n1 = GetNonEndNum(A);
            for (j = 0; FOLLOW[n1][j] != '#'; j++) 
			{
                n2 = GetEndNum(FOLLOW[n1][j]);
                L[n1][n2] = i; 
            }
        }              
    }
    for (i = 0; i < NumOfNonEnd; i++)  //置同步出错信息 
	{
        for (j = 0; FOLLOW[i][j] != '#'; j++)  //若b∈FOLLOW(A)，且M[A,b]为ERROR，则把M[A,b]赋值为为同步信息SYNCH
		{
            n1 = GetEndNum(FOLLOW[i][j]);
            if (L[i][n1] == ERROR)
                L[i][n1] = SYNCH;
        }
    }
}
void GetString() //获取输入的语句串
{
    int len = 0;  //待分析的输入表达式的长度 
    int flag = FALSE;
    do 
	{
        printf ("请输入待分析语句串，用'$'结束:\n");
        scanf ("%s", &str);
        len = GetStrLen ();
        if (str[len-1] != '$')  //若每输入结尾符'$'，将其补在待分析输入表达式的最后 
		{
            str[len] = '$';
            str[len+1] = '\0';
            len ++;
        }
        if (len > MaxLenOfStr)  //判断输入表达式是否过长 
            printf ("语句过长，请重新输入！\n");
        flag = JuegeOverflow();  //判断输入表达式中代表num的子串是否过长
        if (flag)
            printf ("数字过长，请重新输入！\n"); 
    } while (len > MaxLenOfStr || flag);
}
void Analysis() //使用预测分析表进行分析
{
    int ip = 0;  //输入缓冲区指针 
    int step = 0;  //分析步数 
    char X, a, c;
    int i, j, n1, n2, bp;
    int width = 0;  //输入表达式中代表num的子串的长度 
    char b[MaxLenOfNum+1];  //存储输入表达式中代表num的子串 
    int flag1 = OTHER, flag2 = OTHER;  //字符类型标记 
    do 
	{
        step++;
        printf ("Step %d :\n", step);
        X = GetTop ();  //获取栈顶符号 
        a = str[ip];  //获取输入串中将要进行分析的符号
        for (j = 0; j <= MaxLenOfNum; j++)  //对存储输入串中代表num的子串的数组进行初始化 
            b[j] = '\0'; 
        if (a=='+' || a=='-' || a=='*' || a=='/' || a=='(' || a==')' || a=='$')  //对输入串中将要进行分析的符号进行处理 
		{
            width = 1;
            b[0] = a;
        } 
        else  //处理输入表达式中代表num的子串，将它们转化为'n'进行分析     
		{
            bp = ip; 
            width = 0;    
            if (JudgeNum (a))
                flag1 = flag2 = NUM;
            bp ++;
            c = a;
            do  //获取输入表达式中代表num的子串，将其存入数组b 
			{
                width ++;
                b[width-1] = c;
                c = str[bp];
                if (JudgeNum (c))
                    flag2 = NUM;
                else
                    flag2 = OTHER;
                bp ++;
            } while (flag1 == flag2);  //两个标记相等，即子串没有结束，继续循环 
            
            if (flag1 == NUM)
                a = 'n';    
        }
        PrintStack();  //打印当前栈中的符号
        PrintBuffer(ip);  //打印当前输入缓冲区中的符号串          
        if (JudgeEnd(X))  //栈顶符号是终结符：不论正确与否，都弹出栈顶符，ip前移 
		{
            if (X == a) 
			{
                Pop();
                ip += width;
            }
            else  //若栈顶终结符与ip指向的字符不匹配，提示错误 
			{
                Pop();
                ip += width;
                printf ("  输出：错误 !\n");
            }
        }
        else  //栈顶符号是非终结符 
		{
            n1 = GetNonEndNum(X);
            n2 = GetEndNum(a);
            if (L[n1][n2] != ERROR && L[n1][n2] != SYNCH)  //正确情形：分析表项M[X][a]是非终结符X的一个产生式 
			{
                Pop();  //弹出栈顶符 
                i = L[n1][n2];
                if (grammar[i][2] != 'e')  //如果产生式右边不是ε，将对应产生式的右边逆序压入栈中 
				{
                    for (j = 2; grammar[i][j] != '#'; j++);  //将对应产生式的右边逆序压入栈中 
                    for (j--; j >= 2; j--)
                        Push(grammar[i][j]);
                }
                PrintPro(i);  //输出产生式  
            }
            else if (L[n1][n2] == ERROR)  //分析表项M[X][a]为空，ip前移，跳过当前输入字符（串） 
			{
                ip += width;
                printf ("  输出：错误 ! 跳过 %s\n", b);
            } 
            else if (L[n1][n2] == SYNCH)  //分析表项M[X][a]为同步信息，则弹出栈顶符
			{
                Pop();
                printf ("  输出：错误 ! 弹栈 ");
                switch (X)
				{
                    case 'P':  //将P转换为E'输出 
                        printf ("E'");
                        break;
                    case 'Q':  //将Q转换为T'输出  
                        printf ("T'");
                        break;              
                    default:  //其他情况直接输出 
                        printf ("%c", X);
                    break;
                }
                printf ("\n");                
            } 
        }         
    } while (X != '$');
}