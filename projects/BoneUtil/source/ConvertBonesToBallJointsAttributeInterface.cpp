/**
 *  @brief  �{�[�����{�[���W���C���g�ɕϊ�.
 */

#include "ConvertBonesToBallJointsAttributeInterface.h"
#include "BoneUtil.h"

CConvertBonesToBallJointsAttributeInterface::CConvertBonesToBallJointsAttributeInterface (sxsdk::shade_interface &shade) : shade(shade)
{
}

CConvertBonesToBallJointsAttributeInterface::~CConvertBonesToBallJointsAttributeInterface ()
{
}

// �J�X�^����񂪑I�����ꂽ.
bool CConvertBonesToBallJointsAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	if (!BoneUtil::IsBoneBallJoint(shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone_balljoint"), false);
		return false;
	}

	m_ConvertToBallJoints(scene, shape);

	return true;
}

/**
 * �{�[�����{�[���W���C���g�ɕϊ�.
 */
void CConvertBonesToBallJointsAttributeInterface::m_ConvertToBallJoints (sxsdk::scene_interface *scene, sxsdk::shape_class& active_shape)
{
	//sxsdk::shape_class& active_shape = scene->active_shape();
	if (!BoneUtil::IsBoneBallJoint(active_shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone_balljoint"), false);
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
	m_ConvertToBallJointsLoop(scene, active_shape, NULL, cOrgShapeList, cShapeList);
	scene->end_creating();

	// �{�[����u�������B�X�L�������蓖�Ă��|���S�����b�V�������݂���ꍇ�́A������V�������̂ɒu������.
	if (cShapeList.size() > 0) {
		// �|���S�����b�V���̃X�L���ݒ��V�����{�[���ɒu������.
		if (bIndex >= 0) {
			m_ReplaceMeshSkin(*meshList[bIndex], cOrgShapeList, cShapeList);
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
int CConvertBonesToBallJointsAttributeInterface::m_FindBoneIndex(sxsdk::shape_class* pBoneShape, std::vector<sxsdk::shape_class *>& bonesList)
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
void CConvertBonesToBallJointsAttributeInterface::m_ReplaceMeshSkin(sxsdk::shape_class& meshShape, std::vector<sxsdk::shape_class *>& cOrgBonesList, std::vector<sxsdk::shape_class *>& cNewBonesList)
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

void CConvertBonesToBallJointsAttributeInterface::m_ConvertToBallJointsLoop (sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, std::vector<sxsdk::shape_class *>& cOrgShapeList, std::vector<sxsdk::shape_class *>& cShapeList)
{
	if (!BoneUtil::IsBoneBallJoint(shape)) return;

	float size = 0.0f;
	const sxsdk::mat4 lwMat = (parentPart) ? ((parentPart->get_transformation()) * (parentPart->get_local_to_world_matrix())) : shape.get_local_to_world_matrix();

	sxsdk::vec3 center = BoneUtil::GetBoneBallJointCenter(shape, &size);
	const bool isBone = BoneUtil::IsBone(shape);

	center = center * inv(lwMat);

	sxsdk::part_class& part = scene->begin_ball_joint(center, size);
	cOrgShapeList.push_back(&shape);
	cShapeList.push_back(&part);

	// ���O�̕ύX.
	std::string name = shape.get_name();
	part.set_name(name.c_str());

	if (shape.has_son()) {
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_ConvertToBallJointsLoop(scene, *pShape, &part, cOrgShapeList, cShapeList);
		}
	}

	scene->end_ball_joint();
}
