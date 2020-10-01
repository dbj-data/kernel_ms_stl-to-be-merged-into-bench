::
:: any argument will make exe's and pdb's go also
//
@del /s *.obj >NUL
@del /s *.ilk >NUL
@IF [%1] == [] GOTO script_exit
@del /s *.exe >NUL
@del /s *.pdb >NUL
:script_exit