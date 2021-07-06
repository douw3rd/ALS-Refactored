#include "AlsCharacterExample.h"

#include "Components/InputComponent.h"
#include "Utility/AlsMath.h"

void AAlsCharacterExample::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("LookUpMouse"), this, &ThisClass::InputLookUpMouse);
	PlayerInputComponent->BindAxis(TEXT("LookRightMouse"), this, &ThisClass::InputLookRightMouse);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ThisClass::InputLookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &ThisClass::InputLookRight);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ThisClass::InputMoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ThisClass::InputMoveRight);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ThisClass::InputSprintPressed);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ThisClass::InputSprintReleased);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_DoubleClick, this, &ThisClass::InputRoll);

	PlayerInputComponent->BindAction(TEXT("Walk"), IE_Pressed, this, &ThisClass::InputWalk);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ThisClass::InputCrouch);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ThisClass::InputJumpPressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ThisClass::InputJumpReleased);

	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ThisClass::InputAimPressed);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &ThisClass::InputAimReleased);

	PlayerInputComponent->BindAction(TEXT("RotateToVelocityDirection"), IE_Pressed, this, &ThisClass::VelocityDirectionPressed);
	PlayerInputComponent->BindAction(TEXT("RotateToLookDirection"), IE_Pressed, this, &ThisClass::LookingDirectionPressed);

	PlayerInputComponent->BindAction(TEXT("Ragdoll"), IE_Pressed, this, &ThisClass::RagdollPressed);

	PlayerInputComponent->BindAction(TEXT("ViewMode"), IE_Pressed, this, &ThisClass::ViewModePressed);
}

void AAlsCharacterExample::InputLookUpMouse(const float Value)
{
	AAlsCharacterExample::AddControllerPitchInput(-Value * LookUpMouseSensitivity);
}

void AAlsCharacterExample::InputLookRightMouse(const float Value)
{
	AAlsCharacterExample::AddControllerYawInput(Value * LookRightMouseSensitivity);
}

void AAlsCharacterExample::InputLookUp(const float Value)
{
	AAlsCharacterExample::AddControllerPitchInput(-Value * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAlsCharacterExample::InputLookRight(const float Value)
{
	AAlsCharacterExample::AddControllerYawInput(Value * LookRightRate * GetWorld()->GetDeltaSeconds());
}

void AAlsCharacterExample::InputMoveForward(const float Value)
{
	AddMovementInput(FVector{UAlsMath::AngleToDirection(GetAimingState().SmoothRotation.Yaw), 0.0f},
	                 UAlsMath::FixGamepadDiagonalValues(Value, GetInputAxisValue(TEXT("MoveRight"))));
}

void AAlsCharacterExample::InputMoveRight(const float Value)
{
	AddMovementInput(FVector{UAlsMath::AngleToDirection(GetAimingState().SmoothRotation.Yaw + 90.0f), 0.0f},
	                 UAlsMath::FixGamepadDiagonalValues(Value, GetInputAxisValue(TEXT("MoveForward"))));
}

void AAlsCharacterExample::InputSprintPressed()
{
	SetDesiredGait(EAlsGait::Sprinting);
}

void AAlsCharacterExample::InputSprintReleased()
{
	SetDesiredGait(EAlsGait::Running);
}

void AAlsCharacterExample::InputRoll()
{
	TryStartRolling(1.3f);
}

void AAlsCharacterExample::InputWalk()
{
	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
	// ReSharper disable once CppIncompleteSwitchStatement
	switch (GetDesiredGait())
	{
		case EAlsGait::Walking:
			SetDesiredGait(EAlsGait::Running);
			break;

		case EAlsGait::Running:
			SetDesiredGait(EAlsGait::Walking);
			break;
	}
}

void AAlsCharacterExample::InputCrouch()
{
	switch (GetDesiredStance())
	{
		case EAlsStance::Standing:
			SetDesiredStance(EAlsStance::Crouching);
			break;

		case EAlsStance::Crouching:
			SetDesiredStance(EAlsStance::Standing);
			break;
	}
}

void AAlsCharacterExample::InputJumpPressed()
{
	if (TryStopRagdolling())
	{
		return;
	}

	if (TryStartMantlingGrounded())
	{
		return;
	}

	if (GetStance() == EAlsStance::Crouching)
	{
		SetDesiredStance(EAlsStance::Standing);
		return;
	}

	Jump();
}

void AAlsCharacterExample::InputJumpReleased()
{
	StopJumping();
}

void AAlsCharacterExample::InputAimPressed()
{
	SetDesiredAiming(true);
}

void AAlsCharacterExample::InputAimReleased()
{
	SetDesiredAiming(false);
}

void AAlsCharacterExample::VelocityDirectionPressed()
{
	SetDesiredRotationMode(EAlsRotationMode::VelocityDirection);
}

void AAlsCharacterExample::LookingDirectionPressed()
{
	SetDesiredRotationMode(EAlsRotationMode::LookingDirection);
}

void AAlsCharacterExample::ViewModePressed()
{
	SetViewMode(GetViewMode() == EAlsViewMode::FirstPerson
		            ? EAlsViewMode::ThirdPerson
		            : EAlsViewMode::FirstPerson);
}

void AAlsCharacterExample::RagdollPressed()
{
	if (!TryStopRagdolling())
	{
		StartRagdolling();
	}
}
