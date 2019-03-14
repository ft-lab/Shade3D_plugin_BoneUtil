/**
 *   @brief  ボーンより、それを囲むような直方体を生成.
 */

#include "CreateBonesMeshAttributeInterface.h"

#include "RigCtrl.h"
#include "StreamCtrl.h"
#include "CreateBoneToMesh.h"
#include "BoneUtil.h"

enum {
	dlg_fat_size_id = 101,		// 膨らました際の大きさ.
};

CCreateBonesMeshAttributeInterface::CCreateBonesMeshAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CCreateBonesMeshAttributeInterface::~CCreateBonesMeshAttributeInterface()
{
}

// カスタム情報が選択された.
bool CCreateBonesMeshAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	if (!BoneUtil::IsBone(shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return false;
	}

	m_createData = StreamCtrl::LoadBonesMeshCreateInfo(scene);

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(CREATE_BONES_MESH_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("create_bones_mesh_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	if(dlg->ask()) {
		// メッシュの作成.
		try {
			compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
			CCreateBoneToMesh createBoneMesh(&shade);
			createBoneMesh.CreateMesh(scene, shape, m_createData.fat_size);
		} catch (...) { }

		StreamCtrl::SaveBonesMeshCreateInfo(scene, m_createData);
	}

	return true;
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CCreateBonesMeshAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CCreateBonesMeshAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_fat_size_id) {
		m_createData.fat_size = item.get_float();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CCreateBonesMeshAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_fat_size_id));
		item->set_float(m_createData.fat_size);
	}
}

// 値の変更を保存するときに呼ばれる.
void CCreateBonesMeshAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CCreateBonesMeshAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}

