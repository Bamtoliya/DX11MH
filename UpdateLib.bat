



xcopy /d /y ".\Engine\Public\*.h"				".\EngineSDK\Inc\"
xcopy /d /y ".\Engine\Bin\*.lib"				".\EngineSDK\Lib\"
xcopy /d /y ".\Engine\Bin\*.dll"				".\Client\Bin\"
xcopy /d /y ".\Engine\Bin\*.dll"				".\Editor\Bin\"
xcopy /d /y ".\External Libraries\physx\bin\debug\*.dll"	".\Client\Bin\"
xcopy /d /y ".\External Libraries\physx\bin\debug\*.dll"	".\Ediotor\Bin\"