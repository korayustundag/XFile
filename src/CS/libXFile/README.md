# XFile - Binary File Management

XFile is a C# library for working with binary files that have a specific header. It provides features for reading, writing, and managing these binary files while maintaining the required file format.

## Features

- **Create XFiles**: Initialize new binary files with a custom or default XFile header.
- **Write Binary Data**: Write binary data to a file, including the XFile header.
- **Append Data**: Append binary data to an existing file.
- **Read Binary Data**: Read binary data from a file, skipping the XFile header.
- **Read Text Data**: Read text data from a file, supporting multiple encodings.

Please read the [documentation](https://github.com/korayustundag/XFile/wiki/Documentation) for more information.

## Usage

### Creating an XFile

```csharp
XFile xFile = new XFile();
```
### Writing Binary Data
```csharp
byte[] data = // data
xFile.WriteAllBytes("file.dat", data);
```
### Appending Data
```csharp
byte[] additionalData = // data
xFile.Append("file.dat", additionalData);
```
### Reading Binary Data
```csharp
byte[] data = xFile.ReadAllBytes("file.dat");
```
### Reading Text Data
```csharp
string text = xFile.ReadAllText("textfile.txt");
```
## Installation
You can include XFile in your project using NuGet:

    dotnet add package libXFile

## License
This project is licensed under the MIT License - see the [LICENSE](https://github.com/korayustundag/XFile/blob/main/LICENSE) file for details.

## Contributing

Contributions and feedback are welcome! Feel free to open issues, create pull requests, or provide suggestions.

## Acknowledgments

-   This project was inspired by the need to work with a specific file format.
