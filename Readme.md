# @find — Global File Finder Tool

I know that for large files,currenlty it is too slow . 
I am working on it

## Requirements
- Root permissions (for installation only)

## Installation
Run the installer script:

```bash
sudo ./installer.sh
```

The installer will:
- Install the binary globally to `/usr/local/bin/@find`
- Ensure executable permissions
- Create an uninstaller script

##  Usage

From any directory in your terminal, simply type:

```bash
@find <partial-filename>
```

For example:

```bash
@find main
```

This will display a list of all files in the current directory (and subdirectories) that match "main" in their filename.

##Example Output

```
@find main

Found 3 matches:
  ./src/main.cpp
  ./tests/test_main.cpp
  ./examples/domain.cpp
```

The matched portion of each filename is highlighted in green and underlined for better visibility.

##Interactive Mode

`@find` operates in interactive mode by default:

1. Start typing your search term
2. Results update in real-time as you type
3. Use backspace to modify your search
4. Press Enter to complete the search, or Ctrl+C to exit

##  Uninstallation

To remove `@find` from your system:

```bash
sudo uninstall-@find
```

## How It Works

`@find` utilizes C++17's `std::filesystem` library to efficiently traverse directory structures. It leverages:

- Non-blocking terminal input for real-time updates
- ANSI color codes for visual highlighting
- Case-insensitive string matching algorithms
- Optimized recursive directory traversal

## Terminal Examples

##Finding Configuration Files

```bash
@find config
```

##Locating Specific File Types

```bash
@find .jpg
```

##Searching for Project Files

```bash
@find project
```

## Troubleshooting

If you encounter any issues:

1. Ensure you have C++17 support in your compiler
2. Verify the installation path is in your system's PATH
3. Check terminal compatibility for ANSI color codes

## Contributing

Contributions are welcome! Feel free to submit pull requests or open issues for bugs and feature requests.

