/**
 *  @brief  �{�[����胁�b�V���𐶐�.
 *  @date   2014.08.10 - 2014.08.10.
 */

#ifndef _CREATEBONETOMESH_H
#define _CREATEBONETOMESH_H

#include "GlobalHeader.h"

class CCreateBoneToMesh {
private:
	sxsdk::shade_interface* m_shade;

	class CBoneInfo {
	public:
		sxsdk::shape_class* pShape;		// �`��̃|�C���^.
		sxsdk::vec3 wPos;				// ���[���h���W�ʒu.
		int parentBoneIndex;			// �e�{�[��.
		float boneSize;					// �{�[���̑傫��.

		CBoneInfo() {
			pShape = NULL;
			wPos   = sxsdk::vec3(0, 0, 0);
			parentBoneIndex = -1;
			boneSize = 1.0f;
		}
	};

	std::vector<CBoneInfo> m_bonesInfo;		// �ꎞ�I�ɒ~����{�[�����.

	/**
	 * �{�[�����X�g���擾.
	 */
	void m_GetBonesList(sxsdk::shape_class& boneRootShape);
	void m_GetBonesListLoop(int depth, sxsdk::shape_class& shape, const int parentBoneIndex);

	/**
	 * �w��̎n�_�ƏI�_�����ԃ{�b�N�X�̃��b�V�����_���擾.
	 */
	std::vector<sxsdk::vec3> m_CalcBox(const sxsdk::vec3 stWPos, const sxsdk::vec3 endWPos, const float size);

public:
	CCreateBoneToMesh(sxsdk::shade_interface* shade);
	~CCreateBoneToMesh();

	/**
	 * �{�[�����炻����͂ރ��b�V�����쐬.
	 */
	sxsdk::shape_class* CreateMesh(sxsdk::scene_interface* scene, sxsdk::shape_class& boneRootShape, const float fatSize = 1.0f);
};

#endif
