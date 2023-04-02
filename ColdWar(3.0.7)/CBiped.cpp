#include "CBiped.h"

CBiped::CBiped()
{
	m_eTargetCode = 0;			//Ÿ�� �ִϸ��̼�
	m_bIsIdling = true;						//�ʱ� �ִϸ��̼� Idle�� ����

	m_spCharRoot = NiNew NiNode;			//ĳ���� ��Ʈ ��� ����
	assert(m_spCharRoot);
}


CBiped::~CBiped()
{

}

bool CBiped::Init()
{
	NiNode* NifRootNode = GetNIFRoot();
	
	assert(NifRootNode);
	if(!NifRootNode)
	{
		return false;
	}
	NiMatrix3 kZRot;
	kZRot.MakeZRotation(-90*(NI_PI/180));
	NifRootNode->SetRotate(kZRot);
	
	m_spCharRoot->AttachChild(GetNIFRoot());		//actor�Ŵ������� NIF�� �ε��ؼ� ĳ���� ��Ʈ ����� �ڽ����� �ٿ��ش�.

	bool bSelectedUpdate = true;
	bool bRigid = false;

	m_spCharRoot->SetSelectiveUpdateFlags(bSelectedUpdate, true, bRigid);

	m_spCharRoot->UpdateProperties();
	m_spCharRoot->UpdateEffects();
	m_spCharRoot->Update(0.0f);
	m_spCharRoot->UpdateNodeBound();

	/*//extra data ����
	NiNode* pkPart = (NiNode*) m_spCharRoot->GetObjectByName("head");
	NiIntegerExtraData* pkHeadED = NiNew NiIntegerExtraData(HEAD);
	pkPart->AddExtraData("part",pkHeadED);

	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("R_leg");
	NiIntegerExtraData* pkRightLeg = NiNew NiIntegerExtraData(RIGHT_LEG);
	pkPart->AddExtraData("part",pkRightLeg);

	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("L_leg");
	NiIntegerExtraData* pkLeftLeg = NiNew NiIntegerExtraData(LEFT_LEG);
	pkPart->AddExtraData("part",pkLeftLeg);

	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("body");
	NiIntegerExtraData* pkBody = NiNew NiIntegerExtraData(BODY);
	pkPart->AddExtraData("part",pkBody);

	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("L_hand");
	NiIntegerExtraData* pkLeftArm = NiNew NiIntegerExtraData(LEFT_ARM);
	pkPart->AddExtraData("part",pkLeftArm);

	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("R_hand");
	NiIntegerExtraData* pkRightArm = NiNew NiIntegerExtraData(RIGHT_ARM);
	pkPart->AddExtraData("part",pkRightArm);*/

	//�Ӹ�
	NiNode* pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_Head");
	assert(pkPart);
	NiIntegerExtraData* pkHeadED = NiNew NiIntegerExtraData(HEAD);
	pkPart->AddExtraData("pick",pkHeadED);

	//��ü
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_Body");
	assert(pkPart);
	NiIntegerExtraData* pkBody = NiNew NiIntegerExtraData(BODY);
	pkPart->AddExtraData("pick",pkBody);

	//����
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_LArm");
	assert(pkPart);
	NiIntegerExtraData* pkLeftArm = NiNew NiIntegerExtraData(LEFT_ARM);
	pkPart->AddExtraData("pick",pkLeftArm);

	//������
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_RArm");
	assert(pkPart);
	NiIntegerExtraData* pkRightArm = NiNew NiIntegerExtraData(RIGHT_ARM);
	pkPart->AddExtraData("pick",pkRightArm);
	
	//���� �ϴ� �ٸ�
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_LLeg01");
	assert(pkPart);
	NiIntegerExtraData* pkLeftLeg01 = NiNew NiIntegerExtraData(LEFT_LEG);
	pkPart->AddExtraData("pick",pkLeftLeg01);
	
	//���� ��� �ٸ�
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_LLeg02");
	assert(pkPart);
	NiIntegerExtraData* pkLeftLeg02 = NiNew NiIntegerExtraData(LEFT_LEG);
	pkPart->AddExtraData("pick",pkLeftLeg02);

	//������ �ϴ� �ٸ�
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_RLeg01");
	assert(pkPart);
	NiIntegerExtraData* pkRightLeg01 = NiNew NiIntegerExtraData(RIGHT_LEG);
	pkPart->AddExtraData("pick",pkRightLeg01);

	//������ ��� �ٸ�
	pkPart = (NiNode*) m_spCharRoot->GetObjectByName("Pick_RLeg02");
	assert(pkPart);
	NiIntegerExtraData* pkRightLeg02 = NiNew NiIntegerExtraData(RIGHT_LEG);
	pkPart->AddExtraData("pick",pkRightLeg02);

	return true;
}

void CBiped::Update(float fTime)
{
	InterpretInput(fTime);					//��ǲ�� ���� �ִϸ��̼� ����
	UpdateBipedRootTransform(fTime);		//ĳ���� ��Ʈ ��ȯ

	m_spActorManager->Update(fTime);
	m_spCharRoot->UpdateSelected(fTime);
}

void CBiped::Reset()
{
		
}

NiNode* CBiped::GetNIFRoot() const
{
	return (NiNode*) m_spActorManager->GetNIFRoot();
}

NiNode* CBiped::GetCharRoot() const
{
	return m_spCharRoot;
}

NiActorManager* CBiped::GetActorManager()	//actor�Ŵ��� ����
{
	return m_spActorManager;
}


NiPoint3 CBiped::GetCharacterPos()
{
	return m_spCharRoot->GetTranslate();
}