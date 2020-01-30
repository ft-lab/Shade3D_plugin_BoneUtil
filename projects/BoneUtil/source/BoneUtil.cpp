/**
 *  @brief   ボーン制御用のユーティリティー.
 *  @date    2014.08.07 - 2014.08.07.
 */

#include "BoneUtil.h"

/**
 * 指定の形状がボーンかどうか.
 */
bool BoneUtil::IsBone (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::part) return false;
	sxsdk::part_class& part = shape.get_part();
	if (part.get_part_type() == sxsdk::enums::bone_joint) return true;
	return false;
}

/**
 * 指定の形状がボールジョイントかどうか.
 */
bool BoneUtil::IsBallJoint (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::part) return false;
	sxsdk::part_class& part = shape.get_part();
	if (part.get_part_type() == sxsdk::enums::ball_joint) return true;
	return false;
}

/**
 * 指定の形状がボーンまたはボールジョイントかどうか.
 */
bool BoneUtil::IsBoneBallJoint (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::part) return false;
	sxsdk::part_class& part = shape.get_part();
	if (part.get_part_type() == sxsdk::enums::bone_joint || part.get_part_type() == sxsdk::enums::ball_joint) return true;
	return false;
}

/**
 * ボーンのワールド座標での中心位置とボーンサイズを取得.
 */
sxsdk::vec3 BoneUtil::GetBoneCenter(sxsdk::shape_class& shape, float *size)
{
	if (size) *size = 0.0f;
	if (!BoneUtil::IsBone(shape)) return sxsdk::vec3(0, 0, 0);

	const sxsdk::mat4 lwMat = shape.get_local_to_world_matrix();
	const sxsdk::vec3 center = sxsdk::vec3(0, 0, 0) * shape.get_transformation() * lwMat;

	try {
		compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
		if (size) *size = bone->get_size();
	} catch (...) { }

	return center;
}

/**
 * ボールジョイントのワールド座標での中心位置とボールジョイントサイズを取得.
 */
sxsdk::vec3 BoneUtil::GetBallJointCenter(sxsdk::shape_class& shape, float *size)
{
	if (size) *size = 0.0f;
	if (!BoneUtil::IsBallJoint(shape)) return sxsdk::vec3(0, 0, 0);

	const sxsdk::mat4 lwMat = shape.get_local_to_world_matrix();
	const sxsdk::part_class& part = shape.get_part();
	const sxsdk::mat4 m2 = inv(part.get_transformation_matrix()) * part.get_transformation();

	compointer<sxsdk::ball_joint_interface> ball_joint(part.get_ball_joint_interface());
	const sxsdk::vec3 pos = ball_joint->get_position();
	const sxsdk::vec4 v4 = sxsdk::vec4(pos, 1) * m2 * lwMat;
	const sxsdk::vec3 center(v4.x, v4.y, v4.z);

	try {
		compointer<sxsdk::ball_joint_interface> ballJoint(shape.get_ball_joint_interface());
		if (size) *size = ballJoint->get_size();
	} catch (...) { }

	return center;
}

/**
 * ボーンまたはボールジョイントのワールド座標での中心位置とサイズを取得.
 */
sxsdk::vec3 BoneUtil::GetBoneBallJointCenter(sxsdk::shape_class& shape, float *size)
{
	if (BoneUtil::IsBallJoint(shape)) return GetBallJointCenter(shape, size);
	if (BoneUtil::IsBone(shape)) return GetBoneCenter(shape, size);
	if (size) *size = 0.0f;
	return sxsdk::vec3(0, 0, 0);
}

/**
 * テキストをSJISに変換.
 */
std::string BoneUtil::ConvUTF8ToSJIS(sxsdk::shade_interface& shade, const std::string str)
{
	std::string str2 = shade.encode(str.c_str(), sxsdk::enums::shift_jis_encoding);
	return str2;
}

/**
 * テキストをSJISからUTF-8に変換.
 */
std::string BoneUtil::ConvSJISToUTF8(sxsdk::shade_interface& shade, const std::string str)
{
	std::string str2 = shade.decode(str.c_str(), sxsdk::enums::shift_jis_encoding);
	return str2;
}

/**
 * C/C++上に埋め込まれたテキストをUTF-8として取得.
 * Win環境の場合は、C/C++のテキストはSJISになっている.
 * MacはUTF-8かも.
 */
std::string BoneUtil::GetUTF8Text(sxsdk::shade_interface& shade, const std::string str)
{
#if SXWINDOWS
	return ConvSJISToUTF8(shade, str);
#else
	return str;
#endif

}

/**
 * 指定のポリゴンメッシュに割り当てられているボーン/ボールジョイントのルートを取得.
 */
sxsdk::shape_class* BoneUtil::GetBoneRoot(sxsdk::shape_class& shapePolygonMesh)
{
	sxsdk::shape_class *pBoneRoot = NULL;
	if (shapePolygonMesh.get_type() != sxsdk::enums::polygon_mesh) return pBoneRoot;

	sxsdk::polygon_mesh_class& pmesh = shapePolygonMesh.get_polygon_mesh();
	const int verCou = pmesh.get_total_number_of_control_points();

	for (int i = 0; i < verCou; i++) {
		sxsdk::vertex_class& v = pmesh.vertex(i);
		sxsdk::skin_class& skin = v.get_skin();
		const int bind_cou = skin.get_number_of_binds();
		if (bind_cou <= 0) continue;

		for (int j = 0; j < bind_cou; j++) {
			sxsdk::shape_class *pShape = skin.get_bind(j).get_shape();

			sxsdk::shape_class *pBoneShape = NULL;
			if (pShape->get_type() == sxsdk::enums::part) {
				sxsdk::part_class *part = &(pShape->get_part());
				const int partType = part->get_part_type();
				if (partType == sxsdk::enums::bone_joint || partType == sxsdk::enums::ball_joint) {
					pBoneShape = pShape;
				}
			}
			if (pBoneShape) {
				while (pBoneShape->has_dad()) {
					sxsdk::part_class *part = pBoneShape->get_dad();
					const int partType = part->get_part_type();
					if (partType == sxsdk::enums::bone_joint || partType == sxsdk::enums::ball_joint) {
						pBoneShape = part;
					} else {
						break;
					}
				}
			}
			if (pBoneShape) {
				pBoneRoot = pBoneShape;
				break;
			}
		}
		if (pBoneRoot) break;
	}

	return pBoneRoot;
}

namespace {
	void m_GetBoneMeshsListLoop(sxsdk::shape_class& shape, std::vector<sxsdk::shape_class *>& retMeshList, std::vector<sxsdk::shape_class *>& retBoneRootList) {

		if ((shape.get_type()) == sxsdk::enums::polygon_mesh) {
			sxsdk::shape_class *pBoneRoot = BoneUtil::GetBoneRoot(shape);
			if (pBoneRoot) {
				retMeshList.push_back(&shape);
				retBoneRootList.push_back(pBoneRoot);
			}
		}

		if (shape.has_son()) {
			sxsdk::shape_class* pShape = shape.get_son();
			while (pShape->has_bro()) {
				pShape = pShape->get_bro();
				m_GetBoneMeshsListLoop(*pShape, retMeshList, retBoneRootList);
			}
		}
	}
}

/**
 * シーン内に存在するボーンとメッシュの組み合わせを取得.
 */
int BoneUtil::GetBoneMeshsList(sxsdk::scene_interface* scene, std::vector<sxsdk::shape_class *>& retMeshList, std::vector<sxsdk::shape_class *>& retBoneRootList)
{
	retMeshList.clear();
	retBoneRootList.clear();
	
	sxsdk::shape_class* pRootShape = &(scene->get_shape());
	if (!(pRootShape->has_son())) return 0;
	m_GetBoneMeshsListLoop(*pRootShape, retMeshList, retBoneRootList);

	return retMeshList.size();
}

/**
 * 指定のボーンの親をたどっていくと、ルートボーンにたどりつくか.
 */
bool BoneUtil::ChkRootBone(sxsdk::shape_class& shape, sxsdk::shape_class& rootBone)
{
	if (shape.get_handle() == rootBone.get_handle()) return true;

	void * rootHandle = rootBone.get_handle();

	sxsdk::shape_class* pShape = &shape;
	while (pShape->has_dad()) {
		pShape = pShape->get_dad();
		if (pShape->get_handle() == rootHandle) return true;
	}
	return false;
}

