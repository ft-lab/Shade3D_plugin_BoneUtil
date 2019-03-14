/**
 *  @brief  ボーンの回転とスケールを初期化して複製する機能.
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
	 * ボーンの回転とスケールのクリア処理実行.
	 */
	void m_DoBoneClear(sxsdk::scene_interface *scene);
	void m_DoBoneClearLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, std::vector<sxsdk::shape_class *>& cOrgShapeList, std::vector<sxsdk::shape_class *>& cShapeList);

	/**
	 * 指定のポリゴンメッシュのスキンを入れ替え.
	 */
	void m_ReplaceMeshSkin(sxsdk::shape_class& meshShape, std::vector<sxsdk::shape_class *>& cOrgBonesList, std::vector<sxsdk::shape_class *>& cNewBonesList);

	/**
	 * 指定のボーンがリストに存在する場合、そのインデックスを返す.
	 */
	int m_FindBoneIndex(sxsdk::shape_class* pBoneShape, std::vector<sxsdk::shape_class *>& bonesList);

	//--------------------------------------------------//
	//	ダイアログのイベント処理用						//
	//--------------------------------------------------//
	// ダイアログの初期化.
	virtual void initialize_dialog (sxsdk::dialog_interface &d, void * = 0);

	// ダイアログのイベントを受け取る.
	virtual bool respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void * = 0);

	// ダイアログのデータを設定する.
	virtual void load_dialog_data (sxsdk::dialog_interface &d, void * = 0);

	// 値の変更を保存するときに呼ばれる.
	virtual void save_dialog_data (sxsdk::dialog_interface &d, void * = 0);

	// ダイアログが閉じられるときに呼ばれる.
	virtual int terminate (sxsdk::dialog_interface &d, int result, void * = 0);

public:
	CBoneClearAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CBoneClearAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return BONECLEAR_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("boneclear_title"); }

	// 形状のカスタム情報を有効化.
	virtual void accepts_shape (bool &accept, void *) { accept = true; }

	// カスタム情報が選択された.
	virtual bool ask_shape (sxsdk::shape_class &shape, void * = 0);
};

#endif
