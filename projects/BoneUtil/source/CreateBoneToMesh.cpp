/**
 *  @brief  ボーンよりメッシュを生成.
 *  @date   2014.08.10 - 2014.08.10.
 */

#include "CreateBoneToMesh.h"
#include "BoneUtil.h"

CCreateBoneToMesh::CCreateBoneToMesh(sxsdk::shade_interface* shade)
{
	m_shade = shade;
}

CCreateBoneToMesh::~CCreateBoneToMesh()
{
}

/**
 * ボーンリストを取得.
 */
void CCreateBoneToMesh::m_GetBonesList(sxsdk::shape_class& boneRootShape)
{
	m_bonesInfo.clear();
	m_GetBonesListLoop(0, boneRootShape, -1);
}

void CCreateBoneToMesh::m_GetBonesListLoop(int depth, sxsdk::shape_class& shape, const int parentBoneIndex)
{
	if (!BoneUtil::IsBone(shape)) return;

	float size;
	sxsdk::vec3 wPos;
	wPos = BoneUtil::GetBoneCenter(shape, &size);

	CBoneInfo bInfo;
	bInfo.pShape   = &shape;
	bInfo.wPos     = wPos;
	bInfo.boneSize = size;
	bInfo.parentBoneIndex = parentBoneIndex;

	const int boneIndex = m_bonesInfo.size();
	m_bonesInfo.push_back(bInfo);

	if (shape.has_son()) {
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_GetBonesListLoop(depth + 1, *pShape, boneIndex);
		}
	}
}

/**
 * ボーンからそれを囲むメッシュを作成.
 */
sxsdk::shape_class* CCreateBoneToMesh::CreateMesh(sxsdk::scene_interface* scene, sxsdk::shape_class& boneRootShape, const float fatSize)
{
	const bool sequenceMode = scene->get_sequence_mode();
	if (sequenceMode) scene->set_sequence_mode(false);

	// ボーンリスト.
	m_GetBonesList(boneRootShape);

	// 1つのボーンに8頂点を使用し、ポリゴンメッシュを生成.
	scene->begin_creating();

	sxsdk::shape_class* pMeshShape = &(scene->begin_polygon_mesh("Mesh"));

	int verCou = 0;
	std::vector<sxsdk::vec3> versBox;
	std::vector<int> boxIndexList;
	for (int loop = 0; loop < m_bonesInfo.size(); loop++) {
		CBoneInfo& bInfo = m_bonesInfo[loop];
		if (bInfo.parentBoneIndex < 0) continue;

		const sxsdk::vec3 stPos  = m_bonesInfo[bInfo.parentBoneIndex].wPos;
		const sxsdk::vec3 endPos = bInfo.wPos;
		const float size = (bInfo.boneSize + m_bonesInfo[bInfo.parentBoneIndex].boneSize) * 0.5f * fatSize;

		// stPos - endWPosが作る頂点（8つ）を計算.
		versBox = m_CalcBox(stPos, endPos, size);

		// 頂点の追加.
		if (versBox.size() > 0) {
			for (int i = 0; i < versBox.size(); i++) {
				scene->append_polygon_mesh_vertex(versBox[i]);
			}
			verCou += versBox.size();
			boxIndexList.push_back(bInfo.parentBoneIndex);
		}
	}

	// 面の追加.
	if (verCou > 0) {
		for (int i = 0; i < verCou; i+= 8) {
			scene->append_polygon_mesh_face(i + 0, i + 3, i + 2, i + 1);
			scene->append_polygon_mesh_face(i + 4, i + 5, i + 6, i + 7);
			scene->append_polygon_mesh_face(i + 5, i + 1, i + 2, i + 6);
			scene->append_polygon_mesh_face(i + 6, i + 2, i + 3, i + 7);
			scene->append_polygon_mesh_face(i + 7, i + 3, i + 0, i + 4);
			scene->append_polygon_mesh_face(i + 4, i + 0, i + 1, i + 5);
		}
	}
	scene->append_polygon_mesh_edges();

	scene->end_polygon_mesh();
	scene->end_creating();

	// スキンの割り当て.
	{
		int vPos = 0;
		sxsdk::polygon_mesh_class& pMesh = pMeshShape->get_polygon_mesh();

		pMeshShape->set_skin_type(1);		// 頂点ブレンド方式のスキン.
		for (int loop = 0; loop < boxIndexList.size(); loop++) {
			const int boneIndex = boxIndexList[loop];

			for (int i = 0; i < 8; i++) {
				sxsdk::vertex_class& v = pMesh.vertex(vPos + i);
				sxsdk::skin_class& skin = v.get_skin();
				skin.append_bind();
				sxsdk::skin_bind_class& skinBind = skin.get_bind(0);
				skinBind.set_weight(1.0f);
				skinBind.set_shape(m_bonesInfo[boneIndex].pShape);
			}
			vPos += 8;
		}
		pMesh.update_skin_bindings();
	}

	pMeshShape->select();

	scene->set_sequence_mode(sequenceMode);

	return pMeshShape;
}

/**
 * 指定の始点と終点を結ぶボックスのメッシュ頂点を取得.
 */
std::vector<sxsdk::vec3> CCreateBoneToMesh::m_CalcBox(const sxsdk::vec3 stWPos, const sxsdk::vec3 endWPos, const float size)
{
	const float sizeH  = size * 0.5f;
	const float height = sxsdk::absolute(stWPos - endWPos);
	const sxsdk::vec3 vDir = normalize(endWPos - stWPos);

	std::vector<sxsdk::vec3> posList;
	if (height < 1e-3 || size < 1e-3) return posList;

	posList.push_back(sxsdk::vec3(-sizeH,   0.0f, -sizeH));
	posList.push_back(sxsdk::vec3(-sizeH,   0.0f,  sizeH));
	posList.push_back(sxsdk::vec3( sizeH,   0.0f,  sizeH));
	posList.push_back(sxsdk::vec3( sizeH,   0.0f, -sizeH));
	posList.push_back(sxsdk::vec3(-sizeH, height, -sizeH));
	posList.push_back(sxsdk::vec3(-sizeH, height,  sizeH));
	posList.push_back(sxsdk::vec3( sizeH, height,  sizeH));
	posList.push_back(sxsdk::vec3( sizeH, height, -sizeH));

	const sxsdk::mat4 rotM = sxsdk::mat4::rotate(sxsdk::vec3(0, 1, 0), vDir);
	for (int i = 0; i < posList.size(); i++) {
		posList[i] = (posList[i] * rotM) + stWPos;
	}

	return posList;
}

