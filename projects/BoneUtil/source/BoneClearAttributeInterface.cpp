/**
 *  @brief  ボーンの回転とスケールを初期化して複製する機能.
 *  @date   2014.08.07 - 2014.09.12.  
 */

#include "BoneClearAttributeInterface.h"
#include "BoneUtil.h"
#include "StreamCtrl.h"

enum {
	dlg_replace_id = 101,		// ボーンの置き換え.
};

CBoneClearAttributeInterface::CBoneClearAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CBoneClearAttributeInterface::~CBoneClearAttributeInterface()
{
}

// カスタム情報が選択された.
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
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	m_data = StreamCtrl::LoadBonesClearInfo(scene);

	// ダイアログの表示.
	if(dlg->ask()) {
		StreamCtrl::SaveBonesClearInfo(scene, m_data);
		m_DoBoneClear(scene);
	}

	return true;
}

/**
 * ボーンの回転とスケールのクリア処理実行.
 */
void CBoneClearAttributeInterface::m_DoBoneClear(sxsdk::scene_interface *scene)
{
	sxsdk::shape_class& active_shape = scene->active_shape();
	if (!BoneUtil::IsBone(active_shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return;
	}

	// active_shapeのボーンを参照しているポリゴンメッシュを取得.
	std::vector<sxsdk::shape_class *> meshList, boneList;
	int bIndex = -1;
	if (BoneUtil::GetBoneMeshsList(scene, meshList, boneList)) {	// シーン内のメッシュとボーンのリストを取得.
		for (int i = 0; i < boneList.size(); i++) {
			if (BoneUtil::ChkRootBone(active_shape, *boneList[i])) {
				bIndex = i;
				break;
			}
		}
	}

	// ボーン構造を複製しつつ、ボーンの回転とスケールのクリア処理を実行.
	std::vector<sxsdk::shape_class *> cOrgShapeList, cShapeList;
	scene->begin_creating_at(&active_shape);
	m_DoBoneClearLoop(scene, active_shape, NULL, cOrgShapeList, cShapeList);
	scene->end_creating();

	// ボーンを置き換え。スキンを割り当てたポリゴンメッシュが存在する場合は、それも新しいものに置き換え.
	if (m_data.replaceAndUpdateSkin && cShapeList.size() > 0) {
		// ポリゴンメッシュのスキン設定を新しいボーンに置き換え.
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

	// 複製した先頭のボーンを選択.
	if (cShapeList.size() > 0) {
		cShapeList[0]->select();
	}
}

/**
 * 指定のボーンがリストに存在する場合、そのインデックスを返す.
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
 * 指定のポリゴンメッシュのスキンを入れ替え.
 */
void CBoneClearAttributeInterface::m_ReplaceMeshSkin(sxsdk::shape_class& meshShape, std::vector<sxsdk::shape_class *>& cOrgBonesList, std::vector<sxsdk::shape_class *>& cNewBonesList)
{
	if (meshShape.get_type() != sxsdk::enums::polygon_mesh) return;

	sxsdk::polygon_mesh_class& pmesh = meshShape.get_polygon_mesh();
	const int verCou = pmesh.get_total_number_of_control_points();

	// 一度スキンのパラメータを保持。こうしないと、変形前に戻ってからそれにスキン割り当てされてしまう.
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

	// スキン情報をすべてクリア.
	pmesh.clear_skin();
	pmesh.update_skin_bindings();
	pmesh.set_skin_type(-1);

	// スキンを再割り当て （ここがボトルネック）.
	pmesh.set_skin_type(1);
	for (int vLoop = 0; vLoop < verCou; vLoop++) {
		sxsdk::vertex_class& v = pmesh.vertex(vLoop);
		sxsdk::skin_class& skin = v.get_skin();

		std::vector<CSkinV>& skinV = vSkinInfo[vLoop];
		for (int i = 0; i < skinV.size(); i++) {
			skin.append_bind();
			sxsdk::skin_bind_class& skin_bind = skin.get_bind(i);
			skin_bind.set_shape(skinV[i].shape);
			skin_bind.set_weight(skinV[i].weight);		// これが重い？.
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

	// 名前の変更.
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

// ダイアログの初期化.
void CBoneClearAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CBoneClearAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_replace_id) {
		m_data.replaceAndUpdateSkin = item.get_bool();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CBoneClearAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_replace_id));
		item->set_bool(m_data.replaceAndUpdateSkin);
	}
}

// 値の変更を保存するときに呼ばれる.
void CBoneClearAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CBoneClearAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}
