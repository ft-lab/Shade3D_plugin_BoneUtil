/**
 *  @brief  �{�[���̉�]�ƃX�P�[�������������ĕ�������@�\.
 *  @date   2014.08.07 - 2014.08.07.  
 */

#ifndef _BONECLEARATTRIBUTEINTERACE_H
#define _BONECLEARATTRIBUTEINTERACE_H

#include "GlobalHeader.h"

class CBoneClearAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;
	BoneClearData m_data;

	/**
	 * �{�[���̉�]�ƃX�P�[���̃N���A�������s.
	 */
	void m_DoBoneClear(sxsdk::scene_interface *scene);
	void m_DoBoneClearLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, std::vector<sxsdk::shape_class *>& cOrgShapeList, std::vector<sxsdk::shape_class *>& cShapeList);

	/**
	 * �w��̃|���S�����b�V���̃X�L�������ւ�.
	 */
	void m_ReplaceMeshSkin(sxsdk::shape_class& meshShape, std::vector<sxsdk::shape_class *>& cOrgBonesList, std::vector<sxsdk::shape_class *>& cNewBonesList);

	/**
	 * �w��̃{�[�������X�g�ɑ��݂���ꍇ�A���̃C���f�b�N�X��Ԃ�.
	 */
	int m_FindBoneIndex(sxsdk::shape_class* pBoneShape, std::vector<sxsdk::shape_class *>& bonesList);

	//--------------------------------------------------//
	//	�_�C�A���O�̃C�x���g�����p						//
	//--------------------------------------------------//
	// �_�C�A���O�̏�����.
	virtual void initialize_dialog (sxsdk::dialog_interface &d, void * = 0);

	// �_�C�A���O�̃C�x���g���󂯎��.
	virtual bool respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void * = 0);

	// �_�C�A���O�̃f�[�^��ݒ肷��.
	virtual void load_dialog_data (sxsdk::dialog_interface &d, void * = 0);

	// �l�̕ύX��ۑ�����Ƃ��ɌĂ΂��.
	virtual void save_dialog_data (sxsdk::dialog_interface &d, void * = 0);

	// �_�C�A���O��������Ƃ��ɌĂ΂��.
	virtual int terminate (sxsdk::dialog_interface &d, int result, void * = 0);

public:
	CBoneClearAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CBoneClearAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return BONECLEAR_ATTRIBUTE_INTERFACE_ID; }

	// �v���O�C������SXUL(text.sxul)���擾.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("boneclear_title"); }

	// �`��̃J�X�^������L����.
	virtual void accepts_shape (bool &accept, void *) { accept = true; }

	// �J�X�^����񂪑I�����ꂽ.
	virtual bool ask_shape (sxsdk::shape_class &shape, void * = 0);
};

#endif
