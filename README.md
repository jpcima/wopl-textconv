# wopl-textconv
A tool to compare WOPL/WOPN timbre banks as text, integrated with git-diff

These file formats specify instrument storage for OPL/OPN FM chips by Yamaha. See:
- https://github.com/Wohlstand/OPL3BankEditor
- https://github.com/Wohlstand/OPN2BankEditor

## Building and installing

Follow build steps based on CMake:

```
git clone https://github.com/jpcima/wopl-textconv.git
mkdir wopl-textconv/build
cd wopl-textconv/build
cmake ..
cmake --build .
sudo cmake --build . --target install
```

## Using with git

To make WOPL/WOPN comparisons appear as text, some configuration has to be written.

- In the root of the repository,  add "diff=wopl" attributes to `.gitattributes`.

```
*.wopl diff=wopl
*.wopn diff=wopn
```

- In the user configuration `~/.gitconfig`, associate "diff" with `wopl-textconv`.

```
[diff "wopl"]
    textconv = wopl-textconv
    binary = true
[diff "wopn"]
    textconv = wopl-textconv
    binary = true
```
