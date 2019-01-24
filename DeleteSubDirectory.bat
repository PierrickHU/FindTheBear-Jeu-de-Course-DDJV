@echo off

set nomDossier=%1

if not defined nomDossier (
 echo Pas de dossier a supprimer
 exit /B 1
)

set dossierSuppr=
echo.
for /f "delims=" %%p in ('dir /b /ad') do (
	if %%p==%nomDossier% (
		echo suppression de "%%p"
	    rd /s /q %%p
	    set dossierSuppr=1
	) else (
	    for /f "delims=" %%d in ('dir /b /ad %%p') do if %%d==%nomDossier% (
	    	echo suppression de "%%p\%%d"
	    	rd /s /q %%p\%%d
	    	set dossierSuppr=1
	    )
    )
)
for /f "delims=" %%p in ('dir /b /ad /ah') do (
	if %%p==%nomDossier% (
		echo suppression de "%%p"
	    rd /s /q %%p
	    set dossierSuppr=1
	) else (
	    for /f "delims=" %%d in ('dir /b /ad /ah %%p') do if %%d==%nomDossier% (
	    	echo suppression de "%%p\%%d"
	    	rd /s /q %%p\%%d
	    	set dossierSuppr=1
	    )
    )
)

if defined dossierSuppr (
 	goto :success
) else (
	goto :fail
)

:success
    echo.
    echo Suppression des dossiers "%nomDossier%" finie
    goto end

:fail
    echo.
    echo Aucune suppression des dossiers "%nomDossier%"
    goto end

:end
