// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonMPCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ThirdPersonProjectile.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AThirdPersonMPCharacter

AThirdPersonMPCharacter::AThirdPersonMPCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;

	// Initialize fire specific memebers
	FireRate = 0.25f;
	bFiringWeapon = false;
	ProjectileClass = AThirdPersonProjectile::StaticClass();

	// Initialize destruction vars
	DestructionTime = 3.6f;
	bDead = false;
}

void AThirdPersonMPCharacter::SetCurrentHealth(float const InHealthValue) noexcept
{
	if(GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(InHealthValue, 0.f, MaxHealth);
		OnHealthUpdate();  // Call on health update because rep notify does not notify the actual server itself
	}
}

void AThirdPersonMPCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AThirdPersonMPCharacter::OnHealthUpdate() noexcept
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("My role is %s"), GetLocalRole() == ROLE_Authority ? TEXT("Authority") : TEXT("Client")));
	
	// Specific to local player (Client)
	if(IsLocallyControlled())
	{
		FString const LHealthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LHealthMessage);

		if(CurrentHealth <= 0 && !bDead)
		{
			FString const LDeathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LDeathMessage);
		}
	}

	// Server specific logic
	if(GetLocalRole() == ROLE_Authority)
	{
		FString const LHealthMessage = FString::Printf(TEXT("%s now have %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, LHealthMessage);

		// Utilize the opprotunity of being on the server, and restart the player right away
		if(CurrentHealth <= 0 && !bDead)
		{
			// While restarting. Destroy this pawn.
			PlayerDie();
		}
	}

	// Any other logic to trigger both on server or client shoudl be added here
}

float AThirdPersonMPCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float const LAppliedDamage = CurrentHealth - DamageAmount;
	SetCurrentHealth(LAppliedDamage);
	return LAppliedDamage;
}

void AThirdPersonMPCharacter::StartFire()
{
	if(!bFiringWeapon)
	{
		bFiringWeapon = true;
		GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &AThirdPersonMPCharacter::StopFire,
			FireRate, false);
		
		HandleFire();
	}
}

void AThirdPersonMPCharacter::StopFire()
{
	bFiringWeapon = false;
}

//@ note: Ensured to be called on server
void AThirdPersonMPCharacter::HandleFire_Implementation()
{
	// Spawn in front of the player, a little bit higher.
	FVector const LSpawnLocation = GetActorLocation() +
		(GetActorRotation().Vector() * 100.f) + (GetActorUpVector() * 50.f);
	FRotator const LSpawnRotation = GetActorRotation();

	FActorSpawnParameters LSpawnParameters;
	LSpawnParameters.Instigator = GetInstigator();
	LSpawnParameters.Owner = this;

	// Spawn actor at the server
	GetWorld()->SpawnActor<AThirdPersonProjectile>(ProjectileClass, LSpawnLocation, LSpawnRotation, LSpawnParameters);
}

void AThirdPersonMPCharacter::PlayerDie()
{
	// Ensure to call only on the server
	if(ensureMsgf(GetLocalRole() == ROLE_Authority, TEXT("Bad function call. %s can be called only on the server!"),
		*FString(__FUNCTION__)))
	{
		// Set state of the player as dead
		bDead = true;
		
		// Simulate physics for the mesh to immitate player's death
		GetMesh()->SetAllBodiesSimulatePhysics(true);

		GetWorld()->GetTimerManager().SetTimer(DestroyDelayTimer, [this]
		{
			
			// Notify anyone who was expecting the player to be dead
			OnPlayerDied.Broadcast();
			Destroy();
		
		}, DestructionTime, false);
		
	}
}

void AThirdPersonMPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonMPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonMPCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonMPCharacter::Look);

		// Fire action
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AThirdPersonMPCharacter::StartFire);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AThirdPersonMPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThirdPersonMPCharacter, CurrentHealth);
	DOREPLIFETIME(AThirdPersonMPCharacter, bDead);
}

void AThirdPersonMPCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AThirdPersonMPCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}