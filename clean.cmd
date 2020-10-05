::
:: any argument will make exe's and pdb's be deleted, too
::
@echo off
@del /s *.obj>nul
@del /s *.ilk>nul
@IF [%1] == [] GOTO script_exit
@del /s *.exe>nul
@del /s *.pdb>nul
:script_exit