#include "BridgeTrainer.h"


_ConnectionPtr  m_pConnection = NULL;
_RecordsetPtr m_pRecordset = NULL;

string convertToASCII(const char infoSet[INFOSETLEN]) {
	string t;
	for (int i = 0; i < INFOSETLEN - 1; i++) {
		if (infoSet[i] == '\\') {
			t = t + "\\\\";
		}
		else {
			t = t + infoSet[i];
		}
	}
	return t;
}

//BridgeNode
//============================
BridgeNode::BridgeNode(const char infoSet[INFOSETLEN], bool isBanker, int actionsNum) {
	this->actionsNum = actionsNum;
	if (actionsNum == 1) {
		regretSum[0] = 1.0;
		return;
	}

	memcpy(this->infoSet, infoSet, INFOSETLEN);
	this->isBanker = isBanker;

	//ͨ��DAO�����ݿ��ѯ��Ϣ�������ʧ��������¼�¼
	try {
		string sql("select timestamp,lastUtil");
		for (int i = 0; i < actionsNum; i++) {
			char index[3];
			itoa(i, index, 10);
			sql = sql + ",regret" + index;
		}
		sql = sql + " from " + (isBanker ? "banker" : "breaker") + " where infoSet='" + convertToASCII(infoSet) + "';";

		m_pRecordset = m_pConnection->Execute(_bstr_t(sql.c_str()), NULL, adCmdText);

		if (!m_pRecordset->adoEOF)//����Ϊʲô��adoEOF������EOF��?���ǵ�rename(\"EOF\",\"adoEOF\")��һ����?
		{
			fromDB = true;
			_variant_t data;
			switch (actionsNum) {
			case 13:
				data = m_pRecordset->GetCollect("regret12");
				regretSum[12] = data.dblVal;
			case 12:
				data = m_pRecordset->GetCollect("regret11");
				regretSum[11] = data.dblVal;
			case 11:
				data = m_pRecordset->GetCollect("regret10");
				regretSum[10] = data.dblVal;
			case 10:
				data = m_pRecordset->GetCollect("regret9");
				regretSum[9] = data.dblVal;
			case 9:
				data = m_pRecordset->GetCollect("regret8");
				regretSum[8] = data.dblVal;
			case 8:
				data = m_pRecordset->GetCollect("regret7");
				regretSum[7] = data.dblVal;
			case 7:
				data = m_pRecordset->GetCollect("regret6");
				regretSum[6] = data.dblVal;
			case 6:
				data = m_pRecordset->GetCollect("regret5");
				regretSum[5] = data.dblVal;
			case 5:
				data = m_pRecordset->GetCollect("regret4");
				regretSum[4] = data.dblVal;
			case 4:
				data = m_pRecordset->GetCollect("regret3");
				regretSum[3] = data.dblVal;
			case 3:
				data = m_pRecordset->GetCollect("regret2");
				regretSum[2] = data.dblVal;
			case 2:
				data = m_pRecordset->GetCollect("regret1");
				regretSum[1] = data.dblVal;
			case 1:
				data = m_pRecordset->GetCollect("regret0");
				regretSum[0] = data.dblVal;
			case 0:
				data = m_pRecordset->GetCollect("timestamp");
				timestamp = data.llVal;
				data = m_pRecordset->GetCollect("lastUtil");
				util = data.dblVal;
			}
		} else {
			fromDB = false;
			timestamp = 0;
			memset(regretSum, 0, sizeof(regretSum));
		}
	}
	catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
		memset(regretSum, 0, sizeof(regretSum));
	}
}

//�Ե�ǰ��Ϣ���������ź��������
void BridgeNode::getStrategy(double strategy[13]) {
	double normalizingSum = 0;
	for (int i = 0; i < actionsNum; i++) {
		strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0;//�ź�Ϊ���򲻼�����Լ�
		normalizingSum += strategy[i];
	}
	for (int i = 0; i < actionsNum; i++) {
		if (normalizingSum > 0) {
			strategy[i] /= normalizingSum;
		} else {
			strategy[i] = 1.0 / actionsNum;
		}
	}
}

void BridgeNode::accRegret(double regret[13]) {
	
	if (actionsNum == 1) {
		return;
	}
	
	int i;
	bool allZero = true;
	for (i = 0; i < actionsNum; i++) {
		regretSum[i] += regret[i];
		if (allZero && regret[i] != 0) {
			allZero = false;
		}
	}

	if (allZero) {
		return;
	}

	//ͨ��DAO������Ϣ��
	try {
		_CommandPtr pCommand;
		pCommand.CreateInstance(__uuidof(Command));
		pCommand->ActiveConnection = m_pConnection;//�����ַ���
		pCommand->CommandType = adCmdStoredProc;//��Ǹò���Ϊ�洢����

		_ParameterPtr pParam;
		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vinfoSet"), adChar, adParamInput, strlen(infoSet));//���������ø�����
		pParam->Value = _variant_t(infoSet);
		pCommand->Parameters->Append(pParam);//���뵽Command����Ĳ�����������

		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vtimestamp"), adBigInt, adParamInput, sizeof(__int64));//���������ø�����
		pParam->Value = _variant_t(timestamp);
		pCommand->Parameters->Append(pParam);//���뵽Command����Ĳ�����������

		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vlastUtil"), adDouble, adParamInput, sizeof(double));//���������ø�����
		pParam->Value = _variant_t(util);
		pCommand->Parameters->Append(pParam);//���뵽Command����Ĳ�����������

		for (i = 0; i < actionsNum; i++) {
			char index[10];
			sprintf(index, "vregret%d", i);
			pParam.CreateInstance(__uuidof(Parameter));
			pParam = pCommand->CreateParameter(_bstr_t(index), adDouble, adParamInput, sizeof(double));//���������ø�����
			pParam->Value = _variant_t(regretSum[i]);
			pCommand->Parameters->Append(pParam);
		}
		for (; i < 13; i++) {
			char index[10];
			sprintf(index, "vregret%d", i);
			pParam.CreateInstance(__uuidof(Parameter));
			pParam = pCommand->CreateParameter(_bstr_t(index), adDouble, adParamInput, sizeof(double));//���������ø�����
			pParam->Value = _variant_t(0.0);
			pCommand->Parameters->Append(pParam);
		}

		if (fromDB) {//����
			if (isBanker) {
				pCommand->CommandText = _bstr_t("UPDATE_FOR_BANKER");//�洢������
			}
			else {
				pCommand->CommandText = _bstr_t("UPDATE_FOR_BREAKER");//�洢������
			}
		}
		else {//����
			if (isBanker) {
				pCommand->CommandText = _bstr_t("INSERT_FOR_BANKER");//�洢������
			}
			else {
				pCommand->CommandText = _bstr_t("INSERT_FOR_BREAKER");//�洢������
			}
		}
		pCommand->Execute(NULL, NULL, adCmdStoredProc);
	}
	catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
	}
}


//BridgeTrainer
//============================

BridgeTrainer::BridgeTrainer() {
	m_pConnection = NULL;
	HRESULT hr = m_pConnection.CreateInstance(__uuidof(Connection));
	if (FAILED(hr)) {
		MessageBox(NULL, "����Connectionʧ��", "error", MB_OK);
		exit(0);
	}
	try {
		//��������ʱ��  
		m_pConnection->ConnectionTimeout = 5;
		//�����ݿ�����  
		HRESULT hr = m_pConnection->Open("Driver=MySQL ODBC 5.3 ANSI Driver;SERVER=localhost;UID=root;PWD=ywhjames;DATABASE=bridgedb;PORT=3306", "", "", adModeUnknown);
	} catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
		exit(0);
	}

	//��ʼ�����ݼ�����
	m_pRecordset = NULL;
	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr)) {
		MessageBox(NULL, "����Recordsetʧ��", "error", MB_OK);
		exit(0);
	}
}

BridgeTrainer::~BridgeTrainer() {
	try {
		m_pRecordset->Close();
		m_pConnection->Close();
	}
	catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
		exit(0);
	}
}

/**
 * cfr - cfr�㷨ʵ��
 * @return: ����ׯ�ҵ�������ȡ�ܶ���
 * @description: ����δ����ģ����δ���ɻ�ȡ������Ϊ����ָ�ꡣ
 *               ���δ����ģ���ؿ��ǽ���ڲ�������ռ�ݵı��أ������ָ��ʡ�
 *               ��Ϊ�ѻ�ȡ��������ʷ��ȷ���ģ���δ��������Ӱ�죬�ʱ�������
 *               �����Ƽ��Ϻϲ����ɼ��ټ�¼��Ŀ����
 *
 *                 ׯ�һ�ȡ���� + �мһ�ȡ���� = �ѽ������� -> 13
 *
 *                 �ܻ�ȡ���� = �ѻ�ȡ���� + δ���ɻ�ȡ����
 *                 cfr����ֵ    ��ʷ��Ϣ     �ŷ�Ч�� => ���Ч�� => �ŷ��ź�
 */
double BridgeTrainer::cfr() {
	if (!bridgeModel.gameNext()) {//�ƾֽ���������ׯ������
		return bridgeModel.getBankerPayoff();
	}

	int player = bridgeModel.getCurPlayer();
	int actions[13];
	int actionsNum = bridgeModel.getUseableCard(actions);//ȡ�ÿ����ŷ�
	double regret[13], strategy[13], util[13], nodeUtil = 0;

	char infoSet[INFOSETLEN];
	bridgeModel.getInfoSet(infoSet);
	BridgeNode bridgeNode(infoSet, (player % 2 == 0), actionsNum);
	
	if (bridgeNode.timestamp == timestamp) {
		if (player == 0 || player == 2) {
			return bridgeNode.util + bridgeModel.getBankerPayoff();
		}
		else {
			return  13 - bridgeNode.util - bridgeModel.getBreakerPayoff();
		}
	}
	else {
		bridgeNode.timestamp = timestamp;
	}

	if (player == 0 || player == 2) {

		//ȡ���ŷ�����
		bridgeNode.getStrategy(strategy);

		//����ÿһ���ŷ���Ч��
		double bankerCurPayoff = bridgeModel.getBankerPayoff();
		for (int i = 0; i < actionsNum; i++) {
			bridgeModel.lead(actions[i]);
			util[i] = cfr() - bankerCurPayoff;
			nodeUtil += strategy[i] * util[i];
			bridgeModel.recover();
		}

		//���㲢�ۼ��ź�ֵ
		for (int i = 0; i < actionsNum; i++) {
			regret[i] = util[i] - nodeUtil;
		}
		bridgeNode.util = nodeUtil;
		bridgeNode.accRegret(regret);

		return nodeUtil + bankerCurPayoff;
	} else {

		//ȡ���ŷ�����
		bridgeNode.getStrategy(strategy);

		//����ÿһ���ŷ���Ч��
		double breakerCurPayoff = bridgeModel.getBreakerPayoff();
		for (int i = 0; i < actionsNum; i++) {
			bridgeModel.lead(actions[i]);
			util[i] = 13 - cfr() - breakerCurPayoff;
			nodeUtil += strategy[i] * util[i];
			bridgeModel.recover();
		}

		//���㲢�ۼ��ź�ֵ
		for (int i = 0; i < actionsNum; i++) {
			regret[i] = util[i] - nodeUtil;
		}
		bridgeNode.util = nodeUtil;
		bridgeNode.accRegret(regret);

		return  13 - nodeUtil - breakerCurPayoff;
	}
}

void BridgeTrainer::storeGameInfoIntoDB() {
	try {
		m_pConnection->Execute(_bstr_t("delete from bridgedb.gameInfo;"), NULL, adCmdText);

		_CommandPtr pCommand;
		pCommand.CreateInstance(__uuidof(Command));
		pCommand->ActiveConnection = m_pConnection;//�����ַ���
		pCommand->CommandType = adCmdStoredProc;//��Ǹò���Ϊ�洢����
		pCommand->CommandText = _bstr_t("STOREGAMEINFO");//�洢������

		_ParameterPtr pParam;
		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vtimestamp"), adBigInt, adParamInput, sizeof(__int64));//���������ø�����
		pParam->Value = _variant_t(timestamp);
		pCommand->Parameters->Append(pParam);//���뵽Command����Ĳ�����������

		int handCard[4][13], stone;
		bridgeModel.getGameInfo(handCard, &stone);

		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vstone"), adInteger, adParamInput, sizeof(int));//���������ø�����
		pParam->Value = _variant_t(stone);
		pCommand->Parameters->Append(pParam);//���뵽Command����Ĳ�����������

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 13; j++) {
				char index[15];
				sprintf(index, "vplayer%d_%d", i, j);
				pParam.CreateInstance(__uuidof(Parameter));
				pParam = pCommand->CreateParameter(_bstr_t(index), adInteger, adParamInput, sizeof(int));//���������ø�����
				pParam->Value = _variant_t(handCard[i][j]);
				pCommand->Parameters->Append(pParam);
			}
		}

		pCommand->Execute(NULL, NULL, adCmdStoredProc);
	}
	catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
	}
}

void BridgeTrainer::restoreGameInfoFromDB() {
	try {
		string sql("select * from gameInfo");

		m_pRecordset = m_pConnection->Execute(_bstr_t(sql.c_str()), NULL, adCmdText);

		if (!m_pRecordset->adoEOF)
		{
			int handCard[4][13], stone;
			_variant_t data;

			data = m_pRecordset->GetCollect("timestamp");
			timestamp = data.llVal;

			data = m_pRecordset->GetCollect("stone");
			stone = data.intVal;
			
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 13; j++) {
					char index[15];
					sprintf(index, "player%d_%d", i, j);
					data = m_pRecordset->GetCollect(index);
					handCard[i][j] = data.intVal;
				}
			}

			bridgeModel.setGameInfo(handCard, stone);
		}
	}
	catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
	}
}

void BridgeTrainer::train(int iterations) {
	double util = 0;

	//cfr�㷨�������⻯һ���ƾ֣�Ȼ��ͨ����ȫչ�����������з�������������Ϊ����ָ�ꡣ
	for (int i = 0; i < iterations; i++) {
		time_t start;
		time(&start);
		timestamp = start; 
		bridgeModel.startGame();
		if (i == 0) {
			restoreGameInfoFromDB();
		}
		storeGameInfoIntoDB();
		bridgeModel.printCard();
		util += cfr();
		time_t end;
		time(&end);
		printf("\n\ntime: %I64ds\n", end - start);
		printf("banker util: %lf\n\n", util);
	}
	printf("end\n");
	getchar();
	getchar();
}