#include "FireCameraShake.h"

UFireCameraShake::UFireCameraShake()
{
	OscillationDuration = 0.25f;
	OscillationBlendInTime = 0.2f;
	OscillationBlendOutTime = 0.2f;
	
	RotOscillation.Pitch.Amplitude = 7.5f;
	RotOscillation.Pitch.Frequency = 1.f;
	RotOscillation.Pitch.InitialOffset = EOO_OffsetZero;
}
