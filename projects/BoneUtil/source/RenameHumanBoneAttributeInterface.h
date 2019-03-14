/**
 *  @brief  人体ボーン名を相互変換.
 *  @date   2014.08.15 - 2014.08.15
 */

#ifndef _RENAMEHUMANBONEATTRIBUTEINTERFACE_H
#define _RENAMEHUMANBONEATTRIBUTEINTERFACE_H

#include "GlobalHeader.h"


class CRenameHumanBoneAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;

	HumanBoneRenameData m_data;

	/**
	 * 指定のボーンの構造より、リネーム.
	 */
	void m_Rename(sxsdk::shape_class& targetBoneRoot, const int orgRigType, const int convRigType);

	/**
	 * リネームの再帰.
	 */
	void m_RenameLoop(sxsdk::shape_class& shape, const int orgRigType, const int convRigType);

public:
	CRenameHumanBoneAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CRenameHumanBoneAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return RENAME_HUMANBONE_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("rename_human_bone_title"); }

	// 形状のカスタム情報を有効化.
	virtual void accepts_shape (bool &accept, void *) { accept = true; }

	// カスタム情報が選択された.
	virtual bool ask_shape (sxsdk::shape_class &shape, void * = 0);

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
};

#endif
