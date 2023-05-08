#include "HealthBarWidget.h"

#include "StatComponent.h"

#include "Components/ProgressBar.h"

void UHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HealthBarState == EHealthBarState::TakeDamage)
	{
		float InterpValue = FMath::FInterpConstantTo(BackgroundBar->GetPercent(), ForegroundBar->GetPercent(), InDeltaTime, 0.3f);
		float ClampValue = FMath::Clamp(InterpValue, 0.0f, 1.0f);
		BackgroundBar->SetPercent(ClampValue);

		if (FMath::IsNearlyEqual(BackgroundBar->GetPercent(), ForegroundBar->GetPercent()))
			HealthBarState = EHealthBarState::Normal;
	}
}

void UHealthBarWidget::OnCurrHPChanged(float NewRatio, ECurrHPChangeType ChangeType)
{
	switch (ChangeType)
	{
	case ECurrHPChangeType::Init:
		BackgroundBar->SetPercent(NewRatio);
		ForegroundBar->SetPercent(NewRatio);
		break;
		
	case ECurrHPChangeType::TakeDamage:
		TakeDamage(NewRatio);
		break;
		
	case ECurrHPChangeType::InstanceHeal:
		InstantHeal(NewRatio);
		break;
	}
}

void UHealthBarWidget::TakeDamage(float NewRatio)
{
	if (HealthBarState == EHealthBarState::WaitForHeal)
	{
		float Delta = ForegroundBar->GetPercent() - NewRatio;
		BackgroundBar->SetPercent(FMath::Max(BackgroundBar->GetPercent() - Delta, 0.f));
		
		ForegroundBar->SetPercent(NewRatio);
	}
	else
	{
		BackgroundBar->SetFillColorAndOpacity(FLinearColor::Red);
		ForegroundBar->SetPercent(NewRatio);
		
		HealthBarState = EHealthBarState::TakeDamage;
	}
}

void UHealthBarWidget::InstantHeal(float NewRatio)
{
	if (HealthBarState == EHealthBarState::Normal)
	{
		BackgroundBar->SetPercent(NewRatio);
		ForegroundBar->SetPercent(NewRatio);
	}
	else if (HealthBarState == EHealthBarState::TakeDamage)
	{
		if (BackgroundBar->GetPercent() <= NewRatio)
		{
			BackgroundBar->SetPercent(NewRatio);
			HealthBarState = EHealthBarState::Normal;
		}

		ForegroundBar->SetPercent(NewRatio);
	}
	else if (HealthBarState == EHealthBarState::WaitForHeal)
	{
		float Delta = NewRatio - ForegroundBar->GetPercent();
		BackgroundBar->SetPercent(BackgroundBar->GetPercent() + Delta);
		
		ForegroundBar->SetPercent(NewRatio);
	}
}

void UHealthBarWidget::WaitForHeal(float NewRatio)
{
	if (HealthBarState == EHealthBarState::WaitForHeal)
		return;
	
	BackgroundBar->SetPercent(NewRatio);
	BackgroundBar->SetFillColorAndOpacity(FLinearColor(0.28f, 0.28f, 0.28f));
	
	HealthBarState = EHealthBarState::WaitForHeal;
}

void UHealthBarWidget::ApplyHeal()
{
	if (HealthBarState != EHealthBarState::WaitForHeal)
		return;

	BackgroundBar->SetPercent(FMath::Clamp(BackgroundBar->GetPercent(), 0.f, 1.f));
	ForegroundBar->SetPercent(BackgroundBar->GetPercent());
	HealthBarState = EHealthBarState::Normal;
}

void UHealthBarWidget::CancelHeal()
{
	if (HealthBarState != EHealthBarState::WaitForHeal)
		return;

	BackgroundBar->SetPercent(ForegroundBar->GetPercent());
	HealthBarState = EHealthBarState::Normal;
}
