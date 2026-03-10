#pragma once
#include "nvse/PluginAPI.h"
#include "shared/Defines.hpp"
#include <Windows.h>
#include <cstdint>

#define ADDR_ReturnTrue 0x8D0360
#define IS_ACTOR(form) ((*(uint32_t **)form)[0x100 >> 2] == ADDR_ReturnTrue)

constexpr auto PLUGIN_NAME = "BurdenToBearRestored";
constexpr int VERSION = 100;

constexpr uint32_t kHookAddr = 0x647D64;
constexpr uint32_t kHookNop = 0x647D69;
static const uint32_t skipAddr = 0x647DCE;
static const uint32_t returnAddr = 0x647D6A;

extern HMODULE g_hModule;
extern void *g_burdenToBearPerk;
extern float g_armorSpeedPenaltyMult;

__forceinline static void *GetByID(const char *editorID)
{
        return CdeclCall<void *>(0x483A00, editorID);
}

class Actor
{
      public:
        using GetPerkRank_t = uint8_t(__thiscall *)(Actor *, void *, bool);

        uint8_t GetPerkRank(void *perk, bool alt)
        {
                auto vtbl = *reinterpret_cast<void ***>(this);
                auto fn = reinterpret_cast<GetPerkRank_t>(vtbl[0x128]);
                return fn(this, perk, alt);
        }
};

class ActorValueOwner
{
      public:
        ActorValueOwner();
        ~ActorValueOwner();

        // GetBaseActorValue (used from Eval) result in EAX
        virtual uint32_t GetBaseActorValueInt(uint32_t avCode);
        // GetBaseActorValue internal, result in st
        virtual float GetBaseActorValue(uint32_t avCode);
        // GetActorValue internal, result in EAX
        virtual uint32_t GetActorValueInternal(uint32_t avCode);
        // GetActorValue (used from Eval) result in EAX
        virtual float GetActorValue(uint32_t avCode);
        // GetBaseActorValue04 (internal) result in st
        virtual float GetBaseSubAV(uint32_t avCode);
        // GetDamageActorValue or GetModifiedActorValue
        // called from Fn_08, result in st, added to Fn_01
        virtual float GetSubAVDamage(uint32_t avCode);
        virtual float GetSubAVMod(uint32_t avCode);
        // Manipulate GetPermanentActorValue, maybe convert to integer.
        virtual uint32_t GetPermanentActorValueInt(uint32_t avCode);
        // GetPermanentActorValue (used from Eval) result in EAX
        virtual float GetPermanentActorValue(uint32_t avCode);
        // GetActorBase (= this - 0x100) or GetActorBase (= this - 0x0A4)
        virtual Actor *GetActor();
        virtual uint16_t GetLevel();
};

void LoadConfig();
bool InitPerk();
void OnDeferredInit();
void MessageHandler(NVSEMessagingInterface::Message *msg);
