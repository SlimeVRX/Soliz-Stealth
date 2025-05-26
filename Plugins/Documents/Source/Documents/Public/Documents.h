// Copyright TinNguyen1997.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FDocumentsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
