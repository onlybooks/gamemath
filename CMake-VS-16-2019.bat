@echo off

@echo:
@echo [SoftRenderer Project Generator]
@echo -------------------------------------------------
PUSHD %~dp0
@echo Removing previous project folder.
IF EXIST "Project" rd "Project" /s /q
@echo:
@echo Done!
@echo:
@echo -------------------------------------------------
IF NOT EXIST "Project" @echo Creating a new project folder.
IF NOT EXIST "Project" md Project
IF NOT EXIST "Project" @echo:
cd Project
@echo:
@echo Done!
@echo:
@echo -------------------------------------------------
@echo Running CMAKE script.
@echo:

cmake -G "Visual Studio 16 2019" ..\

@echo:
@echo Done!
@echo:
@echo -------------------------------------------------
@echo A new solution file is generated in %~dp0Project
@echo:
POPD
PAUSE
