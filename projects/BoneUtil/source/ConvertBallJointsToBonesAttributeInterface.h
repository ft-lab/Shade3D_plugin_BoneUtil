/**
 *  @brief  ボールジョイントをボーンに変換.
 */

#ifndef _CONVERT_BALLJOINTS_TO_BONES_ATTRIBUTEINTERACE_H
#define _CONVERT_BALLJOINTS_TO_BONES_ATTRIBUTEINTERACE_H

#include "GlobalHeader.h"

class CConvertBallJointsToBonesAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;

	/**
	 * ボールジョイントをボーンに変換.
	 */
	void m_ConvertToBones (sxsdk::scene_interface *scene, sxsdk::shape_class& active_shape);
	void m_ConvertToBonesLoop (sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, std::vector<sxsdk::shape_class *>& cOrgShapeList, std::vector<sxsdk::shape_class *>& cShapeList);

	/**
	 * 指定のポリゴンメッシュのスキンを入れ替え.
	 */
	void m_ReplaceMeshSkin (sxsdk::shape_class& meshShape, std::vector<sxsdk::shape_class *>& cOrgBonesList, std::vector<sxsdk::shape_class *>& cNewBonesList);

	/**
	 * 指定のボーンがリストに存在する場合、そのインデックスを返す.
	 */
	int m_FindBoneIndex (sxsdk::shape_class* pBoneShape, std::vector<sxsdk::shape_class *>& bonesList);


public:
	CConvertBallJointsToBonesAttributeInterface (sxsdk::shade_interface &shade);
	virtual ~CConvertBallJointsToBonesAttributeInterface ();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return CONVERT_BALLJOINTS_TO_BONES_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("balljoint_to_bone_title"); }

	// 形状のカスタム情報を有効化.
	virtual void accepts_shape (bool &accept, void *) { accept = true; }

	// カスタム情報が選択された.
	virtual bool ask_shape (sxsdk::shape_class &shape, void * = 0);
};

#endif
