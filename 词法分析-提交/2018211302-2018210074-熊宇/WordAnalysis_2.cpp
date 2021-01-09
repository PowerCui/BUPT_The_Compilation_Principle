#include <iostream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include <map>

using namespace std;

int NumOfKey = 11; //内置关键字数目
char *KeyWord[] = {
    "for",
    "while",
    "do",
    "continue",
    "if",
    "else",
    "char",
    "int",
    "double",
    "float",
    "return"}; //内置关键字数组

char *FName = "F:\\Compile\\WordAnalysis\\test.txt";
char Buffer[1026];     //存储一行
char Output[200];      //存储输出
int NumOfLetter = 0;   //字符数目
int NumOfKeyWord = 0;  //关键字数目
int NumOfVarible = 0;  //变量名数目
int NumOfNumber = 0;   //数字常量数目
int NumOfOperator = 0; //分界符和操作符数目

//位置回退
void GoBack(char *&s)
{
    s--;
}

//获取下一个字符
char GetAnother(char *&s)
{
    return *s++;
}

//判断是否是数字
bool IsNum(char s)
{
    if ((s >= '0') && (s <= '9'))
        return true;
    else
        return false;
}

//判断是否为字母
bool IsLetter(char s)
{
    if ((s >= 'a') && (s <= 'z'))
        return true;
    else
    {
        if ((s >= 'A') && (s <= 'Z'))
            return true;
        else
            return false;
    }
}

//判断是否为关键字，如果是返回下标；否则返回0
int IsKey(char *s)
{
    for (int i = 0; i < NumOfKey; i++)
    {
        if (strcmp(s, KeyWord[i]) == 0)
        {
            return i + 1;
            break;
        }
    }
    return 0;
}

//打印结果
void PrintRes(int lineNum, char *in, char *belong)
{
    cout << lineNum << " <" << in << ", " << belong << ">" << endl;
}

//一行一行处理
void Work(char *s, int lineNum)
{
    int state = 0; //状态初始化
    char ch = ' ';
    int pos = 0; //标记位置
    while (ch != '\0')
    {
        switch (state)
        {
        case 0:
        {
            ch = GetAnother(s);
            switch (ch)
            {
            //空格不处理
            case ' ':
                pos = 0;
                break;

            //以下为分解符
            case '[':
            case ']':
            case '(':
            case ')':
            case '{':
            case '}':
            {
                char temp[2];
                temp[0] = ch;
                PrintRes(lineNum, temp, "-");
                pos = 0;
                NumOfOperator++;
                break;
            }

            //除号
            case '\'':
            {
                state = 0;
                while (((ch = GetAnother(s)) != '\'') && (ch != '\0'))
                {
                    Output[pos++] = ch;
                }
                if (ch == '\0')
                {
                    //error
                    //PrintRes("Error", Output);
                }
                Output[pos] = '\0'; //加\0生成完整字符串
                PrintRes(lineNum, "string", Output);
                //输出后重新置pos为0
                pos = 0;
                NumOfOperator++;
                break;
            }

            case '"':
            {
                state = 0;
                while (((ch = GetAnother(s)) != '"') && (ch != '\0'))
                {
                    Output[pos++] = ch;
                }
                if (ch == '\0')
                {
                    //error
                    PrintRes(lineNum, "Error", Output);
                }
                else
                {
                    Output[pos] = '\0';
                    PrintRes(lineNum, "string", Output);
                    pos = 0;
                    NumOfOperator++;
                }
                break;
            }

            //加号
            case '+':
            {
                state = 0;
                ch = GetAnother(s);
                NumOfOperator++;
                switch (ch)
                {
                //++
                case '+':
                    PrintRes(lineNum, "++", "-");
                    pos = 0;
                    break;
                //+=
                case '=':
                    PrintRes(lineNum, "+=", "-");
                    pos = 0;
                    break;
                default:
                    GoBack(s);
                    PrintRes(lineNum, "+", "-");
                    pos = 0;
                    break;
                }
            }

            //-号
            case '-':
            {
                state = 0;
                ch = GetAnother(s);
                NumOfOperator++;
                switch (ch)
                {
                //--
                case '-':
                    PrintRes(lineNum, "--", "-");
                    pos = 0;
                    break;

                //-=
                case '=':
                    PrintRes(lineNum, "-=", "-");
                    pos = 0;
                    break;

                default:
                    GoBack(s);
                    PrintRes(lineNum, "-", "-");
                    pos = 0;
                    break;
                }
                break;
            }

            //=号
            case '=':
            {
                state = 0;
                ch = GetAnother(s);
                NumOfOperator++;
                switch (ch)
                {
                //==
                case '=':
                    PrintRes(lineNum, "==", "-");
                    pos = 0;
                    break;
                default:
                    GoBack(s);
                    PrintRes(lineNum, "=", "-");
                    pos = 0;
                    break;
                }
                break;
            }

            default:
            {
                if (IsNum(ch))
                {
                    Output[pos++] = ch;
                    NumOfNumber++;
                    state = 2;
                }
                else
                {
                    if (IsLetter(ch) || ch == '_')
                    {
                        Output[pos++] = ch;
                        NumOfLetter++;
                        state = 1;
                    }
                }
                break;
            }
            }
            break;
        }
        case 1:
        {
            while (true)
            {
                ch = GetAnother(s);
                if (IsLetter(ch) || IsNum(ch) || ch == '_')
                {
                    Output[pos++] = ch;
                }
                else
                {
                    Output[pos] = '\0';
                    int flag = IsKey(Output);
                    //判断是否为关键字
                    if (flag == 0)
                    {
                        PrintRes(lineNum, "Variable", Output);
                        NumOfVarible++;
                    }
                    else
                    {
                        PrintRes(lineNum, "KeyWord", Output);
                        NumOfKeyWord++;
                    }
                    //回归初始
                    GoBack(s);
                    pos = 0;
                    state = 0;
                    break;
                }
            }
            break;
        }

        case 2:
        {
            while (true)
            {
                ch = GetAnother(s);

                if (IsNum(ch))
                {
                    Output[pos++] = ch;
                }
                else
                {
                    if (ch == '.')
                    {
                        Output[pos++] = ch;
                        state = 3; //进入小数模式
                        break;
                    }
                    else
                    {
                        if (ch == 'E' || ch == 'e')
                        {
                            Output[pos++] = ch;
                            state = 4; //进入科学计数法模式
                            break;
                        }
                        else
                        {
                            Output[pos] = '\0'; //补\0生成完整字符串
                            PrintRes(lineNum, "Number", Output);
                            GoBack(s);
                            pos = 0;
                            state = 0;
                            break;
                        }
                    }
                }
            }
            break;
        }

        case 3:
        {
            while (true)
            {
                ch = GetAnother(s);
                if (IsNum(ch))
                {
                    Output[pos++] = ch;
                }
                else
                {
                    if (ch == 'E' || ch == 'e')
                    {
                        Output[pos++] = ch;
                        state = 4; //进入科学计数法模式
                    }
                    else
                    {
                        Output[pos] = '\0'; //补\0生成完整字符串
                        PrintRes(lineNum, "Number", Output);
                        pos = 0;
                        state = 0;
                        break;
                    }
                }
            }
            break;
        }

        case 4:
        {
            while (true)
            {
                ch = GetAnother(s);
                if (IsNum(ch))
                {
                    Output[pos++] = ch;
                }
                else
                {
                    Output[pos] = '\0';
                    PrintRes(lineNum, "Number", Output);
                    GoBack(s);
                    pos = 0;
                    state = 0;
                    break;
                }
            }
            break;
        }

        default:
            PrintRes(lineNum, "Error", Output);
            break;
        }
    }
}

int main()
{
    FILE *file = fopen(FName, "r");

    int lineNum = 0;

    while (NULL != fgets(Buffer, 1024, file))
    {
        lineNum++;
        Work(Buffer, lineNum);
    }
    cout << endl;
    cout << "行数：" << lineNum << endl;
    cout << "字符数目：" << NumOfLetter << endl;
    cout << "关键字数目：" << NumOfKeyWord << endl;
    cout << "变量名数目" << NumOfVarible << endl;
    cout << "数字常量数目：" << NumOfNumber << endl;
    cout << "分界符和操作符数目：" << NumOfOperator << endl;
    system("pause");
    return 0;
}