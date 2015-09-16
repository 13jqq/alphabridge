#ifndef BRIDGEMODEL
#define BRIDGEMODEL

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#define INFOSETLEN 31
#define INFOLEN 6


class BridgeModel {

public:

	void getGameInfo(int handCard[4][13], int *stone);
	void setGameInfo(int handCard[4][13], int stone);

	void startGame();//��ʼ����
	void startTurn();//��ʼ����
	
	bool gameNext();//���Լ�������
	
	bool lead(const int card);//����
	bool recover();//�ָ�

	int getCurPlayer();//��ǰ���ֺ�
	double getBankerPayoff();
	double getBreakerPayoff();
	int getUseableCard(int actions[13]);//������ֿ����ŷ�
	void getInfoSet(char infoSet[INFOSETLEN]);//��ȡ�����ʶ �ѳ���52bit������52bit��������52bit����Լ��ɫ3bit

	void printCard();
	bool isP();

	

private:

	int handCard[4][13];//����  0Ϊׯ�ң�1Ϊ���֣�2Ϊ�׹���
	int historyCard[52][2];//������ʷ 0�����ű��� 1�������������е����
	int stone;//��Լ��ɫ
	int turn;//����
	int num;//�����ѳ�������
	int leader[14];//����ˣ������ڽ���payoff
	double bankerPayoff, breakerPayoff;
};



#endif