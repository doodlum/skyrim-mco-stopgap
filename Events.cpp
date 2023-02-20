#include "Events.h"
#include "Settings.h"
#include "Utils.h"

namespace Events
{
	enum Direction : std::uint32_t
	{
		kNeutral = 0,
		kForward = 1,
		kRightForward = 2,
		kRight = 3,
		kRightBackward = 4,
		kBackward = 5,
		kLeftBackward = 6,
		kLeft = 7,
		kLeftForward = 8
	};

	InputEventHandler* InputEventHandler::GetSingleton()
	{
		static InputEventHandler singleton;
		return std::addressof(singleton);
	}

	auto InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
		-> EventResult
	{
		using EventType = RE::INPUT_EVENT_TYPE;
		using DeviceType = RE::INPUT_DEVICE;

		if (Settings::uDodgeKey == kInvalid) {
			return EventResult::kContinue;
		}

		if (!a_event) {
			return EventResult::kContinue;
		}

		for (auto event = *a_event; event; event = event->next) {
			if (event->eventType != EventType::kButton) {
				continue;
			}

			auto button = static_cast<RE::ButtonEvent*>(event);
			if (!button->IsDown()) {
				continue;
			}

			auto key = button->idCode;
			switch (button->device.get()) {
			case DeviceType::kMouse:
				key += kMouseOffset;
				break;
			case DeviceType::kKeyboard:
				key += kKeyboardOffset;
				break;
			case DeviceType::kGamepad:
				key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
				break;
			default:
				continue;
			}

			if (key == Settings::uDodgeKey )
				{
				Dodge();
				break;
			}
		}

		return EventResult::kContinue;
	}

	void Dodge()
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerControls = RE::PlayerControls::GetSingleton();

		auto ui = RE::UI::GetSingleton();
		auto controlMap = RE::ControlMap::GetSingleton();

		if (ui->GameIsPaused() || !controlMap->IsMovementControlsEnabled() || !controlMap->IsLookingControlsEnabled() || ui->IsMenuOpen("Dialogue Menu") 
			|| playerCharacter->GetSitSleepState() != RE::SIT_SLEEP_STATE::kNormal || playerCharacter->GetActorValue(RE::ActorValue::kStamina) <= 0) {
			return;
		}
		
		if (!playerCharacter || !playerControls)
		{
			return;
		}

		RE::BShkbAnimationGraph* animationGraph = nullptr;
		RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
		if (playerCharacter->GetAnimationGraphManager(animationGraphManagerPtr)) {
			animationGraph = animationGraphManagerPtr->graphs[animationGraphManagerPtr->activeGraph].get();
		}

		if (!animationGraph)
		{
			return;
		}

		auto normalizedInputDirection = Vec2Normalize(playerControls->data.prevMoveVec);
		if (normalizedInputDirection.x == 0.f && normalizedInputDirection.y == 0.f)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", PI);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kNeutral);
			playerCharacter->NotifyAnimationGraph("Dodge_N");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("neutral");
			return;
		}

		RE::NiPoint2 forwardVector(0.f, 1.f);
		float dodgeAngle = GetAngle(normalizedInputDirection, forwardVector);

		if (dodgeAngle >= -PI8 && dodgeAngle < PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kForward);
			playerCharacter->NotifyAnimationGraph("Dodge_F");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("forward");
		}
		else if (dodgeAngle >= PI8 && dodgeAngle < 3 * PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kRightForward);
			playerCharacter->NotifyAnimationGraph("Dodge_RF");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("right-forward");
		}
		else if (dodgeAngle >= 3 * PI8 && dodgeAngle < 5 * PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kRight);
			playerCharacter->NotifyAnimationGraph("Dodge_R");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("right");
		}
		else if (dodgeAngle >= 5 * PI8 && dodgeAngle < 7 * PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kRightBackward);
			playerCharacter->NotifyAnimationGraph("Dodge_RB");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("right-backward");
		}
		else if (dodgeAngle >= 7 * PI8 || dodgeAngle < 7 * -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kBackward);
			playerCharacter->NotifyAnimationGraph("Dodge_B");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("backward");
		}
		else if (dodgeAngle >= 7 * -PI8 && dodgeAngle < 5 * -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kLeftBackward);
			playerCharacter->NotifyAnimationGraph("Dodge_LB");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("left-backward");
		}
		else if (dodgeAngle >= 5 * -PI8 && dodgeAngle < 3 * -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kLeft);
			playerCharacter->NotifyAnimationGraph("Dodge_L");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("left");
		}
		else if (dodgeAngle >= 3 * -PI8 && dodgeAngle < -PI8)
		{
			animationGraph->SetGraphVariableFloat("Dodge_Angle", dodgeAngle);
			animationGraph->SetGraphVariableInt("Dodge_Direction", kLeftForward);
			playerCharacter->NotifyAnimationGraph("Dodge_LF");
			playerCharacter->NotifyAnimationGraph("Dodge");
			logger::debug("left-forward");
		}
	}

	std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		std::uint32_t index;
		switch (a_key) {
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalid;
			break;
		}

		return index != kInvalid ? index + kGamepadOffset : kInvalid;
	}

	void SinkEventHandlers()
	{
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(InputEventHandler::GetSingleton());
		logger::info("Added input event sink");
	}
}
