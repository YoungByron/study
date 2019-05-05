#include <iostream>
#include <sstream>
#include <list>
#include <numeric>
#include <algorithm>

using namespace std;

//����һ��list������ʵ��LISTINT
//typedef list<int> LISTINT;

//����һ��list������ʵ��LISTSTRING
typedef list<string> LISTSTRING;

class CStackStr //�����һ����ĸӦ����Сд
{
private:
	//��LISTSTRING����һ����ΪlistOne��list����
	LISTSTRING listTwo;
	//����iΪ������
	LISTSTRING::iterator j;
public:
	CStackStr(){}; //top��ʼ�����������
	void push(string str);
	string pop();
	bool isEmpty();
	bool isFull();
	int size();
	void dump();
};
void CStackStr::push(string str) //����Ӧ��д����������
{
	listTwo.push_front (str);
}
string CStackStr::pop() //���ﷵ������Ӧ�÷��ھ���
{
	string retStr = string(*(listTwo.begin()));
	listTwo.pop_front ();

	return retStr; //����ջ��
}
int CStackStr::size() 
{
	return listTwo.size();
}
bool CStackStr::isEmpty() //�ж��Ƿ�Ϊ�� //���ﷵ������Ӧ�÷��ھ���
{
	if(listTwo.size()<=0)return 1;
	else return 0;
}

void CStackStr::dump() //���ﷵ������Ӧ�÷��ھ���
{
	//��ǰ�����ʾlistTwo�е�����
	cout<<"listTwo.begin()---listTwo.end():"<<endl;
	for (j = listTwo.begin(); j != listTwo.end(); ++j)
		::OutputDebugString(string(*j).c_str());
	cout << endl;
}

CStackStr myStack;