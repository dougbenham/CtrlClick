# CtrlClick

Converts `CTRL+Click` to double-click (aka `select word`) for any Scintilla-based editor (**_[Notepad++](https://notepad-plus-plus.org/), [Powershell ISE](https://docs.microsoft.com/en-us/powershell/scripting/windows-powershell/ise/introducing-the-windows-powershell-ise), [SciTE](https://scintilla.org/SciTE.html)_**, and many more) or **_[VS Code](https://code.visualstudio.com/)_**.

The goal is to be able to `CTRL+Click` to select a word, then without releasing `CTRL` you press `C` to copy, then click to select some other word, then press `V` to paste and then finally release `CTRL`. This workflow feels really good to use, so its a shame these editors don't support it natively.

The app has a tiny footprint and runs silently in the background monitoring for any CTRL+Click on a window that matches the filter. It prevents that click and provides its own behavior (double click with no CTRL held). For best efficiency, you should use the included powershell script (in the release) to add the app to your startup.
