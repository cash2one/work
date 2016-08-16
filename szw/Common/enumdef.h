#pragma once

//对象操作定义
enum E_ActionMethod
{
	amNone,                                      //无动作
	amClick,                                     //点击
	amFill,                                      //填充 赋值
	amIdentify,                                  //识别
	amJumpURL,                                   //跳转网页
	amKeyPress,                                  //模拟键盘
	amUploadFile,                                //上传文件
	amUserForm,                                  //自定义表单
	amUserInput,                                 //人工输入
	amShow,                                      //显示
	amHide,                                      //隐藏
	amGetAttribute,                              //提取数据
	amGetIIDK,                                   //提取动参
	amSuccFeature,                               //成功标记
	//amAddAttribute,                              //添加属性
	//amEditAttribute,                             //编辑属性
	//amDeleteAttribute,                           //删除属性
	amFireEvent,                                 //触发事件
	amNextPage,                                  //下一页
	amVerifyCodeSuccFeature,                     //验证码成功标记
	amMarkObject,                                //标记对象 （截图并用方框对指定元素进行标记）
	amSetDataArea,                               //标定数据区
	amSaveItem,                                  //保存查找成功标记要标记的节点，下个步骤需要点击
	amDownLoadFile,                              //提取文件
	amDeleteNode,                                //删除节点
	amSaveDelNode,                               //保存将要删除的节点
	amSaveSuccFeature,						 //预设成功标记，提供给拖动/点击型验证使用
	amMax,                                       //一级菜单的最大
};


//任务类型定义
enum E_TaskType
{
	ttDataNull = -2,                             //未初始化
	ttDataEmpty = -1,                            //已初始化，未有数据
	ttDataReg,                                   //注册数据
	ttDataLogin,                                 //登陆数据
	ttDataLogOut,                                //登出数据
	ttDataPostCP,                                //发布产品数据
	ttDataPostGY,                                //发布供应数据
	ttDataPostQG,                                //发布求购数据
	ttDataPostOther,                             //发布其它数据，作为备用
	ttDataAddExtra,                              //完善资料
	ttDataRefresh,                               //刷新数据
	ttDataDelete,                                //删除数据
	//tmDataReply,                                 //回复/跟贴数据
	//tmDataVote,                                  //投票/顶贴数据
	//tmDataSearch,                                //搜索数据
	//tmDataReadFeedBack,                          //读取留言数据
	//tmDataWriteFeedBack,                         //写留言数据
	ttDataStationFunction,                       //嵌入网站功能
	ttDataPostDomain,                            //域名推广数据
	ttDataRefreshPaiMing,                        //刷新排名数据
	ttDataUpdatePhoto,							 //更新相册数据
	ttDataCompanyPopular,                        //公司知名度数据
	ttDataOnceKeyMove,                           //一键搬家公司数据
	ttDataOnceKeyProduct,                        //一键搬家产品数据
	ttDataExtractionProduct,                     //提取产品列表数据
	ttDataFinished = 999,                        //已经完成
};

//功能类型定义
enum E_StationFunction
{
	sfNone = -1,                                 //
	sfReg = 0,                                   //注册功能
	sfPostCP,                                    //发布产品功能
	sfPostGY,                                    //发布供应功能
	sfPostQG,                                    //发布求购功能
	sfPostOther,                                 //发布其它功能
	sfAddExtra,                                  //更新资料功能
	sfRefreshInfo,                               //刷新信息功能
	sfVerify,                                    //验证功能
	sfDeleteInfo,                                //删除信息功能
	sfPostDomain,                                //域名推广功能
	sfRefreshPaiMing,                            //刷新关键词排名功能
	sfUpdatePhoto,								 //更新相册功能
	sfCompanyPopular,                            //公司知名度功能
	sfOnceKeyMove,                               //一键搬家功能
	sfChangeData,                                //修改资料功能
	sfExtractionProduct,                         //提取产品功能
	//sfEditInfo,                                  //编辑信息功能
	//sfReply,                                     //回复/跟贴功能
	//sfVote,                                      //投票/顶贴功能
	//sfSearch,                                    //搜索功能
	//sfReadFeed,                                  //读留言功能
	//sfWriteFeed,                                 //写留言功能
};

enum E_FILTER_TYPE
{
	flAnd,
	flNot,
	flOr,
};

enum E_CLICK_TYPE
{
	ClickNormal = 0,	//正常点击
	ClickMouse = 1,		//模拟键盘点击
	ClickDlg = 2,		//点击弹框（久久信息网上传文件）
	ClickDBMouse = 3,
	ClickJumpURL = 4,	//点击跳转网页
};

enum E_JUMP_URL_TYPE
{
	JumpNewPage = 0,   //新页面打开
	JumpFrushPage = 1, //打开时强制刷新
	JumpNofrushPage = 2,//打开时不刷新
};

enum E_RUN_MODE
{
	rmNormal,    //正常模式：隐藏窗口屏蔽提示
	rmShow,      //显示模式：显示窗口屏蔽提示、出错后暂停
	rmCaibian,   //采编模式：显示窗口、提示及调试信息
	rmBrowser,   //浏览模式：显示窗口显示提示(相当于IE浏览)
	rmTest,      //测试模式：显示窗口、提示及调试信息、出错后暂停
};

//执行结果
enum E_RESULT
{
	srNoneResult,               //无结果
	srTimeOut,                  //超时 (操作超时，允许重试)
	srFinishedOK,               //成功
	srFinishedOKTolerantFail,   //容错成功
	srFinishedFail,             //失败（可能是找不到对象，允许重试）
	srFinishedException,        //内核步骤异常，主控检测到此错误码时可直接发送Stop停止任务

	//以下错误码仅在内核中使用
	srIndexOutOfVagueLocRange,  //模糊定位时，指定序号大于实际对象的个数
	srFinishedError,            //特殊错误（如找到对象但检查条件失败）直接失败不重试
	srFinishedTaskConfilict,	 //任务冲突错误类型
	srUploadFileError,			 //上传文件失败
	srFinishPost,				 //完成post操作
};

//任务结果的过滤特征类型
enum E_RESULT_FILTER_TYPE
{
	rftNULL = -1,
	rftSucc,          //成功
	rftRePost,        //重复发布
	rftReReg,         //重复注册
	rftMsgFull,       //已发满
	rftFail,          //失败
	rftDataAddExtra,  //完善资料
	//rftLoginFail,     //登录失败
	rftInvalidAccount,//无效账号
	rftAddPhoto,      //完善相册
	rftReCompany,    //公司重名
	rftReEmail,    //邮箱重名

	rftAccountNotAduit, //账号未审核
	rftRegLackInfo,     //注册缺少资料
	rftAddLackInfo,     //完善缺少资料
	rftPostLackInfo,    //发布缺少资料
	rftRePhoneNumber,   //手机号重复

	rftTotalCount,
};

//上层与内核交互数据的状态
enum E_EXCHANGE_STATE
{
	esNULL = -1,
	esINIT,          //上层数据已压入 T_DATA_IN有效
	esEXEC,          //数据正被内核处理
	esDONE,          //数据处理完毕 T_DATA_OUT有效
// 	esRETURN,        //上层已将处理结果取走
};

//识别类型
enum E_VERIFY_TYPE
{
	evtNULL,
	evtPicture = 1,      //图片
	evtQuestion,         //文字
	evtFlash,            //flash
	evtTelephone,        //手机号
	evtTelephoneCode,    //手机验证码
	evtEMail,            //邮箱
	evtEMailCode,        //邮箱验证码
	evtDragCode,		 //拖动型验证码
	evtWeiXin,			 //微信验证码
	
	evtAutoRegonize = 1000,  //自动识别
};

enum EPicShowType
{
	E_Others =0,
	E_Single_Pic = 1, // 单图片
	E_Multi_Pic = 2,  // 多图片
	E_TEXT_PIC = 3,   // 文本格式
	E_SELECTOPT_Pic, //下拉框选项

};

//执行块循环类型
enum E_LOOP_TYPE
{
	eltLoopMax ,  //最大
	eltLoopRandom , //随机
	eltLoopFixed ,  //固定值
	eltLoopFixedLessOne,//固定值-1
};

//网站支持的类型 旗舰版 六度 或者两者都支持
enum E_Support_Type
{
	estExpressUltimate,  //只支持旗舰版
	est6Du, //只支持六度
	estBoth, //两者都支持
};

//验证码自动识别类型
enum E_ORC_Indentify_Type
{
	oitAuto = 0,  //自动识别
	oitLongRange, //远程打码
	oitManmade, //人工输入
};