#include "main.hpp"

HMODULE g_hModule = nullptr;
void *g_burdenToBearPerk = nullptr;
float g_armorSpeedPenaltyMult = 0.0;

static float __cdecl ApplyArmorSpeedPenalty(float value, ActorValueOwner *avOwner)
{
        Actor *actor = avOwner->GetActor();

        if (!actor || !IS_ACTOR(actor))
                return value;

        if (actor->GetPerkRank(g_burdenToBearPerk, false))
                return value * g_armorSpeedPenaltyMult;

        return value;
}

static __declspec(naked) void ArmorSpeedPenaltyHook()
{
        __asm {
		push    dword ptr [ebp+8h] // avOwner
		push    dword ptr [ebp-10h] // penalty value
		call    ApplyArmorSpeedPenalty
		add     esp, 8
		fstp    dword ptr [ebp-10h] // store returned float
		cmp     dword ptr [ebp+0Ch], 0
		jnz     notNull
		jmp     [skipAddr]
	notNull:
		jmp     [returnAddr]
        }
}

static bool InstallHook()
{
        char path[MAX_PATH];
        GetModuleFileNameA(g_hModule, path, MAX_PATH);
        char *ext = strrchr(path, '.');
        if (ext)
                strcpy(ext, ".ini");

        char buf[32];
        GetPrivateProfileStringA("Settings", "SpeedPenaltyMult", "0", buf, sizeof(buf), path);
        g_armorSpeedPenaltyMult = static_cast<float>(strtod(buf, nullptr));

        g_burdenToBearPerk = GetByID("NVDLC04BurdenToBearPerk");
        if (!g_burdenToBearPerk)
        {
                MessageBoxA(nullptr, "Failed to find BurdenToBear perk form.", PLUGIN_NAME, MB_OK | MB_ICONERROR);
                return false;
        }

        WriteRelJump(kHookAddr, reinterpret_cast<uint32_t>(ArmorSpeedPenaltyHook));
        SafeWrite8(kHookNop, 0x90);
        return true;
}

void MessageHandler(NVSEMessagingInterface::Message *msg)
{
        if (msg->type == NVSEMessagingInterface::kMessage_DeferredInit) InstallHook();
}

EXTERN_DLL_EXPORT bool NVSEPlugin_Query(const NVSEInterface *nvse, PluginInfo *info)
{
        info->infoVersion = PluginInfo::kInfoVersion;
        info->name = PLUGIN_NAME;
        info->version = VERSION;
        return true;
}

EXTERN_DLL_EXPORT bool NVSEPlugin_Load(NVSEInterface *nvse)
{
        if (!nvse->isEditor)
        {
                auto &rNVSE = NVSEGlobalManager::GetSingleton();
                rNVSE.Initialize(nvse);
                rNVSE.RegisterPluginEventListener("NVSE", MessageHandler);
        }
        return true;
}

BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID)
{
        if (dwReason == DLL_PROCESS_ATTACH)
                g_hModule = static_cast<HMODULE>(hDllHandle);

        return TRUE;
}
