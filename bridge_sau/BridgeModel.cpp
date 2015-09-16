#include "BridgeModel.h"


int cmp(const void *a, const void *b){
	return *(int*)a - *(int*)b;
}

void BridgeModel::getGameInfo(int handCard[4][13], int *stone) {
	memcpy(handCard, this->handCard, 52 * sizeof(int));
	*stone = this->stone;
}

void BridgeModel::setGameInfo(int handCard[4][13], int stone) {
	memcpy(this->handCard, handCard, 52 * sizeof(int));
	this->stone = stone;
}

void BridgeModel::startGame() {
	turn = -1;
	num = 3;
	bankerPayoff = breakerPayoff = 0;

	//ϴ�ƣ�����
	if ((rand() % 1000) < 5) {
		srand(time(NULL));
	}
	for (int i = 0; i < 52; i++) {
		handCard[i % 4][i / 4] = i;
	}
	for (int i = 0; i < 52; i++) {
		int j = rand() % 52;
		int t = handCard[i % 4][i / 4];
		handCard[i % 4][i / 4] = handCard[j % 4][j / 4];
		handCard[j % 4][j / 4] = t;
	}
	for (int i = 0; i < 4; i++) {
		qsort(handCard[i], 13, sizeof(int), cmp);
	}

	//ָ�ɶ�Լ��ɫ
	stone = rand() % 5;
}

void BridgeModel::startTurn() {
	if (turn == 0) {//����
		leader[turn] = 1;
	} else {
		//������һ�֡�֧����
		int t = turn * 4 - 4;
		int biggerCard = historyCard[t][0];
		leader[turn] = leader[turn - 1];
		for (int i = 1; i < 4; i++) {
			int card = historyCard[t + i][0];
			if ((card % 4 == biggerCard % 4) && (card / 4 > biggerCard / 4)) {//ͬ��ɫ������
				biggerCard = card;
				leader[turn] = (leader[turn - 1] + i) % 4;
			} else if ((biggerCard % 4 != stone) && (card % 4 == stone)) {//����
				biggerCard = card;
				leader[turn] = (leader[turn - 1] + i) % 4;
			}
		}
		if (leader[turn] % 2 == 0) {
			bankerPayoff++;
		} else {
			breakerPayoff++;
		}
	}
}

bool BridgeModel::gameNext() {
	num += 1;
	if (num == 4) {//��ʼ��һ��
		num = 0;
		turn += 1;
		//��ʼ���֣�������һ��
		startTurn();
		if (turn == 13) {//�ƾֽ���
			return false;
		}
	}
	return true;
}

//����
bool BridgeModel::lead(const int card) {
	int player = getCurPlayer();
	for (int i = 0; i < 13; i++) {
		if (handCard[player][i] == card) {
			historyCard[turn * 4 + num][0] = card;
			historyCard[turn * 4 + num][1] = i;
			handCard[player][i] = -1;
			return true;
		}
	}
	return false;
}

//�ָ�
bool BridgeModel::recover() {
	num--;
	if (num < 0) {
		if (leader[turn] % 2 == 0) {
			bankerPayoff--;
		} else {
			breakerPayoff--;
		}
		turn--;
		num = 3;
	}
	int player = getCurPlayer();
	handCard[player][historyCard[turn * 4 + num][1]] = historyCard[turn * 4 + num][0];
	return true;
}

int BridgeModel::getCurPlayer() {
	return (leader[turn] + num) % 4;
}

double BridgeModel::getBankerPayoff() {
	return bankerPayoff;
}

double BridgeModel::getBreakerPayoff() {
	return breakerPayoff;
}

//������ֿ����ŷ�
/**
 * getUseableCard - ������ֿ����ŷ�
 * @card _OUT_: ���ؿ����ŷ�
 * @return: ���ؿ����ŷ�����
 */
int BridgeModel::getUseableCard(int card[13]) {
	int num = 0;
	int player = getCurPlayer();
	if (player == leader[turn]) {//���
		for (int i = 0; i < 13; i++) {
			if (handCard[player][i] != -1) {
				card[num++] = handCard[player][i];
			}
		}
	} else {
		for (int i = 0; i < 13; i++) {
			if ((handCard[player][i] != -1) && (handCard[player][i] % 4 == historyCard[turn * 4][0] % 4)) {
				card[num++] = handCard[player][i];
			}
		}
		if (num == 0) {//û�������ɫ���γ�һ��
			for (int i = 0; i < 13; i++) {
				if (handCard[player][i] != -1) {
					card[num++] = handCard[player][i];
				}
			}
		}
	}
	return num;
}

//���ɾ����ʶ �ѳ���52bit������52bit��������52bit����Լ��ɫ3bit����ǰ���ѳ�����Ϣ18bit
void BridgeModel::getInfoSet(char infoSet[INFOSETLEN]) {
	for (int i = 0; i < INFOSETLEN - 1; i++) {
		infoSet[i] = (char)64;
	}
	infoSet[INFOSETLEN - 1] = 0;
	int i, m, n, index;
	int historyNum = turn * 4 + num;
	for (i = 0; i < historyNum; i++) {
		index = historyCard[i][0];
		m = index / INFOLEN;
		n = index % INFOLEN;
		infoSet[m] |= (1 << n);
	}
	int player = getCurPlayer();
	for (int i = 0; i < 13; i++) {
		index = handCard[player][i] + 52;
		m = index / INFOLEN;
		n = index % INFOLEN;
		infoSet[m] |= (1 << n);
	}
	if (player == 2) {
		for (int i = 0; i < 13; i++) {
			index = handCard[0][i] + 104;
			m = index / INFOLEN;
			n = index % INFOLEN;
			infoSet[m] |= (1 << n);
		}
	} else {
		for (int i = 0; i < 13; i++) {
			index = handCard[2][i] + 104;
			m = index / INFOLEN;
			n = index % INFOLEN;
			infoSet[m] |= (1 << n);
		}
	}
	infoSet[INFOSETLEN - 5] |= stone;
	if (num == 3) {
		infoSet[INFOSETLEN - 2] |= historyCard[turn * 4 + 2][0];
		infoSet[INFOSETLEN - 3] |= historyCard[turn * 4 + 1][0];
		infoSet[INFOSETLEN - 4] |= historyCard[turn * 4][0];
	}
	else if(num == 2) {
		infoSet[INFOSETLEN - 2] |= 52;
		infoSet[INFOSETLEN - 3] |= historyCard[turn * 4 + 1][0];
		infoSet[INFOSETLEN - 4] |= historyCard[turn * 4][0];
	}
	else if (num == 1) {
		infoSet[INFOSETLEN - 2] |= 52;
		infoSet[INFOSETLEN - 3] |= 52;
		infoSet[INFOSETLEN - 4] |= historyCard[turn * 4][0];
	}
	else {
		infoSet[INFOSETLEN - 2] |= 52;
		infoSet[INFOSETLEN - 3] |= 52;
		infoSet[INFOSETLEN - 4] |= 52;
	}
}

void BridgeModel::printCard() {
	for (int i = 0; i < 4; i++) {
		printf("player #%d: ", i);
		for (int j = 0; j < 13; j++) {
			printf("%d ", handCard[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

bool BridgeModel::isP() {
	if (turn == 5 && num == 0)
		return true;
	return false;
}