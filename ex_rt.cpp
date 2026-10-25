/// "Import 3D Objects in C++ Part 1" by EETechStuff on YouTube
/// https://www.youtube.com/watch?v=3bf1jmoQTOo

#include <iostream>
#include <fstream> /// ifstream, ofstream
#include <vector>
#include <string>
#include <sstream>
#include <iomanip> /// to set cout precision
using namespace std;

/// class that stores vector of ints
class Face
{
public:
    std::vector<uint32_t> VertexIndices{};
    std::vector<uint32_t> TextureIndices{};
    std::vector<uint32_t> NormalIndices{};
};

/// ------------------------------- INITIALIZE --------------------------------


/// final output for OBJ file numerical vectors
std::vector<double> xVals;
std::vector<double> yVals;

/// final output for OBJ file numerical vectors
std::vector<std::vector<float>> VertexPositions;
std::vector<std::vector<float>> TextureCoordinates;
std::vector<std::vector<float>> NormalVectors;
std::vector<std::vector<uint32_t>> Indices;
std::vector<Face> Faces;

/// --------------------------------- METHODS ---------------------------------

void FileOpenError(const std::string& path)
{
    void;
}

/// string methods
std::vector<std::string> ReadFromTextFile(const std::string& path)
{
    /// hold contents of CSV file as single long string
    std::string totalCSVContentsString;
    /// define object of "ostringstream" class called myStringStream
    std::ostringstream myStringStream = std::ostringstream{};
    /// make new ifstream object associated with "path" and open
    std::ifstream inputFile(path);
    if (!inputFile.is_open())
    {
        FileOpenError(path);
    }

    /// obtain pointer to string buffer for myStringStream and read into buffer
    /// myStringStream uses string buffer that contains sequence of characters
    /// which can be accessed directly as string object
    myStringStream << inputFile.rdbuf();
    /// take ostringstream and make into string
    totalCSVContentsString = myStringStream.str();
    /// convert string into input string stream so getline() can work on it
    /// define input string stream using just-read array of characters
    std::istringstream myCSVStringStream(totalCSVContentsString);
    std::vector<std::string> records;
    std::string record;
    /// extracts complete line, reading from array of characters until
    /// newline encountered, and stores it in string "record"
    while (std::getline(myCSVStringStream, record))
    {
        records.push_back(record);
    }
    std::cout << "Number of records in " << path << " = " << records.size() << std::endl << std::endl;
    return records;
}

// void PrintVecString(std::vector<std::string> const& input)
// {
    
// }

/// csv methods
// void WriteToCSV(const std::string& path, std::string outString)
// {
    
// }

// void ParseCSVtoDouble(std::vector<std::string> csvRecordStrings)
// {
    
// }

/// obj methods
// std::vector<uint32_t> ParseFaceIndices(std::string indexGroup)
// {
//     
// }

void ParseOBJ(std::vector<std::string> OBJRecordStrings)
{
    /// internal counters 
    std::vector<uint32_t> VertsPerFace; /// to determine if triangles or quads
    uint32_t vertsPerFace = 0;          /// each face, how many vertices it has
    uint32_t numFaces = 0;              /// track number of faces                  
    /// intermediate string values
    std::string value;                  /// "throw away" values during parsing
    std::vector<std::string> values;
    /// intermediate/individual numerical values
    std::vector<float> VertexPosition;  /// for lines starting with "v"
    std::vector<float> TextureCoordinate; /// for lines starting with "vt"
    std::vector<float> NormalVectors; /// for lines starting with "vn"

    /// parse values from OBJ file 
    for (int i = 0; i < OBJRecordStrings.size(); i++)
    {
        std::stringstream sstream(OBJRecordStrings[i]);
        /// check if starting character is "v" for each line
        while (std::getline(sstream, value, ' '))
        {
            /// if start with "v", parse string values + put in vector<string>
            if (value == "v")
            {
                for (int j = 0; j < 3; j++)
                {
                    std::getline(sstream, value, ' ');
                    float valFloat = std::stof(value);
                    VertexPosition.push_back(valFloat);
                }
                VertexPositions.push_back(VertexPosition);
            }
            // if (value == "vt")
            // {
                
            // }
            // if (value == "f")
            // {
                
            // }
        }
    }
}

// void PrintOBJData()
// {
//     
// }

/// ---------------------------------- MAIN -----------------------------------

int main()
{
    /// define file
    std::string filename("ex.obj");
    /// define vector of strings to hold each line of file
    std::vector<std::string> RecordsStr;

    /// get a vector of strings, each string reps each line in file
    // RecordsStr = ReadFromTxtFile(filename);

    /// extracting the last 3 characters of file name
    std::string fileSuffix = filename.substr(filename.length() - 3, filename.length());

    /// if OBJ file
    if (fileSuffix == "obj")
    {
        // ParseOBJ(RecordsStr);
        // PrintOBJData();
    }
    /// if CSV file
    else if (fileSuffix == "csv")
    {
        // ParseCSVtoDouble(RecordsStr);
        for (int i=0; i < xVals.size(); i++)
        {
            std::cout << "X[" << i << "] = "  << yVals[i] << 
                        " Y [" << i << "] = " << yVals[i] << std::endl;
        }
    }
    /// any other file suffix
    else
    {
        std::cout << "Error while reading '" << fileSuffix << "' file." << std::endl;
        // PrintVecString(RecordsStr);
    }

    std::cin.get(); 
}

/// ---------------------------------------------------------------------------

