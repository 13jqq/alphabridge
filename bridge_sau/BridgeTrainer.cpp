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

	//通过DAO向数据库查询信息集，如果失败则插入新纪录
	try {
		string sql("select timestamp,lastUtil");
		for (int i = 0; i < actionsNum; i++) {
			char index[3];
			itoa(i, index, 10);
			sql = sql + ",regret" + index;
		}
		sql = sql + " from " + (isBanker ? "banker" : "breaker") + " where infoSet='" + convertToASCII(infoSet) + "';";

		m_pRecordset = m_pConnection->Execute(_bstr_t(sql.c_str()), NULL, adCmdText);

		if (!m_pRecordset->adoEOF)//这里为什么是adoEOF而不是EOF呢?还记得rename(\"EOF\",\"adoEOF\")这一句吗?
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

//对当前信息集，根据遗憾计算策略
void BridgeNode::getStrategy(double strategy[13]) {
	double normalizingSum = 0;
	for (int i = 0; i < actionsNum; i++) {
		strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0;//遗憾为负则不计入策略集
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

	//通过DAO更新信息集
	try {
		_CommandPtr pCommand;
		pCommand.CreateInstance(__uuidof(Command));
		pCommand->ActiveConnection = m_pConnection;//连接字符串
		pCommand->CommandType = adCmdStoredProc;//标记该操作为存储过程

		_ParameterPtr pParam;
		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vinfoSet"), adChar, adParamInput, strlen(infoSet));//给参数设置各属性
		pParam->Value = _variant_t(infoSet);
		pCommand->Parameters->Append(pParam);//加入到Command对象的参数集属性中

		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vtimestamp"), adBigInt, adParamInput, sizeof(__int64));//给参数设置各属性
		pParam->Value = _variant_t(timestamp);
		pCommand->Parameters->Append(pParam);//加入到Command对象的参数集属性中

		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vlastUtil"), adDouble, adParamInput, sizeof(double));//给参数设置各属性
		pParam->Value = _variant_t(util);
		pCommand->Parameters->Append(pParam);//加入到Command对象的参数集属性中

		for (i = 0; i < actionsNum; i++) {
			char index[10];
			sprintf(index, "vregret%d", i);
			pParam.CreateInstance(__uuidof(Parameter));
			pParam = pCommand->CreateParameter(_bstr_t(index), adDouble, adParamInput, sizeof(double));//给参数设置各属性
			pParam->Value = _variant_t(regretSum[i]);
			pCommand->Parameters->Append(pParam);
		}
		for (; i < 13; i++) {
			char index[10];
			sprintf(index, "vregret%d", i);
			pParam.CreateInstance(__uuidof(Parameter));
			pParam = pCommand->CreateParameter(_bstr_t(index), adDouble, adParamInput, sizeof(double));//给参数设置各属性
			pParam->Value = _variant_t(0.0);
			pCommand->Parameters->Append(pParam);
		}

		if (fromDB) {//更新
			if (isBanker) {
				pCommand->CommandText = _bstr_t("UPDATE_FOR_BANKER");//存储过程名
			}
			else {
				pCommand->CommandText = _bstr_t("UPDATE_FOR_BREAKER");//存储过程名
			}
		}
		else {//插入
			if (isBanker) {
				pCommand->CommandText = _bstr_t("INSERT_FOR_BANKER");//存储过程名
			}
			else {
				pCommand->CommandText = _bstr_t("INSERT_FOR_BREAKER");//存储过程名
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
		MessageBox(NULL, "创建Connection失败", "error", MB_OK);
		exit(0);
	}
	try {
		//设置连接时间  
		m_pConnection->ConnectionTimeout = 5;
		//打开数据库连接  
		HRESULT hr = m_pConnection->Open("Driver=MySQL ODBC 5.3 ANSI Driver;SERVER=localhost;UID=root;PWD=ywhjames;DATABASE=bridgedb;PORT=3306", "", "", adModeUnknown);
	} catch (_com_error &e) {
		MessageBox(NULL, e.Description(), "error", MB_OK);
		exit(0);
	}

	//初始化数据集对象
	m_pRecordset = NULL;
	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr)) {
		MessageBox(NULL, "创建Recordset失败", "error", MB_OK);
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
 * cfr - cfr算法实现
 * @return: 返回庄家的期望获取总墩数
 * @description: 面向未来建模，以未来可获取墩数作为评价指标。
 *               针对未来建模不必考虑结点在博弈树中占据的比重，即出现概率。
 *               因为已获取墩数是历史可确定的，对未来不构成影响，故被舍弃。
 *               对相似集合合并，可减少记录条目数。
 *
 *                 庄家获取墩数 + 闲家获取墩数 = 已进行轮数 -> 13
 *
 *                 总获取墩数 = 已获取墩数 + 未来可获取墩数
 *                 cfr返回值    历史信息     着法效用 => 结点效用 => 着法遗憾
 */
double BridgeTrainer::cfr() {
	if (!bridgeModel.gameNext()) {//牌局结束，返回庄家收益
		return bridgeModel.getBankerPayoff();
	}

	int player = bridgeModel.getCurPlayer();
	int actions[13];
	int actionsNum = bridgeModel.getUseableCard(actions);//取得可行着法
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

		//取得着法策略
		bridgeNode.getStrategy(strategy);

		//计算每一个着法的效用
		double bankerCurPayoff = bridgeModel.getBankerPayoff();
		for (int i = 0; i < actionsNum; i++) {
			bridgeModel.lead(actions[i]);
			util[i] = cfr() - bankerCurPayoff;
			nodeUtil += strategy[i] * util[i];
			bridgeModel.recover();
		}

		//计算并累计遗憾值
		for (int i = 0; i < actionsNum; i++) {
			regret[i] = util[i] - nodeUtil;
		}
		bridgeNode.util = nodeUtil;
		bridgeNode.accRegret(regret);

		return nodeUtil + bankerCurPayoff;
	} else {

		//取得着法策略
		bridgeNode.getStrategy(strategy);

		//计算每一个着法的效用
		double breakerCurPayoff = bridgeModel.getBreakerPayoff();
		for (int i = 0; i < actionsNum; i++) {
			bridgeModel.lead(actions[i]);
			util[i] = 13 - cfr() - breakerCurPayoff;
			nodeUtil += strategy[i] * util[i];
			bridgeModel.recover();
		}

		//计算并累计遗憾值
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
		pCommand->ActiveConnection = m_pConnection;//连接字符串
		pCommand->CommandType = adCmdStoredProc;//标记该操作为存储过程
		pCommand->CommandText = _bstr_t("STOREGAMEINFO");//存储过程名

		_ParameterPtr pParam;
		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vtimestamp"), adBigInt, adParamInput, sizeof(__int64));//给参数设置各属性
		pParam->Value = _variant_t(timestamp);
		pCommand->Parameters->Append(pParam);//加入到Command对象的参数集属性中

		int handCard[4][13], stone;
		bridgeModel.getGameInfo(handCard, &stone);

		pParam.CreateInstance(__uuidof(Parameter));
		pParam = pCommand->CreateParameter(_bstr_t("vstone"), adInteger, adParamInput, sizeof(int));//给参数设置各属性
		pParam->Value = _variant_t(stone);
		pCommand->Parameters->Append(pParam);//加入到Command对象的参数集属性中

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 13; j++) {
				char index[15];
				sprintf(index, "vplayer%d_%d", i, j);
				pParam.CreateInstance(__uuidof(Parameter));
				pParam = pCommand->CreateParameter(_bstr_t(index), adInteger, adParamInput, sizeof(int));//给参数设置各属性
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

	//cfr算法首先虚拟化一个牌局，然后通过完全展开博弈树进行分析，以期望作为评估指标。
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