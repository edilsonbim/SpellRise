#include "SpellRise/Progression/SpellRiseProgressionBalanceData.h"

bool USpellRiseProgressionBalanceData::GetLevelRow(int32 Level, FSpellRiseLevelProgressionRow& OutRow) const
{
	for (const FSpellRiseLevelProgressionRow& Row : Levels)
	{
		if (Row.Level == Level)
		{
			OutRow = Row;
			return true;
		}
	}

	return false;
}
