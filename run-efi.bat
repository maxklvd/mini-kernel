@REM #
@REM # Runs mini-kernel.efi from the current directory if no arguments are given
@REM #

@echo off

@REM #
@REM # Specify the full path to the QEMU executable
@REM #
set QEMU_PATH="C:\Program Files\qemu\qemu-system-x86_64w.exe"
if not exist %QEMU_PATH% goto no_qemu

set QEMU_OPTS=-no-reboot -no-shutdown -net none

call :unzip_ovmf
if not exist uefi\OVMF.fd goto no_ovmf

if not exist hda-contents\efi\boot mkdir hda-contents\efi\boot

if [%~1]==[] goto copy_default_efi
copy /y %1 hda-contents\efi\boot\bootx64.efi
goto run_qemu

:copy_default_efi
copy /y mini-kernel.efi hda-contents\efi\boot\bootx64.efi
goto run_qemu

:run_qemu
start "" %QEMU_PATH% %QEMU_OPTS% -L . -pflash uefi\OVMF.fd -hda fat:rw:hda-contents
goto :EOF

:unzip_ovmf
if exist uefi\OVMF.fd goto :EOF
tar -xf uefi\OVMF.fd.zip -C uefi
goto :EOF

:no_ovmf
echo.
echo ERROR: OVMF.fd not found
echo.
pause
goto :EOF

:no_qemu:
echo.
echo ERROR: QEMU not found. Please check the path to qemu-system-x86_64w.exe in QEMU_PATH (%~nx0)
echo.
pause
