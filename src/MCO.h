#pragma once

class MCO
{
public:
	static MCO* GetSingleton()
	{
		static MCO singleton;
		return &singleton;
	}

	static void InstallHooks()
	{
		Hooks::Install();
	}

	void ProcessBlock();

	struct Hooks
	{

		struct AttackBlockHandler_ProcessButton
		{
			static void thunk(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
			{
				func(a_this, a_event, a_data);
				GetSingleton()->ProcessBlock();
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			if (REL::Module::get().version() >= SKSE::RUNTIME_SSE_1_7_99) {
				stl::write_vfunc<0x6, AttackBlockHandler_ProcessButton>(RE::VTABLE_AttackBlockHandler[0]);
			} else {
				stl::write_vfunc<0x4, AttackBlockHandler_ProcessButton>(RE::VTABLE_AttackBlockHandler[0]);
			}
		}
	};
private:
	MCO()
	{
	}

	MCO(const MCO&) = delete;
	MCO(MCO&&) = delete;

	~MCO() = default;

	MCO& operator=(const MCO&) = delete;
	MCO& operator=(MCO&&) = delete;
};
