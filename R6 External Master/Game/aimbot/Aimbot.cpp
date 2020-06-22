#pragma once
#include "AimMath.h"
#include "../Game.h"
#include "../Offsets.h"
#include "../Vectors.h"

HWND TargetHWND = FindWindowA("R6Game", NULL);

uintptr_t pGameManager = mRead<uintptr_t>(base_address + address_game_manager);
uintptr_t pEntityList = mRead<uintptr_t>(pGameManager + offset_entity_list);
uintptr_t pRender = mRead<uintptr_t>(base_address + address_profile_manager);
auto pGameRenderer = mRead<UINT_PTR>(pRender + 0x78);
auto pEngineLink = mRead<UINT_PTR>(pGameRenderer + 0x0);
auto pEngine = mRead<UINT_PTR>(pEngineLink + 0x130);
uintptr_t pCamera = mRead<uintptr_t>(pEngine + 0x420);

Vector2 GetDisplaySize()
{
	RECT rect;
	GetClientRect(TargetHWND, &rect);
	return Vector2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
}

Vector3 GetViewRight()
{
	if (!pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWRIGHT);
}

Vector3 GetViewUp()
{
	if (pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWUP);
}

Vector3 GetViewForward()
{
	if (!pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWFORWARD);
}

Vector3 GetViewTranslation()
{
	if (!pCamera)
		return Vector3();

	return mRead<Vector3>(pCamera + OFFSET_VIEWTRANSLATION);
}

float GetViewFovX()
{
	if (!pCamera)
		return 0.f;

	return mRead<float>(pCamera + OFFSET_VIEWFOVX);
}

float GetViewFovY()
{
	if (!pCamera)
		return 0.f;

	return mRead<float>(pCamera + OFFSET_VIEWFOVY);
}

bool WorldToScreen(Vector3 position, Vector2* Screen)
{
	if (!pCamera)
		return false;

	Vector3 temp = position - GetViewTranslation();

	float x = temp.Dot(GetViewRight());
	float y = temp.Dot(GetViewUp());
	float z = temp.Dot(GetViewForward() * -1.f);

	Vector2 DisplaySize = GetDisplaySize();

	Screen->x = (DisplaySize.x / 2.f) * (1.f + x / GetViewFovX() / z);
	Screen->y = (DisplaySize.y / 2.f) * (1.f - y / GetViewFovY() / z);

	return z >= 1.0f ? true : false;
}


Vector4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
{
	Vector4 result;
	float cy = cos(yaw * 0.5);
	float sy = sin(yaw * 0.5);
	float cr = cos(roll * 0.5);
	float sr = sin(roll * 0.5);
	float cp = cos(pitch * 0.5);
	float sp = sin(pitch * 0.5);

	result.mData[3] = cy * cr * cp + sy * sr * sp;
	result.mData[0] = cy * sr * cp - sy * cr * sp;
	result.mData[1] = cy * cr * sp + sy * sr * cp;
	result.mData[2] = sy * cr * cp - cy * sr * sp;

	return result;
}

void set_angles(uint64_t local_player, Vector3& angles)
{
	float d2r = 0.01745329251f;
	Vector4 new_angles = CreateFromYawPitchRoll(angles.z * d2r, 0.f, angles.x * d2r);

	uint64_t pSkeleton = mRead<uint64_t>(local_player + 0x20);

	if (pSkeleton)
	{
		uint64_t viewAngle2 = mRead<uint64_t>(pSkeleton + 0x11B8);
		mWrite(viewAngle2 + 0xC0, new_angles);
	}
}

Vector3 get_player_head(uint64_t player)
{
	if (!player)
		return Vector3();

	uint64_t pSkeleton = mRead<uint64_t>(player + 0x20);

	if (!pSkeleton)
		return Vector3();

	return mRead<Vector3>(pSkeleton + OFFSET_HEAD);
}
Vector3 CalcAngle(Vector3 enemypos, Vector3 camerapos)
{
	float r2d = 57.2957795131f;

	Vector3 dir = enemypos - camerapos;

	float x = asin(dir.z / dir.Length()) * r2d;
	float z = atan(dir.y / dir.x) * r2d;

	if (dir.x >= 0.f) z += 180.f;
	if (x > 180.0f) x -= 360.f;
	else if (x < -180.0f) x += 360.f;

	return Vector3(x, 0.f, z + 90.f);

}

Vector2 DisplaySize = GetDisplaySize();

float flCenterX = (DisplaySize.x / 2.f);
float flCenterY = (DisplaySize.y / 2.f);

bool targetWasIterated = false;
uint64_t local_player = 0;
uint64_t m_target = 0;

DWORD64 get_player_count()
{
	mRead<DWORD>(address_game_manager + 0x1D0); //needs updating
}
unsigned short GetPlayerTeam(DWORD64 Player)
{
	DWORD64 Replication = mRead<DWORD64>(Player + OFFSET_ENTITY_REPLICATION);
	if (!Replication)
		return 0xFF; // Means bot - they dont have entity info structure

	unsigned short OnlineTeamID = mRead<unsigned short>(Replication + OFFSET_ENTITY_REPLICATION_TEAM);

	return (OnlineTeamID);
}
DWORD64 GetEntityByID(int it, DWORD64 LocalPlayer)
{
	unsigned short LocalTeam = GetPlayerTeam(LocalPlayer);
	if (LocalTeam == 0xFF)
		return NULL;

	DWORD64 Entity = mRead<DWORD>(pEntityList + (sizeof(PVOID) * it));
	if (!Entity)
		return NULL;

	if (LocalPlayer == Entity)
		return NULL;

	unsigned short EnemyTeam = GetPlayerTeam(Entity);
	if (EnemyTeam == LocalTeam)
		return NULL;

	return Entity;
}
namespace RainbowSix
{
	void Aimbot(bool enable)
	{
		if (enable) {
			unsigned short local_team = GetPlayerTeam(RainbowSix::GetLocalPlayer());

			uint32_t num = get_player_count(); // already above...

			for (uint32_t i = 0; i < num; i++)
			{
				uint64_t target_player = GetEntityByID(i, RainbowSix::GetLocalPlayer());

				if (!target_player)
					continue;

				if (target_player == RainbowSix::GetLocalPlayer())
					continue;

				if (GetPlayerTeam(target_player) == local_team)
					continue;

				Vector3 head_position = get_player_head(target_player);

				Vector2 vHeadScreen;
				WorldToScreen(head_position, &vHeadScreen);

				if (vHeadScreen.x - flCenterX < 70.f && flCenterX - vHeadScreen.x < 70.f && flCenterY - vHeadScreen.y < 70.f && vHeadScreen.y - flCenterY < 70.f)
				{
					if (GetAsyncKeyState(VK_RBUTTON) & 0x8000 && m_target != 0 && m_target == target_player)
					{
						Vector3 angles = CalcAngle(head_position, GetViewTranslation());
						angles.clamp();
						set_angles(local_player, angles);
						targetWasIterated = true;
					}
					else if (GetAsyncKeyState(VK_RBUTTON) && m_target == 0)
					{
						m_target = target_player;
						targetWasIterated = true;
					}

					else if (!GetAsyncKeyState(VK_RBUTTON))
						m_target = 0;
				}
			}
		}
	}
}