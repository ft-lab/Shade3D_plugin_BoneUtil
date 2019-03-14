/**
 *  @brief  人体ボーン名を相互変換.
 *  @date   2014.08.15 - 2014.08.15
 */

#include "RenameHumanBoneAttributeInterface.h"
#include "BoneUtil.h"
#include "StreamCtrl.h"
#include "RigCtrl.h"

enum {
	dlg_change_bone_type = 101,		// 変換後のボーン名の種類.
};

CRenameHumanBoneAttributeInterface::CRenameHumanBoneAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CRenameHumanBoneAttributeInterface::~CRenameHumanBoneAttributeInterface()
{
}

// カスタム情報が選択された.
bool CRenameHumanBoneAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;
	
	sxsdk::shape_class* pTargetBoneShape = &(scene->active_shape());
	if (!BoneUtil::IsBone(*pTargetBoneShape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return false;
	}

	// MMDのボーン構造か調べる.
	CRigCtrl rigCtrl(&shade);
	int boneType = human_rig_type_mmd_jp;
	const float mmdBoneV = rigCtrl.CheckMMDBones(*pTargetBoneShape, &boneType);
	bool doRename = false;
	if (mmdBoneV < 0.5f) {
		if (shade.show_message_box(shade.gettext("msg_not_mmd_rigs"), true)) {
			doRename = true;
		}
	} else {
		doRename = true;
	}
	if (!doRename) return false;


	m_data = StreamCtrl::LoadHumanBoneRenameInfo(scene);

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(RENAME_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("bone_rename_attrib_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	// ダイアログの表示.
	if(dlg->ask()) {
		// 名前変更.
		m_Rename(*pTargetBoneShape, boneType, m_data.conv_bones_type);

		StreamCtrl::SaveHumanBoneRenameInfo(scene, m_data);
	}

	return true;
}

/**
 * 指定のボーンの構造より、リネーム.
 */
void CRenameHumanBoneAttributeInterface::m_Rename(sxsdk::shape_class& targetBoneRoot, const int orgRigType, const int convRigType)
{
	m_RenameLoop(targetBoneRoot, orgRigType, convRigType);
}

/**
 * リネームの再帰.
 */
void CRenameHumanBoneAttributeInterface::m_RenameLoop(sxsdk::shape_class& shape, const int orgRigType, const int convRigType)
{
	if (!BoneUtil::IsBone(shape)) return;

	// 対応する人体リグの番号.
	const int boneRigIndex = CRigCtrl::GetHumanBoneIndex(&shade, shape.get_name(), orgRigType);

	// rename.
	if (boneRigIndex >= 0) {
		std::string name = CRigCtrl::GetHumanBoneName(&shade, boneRigIndex, convRigType);
		shape.set_name(name.c_str());
	}

	if (shape.has_son()) {
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_RenameLoop(*pShape, orgRigType, convRigType);
		}
	}
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CRenameHumanBoneAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CRenameHumanBoneAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_change_bone_type) {
		m_data.conv_bones_type = item.get_selection();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CRenameHumanBoneAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_change_bone_type));
		item->set_selection(m_data.conv_bones_type);
	}
}

// 値の変更を保存するときに呼ばれる.
void CRenameHumanBoneAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CRenameHumanBoneAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}

