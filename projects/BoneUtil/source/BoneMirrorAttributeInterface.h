/**
 *  @brief  ボーンのミラーリング複製機能.
 *  @date   2014.08.05 - 2014.08.05.  
 */

#ifndef _BONEMIRRORATTRIBUTEINTERFACE_H
#define _BONEMIRRORATTRIBUTEINTERFACE_H

#include "GlobalHeader.h"

class CBoneMirrorData {
public:
	int axis;
	float position;
	bool replaceName;
	std::string replaceSrcText;
	std::string replaceDstText;

	CBoneMirrorData() {
		axis           = 0;
		position       = 0.0f;
		replaceName    = true;
		replaceSrcText = "left";
		replaceDstText = "right";
	}
};

class CBoneMirrorAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;
	CBoneMirrorData m_boneMirrorData;

	/**
	 * streamから設定を呼び出し.
	 */
	CBoneMirrorData m_LoadData(sxsdk::scene_interface *scene);

	/**
	 * 設定を保存.
	 */
	void m_SaveData(sxsdk::scene_interface *scene, const CBoneMirrorData data);

	/**
	 * ミラーリング実行.
	 */
	void m_DoBoneMirror(sxsdk::scene_interface *scene, const CBoneMirrorData& data);
	void m_DoBoneMirrorLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, const CBoneMirrorData& data, std::vector<sxsdk::shape_class *>& cShapeList);

public:
	CBoneMirrorAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CBoneMirrorAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return BONEMIRROR_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("title"); }

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
