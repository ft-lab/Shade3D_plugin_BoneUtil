/**
 *  @brief  ボーンの「軸方向」をまとめて変更する機能.
 *  @date   2014.09.13 - 2014.09.13.  
 */

#ifndef _BONEDIRECTIONATTRIBUTEINTERACE_H
#define _BONEDIRECTIONATTRIBUTEINTERACE_H

#include "GlobalHeader.h"

class CBoneDirectionAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;
	BoneDirectionData m_data;

	// 軸方向の変更処理.
	void m_DoBoneDirection(sxsdk::shape_class &shape);
	void m_DoBoneDirectionLoop(sxsdk::shape_class& shape, sxsdk::part_class* parentPart);

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
	CBoneDirectionAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CBoneDirectionAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return BONE_DIRECTION_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("bone_direction_title"); }

	// 形状のカスタム情報を有効化.
	virtual void accepts_shape (bool &accept, void *) { accept = true; }

	// カスタム情報が選択された.
	virtual bool ask_shape (sxsdk::shape_class &shape, void * = 0);
};

#endif
