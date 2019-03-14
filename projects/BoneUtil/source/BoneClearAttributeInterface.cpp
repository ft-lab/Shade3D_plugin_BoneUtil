/**
 *  @brief  �{�[���̉�]�ƃX�P�[�������������ĕ�������@�\.
 *  @date   2014.08.07 - 2014.09.12.  
 */

#include "BoneClearAttributeInterface.h"
#include "BoneUtil.h"
#include "StreamCtrl.h"

enum {
	dlg_replace_id = 101,		// �{�[���̒u������.
};

CBoneClearAttributeInterface::CBoneClearAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CBoneClearAttributeInterface::~CBoneClearAttributeInterface()
{
}

// �J�X�^����񂪑I�����ꂽ.
bool CBoneClearAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	if (!BoneUtil::IsBone(shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return false;
	}

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(BONEMIRROR_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("bone_clear_attrib_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()�ɍ��킹�āA�Q�ƃJ�E���^�𑝂₷�B .

	m_data = StreamCtrl::LoadBonesClearInfo(scene);

	// �_�C�A���O�̕\��.
	if(dlg->ask()) {
		StreamCtrl::SaveBonesClearInfo(scene, m_data);
		m_DoBoneClear(scene);
	}

	return true;
}

/**
 * �{�[���̉�]�ƃX�P�[���̃N���A�������s.
 */
void CBoneClearAttributeInterface::m_DoBoneClear(sxsdk::scene_interface *scene)
{
	sxsdk::shape_class& active_shape = scene->active_shape();
	if (!BoneUtil::IsBone(active_shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return;
	}

	// active_shape�̃{�[�����Q�Ƃ��Ă���|���S�����b�V�����擾.
	std::vector<sxsdk::shape_class *> meshList, boneList;
	int bIndex = -1;
	if (BoneUtil::GetBoneMeshsList(scene, meshList, boneList)) {	// �V�[�����̃��b�V���ƃ{�[���̃��X�g���擾.
		for (int i = 0; i < boneList.size(); i++) {
			if (BoneUtil::ChkRootBone(active_shape, *boneList[i])) {
				bIndex = i;
				break;
			}
		}
	}

	// �{�[���\���𕡐����A�{�[���̉�]�ƃX�P�[���̃N���A���������s.
	std::vector<sxsdk::shape_class *> cOrgShapeList, cShapeList;
	scene->begin_creating_at(&active_shape);
	m_DoBoneClearLoop(scene, active_shape, NULL, cOrgShapeList, cShapeList);
	scene->end_creating();

	// �{�[����u�������B�X�L�������蓖�Ă��|���S�����b�V�������݂���ꍇ�́A������V�������̂ɒu������.
	if (m_data.replaceAndUpdateSkin && cShapeList.size() > 0) {
		// �|���S�����b�V���̃X�L���ݒ��V�����{�[���ɒu������.
		if (bIndex >= 0) {
			//const int stTim = scene->get_current_time();
			m_ReplaceMeshSkin(*meshList[bIndex], cOrgShapeList, cShapeList);
			//const int endTim = scene->get_current_time();
			{
			//	char szStr[256];
			//	sprintf(szStr, "calc time = %d ms", endTim - stTim);
			//	shade.message(szStr);
			}
		}

		cOrgShapeList[0]->remove();
		cShapeList[0]->select();
		return;
	}

	// ���������擪�̃{�[����I��.
	if (cShapeList.size() > 0) {
		cShapeList[0]->select();
	}
}

/**
 * �w��̃{�[�������X�g�ɑ��݂���ꍇ�A���̃C���f�b�N�X��Ԃ�.
 */
int CBoneClearAttributeInterface::m_FindBoneIndex(sxsdk::shape_class* pBoneShape, std::vector<sxsdk::shape_class *>& bonesList)
{
	int index = -1;
	void * handleP = pBoneShape->get_handle();
	for (int i = 0; i < bonesList.size(); i++) {
		if (bonesList[i]->get_handle() == handleP) {
			index = i;
			break;
		}
	}
	return index;
}

/**
 * �w��̃|���S�����b�V���̃X�L�������ւ�.
 */
void CBoneClearAttributeInterface::m_ReplaceMeshSkin(sxsdk::shape_class& meshShape, std::vector<sxsdk::shape_class *>& cOrgBonesList, std::vector<sxsdk::shape_class *>& cNewBonesList)
{
	if (meshShape.get_type() != sxsdk::enums::polygon_mesh) return;

	sxsdk::polygon_mesh_class& pmesh = meshShape.get_polygon_mesh();
	const int verCou = pmesh.get_total_number_of_control_points();

	// ��x�X�L���̃p�����[�^��ێ��B�������Ȃ��ƁA�ό`�O�ɖ߂��Ă��炻��ɃX�L�����蓖�Ă���Ă��܂�.
	class CSkinV {
	public:
		sxsdk::shape_class* shape;
		float weight;
		CSkinV() {
			shape = NULL;
			weight = 0.0f;
		}
	};
	std::vector< std::vector<CSkinV> > vSkinInfo;
	vSkinInfo.resize(verCou);

	for (int vLoop = 0; vLoop < verCou; vLoop++) {
		sxsdk::vertex_class& v  = pmesh.vertex(vLoop);
		sxsdk::skin_class& skin = v.get_skin();
		const int bind_cou = skin.get_number_of_binds();
		if (bind_cou <= 0) continue;

		std::vector<CSkinV>& bindData = vSkinInfo[vLoop];

		for (int i = 0; i < bind_cou; i++) {
			sxsdk::skin_bind_class& skin_bind = skin.get_bind(i);
			int index = m_FindBoneIndex(skin_bind.get_shape(), cOrgBonesList);
			CSkinV skinV;
			if (index >= 0) {
				skinV.shape  = cNewBonesList[index];
			} else {
				skinV.shape  = skin_bind.get_shape();
			}
			skinV.weight = skin_bind.get_weight();
			bindData.push_back(skinV);
		}
	}

	// �X�L���������ׂăN���A.
	pmesh.clear_skin();
	pmesh.update_skin_bindings();
	pmesh.set_skin_type(-1);

	// �X�L�����Ċ��蓖�� �i�������{�g���l�b�N�j.
	pmesh.set_skin_type(1);
	for (int vLoop = 0; vLoop < verCou; vLoop++) {
		sxsdk::vertex_class& v = pmesh.vertex(vLoop);
		sxsdk::skin_class& skin = v.get_skin();

		std::vector<CSkinV>& skinV = vSkinInfo[vLoop];
		for (int i = 0; i < skinV.size(); i++) {
			skin.append_bind();
			sxsdk::skin_bind_class& skin_bind = skin.get_bind(i);
			skin_bind.set_shape(skinV[i].shape);
			skin_bind.set_weight(skinV[i].weight);		// ���ꂪ�d���H.
		}
	}

	pmesh.update_skin_bindings();
}

void CBoneClearAttributeInterface::m_DoBoneClearLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, std::vector<sxsdk::shape_class *>& cOrgShapeList, std::vector<sxsdk::shape_class *>& cShapeList)
{
	if (!BoneUtil::IsBone(shape)) return;

	float size = 0.0f;
	const sxsdk::mat4 lwMat = (parentPart) ? ((parentPart->get_transformation()) * (parentPart->get_local_to_world_matrix())) : shape.get_local_to_world_matrix();

	sxsdk::vec3 center = BoneUtil::GetBoneCenter(shape, &size);

	center = center * inv(lwMat);
	sxsdk::vec3 axis_dir(1, 0, 0);

	sxsdk::part_class& part = scene->begin_bone_joint(center, size, false, axis_dir);
	cOrgShapeList.push_back(&shape);
	cShapeList.push_back(&part);

	// ���O�̕ύX.
	std::string name = shape.get_name();
	part.set_name(name.c_str());

	if (shape.has_son()) {
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_DoBoneClearLoop(scene, *pShape, &part, cOrgShapeList, cShapeList);
		}
	}

	scene->end_bone_joint();
}

// �_�C�A���O�̏�����.
void CBoneClearAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// �_�C�A���O�̃C�x���g���󂯎��.
bool CBoneClearAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// �A�N�V�������������_�C�A���O�A�C�e����ID.

	if (id == dlg_replace_id) {
		m_data.replaceAndUpdateSkin = item.get_bool();
		return true;
	}

	return false;
}

// �_�C�A���O�̃f�[�^��ݒ肷��.
void CBoneClearAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_replace_id));
		item->set_bool(m_data.replaceAndUpdateSkin);
	}
}

// �l�̕ύX��ۑ�����Ƃ��ɌĂ΂��.
void CBoneClearAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// �_�C�A���O��������Ƃ��ɌĂ΂��.
int CBoneClearAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}
