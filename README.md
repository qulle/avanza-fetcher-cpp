# Avanza Fetcher
### A small program that fetches the current stock values from the specified endpoints in the json-file

## Screenshot
![Screenshot of the program](images/result.png?raw=true "Screenshot of the program")

The program was run after hours, that's why 'buy' and 'sell' columns are empty.

## Note
If you just want to use the program may i suggest the python version instead [https://github.com/qulle/avanza-fetcher-py](https://github.com/qulle/avanza-fetcher-py). But if you are in for a challenge, keep on reading and we will build OpenSSL and compile this program to work together with OpenSSL.

# Installation
These instructions are for Windows.

The installation process is split up into two parts, part one covers setup of OpenSSL and part two covers compiling of this program to work together with OpenSSL.

Before you get started make shure that you have **Microsoft Developer Command Prompt**, from now on refered to as **DCP**, installed and MSVC (CL) installed and available from the DCP. You can check if and where the CL is located by running the command `where cl` in the DCP.

## Part 1 - Building OpenSSL
1. Start by downloading and installing\
    1.1 Strawberry Perl [https://strawberryperl.com/](https://strawberryperl.com/)\
    1.2 NASM [https://www.nasm.us/](https://www.nasm.us/)

2. Make sure both Perl and NASM are on your %PATH%

    **Tip** You can check your PATH-variable from the console (cmd) by entering the command `path` but the output is kind of hard to read. Better you can run this script `ECHO.%PATH:;= & ECHO.%` and get a nice list in the console. 
    
    My PATH variables for Perl and NASM looks like this
    ```
    C:\Strawberry\c\bin
    C:\Strawberry\perl\site\bin
    C:\Strawberry\perl\bin
    C:\Program Files\NASM
    ```
    These need to be on your PATH or the next steps will not work. The PATH works from the top down so if you get problems with later commands try moving the Perl and NASM entries to the top of the PATH-list.

3. You now need to configure the DCP for either x86 or x64. This is done through a .bat file that comes with your installation of MSVC/Visual Studio.

    My bat-files are located in\
    `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build`

    The files you are looking for are\
    `vcvars32.bat`, `vcvars64.bat`, `vcvarsall.bat`
    
    You can run `vcvarsall.bat` and give one of the options `x86`, `x86_amd64`, `x86_arm`, `x86_arm64`, `amd64`, `amd64_x86`, `amd64_arm`, or `amd64_arm64`.
    
    I ran the file `vcvars64.bat` in the DCP. That file just calls the `vcvarsall.bat` file with an argument for the x64 architecture.

    **Note** It's important that it's the same DCP that you enter all the commands in. The commands only apply to that session and has to be re-entered if you close the DCP.
    
    **Note** If you can't find the bat-files, don't go on Google and download some random version of these files. The files and system-dependet and can vary depending on what machine they where generated on. Rerun the Visual Studio installer to replace the missing files.

4. Clone the OpenSSL repo from github to a location of your choice.\
    `$ git clone https://github.com/openssl/openssl.git`

5. From the root of the OpenSSL repo run the command\
    `perl Configure VC-WIN32`  or\
    `perl Configure VC-WIN64A` or\
    `perl Configure`

    This will set up environment variables needed for later steps.\
    The last one lets Perl figure out the platform, but it is important that you keep track of what platform that is used.\
    I used the second (VC-WIN64A) command.

    **Note** If you get errors in this step. Make shure that the PATH-variables from step 2 is set up correctly and try moving the Perl and NASM to the top of the PATH-list if you haven't already done so. It is possible that you have other instances of Perl that gets used insted of the Strawberry. If you update the order of you PATH-list you need to close and open the DCP again. You need to redo step 3.

6. You are now ready to build the OpenSSL source. Enter the command `nmake`\
    This step takes time, so go and grab a coffee and come back once in a while to check if there has been any errors.
    
    **Note** If you get a error saying somthing like **NMAKE : fatal error U1077** this can be due to that you didn't use x86 / x64 consistently and are using a combination of both. To start over run `nmake clean` and go back from step 3.

7. If everything went well. Now enter `nmake test` This will also take some time.

8. If all tests pass, you can now install by entering the command  `nmake install`

9. You should now have a installation the OpenSSL on you computer.\
    My is located in `C:\Program Files\OpenSSL`

**Congratulations, you have built, tested and installed OpenSSL.**

## Part 2 - Compiling 
To compile the `avanza-fetcher.cpp` you still need to be in the same DCP. If you have closed it, don't worrie, just enter the correct bat-script from step 3 in the previous part, this sets up the DCP to target the correct enviroment.

**Note** It needs to be the same target-environment as you used to build OpenSSL or you will get LINK-errors.

Start by copying the files `libcrypto-3-x64.dll` and `libssl-3-x64.dll` from the bin folder where OpenSSL was installed in the previous part. Place thes DLL:s next to the `avanza-fetcher.cpp`.

Enter the command `cl /EHsc /std:c++17 /I "C:\Program Files\OpenSSl\include" avanza-fetcher.cpp libssl.lib libcrypto.lib /link /LIBPATH:"C:\Program Files\OpenSSl\lib"`
 
| Command       | Explanation   |
| ------------- | ------------- |
| `cl`          | The MSVC compiler. |

| Argument      | Explanation   |
| ------------- | ------------- |
| `/EHsc`       | How the compiler handles errors. |
| `/std:c++17`  | The C++ standard to compile with, `std:c++17` is currently default, but i like to be explicit. |
| `/I`          | Tells the compiler to look for include-files in the specified location, if it cant find it locally or in the standard includes directory. |
| `avanza-fetcher.cpp` | The avanza source code. |
| `libssl.lib`    | This lib was genereated by the OpenSSL build. |
| `libcrypto.lib` | This lib was genereated by the OpenSSL build. |
| `/link`         | Directive for the compiler to receive a linker-option. |
| `/LIBPATH:`     | Tells the compiler where to look for `libssl.lib` and `libcrypto.lib` |

If all works out you should now have successfully compiled the program.
 
## 3. Runtime
You run the program by entering the command `avanza-fetcher.exe urls.json`\
From this point the program is a compiled binary and the DCP is no longer needed, you can now use a normal CMD prompt.

## Libraries used in this project
1. [https://github.com/openssl/openssl](https://github.com/openssl/openssl) 
2. [https://github.com/nlohmann/json](https://github.com/nlohmann/json)
3. [https://github.com/yhirose/cpp-httplib](https://github.com/yhirose/cpp-httplib)

## Author
[Qulle](https://github.com/qulle/)