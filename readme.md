
# DBJ*Playground
## MS STL + /kernel builds

> (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

What seems to be the issue? The issue seems to be it is undocumented how to use MS STL with cl, while using [the cl /kernel switch](https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019).

I am using this little project to approve or disapprove my doubts. On the official side, things are happening around this issue, albeit not that quickly:

- https://github.com/microsoft/STL/issues/1289
- https://github.com/microsoft/STL/issues/639


Findings and thoughts are in the main.cpp. In comment or snippets and samples. What is interesting here is to use this project, to try and use different parts of MS STL in cl kernel build. And then follow through debugger where is your sample taking you in the depths of the vast MS STL code base.

## Usage

Of course this is strictly Windows code. This is **VS Code** project. For debug builds make sure main.cpp is the current file in your VS Code, and then do (Ctrl+Shit+D) or click on the Debug icon on the left side toolbar, to actually start debugging. That action will first (and always) do the  debug build. 

If you are reading this it is safe to assume you know how to use VS Code to do C++ builds. For release builds there is build.cmd . It calls clean.cmd, you can use on its own too.

If SEH exception is thrown that is caught in main, and "minidump" dmp file is created. You are informed where, and what is the full path.

To open that file you need **Visual Studio**. After which in the upper right corner like "smallish windows" you will spot the link to native debugging. Click on that and soon you will be jumped to the point where the actual C++ or SEH exception was thrown from. Thus you need Visual Studio too.

