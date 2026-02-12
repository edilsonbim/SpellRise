#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PrimaryAttributeRow.generated.h"

USTRUCT(BlueprintType)
struct FPrimaryAttributeRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FPrimaryAttributeRow()
		: RowName(NAME_None)
		, Abbr(TEXT(""))
		, Description(TEXT(""))
		, Min(0)
		, Max(0)
		, Base(0)
	{}

	// (Opcional) Você não precisa disso na DataTable porque o RowName já existe no DataTable.
	// Se quiser manter, ok.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Abbr = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FString Description = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Min = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Max = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Base = 0;
};
