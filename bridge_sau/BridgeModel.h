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

	void startGame();//初始化局
	void startTurn();//初始化轮
	
	bool gameNext();//可以继续进行
	
	bool lead(const int card);//出牌
	bool recover();//恢复

	int getCurPlayer();//当前牌手号
	double getBankerPayoff();
	double getBreakerPayoff();
	int getUseableCard(int actions[13]);//获得牌手可行着法
	void getInfoSet(char infoSet[INFOSETLEN]);//获取局面标识 已出牌52bit：手牌52bit：明手牌52bit：定约花色3bit

	void printCard();
	bool isP();

	

private:

	int handCard[4][13];//手牌  0为庄家，1为明手，2为首攻人
	int historyCard[52][2];//出牌历史 0：牌张编码 1：牌在牌手手中的序号
	int stone;//定约花色
	int turn;//轮数
	int num;//本轮已出牌人数
	int leader[14];//领出人，可用于结算payoff
	double bankerPayoff, breakerPayoff;
};



#endif