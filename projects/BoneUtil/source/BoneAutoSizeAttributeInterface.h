/**
 *  @brief  ボーンのサイズを長さから自動調整する機能.
 *  @date   2014.08.07 - 2014.08.07
 */

#ifndef _BONEAUTOSIZEATTRIBUTEINTERFACE_H
#define _BONEAUTOSIZEATTRIBUTEINTERFACE_H

#include "GlobalHeader.h"

class CBoneAutoSizeData {
public:
	bool changeByDistance;		// 距離により自動調整する.
	float boneSize;				// ボーンのサイズ.
	float scale;				// スケーリング値.

	CBoneAutoSizeData() {
		changeByDistance = true;
		boneSize         = 10.0f;
		scale            = 1.0f;
	}
};

class CBoneAutoSizeAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;

	CBoneAutoSizeData m_data;

	/**
	 * streamから設定を呼び出し.
	 */
	CBoneAutoSizeData m_LoadData(sxsdk::scene_interface *scene);

	/**
	 * 設定を保存.
	 */
	void m_SaveData(sxsdk::scene_interface *scene, const CBoneAutoSizeData data);

	/**
	 * ボーンのサイズ変更処理実行.
	 */
	void m_DoBoneAutoSize(sxsdk::scene_interface *scene);
	void m_DoBoneAutoSizeLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::shape_class* prevShape);

public:
	CBoneAutoSizeAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CBoneAutoSizeAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return BONEAUTOSIZE_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("boneautosize_title"); }

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
