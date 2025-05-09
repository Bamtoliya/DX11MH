xcopy ".\Engine\Public\*.h"				".\EngineSDK\Inc\" /y
xcopy ".\Engine\Bin\*.lib"				".\EngineSDK\Lib\" /y
xcopy ".\Engine\Bin\*.dll"				".\Client\Bin\" /y 
xcopy ".\Engine\Bin\*.dll"				".\Editor\Bin\" /y 
xcopy ".\External Libraries\physx\bin\debug\*.dll"	".\Client\Bin\" /y 
xcopy ".\External Libraries\physx\bin\debug\*.dll"	".\Editor\Bin\" /y 
xcopy ".\External Libraries\assimp\bin\debug\*.dll"	".\Client\Bin\" /y 
xcopy ".\External Libraries\assimp\bin\debug\*.dll"	".\Editor\Bin\" /y 