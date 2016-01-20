#pragma once

//�����������
enum E_ActionMethod
{
	amNone,                                      //�޶���
	amClick,                                     //���
	amFill,                                      //��� ��ֵ
	amIdentify,                                  //ʶ��
	amJumpURL,                                   //��ת��ҳ
	amKeyPress,                                  //ģ�����
	amUploadFile,                                //�ϴ��ļ�
	amUserForm,                                  //�Զ����
	amUserInput,                                        //�˹�����
	amShow,                                      //��ʾ
	amHide,                                      //����
	amGetAttribute,                              //��ȡ����
	amGetIIDK,                                   //��ȡ����
	amSuccFeature,                               //�ɹ����
	//amAddAttribute,                              //�������
	//amEditAttribute,                             //�༭����
	//amDeleteAttribute,                           //ɾ������
	amFireEvent,                                 //�����¼�
	amNextPage,                                  //��һҳ
	amVerifyCodeSuccFeature,                     //��֤��ɹ����
	amMarkObject,                                //��Ƕ��� ����ͼ���÷����ָ��Ԫ�ؽ��б�ǣ�
	amSetDataArea,                               //�궨������
	amSaveItem,                                  //������ҳɹ����Ҫ��ǵĽڵ㣬�¸�������Ҫ���
	amDownLoadFile,                              //��ȡ�ļ�
	amDeleteNode,                                //ɾ���ڵ�
	amSaveDelNode,                               //���潫Ҫɾ���Ľڵ�
	amMax,                                       //һ���˵������ֵ
};

//�������Ͷ���
enum E_TaskType
{
	ttDataNull = -2,                             //δ��ʼ��
	ttDataEmpty = -1,                            //�ѳ�ʼ����δ������
	ttDataReg,                                   //ע������
	ttDataLogin,                                 //��½����
	ttDataLogOut,                                //�ǳ�����
	ttDataPostCP,                                //������Ʒ����
	ttDataPostGY,                                //������Ӧ����
	ttDataPostQG,                                //����������
	ttDataPostOther,                             //�����������ݣ���Ϊ����
	ttDataAddExtra,                              //��������
	ttDataRefresh,                               //ˢ������
	ttDataDelete,                                //ɾ������
	//tmDataReply,                                 //�ظ�/��������
	//tmDataVote,                                  //ͶƱ/��������
	//tmDataSearch,                                //��������
	//tmDataReadFeedBack,                          //��ȡ��������
	//tmDataWriteFeedBack,                         //д��������
	ttDataStationFunction,                       //Ƕ����վ����
	ttDataPostDomain,                            //�����ƹ�����
	ttDataRefreshPaiMing,                        //ˢ����������
	ttDataUpdatePhoto,							 //�����������
	ttDataCompanyPopular,                        //��˾֪��������
	ttDataOnceKeyMove,                           //һ����ҹ�˾����
	ttDataOnceKeyProduct,                        //һ����Ҳ�Ʒ����
	ttDataExtractionProduct,                     //��ȡ��Ʒ�б�����
	ttDataFinished = 999,                        //�Ѿ����
};

//�������Ͷ���
enum E_StationFunction
{
	sfNone = -1,                                 //
	sfReg = 0,                                   //ע�Ṧ��
	sfPostCP,                                    //������Ʒ����
	sfPostGY,                                    //������Ӧ����
	sfPostQG,                                    //�����󹺹���
	sfPostOther,                                 //������������
	sfAddExtra,                                  //�������Ϲ���
	sfRefreshInfo,                               //ˢ����Ϣ����
	sfVerify,                                    //��֤����
	sfDeleteInfo,                                //ɾ����Ϣ����
	sfPostDomain,                                //�����ƹ㹦��
	sfRefreshPaiMing,                            //ˢ�¹ؼ�����������
	sfUpdatePhoto,								 //������Ṧ��
	sfCompanyPopular,                            //��˾֪���ȹ���
	sfOnceKeyMove,                               //һ����ҹ���
	sfChangeData,                                //�޸����Ϲ���
	sfExtractionProduct,                         //��ȡ��Ʒ����
	//sfEditInfo,                                  //�༭��Ϣ����
	//sfReply,                                     //�ظ�/��������
	//sfVote,                                      //ͶƱ/��������
	//sfSearch,                                    //��������
	//sfReadFeed,                                  //�����Թ���
	//sfWriteFeed,                                 //д���Թ���
};

enum E_FILTER_TYPE
{
	flAnd,
	flNot,
	flOr,
};

enum E_CLICK_TYPE
{
	ClickNormal = 0,//�������
	ClickMouse = 1,//ģ����̵��
	ClickDlg = 2,  //������򣨾þ���Ϣ���ϴ��ļ���
	ClickJumpURL = 3,//�����ת��ҳ
};

enum E_JUMP_URL_TYPE
{
	JumpNewPage = 0,//��ҳ���
	JumpFrushPage = 1,//��ʱǿ��ˢ��
	JumpNofrushPage = 2,//��ʱ��ˢ��
};

enum E_RUN_MODE
{
	rmNormal,    //����ģʽ�����ش���������ʾ
	rmShow,      //��ʾģʽ����ʾ����������ʾ���������ͣ
	rmCaibian,   //�ɱ�ģʽ����ʾ���ڡ���ʾ��������Ϣ
	rmBrowser,   //���ģʽ����ʾ������ʾ��ʾ(�൱��IE���)
	rmTest,      //����ģʽ����ʾ���ڡ���ʾ��������Ϣ���������ͣ
};

//ִ�н��
enum E_RESULT
{
	srNoneResult,               //�޽��
	srTimeOut,                  //��ʱ (������ʱ����������)
	srFinishedOK,               //�ɹ�
	srFinishedOKTolerantFail,   //�ݴ�ɹ�
	srFinishedFail,             //ʧ�ܣ��������Ҳ��������������ԣ�
	srFinishedException,        //�ں˲����쳣�����ؼ�⵽�˴�����ʱ��ֱ�ӷ���Stopֹͣ����

	//���´���������ں���ʹ��
	srIndexOutOfVagueLocRange,  //ģ����λʱ��ָ����Ŵ���ʵ�ʶ���ĸ���
	srFinishedError,            //����������ҵ����󵫼������ʧ�ܣ�ֱ��ʧ�ܲ�����
	srFinishedTaskConfilict,	  //�����ͻ��������
};

//�������Ĺ�����������
enum E_RESULT_FILTER_TYPE
{
	rftNULL = -1,
	rftSucc,          //�ɹ�
	rftRePost,        //�ظ�����
	rftReReg,         //�ظ�ע��
	rftMsgFull,       //�ѷ���
	rftFail,          //ʧ��
	rftDataAddExtra,  //��������
	//rftLoginFail,     //��¼ʧ��
	rftInvalidAccount,//��Ч�˺�
	rftAddPhoto,      //�������
	rftReCompany,    //��˾����
	rftReEmail,    //��������

	rftAccountNotAduit, //�˺�δ���
	rftRegLackInfo,     //ע��ȱ������
	rftAddLackInfo,     //����ȱ������
	rftPostLackInfo,    //����ȱ������
	rftRePhoneNumber,   //�ֻ����ظ�

	rftTotalCount,
};

//�ϲ����ں˽������ݵ�״̬
enum E_EXCHANGE_STATE
{
	esNULL = -1,
	esINIT,          //�ϲ�������ѹ�� T_DATA_IN��Ч
	esEXEC,          //���������ں˴���
	esDONE,          //���ݴ������ T_DATA_OUT��Ч
// 	esRETURN,        //�ϲ��ѽ�������ȡ��
};

//ʶ������
enum E_VERIFY_TYPE
{
	evtNULL,
	evtPicture = 1,     //ͼƬ
	evtQuestion,        //����
	evtFlash,           //flash
	evtTelephone,       //�ֻ���
	evtTelephoneCode,    //�ֻ���֤��
	evtEMail,            //����
	evtEMailCode,        //������֤��
	
	evtAutoRegonize = 1000,  //�Զ�ʶ��
};

enum EPicShowType
{
	E_Others =0,
	E_Single_Pic = 1, // ��ͼƬ
	E_Multi_Pic = 2,  // ��ͼƬ
	E_TEXT_PIC = 3,   // �ı���ʽ
	E_SELECTOPT_Pic, //������ѡ��

};

//ִ�п�ѭ������
enum E_LOOP_TYPE
{
	eltLoopMax ,  //���
	eltLoopRandom , //���
	eltLoopFixed ,  //�̶�ֵ
	eltLoopFixedLessOne,//�̶�ֵ-1
};

//��վ֧�ֵ����� �콢�� ���� �������߶�֧��
enum E_Support_Type
{
	estExpressUltimate,  //ֻ֧���콢��
	est6Du, //ֻ֧������
	estBoth, //���߶�֧��
};

//��֤���Զ�ʶ������
enum E_ORC_Indentify_Type
{
	oitAuto = 0,  //�Զ�ʶ��
	oitLongRange, //Զ�̴���
	oitManmade, //�˹�����
};