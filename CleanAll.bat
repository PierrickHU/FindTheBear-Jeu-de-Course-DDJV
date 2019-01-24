@echo off
cls

if not "%1"=="" (
	goto :skip
)

echo You are about to clean up all temporary files in this project... Press enter to continue
pause

:skip
cls

@echo off

echo Starting Project Cleanup

@echo off
del /S *.sln>NUL
del /S *.vcxproj*>NUL
del /S *.exe>NUL
del /S *.ilk>NUL
del /S *.pdb>NUL
del /S *.bsc>NUL
del /S jobfiles*.txt>NUL
del /S CMakeCache.txt>NUL
del /S cmake_install.cmake>NUL

@echo off
call DeleteSubDirectory.bat .vs
call DeleteSubDirectory.bat Debug
call DeleteSubDirectory.bat Release
call DeleteSubDirectory.bat x64
call DeleteSubDirectory.bat build-solution-x64
call DeleteSubDirectory.bat projectSolution
call DeleteSubDirectory.bat CMakeFiles

if exist */Debug ( 
		echo -- Failed to remove a Debug directory
		goto fail
	) else (
		echo -- Cleaned Up all Debug directory successfully
	)

if exist */Release ( 
		echo -- Failed to remove a Release directory
		goto fail
	) else (
		echo -- Cleaned Up all Release directory successfully
	)
if exist */x64 ( 
		echo -- Failed to remove a x64 directory
		goto fail
	) else (
		echo -- Cleaned Up all x64 directory successfully
	)

:success
	echo Project cleaned up successfully
	goto end

:fail
	echo Failure in cleaning project
	goto end

:end
